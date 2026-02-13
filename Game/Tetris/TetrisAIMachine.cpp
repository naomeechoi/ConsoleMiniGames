#include "TetrisAIMachine.h"
#include <cassert>

const float GRAVITY_TIME = 1.0f;
const float ACTION_TICK = 0.07f;

TetrisAIMachine::TetrisAIMachine(Vector2 worldPos, float lockTime)
	:worldPos(worldPos), lockTime(lockTime)
{
    aiBoard = new TetrisBoard(worldPos);
}

TetrisAIMachine::~TetrisAIMachine()
{
}

void TetrisAIMachine::Clear()
{
    if (aiBoard)
        aiBoard->Clear();
    if (aiPlayer)
        aiPlayer->Clear();
    aiBehaviorSequence.clear();
    aiCurState = nullptr;
    aiOneMoveTimer.Reset();
    aiLockDelayTimer.Reset();
    isPlaying = false;
}

void TetrisAIMachine::BeginPlay(std::deque<PieceType> pieceQueue)
{
    if (!aiPlayer)
        aiPlayer = new TetrisPlayer(worldPos);

    if (!ValidCheck()) {
        return;
    }

    aiPlayer->SetPieceQueue(pieceQueue);
    isPlaying = true;
    aiLockDelayTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
    SpawnAINewPiece();
}

void TetrisAIMachine::Tick(float deltaTime)
{
    if (!ValidCheck())
        return;

    if(isPlaying && aiCurState)
        (this->*aiCurState)(deltaTime);

    aiBoard->Tick(deltaTime);
}

void TetrisAIMachine::Draw()
{
    if (aiBoard)
        aiBoard->Draw();

    if (isPlaying && aiPlayer)
	    aiPlayer->Draw();
}

bool TetrisAIMachine::ValidCheck()
{
    if (!aiPlayer
        || !aiBoard)
    {
		assert("TetrisAI ValidCheck failed");
        return false;
    }

    return true;
}

void TetrisAIMachine::AddTrashLines(int count)
{
    aiBoard->AddTrashLines(count);
}

int TetrisAIMachine::GetCleanCount()
{
    int cleanCount = 0;
    if (auto cleanSize = aiBoard->ConsumeCleanLineCount())
        cleanCount = *cleanSize;
    return cleanCount;
}

void TetrisAIMachine::SynclonizeSpawnPieceWithPlayer(PieceType piece)
{
    aiPlayer->InsertPieceQueue(piece);
}

void TetrisAIMachine::InsertPieceToQueue(PieceType piece)
{
    if (aiPlayer)
        aiPlayer->InsertPieceQueue(piece);
}

void TetrisAIMachine::SpawnAINewPiece()
{
    if (!ValidCheck())
        return;

    PieceType nextType = aiPlayer->GetNextPiece();
    int sx, sy, sr;

    if (!aiBoard->GetSpawnPos(nextType, sx, sy, sr))
    {
        AIChangeState(&TetrisAIMachine::AIStateGameOver);
        return;
    }

    if (!AIFindBestPos(nextType, sx, sy, sr))
    {
        AIChangeState(&TetrisAIMachine::AIStateGameOver);
        return;
    }

    aiPlayer->Spawn(nextType, sx, sy, sr);
    AIChangeState(&TetrisAIMachine::AIStateFalling);
    aiOneMoveTimer.Reset();
    aiOneMoveTimer.SetTargetTime(aiOneMoveTime);
}


///////////////////////////////////////////////////////////////////////////////
// AI 관련 함수
bool TetrisAIMachine::AIFindBestPos(PieceType type, int x, int y, int rot)
{
    float maxScore = std::numeric_limits<float>::lowest();
    int bestX = x;
    int bestY = y;
    int bestRot = rot;
    bool foundAny = false;
    for (int tryRot = 0; tryRot < ROTATION_COUNT; tryRot++)
    {
        for (int tryXOffset = -3; tryXOffset < BOARD_WIDTH + 3; tryXOffset++)
        {
            int tryY = -2;
            if (!aiBoard->CanPlace(type, tryRot, tryXOffset, tryY))
                continue;

            while (aiBoard->CanPlace(type, tryRot, tryXOffset, tryY + 1))
            {
                tryY++;
            }

            if (tryY < 0)
                continue;

            float curScore = aiBoard->GetScoreWhenPlacePiece(type, tryRot, tryXOffset, tryY);
            if (curScore > maxScore)
            {
                maxScore = curScore;
                bestX = tryXOffset;
                bestY = tryY;
                bestRot = tryRot;
                foundAny = true;
            }
        }
    }

    if (!foundAny)
        return false;

    return AIGetBehaveSequence(bestX, bestY, bestRot, x, y, rot);
}

bool TetrisAIMachine::AIGetBehaveSequence(int bestX, int bestY, int bestRot, int x, int y, int rot)
{
    aiBehaviorSequence.clear();

    int currentX = x;
    int currentY = y;
    int currentRot = rot;

    if (bestY - currentY <= 0)
        return false;

    // 회전 동작 추가
    while (currentRot != bestRot)
    {
        aiBehaviorSequence.push_back(Behavior::rotate);
        currentRot = (currentRot + 1) % ROTATION_COUNT;
    }

    // 좌우 이동 동작 추가
    while (currentX != bestX)
    {
        if (currentX < bestX)
        {
            aiBehaviorSequence.push_back(Behavior::right); // 2 = move right
            currentX++;
        }
        else if (currentX > bestX)
        {
            aiBehaviorSequence.push_back(Behavior::left); // 3 = move left
            currentX--;
        }
    }

    aiDestY = bestY;
    // 다운 동작 추가
    aiBehaviorSequence.insert(aiBehaviorSequence.end(), bestY - currentY, Behavior::down);

    aiOneMoveTime = ACTION_TICK;
    // 속도 보정
    float totalExpectedTime = aiBehaviorSequence.size() * ACTION_TICK;
    if (totalExpectedTime > GRAVITY_TIME) {
        aiOneMoveTime = GRAVITY_TIME / (float)aiBehaviorSequence.size();
    }
    return true;
}

void TetrisAIMachine::AIChangeState(AIStateFunc nextState)
{
    aiCurState = nextState;
}

void TetrisAIMachine::AIStateFalling(float deltaTime)
{
    aiOneMoveTimer.Tick(deltaTime);
    if (!aiOneMoveTimer.IsTimeOut())
        return;

    aiOneMoveTimer.Reset();

    if (aiBehaviorSequence.empty())
    {
        while (aiBoard->CanPlace(aiPlayer->GetPieceType(), aiPlayer->GetRotation(),
            aiPlayer->GetOffsetX(), aiPlayer->GetOffsetY() + 1))
        {
            aiPlayer->MoveDown();
        }
        AIChangeState(&TetrisAIMachine::AIStateLocking);
        return;
    }

    //한동작 소화
    Behavior currentBehavior = aiBehaviorSequence.front();
    aiBehaviorSequence.pop_front();
    switch (currentBehavior)
    {
    case Behavior::rotate:
        AIRotate();
        break;
    case Behavior::right:
        AIMoveHorizontal(false);
        break;
    case Behavior::left:
        AIMoveHorizontal(true);
        break;
    case Behavior::down:
        AIMoveDown();
        break;
    default:
        break;
    }
}

void TetrisAIMachine::AIStateLocking(float deltaTime)
{
    aiLockDelayTimer.Tick(deltaTime);
    if (!aiLockDelayTimer.IsTimeOut())
        return;

    aiBoard->PlacePiece(aiPlayer->GetPieceType(), aiPlayer->GetRotation(),
        aiPlayer->GetOffsetX(), aiPlayer->GetOffsetY());

    AIChangeState(&TetrisAIMachine::AIStateLineClearing);
}

void TetrisAIMachine::AIStateLineClearing(float deltaTime)
{
    SpawnAINewPiece();
}

void TetrisAIMachine::AIStateGameOver(float deltaTime)
{
    requestedEnd = true;
}

void TetrisAIMachine::AIRotate()
{
    int from = aiPlayer->GetRotation();
    int to = (from + 1) % ROTATION_COUNT;
    aiPlayer->SetRotation(to);
}

void TetrisAIMachine::AIMoveHorizontal(bool isLeft)
{
    int nextX = isLeft ? aiPlayer->GetOffsetX() - 1 : aiPlayer->GetOffsetX() + 1;
    if (aiBoard->CanPlaceForHorizontal(aiPlayer->GetPieceType(), aiPlayer->GetRotation(), nextX, aiPlayer->GetOffsetY()))
    {
        aiPlayer->MoveHorizontal(isLeft);
    }
}

bool TetrisAIMachine::AIMoveDown()
{
    if (aiBoard->CanPlace(aiPlayer->GetPieceType(), aiPlayer->GetRotation(),
        aiPlayer->GetOffsetX(), aiPlayer->GetOffsetY() + 1))
    {
        aiPlayer->MoveDown();
        return true;
    }

    return false;
}

std::optional<bool> TetrisAIMachine::ConsumeRequestedEnd()
{
    auto temp = requestedEnd;
    requestedEnd.reset();
    return temp;
}