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
    LoadOneBrickEdgeTxt();
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

    edgePos.x += 42 + 1;
    Renderer::Get().SubmitMultiLine(
        "N E X T         \0",
        edgePos,
        Color::Yellow
    );

    edgePos.y += 1;
    for (int i = 0; i < 3; i++)
    {
        Renderer::Get().SubmitMultiLine(
            oneBrickEdge.c_str(),
            edgePos,
            Color::Yellow
        );
        edgePos.y += 9;
    }

    edgePos.y = startPos.y + 42 - 10;
    Renderer::Get().SubmitMultiLine(
        "H O L D         \0",
        edgePos,
        Color::Red
    );

    edgePos.y = startPos.y + 42 - 9;
    Renderer::Get().SubmitMultiLine(
        oneBrickEdge.c_str(),
        edgePos,
        Color::Red
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

bool TetrisBoard::GetSpawnPos(PieceType type, int& x, int& y, int& rot)
{
    x = BOARD_WIDTH / 2;
    y = 0;
    rot = 0;

    const auto& piece = g_PieceInfo[(int)type];

    // I L 같은 요소를 위해 2칸까지 위로 올려봄
    for (int yOffset = 0; yOffset >= -2; --yOffset)
    {
        int outOfBoundaryCount = 0;
        bool collision = false;

        for (int i = 0; i < BLOCK_COUNT; ++i)
        {
            int brickX = x + piece.blocks[rot][i].x;
            int brickY = y + yOffset + piece.blocks[rot][i].y;

            if (!CheckOutOfBoundary(brickX, brickY))
            {
                outOfBoundaryCount++;
                continue;
            }

            if (IsOccupied(brickX, brickY))
            {
                collision = true;
                break;
            }
        }

        // 모든 블록이 화면 밖이면 spawn 불가
        if (outOfBoundaryCount == BLOCK_COUNT)
        {
            x = -1;
            y = -1;
            return false;
        }

        if (collision)
            continue; // 다른 yOffset 시도

        // 성공한 위치
        y += yOffset;
        return true;
    }

    // -2까지 다 실패하면 spawn 실패
    x = -1;
    y = -1;
    return false;
}

bool TetrisBoard::IsInside(int x, int y) const
{
    return IsInsideX(x) && IsInsideY(y);
}

bool TetrisBoard::IsInsideX(int x) const
{
    return x >= 0 && x < BOARD_WIDTH;
}

bool TetrisBoard::IsInsideY(int y) const
{
    return y < BOARD_HEIGHT;
}

bool TetrisBoard::IsOccupied(int x, int y)
{
    if(y >= 0 && y < BOARD_HEIGHT &&
       x >= 0 && x < BOARD_WIDTH)
    {
        return grid[y][x].type != PieceType::EMPTY;
	}
    return false;
}

bool TetrisBoard::CanPlace(PieceType type, int rotation, int ox, int oy)
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

bool TetrisBoard::CanPlaceForHorizontal(PieceType type, int rotation, int ox, int oy)
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = ox + piece.blocks[rotation][i].x;
        int y = oy + piece.blocks[rotation][i].y;

        if (!IsInsideX(x) || IsOccupied(x, y))
            return false;
    }
    return true;
}

bool TetrisBoard::PlacePiece(PieceType type, int rotation, int ox, int oy)
{
    const auto& piece = g_PieceInfo[(int)type];
    bool placedAtLeastOne = false;
    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        int x = ox + piece.blocks[rotation][i].x;
        int y = oy + piece.blocks[rotation][i].y;

        if (!CheckOutOfBoundary(x, y))
            continue;

        grid[y][x].type = type;
        grid[y][x].flashFrame = 0.3f;
        placedAtLeastOne = true;
    }

    return placedAtLeastOne;
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

void TetrisBoard::LoadOneBrickEdgeTxt()
{
    std::ifstream file("../Assets/Tetris/OneBrickEdge.txt");
    if (!file.is_open())
    {
        std::cout << "Fail to open BoardEdge.txt\n";
        __debugbreak();
        return;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    oneBrickEdge = buffer.str();
}


bool TetrisBoard::CheckOutOfBoundary(int x, int y)
{
    if (y < 0 || y >= BOARD_HEIGHT)
        return false;

    if (x < 0 || x >= BOARD_WIDTH)
        return false;

    return true;
}
