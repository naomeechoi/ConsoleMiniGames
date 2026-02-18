#include "TetrisBoard.h"
#include "TetrisBlocks.h"
#include "Render/Renderer.h"
#include "Math/Vector2.h"
#include "Util/Color.h"
#include <vector>
#include <optional>
#include <array>
#include "System/FileIO.h"
#include "System/Timer.h"
#include "Util/Random.h"

using namespace MinigameEngine;
using std::vector;
using std::optional;

class TetrisBoard::Impl
{
public:
    struct SCell
    {
        EBlockType type = EBlockType::EMPTY;
        float flashFrame = 0.0f;
    };

    Impl()
    {
        LoadBoardsEdgeTxt();
    }

    void Clear()
    {
        isClearing = false;
        clearingLines.clear();
        cleanCount.reset();

        for (int y = 0; y < BOARD_HEIGHT; ++y)
        {
            for (int x = 0; x < BOARD_WIDTH; ++x)
            {
                grid[y][x].type = EBlockType::EMPTY;
                grid[y][x].flashFrame = 0.0f;
            }
            lineFlashTimers[y] = 0.0f;
        }
    }

    void Draw()
    {
        Vector2 edgePos{ worldPos.x - 1, worldPos.y - 1 };

        Renderer::Get().Submit(
            boardEdge.c_str(),
            edgePos,
            Color::Green
        );

        edgePos.x += SMALL_BOARD_DRAW_PADDING_X + 1;
        Renderer::Get().Submit(
            "N E X T         \0",
            edgePos,
            Color::Yellow
        );

        edgePos.y += 1;
        for (int i = 0; i < 3; i++)
        {
            Renderer::Get().Submit(
                oneBrickEdge.c_str(),
                edgePos,
                Color::Yellow
            );
            edgePos.y += SMALL_BOARD_DRAW_GAP_Y;
        }

        edgePos.y = worldPos.y + 32;
        Renderer::Get().Submit(
            "H O L D         \0",
            edgePos,
            Color::Red
        );

        edgePos.y = worldPos.y + 33;
        Renderer::Get().Submit(
            oneBrickEdge.c_str(),
            edgePos,
            Color::Red
        );

        for (int y = 0; y < BOARD_HEIGHT; ++y)
        {
            for (int x = 0; x < BOARD_WIDTH; ++x)
            {
                if (grid[y][x].type == EBlockType::EMPTY)
                    continue;

                const auto& block = BLOCK_INFO[(int)grid[y][x].type];

                Vector2 pos{
                    worldPos.x + x * BLOCK_SIZE_X,
                    worldPos.y + y * BLOCK_SIZE_Y
                };

                Color drawColor = block.color;

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

                if (grid[y][x].type == EBlockType::DUMMY)
                {
                    drawColor = Color::White;
                }

                Renderer::Get().Submit(
                    BLOCK_STR,
                    pos,
                    drawColor,
                    drawColor,
                    0
                );
            }
        }
    }

    void Tick(float deltaTime)
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

    bool GetSpawnPos(EBlockType type, int& x, int& y, int& rot)
    {
        x = BOARD_WIDTH / 2;
        y = 0;
        rot = 0;

        const auto& block = BLOCK_INFO[(int)type];

        // I L 같은 요소를 위해 2칸까지 위로 올려봄
        for (int yOffset = 0; yOffset >= -3; yOffset--)
        {
            int outOfBoundaryCount = 0;
            bool collision = false;

            for (int i = 0; i < CELL_SIZE; ++i)
            {
                int brickX = x + block.shapes[rot][i].x;
                int brickY = y + yOffset + block.shapes[rot][i].y;

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
            if (outOfBoundaryCount == CELL_SIZE)
            {
                continue;
                /*x = -1;
                y = -1;
                return false;*/
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

    bool IsAboveBottom(int y) const
    {
        return y < BOARD_HEIGHT;
    }

    bool CheckXBoundary(int x) const
    {
        return x >= 0 && x < BOARD_WIDTH;
    }

    bool CheckOutOfBoundary(int x, int y)
    {
        if (y < 0 || y >= BOARD_HEIGHT)
            return false;

        if (x < 0 || x >= BOARD_WIDTH)
            return false;

        return true;
    }

    bool IsOccupied(int x, int y)
    {
        if (y >= 0 && y < BOARD_HEIGHT &&
            x >= 0 && x < BOARD_WIDTH)
        {
            return grid[y][x].type != EBlockType::EMPTY;
        }
        return false;
    }

    bool CanPlace(EBlockType type, int rotation, int ox, int oy)
    {
        const auto& block = BLOCK_INFO[(int)type];

        for (int i = 0; i < CELL_SIZE; i++)
        {
            int x = ox + block.shapes[rotation][i].x;
            int y = oy + block.shapes[rotation][i].y;

            if (!CheckXBoundary(x) || !IsAboveBottom(y) || IsOccupied(x, y))
                return false;
        }
        return true;
    }

    bool PlaceBlock(EBlockType type, int rotation, int ox, int oy)
    {
        const auto& block = BLOCK_INFO[(int)type];
        bool placedAtLeastOne = false;
        for (int i = 0; i < CELL_SIZE; i++)
        {
            int x = ox + block.shapes[rotation][i].x;
            int y = oy + block.shapes[rotation][i].y;

            if (!CheckOutOfBoundary(x, y))
                continue;

            grid[y][x].type = type;
            grid[y][x].flashFrame = 0.3f;
            placedAtLeastOne = true;
        }

        return placedAtLeastOne;
    }

    void ClearLines(float deltaTime)
    {
        if (!isClearing)
        {
            clearingLines.clear();

            for (int y = 0; y < BOARD_HEIGHT; ++y)
            {
                bool full = true;
                for (int x = 0; x < BOARD_WIDTH; ++x)
                {
                    if (grid[y][x].type == EBlockType::EMPTY)
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
                grid[y][x].type = EBlockType::EMPTY;
                grid[y][x].flashFrame = 0.0f;
            }
            lineFlashTimers[y] = 0.0f;
        }

        cleanCount = (int)clearingLines.size();

        isClearing = false;
        clearingLines.clear();
    }


    optional<int> ConsumeCleanLineCount()
    {
        auto temp = cleanCount;
        cleanCount.reset();
        return temp;
    }

    void AddTrashLines(int count)
    {
        if (count <= 0)
            return;

        if (count > BOARD_HEIGHT)
            count = BOARD_HEIGHT;

        for (int y = 0; y < BOARD_HEIGHT - count; y++)
        {
            grid[y] = grid[y + count];
        }

        for (int y = BOARD_HEIGHT - count; y < BOARD_HEIGHT; y++)
        {
            int emptyCell = Random::Random(0, BOARD_WIDTH - 1);

            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                grid[y][x].type =
                    (x == emptyCell) ? EBlockType::EMPTY
                    : EBlockType::DUMMY;
            }
        }
    }

    void LoadBoardsEdgeTxt()
    {
        boardEdge = FileIO::ReadFile("../Assets/Tetris/BoardEdge.txt");
        FileIO::RemoveCR(boardEdge);

        oneBrickEdge = FileIO::ReadFile("../Assets/Tetris/OneBrickEdge.txt");
        FileIO::RemoveCR(oneBrickEdge);
    }

    float GetScoreIfPlaceBlock(EBlockType type, int rotation, int ox, int oy)
    {
        std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> originalGrid = grid;
        std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> testGrid = grid;

        float result = 0.0f;

        const auto& block = BLOCK_INFO[(int)type];
        for (int i = 0; i < CELL_SIZE; i++)
        {
            int x = ox + block.shapes[rotation][i].x;
            int y = oy + block.shapes[rotation][i].y;

            if (!CheckOutOfBoundary(x, y))
                return result;

            testGrid[y][x].type = type;
        }

        // 1. heights sum
        float height = 0;
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            for (int j = 0; j < BOARD_HEIGHT; j++)
            {
                if (testGrid[j][i].type != EBlockType::EMPTY)
                {
                    height += (BOARD_HEIGHT - j);
                    break;
                }
            }
        }
        result += height * (-4.5f);

        // 2. Head is blocked and I am a hole
        float emptyCells = 0;
        for (int i = BOARD_HEIGHT - 1; i >= 1; i--)
        {
            for (int j = 0; j < BOARD_WIDTH; j++)
            {
                if (testGrid[i][j].type == EBlockType::EMPTY
                    && testGrid[i - 1][j].type != EBlockType::EMPTY)
                {
                    emptyCells++;
                }
            }
        }
        result += emptyCells * (-8.8f);

        // 3. Number of cells above the hole
        float coveredCells = 0;
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            int curCount = 0;
            for (int j = 0; j < BOARD_HEIGHT - 1; j++)
            {
                if (testGrid[j][i].type != EBlockType::EMPTY)
                {
                    curCount++;
                }
                else
                {
                    coveredCells += curCount;
                    curCount = 0;
                }
            }
        }
        result += coveredCells * (-0.59f);

        // 4. Number of completed lines
        float completeLine = 0;
        for (int i = 0; i < BOARD_HEIGHT; i++)
        {
            bool isComplete = true;
            for (int j = 0; j < BOARD_WIDTH; j++)
            {
                if (testGrid[i][j].type == EBlockType::EMPTY)
                {
                    isComplete = false;
                    break;
                }
            }
            if (isComplete)
                completeLine++;
        }
        result += completeLine * 8.2f;

        // 5. Number of sides of the current block touching left or right walls
        // 6. Number of sides of the current block touching the floor
        vector<vector<int>> offsets = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

        float countTouchingSides = 0;
        float countTouchingBottom = 0;
        for (int i = 0; i < CELL_SIZE; i++)
        {
            int x = ox + block.shapes[rotation][i].x;
            int y = oy + block.shapes[rotation][i].y;
            for (int j = 0; j < (int)offsets.size(); j++)
            {
                int newX = x + offsets[j][0];
                int newY = y + offsets[j][1];
                if (newX == -1 || newX == BOARD_WIDTH)
                {
                    countTouchingSides++;
                }

                if (newY == BOARD_HEIGHT)
                {
                    countTouchingBottom++;
                }
            }
        }
        result += countTouchingSides * 1.5f;
        result += countTouchingBottom * 5.0f;

        // 7. Number of sides of the current block touching existing blocks
        float countTouchingOthers = 0;
        for (int i = 0; i < CELL_SIZE; i++)
        {
            int x = ox + block.shapes[rotation][i].x;
            int y = oy + block.shapes[rotation][i].y;
            for (int j = 0; j < (int)offsets.size(); j++)
            {
                int newX = x + offsets[j][0];
                int newY = y + offsets[j][1];
                if (newX < 0 || newX >= BOARD_WIDTH || newY < 0 || newY >= BOARD_HEIGHT)
                    continue;
                if (originalGrid[newY][newX].type != EBlockType::EMPTY)
                {
                    countTouchingOthers++;
                }
            }
        }
        result += countTouchingOthers * 2.7f;

        return result;
    }



    std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    MinigameEngine::Vector2 worldPos;
    std::string boardEdge;
    std::string oneBrickEdge;
    float lineFlashTimers[BOARD_HEIGHT] = { 0 };

    bool isClearing = false;
    std::vector<int> clearingLines;
    std::optional<int> cleanCount;
};


TetrisBoard::TetrisBoard(Vector2 worldPos)
{
    impl = new Impl();
    impl->worldPos = worldPos;
}

TetrisBoard::~TetrisBoard()
{
    delete impl;
}

void TetrisBoard::Clear()
{
    impl->Clear();
}

void TetrisBoard::Draw()
{
    impl->Draw();
}

void TetrisBoard::Tick(float deltaTime)
{
    impl->Tick(deltaTime);
}

bool TetrisBoard::GetSpawnPos(EBlockType type, int& x, int& y, int& rot)
{
    return impl->GetSpawnPos(type, x, y, rot);
}

bool TetrisBoard::CanPlace(EBlockType type, int rotation, int ox, int oy)
{
    return impl->CanPlace(type, rotation,ox, oy);
}

bool TetrisBoard::PlaceBlock(EBlockType type, int rotation, int ox, int oy)
{
    return impl->PlaceBlock(type, rotation, ox, oy);
}
 
void TetrisBoard::ClearLines(float deltaTime)
{
    impl->ClearLines(deltaTime);
}

optional<int> TetrisBoard::ConsumeCleanLineCount()
{
    return impl->ConsumeCleanLineCount();
}

void TetrisBoard::AddTrashLines(int count)
{
    impl->AddTrashLines(count);
}

float TetrisBoard::GetScoreIfPlaceBlock(EBlockType type, int rotation, int ox, int oy)
{
    return impl->GetScoreIfPlaceBlock(type, rotation, ox, oy);
}

