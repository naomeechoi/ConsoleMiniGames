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

    void Clear();

    // =========================
    // 상태 조회
    // =========================
    bool IsInside(int x, int y) const;
    bool IsInsideX(int x) const;
    bool IsInsideY(int y) const;
    bool IsOccupied(int x, int y);

    PieceType GetCell(int x, int y) const;
    bool IsCellEmpty(int x, int y) const;

    // =========================
    // 배치 가능 검사
    // =========================
    bool CanPlace(PieceType type,
        int rotation,
        int offsetX,
        int offsetY);
    bool CanPlaceForHorizontal(PieceType type,
        int rotation,
        int offsetX,
        int offsetY);

    // =========================
    // 퍼즐 고정
    // =========================
    bool PlacePiece(PieceType type,
        int rotation,
        int offsetX,
        int offsetY);

    // =========================
    // 라인 제거
    // =========================
    void ClearLines(float deltaTime);

    void Draw();
	void Tick(float deltaTime);

    bool GetSpawnPos(PieceType type, int& x, int& y, int& rot);

private:
    // -1 = empty
    std::array<std::array<Cell, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    Vector2 startPos;

private:
    void LoadEdgeTxt();
    void LoadOneBrickEdgeTxt();
    bool CheckOutOfBoundary(int x, int y);

private:
    std::string edge;
    std::string oneBrickEdge;
    float lineFlashTimers[BOARD_HEIGHT] = { 0 };

    bool isClearing = false;
    std::vector<int> clearingLines;

public:
    float GetScoreWhenPlacePiece(PieceType type,
        int rotation,
        int offsetX,
        int offsetY);

    std::optional<int> ConsumeCleanLineCount();
    void AddLine(int count);

    void AddOneLine();

private:
    std::optional<int> cleanCount;

};