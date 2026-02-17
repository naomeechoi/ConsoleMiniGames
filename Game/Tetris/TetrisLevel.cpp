#include "TetrisLevel.h"
#include "TetrisPlayer.h"
#include "TetrisAIMachine.h"
#include "TetrisBoard.h"
#include "TetrisBlocks.h"
#include "TetrisRotationSystem.h"
#include "Common/GameCommon.h"
#include "System/Input.h"
#include "Util/Random.h"
#include "Util/Delete.h"
#include "UI/UITop.h"
#include "UI/UIMessage.h"
#include <limits>

const int BOARDS_WIDTH = 55;
const int MAIN_BOARD_HEIGHT = 42;
const int MAX_LOCK_MOVES = 15;
const int BLOCK_QUEUE_MAX_SIZE = 5;
const float GRAVITY_TIME = 1.0f;
const int MESSAGE_UI_OFFSET_X = 3;
const std::string AI_USAGE_GUIDE = "Press F1 to toggle AI Battle Mode.";
const float MOVE_INTERVAL = 0.05f;
const float LOCK_DELAY_INTERVAL = 0.5f;

TetrisLevel::TetrisLevel() : player(nullptr), board(nullptr), curState(nullptr) {}

TetrisLevel::~TetrisLevel()
{
    OnExit();
	SafeDelete(player);
	SafeDelete(board);
	SafeDelete(aiMachine);
    SafeDelete(topUI);
    SafeDelete(messageUI);
}

void TetrisLevel::OnExit()
{
    hasBeganPlay = false;
    if (board)
        board->Clear();

    if (player)
        player->Clear();

    if (messageUI)
        messageUI->Clear();

    curState = nullptr;
    gravityTimer.Reset();
    softDropTimer.Reset();
    horizontalTimer.Reset();
    lockDelayTimer.Reset();

    lockMoveCount = 0;

    // AI 상태 초기화
    AIModeClear();
}

void TetrisLevel::BeginPlay()
{
    if (hasBeganPlay)
        return;

    int totalBoardWidth = BOARDS_WIDTH * 2;
    int remainingSpace = displaySize.x - totalBoardWidth;
    int spacing = remainingSpace / 3;

    Vector2 worldPos;
    worldPos.x = spacing;
    worldPos.y = (displaySize.y - MAIN_BOARD_HEIGHT) / 2 + 1;

    if (!player)
        player = new TetrisPlayer(worldPos);

    if (!board)
        board = new TetrisBoard(worldPos);

    worldPos.x += (BOARDS_WIDTH + spacing);

    if (!aiMachine)
        aiMachine = new TetrisAIMachine(worldPos, 0.5f);

    if (!topUI)
        topUI = new UITop(displaySize.x, Vector2(UI_START_POS_X, UI_START_POS_Y), "Tetris");

    if (!messageUI)
        messageUI = new UIMessage();

    if (!ValidCheck()) {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }
    hasBeganPlay = true;

    messageUI->Start(displaySize.x - 2, Vector2(UI_START_POS_X, displaySize.y - MESSAGE_UI_OFFSET_X), AI_USAGE_GUIDE, " ");

    // 타이머 초기 설정
    gravityTimer.SetTargetTime(GRAVITY_TIME);
    softDropTimer.SetTargetTime(MOVE_INTERVAL);
    horizontalTimer.SetTargetTime(MOVE_INTERVAL);
    lockDelayTimer.SetTargetTime(LOCK_DELAY_INTERVAL);
    
    for (int i = 0; i < BLOCK_QUEUE_MAX_SIZE; i++)
        GenerateUpcomingBlock();

    SpawnNewBlock();
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
        if(player->SetHoldBlockAndCheckNeedSpawn())
            ChangeState(&TetrisLevel::StateLineClearing);
		canHold = false;
    }

    if (input->IsKeyPressed(VK_F1))
    {
        if (!isAIPlayMode)
        {
            isAIPlayMode = true;
            std::deque<EBlockType> tempQ = player->GetBlockQueue();
            tempQ.push_front(player->GetBlockType());
            aiMachine->BeginPlay(tempQ);
        }
        else
        {
            AIModeClear();
        }
    }

    board->Tick(deltaTime);

    // 현재 상태 함수 실행
    if (curState)
        (this->*curState)(deltaTime, input);

    if (isAIPlayMode && aiMachine)
    {
        aiMachine->Tick(deltaTime);

        if (auto cleanSize = board->ConsumeCleanLineCount())
        {
            // ai보드에 블럭 쌓기
            aiMachine->AddTrashLines(*cleanSize);
        }

        board->AddTrashLines(aiMachine->GetCleanCount());
    }

    GenerateUpcomingBlock();

    CheckAILose();
}

void TetrisLevel::CheckAILose()
{
    if (!isAIPlayMode)
        return;
    if (!ValidCheck())
        return;

    if (auto aiLose = aiMachine->ConsumeRequestedGameEnd())
    {
        if (!*aiLose)
            return;
        RequestShowResult(EResult::success);
        RequestChangeLevel((int)LevelType::GameResult);
    }
}

void TetrisLevel::GenerateUpcomingBlock()
{
    EBlockType futureBlock = (EBlockType)Random::Random((int)EBlockType::I, (int)EBlockType::Z);
    player->InsertBlockQueue(futureBlock);
    if (isAIPlayMode && aiMachine)
    {
        aiMachine->InsertBlockToQueue(futureBlock);
    }
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
        board->PlaceBlock(player->GetBlockType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());
        ChangeState(&TetrisLevel::StateLineClearing);
        return;
    }

    // 조작 후 다시 아래로 갈 수 있게 되었다면 Falling으로 복귀
    if (board->CanPlace(player->GetBlockType(), player->GetRotation(),
        player->GetOffsetX(), player->GetOffsetY() + 1))
    {
        ChangeState(&TetrisLevel::StateFalling);
        return;
    }

    // Lock Delay 진행
    lockDelayTimer.Tick(deltaTime);
    if (lockDelayTimer.IsTimeOut())
    {
        board->PlaceBlock(player->GetBlockType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());

        canHold = true;
        ChangeState(&TetrisLevel::StateLineClearing);
    }
}

void TetrisLevel::StateLineClearing(float deltaTime, MinigameEngine::Input* input)
{
    SpawnNewBlock();
}

void TetrisLevel::StateGameOver(float deltaTime, MinigameEngine::Input* input)
{
    RequestShowResult(EResult::fail);
    RequestChangeLevel((int)LevelType::GameResult);
}

void TetrisLevel::SpawnNewBlock()
{
    lockMoveCount = 0;

    EBlockType nextType = player->GetNextBlock();
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
    if (board->CanPlace(player->GetBlockType(), player->GetRotation(),
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
    if (board->CanPlace(player->GetBlockType(), player->GetRotation(), nextX, player->GetOffsetY()))
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
    int to = (from + 1) % ROTATION_SIZE;
    EBlockType type = player->GetBlockType();

    // I 미노인지 아닌지에 따라 루프 횟수와 테이블만 교체
    const int (*kickTable)[2] = (type == EBlockType::I) ? I_KICK_SIMPLE : COMMON_KICK;
    int testCount = (type == EBlockType::I) ? 9 : 5;

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
    while (board->CanPlace(player->GetBlockType(), player->GetRotation(), player->GetOffsetX(), gy + 1))
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
    player->DrawGhost(GetGhostY());
    player->Draw();
    aiMachine->Draw();
    topUI->Draw();
    messageUI->Draw();
}

void TetrisLevel::AIModeClear()
{
    aiMachine->Clear();
    isAIPlayMode = false;
}

bool TetrisLevel::ValidCheck()
{
    if (!player
        || !board
        || !topUI
        || !messageUI
        || !aiMachine)
        return false;

    return true;
}