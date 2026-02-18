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

const int MAX_LOCK_MOVES = 15;
const int BLOCK_QUEUE_MAX_SIZE = 5;
const float GRAVITY_TIME = 1.0f;
const int MESSAGE_UI_OFFSET_X = 3;
const float MOVE_INTERVAL = 0.05f;
const float LOCK_DELAY_INTERVAL = 0.5f;

using namespace MinigameEngine;

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
    SetTimers(false);

    lockMoveCount = 0;

    // AI 상태 초기화
    AIModeClear();

    SafeDelete(player);
    SafeDelete(board);
    SafeDelete(aiMachine);
    SafeDelete(topUI);
    SafeDelete(messageUI);
}

void TetrisLevel::BeginPlay()
{
    if (hasBeganPlay)
        return;

    const int BOARDS_SIZE = 2;
    const int BOARDS_WIDTH = 55;
    const int TOTAL_BOARDS_WIDTH = BOARDS_WIDTH * BOARDS_SIZE;
    const int MAIN_BOARD_HEIGHT = 42;

    const int SPACES_SIZE = 3;
    const int SPACE = (displaySize.x - TOTAL_BOARDS_WIDTH) / SPACES_SIZE;

    Vector2 worldPos;
    worldPos.x = SPACE;
    worldPos.y = (displaySize.y - MAIN_BOARD_HEIGHT) / 2 + 1;

    if (!player)
        player = new TetrisPlayer(worldPos);

    if (!board)
        board = new TetrisBoard(worldPos);

    worldPos.x += (BOARDS_WIDTH + SPACE);

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

    messageUI->Start(displaySize.x,
        Vector2(UI_START_POS_X, displaySize.y - MESSAGE_UI_OFFSET_X),
        "Press F1 to toggle AI Battle Mode.", " ");

    SetTimers(true);
    
    for (int i = 0; i < BLOCK_QUEUE_MAX_SIZE; i++)
        GenerateUpcomingBlock();

    SpawnNewBlock();
}

void TetrisLevel::Tick(float deltaTime, MinigameEngine::Input* input)
{
    if (!ValidCheck())
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    if (input->IsKeyPressed(VK_ESCAPE))
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    if (input->IsKeyPressed(VK_SHIFT))
    {
        ToggleHold();
    }

    if (input->IsKeyPressed(VK_F1))
    {
        ToggleAIMode();
    }

    board->Tick(deltaTime);

    if (curState)
        (this->*curState)(deltaTime, input);

    AITick(deltaTime);

    GenerateUpcomingBlock();
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

bool TetrisLevel::ValidCheck()
{
    const int pointSize = 5;
    void* checks[pointSize] = { player, board, topUI, messageUI, aiMachine };
    for (int i = 0; i < pointSize; i++)
    {
        if (checks[i] == nullptr)
            return false;
    }
    return true;
}

void TetrisLevel::SetTimers(bool isNotReset)
{
    if (isNotReset)
    {
        gravityTimer.SetTargetTime(GRAVITY_TIME);
        softDropTimer.SetTargetTime(MOVE_INTERVAL);
        horizontalTimer.SetTargetTime(MOVE_INTERVAL);
        lockDelayTimer.SetTargetTime(LOCK_DELAY_INTERVAL);
    }
    else
    {
        gravityTimer.Reset();
        softDropTimer.Reset();
        horizontalTimer.Reset();
        lockDelayTimer.Reset();
    }
}

void TetrisLevel::GenerateUpcomingBlock()
{
    if (!ValidCheck())
        return;

    EBlockType upcomingBlock = (EBlockType)Random::Random((int)EBlockType::I, (int)EBlockType::Z);
    player->InsertBlockQueue(upcomingBlock);
    if (isAIPlayMode)
    {
        aiMachine->InsertBlockToQueue(upcomingBlock);
    }
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

void TetrisLevel::ChangeState(StateFunc nextState)
{
    curState = nextState;
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

    MoveHorizontal(deltaTime, input);
    Rotate(input);

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
    else if (input->IsKeyPressed(VK_SPACE))
    {
        HardDrop();
        ChangeState(&TetrisLevel::StateLocking);
    }
}

void TetrisLevel::StateLocking(float deltaTime, MinigameEngine::Input* input)
{
    if(MoveHorizontal(deltaTime, input))
		lockMoveCount++;
    if(Rotate(input))
		lockMoveCount++;

    // no more moves ard allowed in locking state 
    if (lockMoveCount >= MAX_LOCK_MOVES)
    {
        board->PlaceBlock(player->GetBlockType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());
        ChangeState(&TetrisLevel::StateSpawnNewBlock);
        return;
    }

    // can fall again
    if (board->CanPlace(player->GetBlockType(), player->GetRotation(),
        player->GetOffsetX(), player->GetOffsetY() + 1))
    {
        ChangeState(&TetrisLevel::StateFalling);
        return;
    }

    // check if the lock time is finised
    lockDelayTimer.Tick(deltaTime);
    if (lockDelayTimer.IsTimeOut())
    {
        board->PlaceBlock(player->GetBlockType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());

        canHold = true;
        ChangeState(&TetrisLevel::StateSpawnNewBlock);
    }
}

void TetrisLevel::StateSpawnNewBlock(float deltaTime, MinigameEngine::Input* input)
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

bool TetrisLevel::MoveHorizontal(float deltaTime, MinigameEngine::Input* input)
{
    if (input->IsKeyHeld(VK_LEFT) || input->IsKeyHeld(VK_RIGHT))
    {
        bool isLeft = input->IsKeyHeld(VK_LEFT);
        horizontalTimer.Tick(deltaTime);
        if (horizontalTimer.IsTimeOut())
        {
            horizontalTimer.Reset();
            int nextX = player->GetOffsetX() + (isLeft ? - 1 : 1);
            if (board->CanPlace(player->GetBlockType(), player->GetRotation(), nextX, player->GetOffsetY()))
            {
                player->MoveHorizontal(isLeft);
                return true;
            }
        }
    }
    else
    {
        horizontalTimer.Reset();
    }
    return false;
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

bool TetrisLevel::Rotate(MinigameEngine::Input* input)
{
    if (!input->IsKeyPressed(VK_UP))
        return false;

    int from = player->GetRotation();
    int to = (from + 1) % ROTATION_SIZE;
    EBlockType type = player->GetBlockType();

    const int (*kickTable)[2];
    int testCount;

    if (type == EBlockType::I)
    {
        kickTable = I_KICK_SIMPLE;
        testCount = sizeof(I_KICK_SIMPLE) / sizeof(I_KICK_SIMPLE[0]);
    }
    else
    {
        kickTable = COMMON_KICK;
        testCount = sizeof(COMMON_KICK) / sizeof(COMMON_KICK[0]);
    }

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
    lockDelayTimer.Tick(LOCK_DELAY_INTERVAL);
}

void TetrisLevel::ToggleHold()
{
    if (!canHold)
        return;

    if (player->SetHoldBlockAndCheckNeedSpawn())
        ChangeState(&TetrisLevel::StateSpawnNewBlock);
    canHold = false;
}

void TetrisLevel::ToggleAIMode()
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

void TetrisLevel::AITick(float deltaTime)
{
    if (!isAIPlayMode)
        return;

    if (!aiMachine)
        return;

    aiMachine->Tick(deltaTime);

    // add trash lines
    if (auto cleanSize = board->ConsumeCleanLineCount())
        aiMachine->AddTrashLines(*cleanSize);
    board->AddTrashLines(aiMachine->GetCleanCount());

    // check ai lose
    if (auto aiLose = aiMachine->ConsumeRequestedGameEnd())
    {
        if (*aiLose)
        {
            RequestShowResult(EResult::success);
            RequestChangeLevel((int)LevelType::GameResult);
            return;
        }
    }
}

void TetrisLevel::AIModeClear()
{
    if (aiMachine) {
        aiMachine->Clear();
        isAIPlayMode = false;
    }
}