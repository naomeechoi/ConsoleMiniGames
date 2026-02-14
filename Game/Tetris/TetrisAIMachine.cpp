#include "TetrisAIMachine.h"
#include "TetrisBoard.h"
#include "TetrisPlayer.h"
#include <cassert>
#include <limits>

namespace
{
    constexpr float GRAVITY_TIME = 1.0f;
    constexpr float ACTION_TICK = 0.07f;
}

using namespace MinigameEngine;
using std::deque;
using std::optional;
using std::numeric_limits;

TetrisAIMachine::TetrisAIMachine(Vector2 worldPosition, float lockTime)
	:worldPosition(worldPosition), lockTime(lockTime)
{
    aiBoard = new TetrisBoard(worldPosition);
}

TetrisAIMachine::~TetrisAIMachine()
{
    if(aiPlayer)
    {
        delete aiPlayer;
        aiPlayer = nullptr;
    }

    if (aiBoard)
    {
        delete aiBoard;
        aiBoard = nullptr;
    }
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
    aiDyingTermTimer.Reset();
    isPlaying = false;
}

void TetrisAIMachine::BeginPlay(deque<PieceType> pieceQueue)
{
    if (!aiPlayer)
        aiPlayer = new TetrisPlayer(worldPosition);

    if (!ValidCheck())
    {
        return;
    }

    aiPlayer->SetPieceQueue(pieceQueue);
    isPlaying = true;
    aiLockDelayTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
    aiDyingTermTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
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
    if (!aiPlayer || !aiBoard)
    {
		assert(false && "TetrisAI ValidCheck failed");
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
    if (auto cleanSize = aiBoard->ConsumeCleanLineCount())
        return *cleanSize;
    return 0;
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
        ChangeAIState(&TetrisAIMachine::AIStateGameOver);
        return;
    }

    FindBestPosition(nextType, sx, sy, sr);
    aiPlayer->Spawn(nextType, sx, sy, sr);
    ChangeAIState(&TetrisAIMachine::AIStateFalling);
    aiOneMoveTimer.Reset();
    aiOneMoveTimer.SetTargetTime(aiOneMoveTime);
}


///////////////////////////////////////////////////////////////////////////////
// AI 관련 함수
bool TetrisAIMachine::FindBestPosition(PieceType type, int x, int y, int rot)
{
    float maxScore = numeric_limits<float>::lowest();
    int bestX = x;
    int bestY = y;
    int bestRot = rot;

    constexpr int HORIZONTAL_MARGIN = 3;
    constexpr int Y_OFFSET_START = -2;
    for (int tryRot = 0; tryRot < ROTATION_COUNT; tryRot++)
    {
        for (int tryXOffset = HORIZONTAL_MARGIN * (-1); tryXOffset < BOARD_WIDTH + HORIZONTAL_MARGIN; tryXOffset++)
        {
            int tryY = Y_OFFSET_START;
            if (!aiBoard->CanPlace(type, tryRot, tryXOffset, tryY))
                continue;

            while (aiBoard->CanPlace(type, tryRot, tryXOffset, tryY + 1))
            {
                tryY++;
            }

            float curScore = aiBoard->GetScoreWhenPlacePiece(type, tryRot, tryXOffset, tryY);
            if (curScore > maxScore)
            {
                maxScore = curScore;
                bestX = tryXOffset;
                bestY = tryY;
                bestRot = tryRot;
            }
        }
    }

    return GetBehaveSequence(bestX, bestY, bestRot, x, y, rot);
}

bool TetrisAIMachine::GetBehaveSequence(int bestX, int bestY, int bestRot, int x, int y, int rot)
{
    aiBehaviorSequence.clear();

    int currentX = x;
    int currentY = y;
    int currentRot = rot;

    if (bestY < currentY)
        return false;

    // 회전 동작 추가
    while (currentRot != bestRot)
    {
        aiBehaviorSequence.push_back(EBehavior::ROTATE);
        currentRot = (currentRot + 1) % ROTATION_COUNT;
    }

    // 좌우 이동 동작 추가
    while (currentX != bestX)
    {
        if (currentX < bestX)
        {
            aiBehaviorSequence.push_back(EBehavior::RIGHT);
            currentX++;
        }
        else if (currentX > bestX)
        {
            aiBehaviorSequence.push_back(EBehavior::LEFT);
            currentX--;
        }
    }

    // 다운 동작 추가
    int downCount = bestY - currentY;
    if (downCount < 1) downCount = 1;
    aiBehaviorSequence.insert(aiBehaviorSequence.end(), downCount, EBehavior::DOWN);

    aiOneMoveTime = ACTION_TICK;
    // 속도 보정
    float totalExpectedTime = static_cast<float>(aiBehaviorSequence.size()) * ACTION_TICK;
    if (totalExpectedTime > GRAVITY_TIME) {
        aiOneMoveTime = GRAVITY_TIME / (float)aiBehaviorSequence.size();
    }
    return true;
}

void TetrisAIMachine::ChangeAIState(AIStateFunc nextState)
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
        ChangeAIState(&TetrisAIMachine::AIStateLocking);
        return;
    }

    EBehavior currentBehavior = aiBehaviorSequence.front();
    aiBehaviorSequence.pop_front();
    switch (currentBehavior)
    {
    case EBehavior::ROTATE:
        AIRotate();
        break;
    case EBehavior::RIGHT:
        AIMoveHorizontal(false);
        break;
    case EBehavior::LEFT:
        AIMoveHorizontal(true);
        break;
    case EBehavior::DOWN:
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

    ChangeAIState(&TetrisAIMachine::AIStateLineClearing);
}

void TetrisAIMachine::AIStateLineClearing(float deltaTime)
{
    SpawnAINewPiece();
}

void TetrisAIMachine::AIStateGameOver(float deltaTime)
{
    aiDyingTermTimer.Tick(deltaTime);
    if (!aiDyingTermTimer.IsTimeOut())
        return;

    requestedGameEnd = true;
}

void TetrisAIMachine::AIRotate()
{
    int from = aiPlayer->GetRotation();
    int to = (from + 1) % ROTATION_COUNT;
    aiPlayer->SetRotation(to);
}

void TetrisAIMachine::AIMoveHorizontal(bool isLeft)
{
    int curX = aiPlayer->GetOffsetX();
    int nextX = isLeft ? curX - 1 : curX + 1;
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

optional<bool> TetrisAIMachine::ConsumeRequestedGameEnd()
{
    auto temp = requestedGameEnd;
    requestedGameEnd.reset();
    return temp;
}