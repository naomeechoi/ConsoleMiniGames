#include "TetrisLevel.h"
#include "TetrisPlayer.h"
#include "TetrisAI.h"
#include "TetrisBoard.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "TetrisRotationSystem.h"
#include "Util/Random.h"
#include "Util/Delete.h"
#include "UI/UITop.h"
#include "UI/UIMessage.h"
#include <limits>

const int UI_START_POS_X = 3;
const int UI_START_POS_Y = 2;
const int BOARD_DISPLAY_WIDTH = 20;
const int BOARDS_DISPLAY_WIDTH = 55;
const int BOARD_DISPLAY_HEIGHT = 42;
const int MAX_LOCK_MOVES = 15;
const int PIECE_QUEUE_SIZE = 5;
const float GRAVITY_TIME = 1.0f;
const float ACTION_TICK = 0.07f;
const int MESSAGE_UI_OFFSET_X = 3;

TetrisLevel::TetrisLevel() : player(nullptr), board(nullptr), curState(nullptr) {}

TetrisLevel::~TetrisLevel()
{
    OnExit();
	SafeDelete(player);
	SafeDelete(board);
	SafeDelete(aiBoard);
    SafeDelete(topUI);
    SafeDelete(messageUI);
}

void TetrisLevel::BeginPlay()
{
    
    if (hasBeganPlay)
        return;

    hasBeganPlay = true;

    int totalBoardWidth = BOARDS_DISPLAY_WIDTH * 2;
    int remainingSpace = displaySize.x - totalBoardWidth;
    int spacing = remainingSpace / 3;

    Vector2 startPos;
    startPos.x = spacing;
    startPos.y = (displaySize.y - BOARD_DISPLAY_HEIGHT) / 2 + 1;

    if (!player)
        player = new TetrisPlayer(startPos);

    if (!board)
        board = new TetrisBoard(startPos);

    startPos.x += (BOARDS_DISPLAY_WIDTH + spacing);

    if (!aiBoard)
        aiBoard = new TetrisBoard(startPos);

    if(!aiPlayer)
        aiPlayer = new TetrisAI(startPos);

    if (!topUI)
        topUI = new UITop(displaySize.x, Vector2(UI_START_POS_X, UI_START_POS_Y), "Tetris");

    if (!messageUI)
        messageUI = new UIMessage();

    if (!ValidCheck()) {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    std::string finalMessage = "Press F1 to toggle AI Battle Mode.";
    messageUI->Start(displaySize.x - 2, Vector2(3, displaySize.y - MESSAGE_UI_OFFSET_X), finalMessage, " ");

    // 타이머 초기 설정
    gravityTimer.SetTargetTime(GRAVITY_TIME);
    softDropTimer.SetTargetTime(0.05f);
    horizontalTimer.SetTargetTime(0.05f);
    lockDelayTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
    aiLockDelayTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
    
    for (int i = 0; i < PIECE_QUEUE_SIZE; i++)
        GenerateFuturePiece();

    SpawnNewPiece();
}

void TetrisLevel::ChangeState(StateFunc nextState)
{
    curState = nextState;
}

void TetrisLevel::Tick(float deltaTime, MinigameEngine::Input* input)
{
    if (!ValidCheck())
        return;

    if (input->IsKeyPressed(VK_ESCAPE))
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    if (input->IsKeyPressed(VK_SHIFT) && canHold)
    {
		player->SetHoldPiece();
        ChangeState(&TetrisLevel::StateLineClearing);
		canHold = false;
    }

    if (input->IsKeyPressed(VK_F1))
    {
        if (!isAIPlayMode)
        {
            isAIPlayMode = true;
            SpawnAINewPiece();
        }
        else
        {
            isAIPlayMode = false;
            AIModeClear();
        }
    }

    board->Tick(deltaTime);

    // 현재 상태 함수 실행
    if (curState)
        (this->*curState)(deltaTime, input);

    if (isAIPlayMode)
    {
        if (aiCurState)
            (this->*aiCurState)(deltaTime);
        aiBoard->Tick(deltaTime);

        if (auto cleanSize = board->ConsumeCleanLineCount())
        {
            // ai보드에 블럭 쌓기
			aiBoard->AddLine(*cleanSize);
        }

        if (auto cleanSize = aiBoard->ConsumeCleanLineCount())
        {
            // 유저보드에 블럭 쌓기
            board->AddLine(*cleanSize);
        }
    }

    GenerateFuturePiece();
}

void TetrisLevel::GenerateFuturePiece()
{
    PieceType futurePiece = (PieceType)Random::Random((int)PieceType::I, (int)PieceType::Z);
    player->InsertPieceQueue(futurePiece);
    aiPlayer->InsertPieceQueue(futurePiece);
}

void TetrisLevel::StateFalling(float deltaTime, MinigameEngine::Input* input)
{
    gravityTimer.Tick(deltaTime);
    if (gravityTimer.IsTimeOut())
    {
        if (!MoveDown())
        {
            ChangeState(&TetrisLevel::StateLocking);
            lockDelayTimer.Reset();
        }
        gravityTimer.Reset();
    }

    // 2. 조작 처리
    HandleHorizontalInput(deltaTime, input);
    Rotate(input);

    // 3. 소프트 드롭
    if (input->IsKeyHeld(VK_DOWN))
    {
        softDropTimer.Tick(deltaTime);
        if (softDropTimer.IsTimeOut())
        {
            if (!MoveDown())
            {
                ChangeState(&TetrisLevel::StateLocking);
                lockDelayTimer.Reset();
            }
            softDropTimer.Reset();
        }
    }

    // 4. 하드 드롭
    if (input->IsKeyPressed(VK_SPACE))
    {
        HardDrop();
        ChangeState(&TetrisLevel::StateLocking);
        // 하드드롭 시 즉시 고정되도록 타이머 강제 종료 효과
        lockDelayTimer.Tick(10.0f);
    }
}

void TetrisLevel::StateLocking(float deltaTime, MinigameEngine::Input* input)
{
    if(HandleHorizontalInput(deltaTime, input))
		lockMoveCount++;
    if(Rotate(input))
		lockMoveCount++;

    if (lockMoveCount >= MAX_LOCK_MOVES)
    {
        board->PlacePiece(player->GetPieceType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());
        ChangeState(&TetrisLevel::StateLineClearing);
        return;
    }

    // 조작 후 다시 아래로 갈 수 있게 되었다면 Falling으로 복귀
    if (board->CanPlace(player->GetPieceType(), player->GetRotation(),
        player->GetOffsetX(), player->GetOffsetY() + 1))
    {
        ChangeState(&TetrisLevel::StateFalling);
        return;
    }

    // Lock Delay 진행
    lockDelayTimer.Tick(deltaTime);
    if (lockDelayTimer.IsTimeOut())
    {
        board->PlacePiece(player->GetPieceType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());

        canHold = true;
        ChangeState(&TetrisLevel::StateLineClearing);
    }
}

void TetrisLevel::StateLineClearing(float deltaTime, MinigameEngine::Input* input)
{
    SpawnNewPiece();
}

void TetrisLevel::StateGameOver(float deltaTime, MinigameEngine::Input* input)
{
    RequestShowResult(EResult::fail);
    RequestChangeLevel((int)LevelType::GameResult);
}

void TetrisLevel::SpawnNewPiece()
{
    lockMoveCount = 0;

    PieceType nextType = player->GetNextPiece();
    int sx, sy, sr;

    if (!board->GetSpawnPos(nextType, sx, sy, sr))
    {
        ChangeState(&TetrisLevel::StateGameOver);
        return;
    }

    player->Spawn(nextType, sx, sy, sr);
    ChangeState(&TetrisLevel::StateFalling);
    gravityTimer.Reset();
}

bool TetrisLevel::HandleHorizontalInput(float deltaTime, MinigameEngine::Input* input)
{
    if (input->IsKeyHeld(VK_LEFT) || input->IsKeyHeld(VK_RIGHT))
    {
        horizontalTimer.Tick(deltaTime);
        if (horizontalTimer.IsTimeOut())
        {
            horizontalTimer.Reset();
            return MoveHorizontal(input->IsKeyHeld(VK_LEFT));
        }
    }
    else
    {
        horizontalTimer.Reset();
    }
}

bool TetrisLevel::MoveDown()
{
    if (board->CanPlace(player->GetPieceType(), player->GetRotation(),
        player->GetOffsetX(), player->GetOffsetY() + 1))
    {
        player->MoveDown();
        return true;
    }
    return false;
}

bool TetrisLevel::MoveHorizontal(bool isLeft)
{
    int nextX = isLeft ? player->GetOffsetX() - 1 : player->GetOffsetX() + 1;
    if (board->CanPlaceForHorizontal(player->GetPieceType(), player->GetRotation(), nextX, player->GetOffsetY()))
    {
        player->MoveHorizontal(isLeft);
        return true;
    }

    return false;
}

bool TetrisLevel::Rotate(MinigameEngine::Input* input)
{
    if (!input->IsKeyPressed(VK_UP)) return false;

    int from = player->GetRotation();
    int to = (from + 1) % ROTATION_COUNT;
    PieceType type = player->GetPieceType();

    // I 미노인지 아닌지에 따라 루프 횟수와 테이블만 교체
    const int (*kickTable)[2] = (type == PieceType::I) ? I_KICK_SIMPLE : COMMON_KICK;
    int testCount = (type == PieceType::I) ? 9 : 5;

    for (int i = 0; i < testCount; ++i)
    {
        int nx = player->GetOffsetX() + kickTable[i][0];
        int ny = player->GetOffsetY() + kickTable[i][1];

        if (board->CanPlace(type, to, nx, ny))
        {
            player->SetOffset(nx, ny);
            player->SetRotation(to);
            return true;
        }
    }
    return false;
}

void TetrisLevel::HardDrop()
{
    player->SetOffset(player->GetOffsetX(), GetGhostY());
}

int TetrisLevel::GetGhostY() const
{
    int gy = player->GetOffsetY();
    while (board->CanPlace(player->GetPieceType(), player->GetRotation(), player->GetOffsetX(), gy + 1))
    {
        gy++;
    }
    return gy;
}

void TetrisLevel::Draw()
{
    if (!ValidCheck())
        return;

    board->Draw();
    aiBoard->Draw();
    player->DrawGhost(GetGhostY());
    player->Draw();

    if (isAIPlayMode)
    {
        //aiPlayer->DrawGhost(GetGhostY());
        aiPlayer->Draw();
    }
    topUI->Draw();
    messageUI->Draw();
}

void TetrisLevel::OnExit()
{
    hasBeganPlay = false;
    if (board)
        board->Clear();

    if (player)
        player->Clear();

    curState = nullptr;
    gravityTimer.Reset();
    softDropTimer.Reset();
    horizontalTimer.Reset();
    lockDelayTimer.Reset();

	lockMoveCount = 0;

    // AI 상태 초기화
    AIModeClear();

    messageUI->Clear();
}

void TetrisLevel::AIModeClear()
{
    if (aiBoard)
        aiBoard->Clear();
    if (aiPlayer)
        aiPlayer->Clear();
    aiBehaviorSequence.clear();
    aiCurState = nullptr;
    aiOneMoveTimer.Reset();
    aiLockDelayTimer.Reset();
    isAIPlayMode = false;
}

bool TetrisLevel::ValidCheck()
{
    if (!player
        || !board
        || !topUI
        || !messageUI
        || !aiPlayer
        || !aiBoard)
        return false;

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// AI 관련 함수
bool TetrisLevel::AIFindBestPos(PieceType type, int x, int y, int rot)
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
            if(!aiBoard->CanPlace(type, tryRot, tryXOffset, tryY))
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

bool TetrisLevel::AIGetBehaveSequence(int bestX, int bestY, int bestRot, int x, int y, int rot)
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

void TetrisLevel::SpawnAINewPiece()
{
    if (!aiPlayer || !aiBoard)
        return;

    PieceType nextType = aiPlayer->GetNextPiece();
    int sx, sy, sr;

    if (!aiBoard->GetSpawnPos(nextType, sx, sy, sr))
    {
        AIChangeState(&TetrisLevel::AIStateGameOver);
        return;
    }

    if (!AIFindBestPos(nextType, sx, sy, sr))
    {
        AIChangeState(&TetrisLevel::AIStateGameOver);
        return;
    }

    aiPlayer->Spawn(nextType, sx, sy, sr);
    AIChangeState(&TetrisLevel::AIStateFalling);
    aiOneMoveTimer.Reset();
    aiOneMoveTimer.SetTargetTime(aiOneMoveTime);
}

void TetrisLevel::AIChangeState(AIStateFunc nextState)
{
    aiCurState = nextState;
}

void TetrisLevel::AIStateFalling(float deltaTime)
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
        AIChangeState(&TetrisLevel::AIStateLocking);
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

void TetrisLevel::AIStateLocking(float deltaTime)
{
    aiLockDelayTimer.Tick(deltaTime);
    if (!aiLockDelayTimer.IsTimeOut())
        return;

    aiBoard->PlacePiece(aiPlayer->GetPieceType(), aiPlayer->GetRotation(),
        aiPlayer->GetOffsetX(), aiPlayer->GetOffsetY());

    AIChangeState(&TetrisLevel::AIStateLineClearing);
}

void TetrisLevel::AIStateLineClearing(float deltaTime)
{
    SpawnAINewPiece();
}

void TetrisLevel::AIStateGameOver(float deltaTime)
{
    RequestShowResult(EResult::success);
    RequestChangeLevel((int)LevelType::GameResult);
}

void TetrisLevel::AIRotate()
{
    int from = aiPlayer->GetRotation();
    int to = (from + 1) % ROTATION_COUNT;
    aiPlayer->SetRotation(to);
}

void TetrisLevel::AIMoveHorizontal(bool isLeft)
{
    int nextX = isLeft ? aiPlayer->GetOffsetX() - 1 : aiPlayer->GetOffsetX() + 1;
    if (aiBoard->CanPlaceForHorizontal(aiPlayer->GetPieceType(), aiPlayer->GetRotation(), nextX, aiPlayer->GetOffsetY()))
    {
        aiPlayer->MoveHorizontal(isLeft);
    }
}

bool TetrisLevel::AIMoveDown()
{
    if (aiBoard->CanPlace(aiPlayer->GetPieceType(), aiPlayer->GetRotation(),
        aiPlayer->GetOffsetX(), aiPlayer->GetOffsetY() + 1))
    {
        aiPlayer->MoveDown();
        return true;
    }

    return false;
}
