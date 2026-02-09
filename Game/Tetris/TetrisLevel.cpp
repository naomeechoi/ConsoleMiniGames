#include "TetrisLevel.h"
#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "TetrisRotationSystem.h"
#include "Util/Random.h"
#include "Util/Delete.h"

const int BOARD_DISPLAY_WIDTH = 20;
const int BOARDS_DISPLAY_WIDTH = 60;
const int BOARD_DISPLAY_HEIGHT = 42;
const int MAX_LOCK_MOVES = 15;
const int PIECE_QUEUE_SIZE = 5;

TetrisLevel::TetrisLevel() : player(nullptr), board(nullptr), curState(nullptr) {}

TetrisLevel::~TetrisLevel()
{
    OnExit();
	SafeDelete(player);
	SafeDelete(board);
	SafeDelete(board2);
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
    startPos.y = (displaySize.y - BOARD_DISPLAY_HEIGHT) / 2;

    if (!player)
        player = new TetrisPlayer(startPos);

    if (!board)
        board = new TetrisBoard(startPos);

    startPos.x += (BOARDS_DISPLAY_WIDTH + spacing);

    if (!board2)
        board2 = new TetrisBoard(startPos);

    if (!player || !board) {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    // 타이머 초기 설정
    gravityTimer.SetTargetTime(1.0f);
    softDropTimer.SetTargetTime(0.05f);
    horizontalTimer.SetTargetTime(0.05f);
    lockDelayTimer.SetTargetTime(0.5f); // 표준 0.5초 딜레이
    
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
    if (!player || !board) return;

    if (input->IsKeyPressed(VK_ESCAPE))
    {
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    board->Tick(deltaTime);
    //board2->Tick(deltaTime);

    // 현재 상태 함수 실행
    if (curState)
        (this->*curState)(deltaTime, input);

    GenerateFuturePiece();
}

void TetrisLevel::GenerateFuturePiece()
{
    PieceType futurePiece = (PieceType)Random::Random((int)PieceType::I, (int)PieceType::Z);
    player->InsertPieceQueue(futurePiece);
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
    if (!player || !board)
        return;

    board->Draw();
    board2->Draw();
    player->DrawGhost(GetGhostY());
    player->Draw();
}

void TetrisLevel::OnExit()
{
    hasBeganPlay = false;
    if (board) board->Clear();

    curState = nullptr;
    gravityTimer.Reset();
    softDropTimer.Reset();
    horizontalTimer.Reset();
    lockDelayTimer.Reset();

	lockMoveCount = 0;
}