#pragma once
#include "TetrisPieces.h"
#include "Math/Vector2.h"
#include <string>
#include <vector>
#include <array>

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
    bool IsOccupied(int x, int y) const;

    PieceType GetCell(int x, int y) const;
    bool IsCellEmpty(int x, int y) const;

    // =========================
    // 배치 가능 검사
    // =========================
    bool CanPlace(PieceType type,
        int rotation,
        int offsetX,
        int offsetY) const;

    // =========================
    // 퍼즐 고정
    // =========================
    void PlacePiece(PieceType type,
        int rotation,
        int offsetX,
        int offsetY);

    // =========================
    // 라인 제거
    // =========================
    void ClearLines(float deltaTime);

    void Draw();
	void Tick(float deltaTime);

private:
    // -1 = empty
    std::array<std::array<Cell, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    Vector2 startPos;

private:
    void LoadEdgeTxt();

private:
    std::string edge;
    float lineFlashTimers[BOARD_HEIGHT] = { 0 };

    bool isClearing = false;
    std::vector<int> clearingLines;

};