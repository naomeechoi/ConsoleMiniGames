#pragma once
#include "TetrisPieces.h"
#include "Math/Vector2.h"
#include <string>
#include <vector>
#include <array>
#include <optional>

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

class TetrisBoard
{
    struct Cell
    {
        PieceType type = PieceType::EMPTY;
        float flashFrame;
	};

public:
    TetrisBoard(Vector2 startPos);

    void Draw();
    void Tick(float deltaTime);
    void Clear();

    bool CanPlace(PieceType type, int rotation, int ox, int oy);
    bool PlacePiece(PieceType type, int rotation, int ox, int oy);
    float GetScoreIfPlacePiece(PieceType type, int rotation, int ox, int oy);
    void AddTrashLines(int count);

    void ClearLines(float deltaTime);
    std::optional<int> ConsumeCleanLineCount();

    bool GetSpawnPos(PieceType type, int& x, int& y, int& rot);

private:
    void LoadBoardsEdgeTxt();
    bool IsAboveBottom(int y) const;
    bool CheckXBoundary(int x) const;
    bool CheckOutOfBoundary(int x, int y);
    bool IsOccupied(int x, int y);

private:
    std::array<std::array<Cell, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    Vector2 startPos;
    std::string boardEdge;
    std::string oneBrickEdge;
    float lineFlashTimers[BOARD_HEIGHT] = { 0 };

    bool isClearing = false;
    std::vector<int> clearingLines;
    std::optional<int> cleanCount;
};