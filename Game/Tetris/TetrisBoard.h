#pragma once
#include "TetrisPieces.h"

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

class TetrisBoard
{
public:
    TetrisBoard();

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
    int ClearLines();

    void Draw();

private:
    // -1 = empty
    int grid[BOARD_HEIGHT][BOARD_WIDTH];
};