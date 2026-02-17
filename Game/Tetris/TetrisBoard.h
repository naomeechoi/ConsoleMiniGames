#pragma once
#include "TetrisBlocks.h"
#include "Math/Vector2.h"
#include <string>
#include <vector>
#include <array>
#include <optional>

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

class TetrisBoard
{
    struct SCell
    {
        EBlockType type = EBlockType::EMPTY;
        float flashFrame;
	};

public:
    TetrisBoard(MinigameEngine::Vector2 worldPos);

    void Draw();
    void Tick(float deltaTime);
    void Clear();

    bool CanPlace(EBlockType type, int rotation, int ox, int oy);
    bool PlaceBlock(EBlockType type, int rotation, int ox, int oy);
    float GetScoreIfPlaceBlock(EBlockType type, int rotation, int ox, int oy);
    void AddTrashLines(int count);

    void ClearLines(float deltaTime);
    std::optional<int> ConsumeCleanLineCount();

    bool GetSpawnPos(EBlockType type, int& x, int& y, int& rot);

private:
    void LoadBoardsEdgeTxt();
    bool IsAboveBottom(int y) const;
    bool CheckXBoundary(int x) const;
    bool CheckOutOfBoundary(int x, int y);
    bool IsOccupied(int x, int y);

private:
    std::array<std::array<SCell, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    MinigameEngine::Vector2 worldPos;
    std::string boardEdge;
    std::string oneBrickEdge;
    float lineFlashTimers[BOARD_HEIGHT] = { 0 };

    bool isClearing = false;
    std::vector<int> clearingLines;
    std::optional<int> cleanCount;
};