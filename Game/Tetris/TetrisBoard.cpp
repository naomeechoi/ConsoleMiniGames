#include "TetrisBoard.h"

TetrisBoard::TetrisBoard()
{
    Clear();
}

void TetrisBoard::Clear()
{
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            grid[y][x] = -1;
}

bool TetrisBoard::IsInside(int x, int y) const
{
    return x >= 0 && x < BOARD_WIDTH &&
        y >= 0 && y < BOARD_HEIGHT;
}

bool TetrisBoard::IsOccupied(int x, int y) const
{
    return grid[y][x] != -1;
}

bool TetrisBoard::IsCellEmpty(int x, int y) const
{
    return grid[y][x] == -1;
}

PieceType TetrisBoard::GetCell(int x, int y) const
{
    return (PieceType)grid[y][x];
}

bool TetrisBoard::CanPlace(PieceType type,
    int rotation,
    int offsetX,
    int offsetY) const
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = offsetX + piece.blocks[rotation][i].x;
        int y = offsetY + piece.blocks[rotation][i].y;

        if (!IsInside(x, y))
            return false;

        if (IsOccupied(x, y))
            return false;
    }

    return true;
}

void TetrisBoard::PlacePiece(PieceType type,
    int rotation,
    int offsetX,
    int offsetY)
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = offsetX + piece.blocks[rotation][i].x;
        int y = offsetY + piece.blocks[rotation][i].y;

        grid[y][x] = (int)type;
    }
}

int TetrisBoard::ClearLines()
{
    int cleared = 0;

    for (int y = BOARD_HEIGHT - 1; y >= 0; --y)
    {
        bool full = true;

        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (grid[y][x] == -1)
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            ++cleared;

            for (int yy = y; yy > 0; --yy)
                for (int x = 0; x < BOARD_WIDTH; ++x)
                    grid[yy][x] = grid[yy - 1][x];

            for (int x = 0; x < BOARD_WIDTH; ++x)
                grid[0][x] = -1;

            ++y;
        }
    }

    return cleared;
}