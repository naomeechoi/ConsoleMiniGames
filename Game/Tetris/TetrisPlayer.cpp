#include "TetrisPlayer.h"
#include "TetrisBoard.h"

TetrisPlayer::TetrisPlayer()
{
    Spawn(PieceType::I);
}

TetrisPlayer::~TetrisPlayer()
{
}

void TetrisPlayer::Tick(float deltaTime, Input* input)
{
    //SetProperties(const char* image, const Vector2 & position, Color color, Color bgColor);
}

void TetrisPlayer::Spawn(PieceType t)
{
    type = t;
    rotation = 0;

    offsetX = BOARD_WIDTH / 2;
    offsetY = 0;
}

void TetrisPlayer::MoveLeft(const TetrisBoard& board)
{
    if (board.CanPlace(type, rotation, offsetX - 1, offsetY))
        offsetX--;
}

void TetrisPlayer::MoveRight(const TetrisBoard& board)
{
    if (board.CanPlace(type, rotation, offsetX + 1, offsetY))
        offsetX++;
}

bool TetrisPlayer::MoveDown(const TetrisBoard& board)
{
    if (board.CanPlace(type, rotation, offsetX, offsetY + 1))
    {
        offsetY++;
        return true;
    }
    return false;
}

void TetrisPlayer::RotateCW(const TetrisBoard& board)
{
    int nextRot = (rotation + 1) % 4;

    // 기본 회전
    if (board.CanPlace(type, nextRot, offsetX, offsetY))
    {
        rotation = nextRot;
        return;
    }

    // ===== 최소 wall kick =====
    const int kicks[4][2] =
    {
        {-1,0},
        {1,0},
        {0,-1},
        {0,1}
    };

    for (auto& k : kicks)
    {
        if (board.CanPlace(type, nextRot,
            offsetX + k[0],
            offsetY + k[1]))
        {
            offsetX += k[0];
            offsetY += k[1];
            rotation = nextRot;
            return;
        }
    }
}

void TetrisPlayer::HardDrop(const TetrisBoard& board)
{
    while (MoveDown(board)) {}
}
