#include "TetrisBoard.h"
#include "Math/Vector2.h"
#include "Render/Renderer.h"
#include "Util/Random.h"
#include "System/FileIO.h"

using namespace MinigameEngine;
using std::vector;
using std::optional;

TetrisBoard::TetrisBoard(Vector2 worldPos)
    : worldPos(worldPos)
{
    LoadBoardsEdgeTxt();
    Clear();
}

void TetrisBoard::Clear()
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

void TetrisBoard::Draw()
{
    Vector2 edgePos{ worldPos.x - 1, worldPos.y - 1 };

    Renderer::Get().SubmitMultiLine(
        boardEdge.c_str(),
        edgePos,
        Color::Green
    );

    edgePos.x += SMALL_BOARD_DRAW_PADDING_X + 1;
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
        edgePos.y += SMALL_BOARD_DRAW_GAP_Y;
    }

    edgePos.y = worldPos.y + 32;
    Renderer::Get().SubmitMultiLine(
        "H O L D         \0",
        edgePos,
        Color::Red
    );

    edgePos.y = worldPos.y + 33;
    Renderer::Get().SubmitMultiLine(
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

            Renderer::Get().SubmitMultiLine(
                BLOCK_STR,
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

bool TetrisBoard::GetSpawnPos(EBlockType type, int& x, int& y, int& rot)
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

bool TetrisBoard::IsAboveBottom(int y) const
{
    return y < BOARD_HEIGHT;
}

bool TetrisBoard::CheckXBoundary(int x) const
{
    return x >= 0 && x < BOARD_WIDTH;
}

bool TetrisBoard::CheckOutOfBoundary(int x, int y)
{
    if (y < 0 || y >= BOARD_HEIGHT)
        return false;

    if (x < 0 || x >= BOARD_WIDTH)
        return false;

    return true;
}

bool TetrisBoard::IsOccupied(int x, int y)
{
    if(y >= 0 && y < BOARD_HEIGHT &&
       x >= 0 && x < BOARD_WIDTH)
    {
        return grid[y][x].type != EBlockType::EMPTY;
	}
    return false;
}

bool TetrisBoard::CanPlace(EBlockType type, int rotation, int ox, int oy)
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

bool TetrisBoard::PlaceBlock(EBlockType type, int rotation, int ox, int oy)
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


optional<int> TetrisBoard::ConsumeCleanLineCount()
{
    auto temp = cleanCount;
    cleanCount.reset();
    return temp;
}

void TetrisBoard::AddTrashLines(int count)
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

void TetrisBoard::LoadBoardsEdgeTxt()
{
    boardEdge = FileIO::ReadFile("../Assets/Tetris/BoardEdge.txt");
    FileIO::RemoveCR(boardEdge);

    oneBrickEdge = FileIO::ReadFile("../Assets/Tetris/OneBrickEdge.txt");
    FileIO::RemoveCR(oneBrickEdge);
}

float TetrisBoard::GetScoreIfPlaceBlock(EBlockType type, int rotation, int ox, int oy)
{
    std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> originalGrid = grid;
    std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> testGrid = grid;

    float result = 0.0f;

    // 가상으로 채워넣기
    const auto& block = BLOCK_INFO[(int)type];
    for (int i = 0; i < CELL_SIZE; i++)
    {
        int x = ox + block.shapes[rotation][i].x;
        int y = oy + block.shapes[rotation][i].y;

        if (!CheckOutOfBoundary(x, y))
            return result;
        
        testGrid[y][x].type = type;
    }

    // 1. 블록 높이의 합
    float height = 0;
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        for(int j = 0; j < BOARD_HEIGHT; j++)
        {
            if(testGrid[j][i].type != EBlockType::EMPTY)
            {
                height += (BOARD_HEIGHT - j);
                break;
            }
		}
    }
    result += height * (-4.5f);

    // 2. 머리 위는 막혀 있고 나는 구멍일 때
    float emptyCells = 0;
    for (int i = BOARD_HEIGHT - 1; i >= 1; i--)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            if (testGrid[i][j].type == EBlockType::EMPTY
                && testGrid[i-1][j].type != EBlockType::EMPTY)
            {
                emptyCells++;
            }
        }
    }
    result += emptyCells * (-8.8f);

    // 3. 구멍 위에 있는 셀의 수
    float coveredCells = 0;
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        int curCount = 0;
       for(int j = 0; j < BOARD_HEIGHT - 1; j++)
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

    // 4. 완성된 줄의 개수
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

    // 5. 현재 블록이 좌우 벽면과 닿는 면의 개수
    // 6. 현재 블록이 바닥면과 닿는 개수
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

    // 7. 현재 블록이 기존 블록들과 닿는 개수
    float countTouchingOthers = 0;
    for (int i = 0; i < CELL_SIZE; i++)
    {
        int x = ox + block.shapes[rotation][i].x;
        int y = oy + block.shapes[rotation][i].y;
        for (int j = 0; j < (int)offsets.size(); j++)
        {
            int newX = x + offsets[j][0];
            int newY = y + offsets[j][1];
            if(newX < 0 || newX >= BOARD_WIDTH || newY < 0 || newY >= BOARD_HEIGHT)
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

