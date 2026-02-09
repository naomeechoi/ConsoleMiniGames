#include "TetrisLevel.h"
#include "World/Level.h"
#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "TetrisRotationSystem.h"
#include "Util/Random.h"
#include <iostream>

using namespace MinigameEngine;

TetrisLevel::TetrisLevel()
    : player(nullptr), board(nullptr),
    currentState(TetrisState::Falling)
{
}

TetrisLevel::~TetrisLevel()
{
    OnExit();
    delete player;
    player = nullptr;
    delete board;
    board = nullptr;
}

void TetrisLevel::BeginPlay()
{
    if (hasBeganPlay)
        return;

    hasBeganPlay = true;

    Vector2 startPos;
    startPos.x = (displaySize.x / 2 - 20) / 2;
    startPos.y = (displaySize.y - 40) / 2;

    if (!player) player = new TetrisPlayer(startPos);
    if (!board) board = new TetrisBoard(startPos);

    if (!player || !board)
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    playerDownTime.SetTargetTime(1.0f);
    softDropTimer.SetTargetTime(0.05f);
    horizontalMoveTimer.SetTargetTime(0.05f);

    SpawnNewPiece();
}

void TetrisLevel::OnExit()
{
    hasBeganPlay = false;

    playerDownTime.Reset();
    softDropTimer.Reset();
    horizontalMoveTimer.Reset();
    lockDelayTimer.Reset();

    if (board)
        board->Clear();
}

void TetrisLevel::Tick(float deltaTime, MinigameEngine::Input* input)
{
    if (!player || !board)
        return;

    if (input->IsKeyPressed(VK_ESCAPE))
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    board->Tick(deltaTime);

    switch (currentState)
    {
    case TetrisState::Falling:
        TickFalling(deltaTime, input);
        break;

    case TetrisState::Locking:
        TickLocking(deltaTime, input);
        break;;
    }
}

void TetrisLevel::TickFalling(float deltaTime, Input* input)
{
    // 자동 낙하
    playerDownTime.Tick(deltaTime);
    if (playerDownTime.IsTimeOut())
    {
        if (!MoveDown())
        {
            EnterLockingState();
        }
        playerDownTime.Reset();
    }

    HandleHorizontalInput(deltaTime, input);

    if (input->IsKeyPressed(VK_UP))
        Rotate();

    // 소프트 드롭
    if (input->IsKeyHeld(VK_DOWN))
    {
        softDropTimer.Tick(deltaTime);
        if (softDropTimer.IsTimeOut())
        {
            if (!MoveDown())
            {
                EnterLockingState();
            }
            softDropTimer.Reset();
        }
    }
    else if (input->IsKeyReleased(VK_DOWN))
    {
        softDropTimer.Reset();
    }

    // 하드 드롭
    if (input->IsKeyPressed(VK_SPACE))
    {
        HardDrop();
        isPlaceNow = true;
        EnterLockingState();
    }
}

void TetrisLevel::TickLocking(float deltaTime, Input* input)
{
    HandleHorizontalInput(deltaTime, input);

    if (input->IsKeyPressed(VK_UP))
        Rotate();

    // 아래로 이동 가능하면 Falling으로 전환
    if (board->CanPlace(player->GetPieceType(), player->GetRotation(),
        player->GetOffsetX(), player->GetOffsetY() + 1))
    {
        currentState = TetrisState::Falling;
        return;
    }

    // Lock Delay 진행
    lockDelayTimer.Tick(deltaTime);
    if (lockDelayTimer.IsTimeOut() || isPlaceNow)
    {
        // 블록을 고정
        board->PlacePiece(player->GetPieceType(), player->GetRotation(),
            player->GetOffsetX(), player->GetOffsetY());

        SpawnNewPiece();
        isPlaceNow = false;
    }
}

void TetrisLevel::Draw()
{
    if (!player || !board)
        return;

    board->Draw();
    player->DrawGhost(GetGhostY());
    player->Draw();
}

bool TetrisLevel::MoveDown()
{
    int x = player->GetOffsetX();
    int y = player->GetOffsetY();
    int rot = player->GetRotation();
    PieceType type = player->GetPieceType();

    if (board->CanPlace(type, rot, x, y + 1))
    {
        player->MoveDown();
        return true;
    }
    return false;
}

void TetrisLevel::HandleHorizontalInput(float deltaTime, Input* input)
{
    if (input->IsKeyHeld(VK_LEFT))
    {
        horizontalMoveTimer.Tick(deltaTime);
        if (horizontalMoveTimer.IsTimeOut())
        {
            MoveHorizontal(true);
            horizontalMoveTimer.Reset();
        }
    }
    else if (input->IsKeyReleased(VK_LEFT))
    {
        horizontalMoveTimer.Reset();
    }

    if (input->IsKeyHeld(VK_RIGHT))
    {
        horizontalMoveTimer.Tick(deltaTime);
        if (horizontalMoveTimer.IsTimeOut())
        {
            MoveHorizontal(false);
            horizontalMoveTimer.Reset();
        }
    }
    else if (input->IsKeyReleased(VK_RIGHT))
    {
        horizontalMoveTimer.Reset();
    }
}

void TetrisLevel::MoveHorizontal(bool isLeft)
{
    int x = player->GetOffsetX();
    int y = player->GetOffsetY();
    int rot = player->GetRotation();
    PieceType type = player->GetPieceType();

    int nextX = isLeft ? x - 1 : x + 1;

    if (board->CanPlaceForHorizontal(type, rot, nextX, y))
    {
        player->MoveHorizontal(isLeft);
    }
}

void TetrisLevel::Rotate()
{
    int x = player->GetOffsetX();
    int y = player->GetOffsetY();
    int from = player->GetRotation();
    int to = (from + 1) % ROTATION_COUNT;
    PieceType type = player->GetPieceType();

    const int (*kick)[2] = (type == PieceType::I) ? I_KICK[from] : JLSTZ_KICK[from];

    for (int i = 0; i < offsetsForRotation; ++i)
    {
        int nx = x + kick[i][0];
        int ny = y + kick[i][1];
        if (board->CanPlace(type, to, nx, ny))
        {
            player->SetOffset(nx, ny);
            player->SetRotation(to);
            return;
        }
    }
}

void TetrisLevel::HardDrop()
{
    int x = player->GetOffsetX();
    int y = player->GetOffsetY();
    int rot = player->GetRotation();
    PieceType type = player->GetPieceType();

    while (board->CanPlace(type, rot, x, y + 1))
    {
        y++;
    }

    player->SetOffset(x, y);
}

void TetrisLevel::SpawnNewPiece()
{
    int randomType = Random::Random((int)PieceType::I, (int)PieceType::Z);
    int spawnX, spawnY, rot;

    if (!board->GetSpawnPos(PieceType(randomType), spawnX, spawnY, rot))
    {
        StateGameOver();
        return;
    }

    player->Spawn(PieceType(randomType), spawnX, spawnY, rot);
    currentState = TetrisState::Falling;
}

void TetrisLevel::EnterLockingState()
{
    lockDelayTimer.Reset();
    lockDelayTimer.SetTargetTime(0.2f);
    currentState = TetrisState::Locking;
}

void TetrisLevel::StateGameOver()
{
    RequestShowResult(EResult::fail);
    RequestChangeLevel((int)LevelType::GameResult);
}

int TetrisLevel::GetGhostY() const
{
    int ghostY = player->GetOffsetY();
    while (board->CanPlace(player->GetPieceType(), player->GetRotation(), player->GetOffsetX(), ghostY + 1))
    {
        ghostY++;
    }
    return ghostY;
}