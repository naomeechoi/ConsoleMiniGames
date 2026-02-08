#include "TetrisBoard.h"
#include "Math/Vector2.h"
#include "Render/Renderer.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace MinigameEngine;

TetrisBoard::TetrisBoard(Vector2 startPos)
    : startPos(startPos)
{
    LoadEdgeTxt();
    Clear();
}

void TetrisBoard::Clear()
{
    isClearing = false;
    clearingLines.clear();

    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            grid[y][x].type = PieceType::EMPTY;
            grid[y][x].flashFrame = 0.0f;
        }
        lineFlashTimers[y] = 0.0f;
    }
}

void TetrisBoard::Draw()
{
    Vector2 edgePos{ startPos.x - 1, startPos.y - 1 };

    Renderer::Get().SubmitMultiLine(
        edge.c_str(),
        edgePos,
        Color::Green
    );

    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (grid[y][x].type == PieceType::EMPTY)
                continue;

            const auto& piece = g_PieceInfo[(int)grid[y][x].type];

            Vector2 pos{
                startPos.x + x * brickXSize,
                startPos.y + y * brickYSize
            };

            Color drawColor = piece.color;

            if (grid[y][x].flashFrame > 0.0f &&
                fmod(grid[y][x].flashFrame, 0.1f) < 0.05f)
            {
                drawColor = Color::White;
            }

            if (lineFlashTimers[y] > 0.0f &&
                fmod(lineFlashTimers[y], 0.1f) < 0.05f)
            {
                drawColor = Color::Black;
            }

            Renderer::Get().SubmitMultiLine(
                brick,
                pos,
                drawColor,
                drawColor,
                0
            );
        }
    }
}

void TetrisBoard::Tick(float deltaTime)
{
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (grid[y][x].flashFrame > 0.0f)
            {
                grid[y][x].flashFrame -= deltaTime;
                if (grid[y][x].flashFrame < 0.0f)
                    grid[y][x].flashFrame = 0.0f;
            }
        }
    }

    ClearLines(deltaTime);
}

bool TetrisBoard::IsInside(int x, int y) const
{
    return x >= 0 && x < BOARD_WIDTH &&
        y >= 0 && y < BOARD_HEIGHT;
}

bool TetrisBoard::IsOccupied(int x, int y) const
{
    return grid[y][x].type != PieceType::EMPTY;
}

bool TetrisBoard::CanPlace(PieceType type, int rotation, int ox, int oy) const
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = ox + piece.blocks[rotation][i].x;
        int y = oy + piece.blocks[rotation][i].y;

        if (!IsInside(x, y) || IsOccupied(x, y))
            return false;
    }
    return true;
}

void TetrisBoard::PlacePiece(PieceType type, int rotation, int ox, int oy)
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = ox + piece.blocks[rotation][i].x;
        int y = oy + piece.blocks[rotation][i].y;

        grid[y][x].type = type;
        grid[y][x].flashFrame = 0.3f;
    }
}

void TetrisBoard::ClearLines(float deltaTime)
{
    if (!isClearing)
    {
        clearingLines.clear();

        for (int y = 0; y < BOARD_HEIGHT; ++y)
        {
            bool full = true;
            for (int x = 0; x < BOARD_WIDTH; ++x)
            {
                if (grid[y][x].type == PieceType::EMPTY)
                {
                    full = false;
                    break;
                }
            }

            if (full)
            {
                clearingLines.push_back(y);
                lineFlashTimers[y] = 0.1f;

                for (int x = 0; x < BOARD_WIDTH; ++x)
                {
                    grid[y][x].flashFrame = 0.0f;
                }
            }
        }

        if (!clearingLines.empty())
            isClearing = true;

        return;
    }

    bool allDone = true;
    for (int y : clearingLines)
    {
        lineFlashTimers[y] -= deltaTime;
        if (lineFlashTimers[y] > 0.0f)
            allDone = false;
    }

    if (!allDone)
        return;

    int dst = BOARD_HEIGHT - 1;

    for (int src = BOARD_HEIGHT - 1; src >= 0; --src)
    {
        if (std::find(clearingLines.begin(), clearingLines.end(), src) != clearingLines.end())
            continue;

        if (dst != src)
            grid[dst] = grid[src];

        dst--;
    }

    for (int y = dst; y >= 0; --y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            grid[y][x].type = PieceType::EMPTY;
            grid[y][x].flashFrame = 0.0f;
        }
        lineFlashTimers[y] = 0.0f;
    }

    isClearing = false;
    clearingLines.clear();
}

void TetrisBoard::LoadEdgeTxt()
{
    std::ifstream file("../Assets/Tetris/BoardEdge.txt");
    if (!file.is_open())
    {
        std::cout << "Fail to open BoardEdge.txt\n";
        __debugbreak();
        return;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    edge = buffer.str();
}
