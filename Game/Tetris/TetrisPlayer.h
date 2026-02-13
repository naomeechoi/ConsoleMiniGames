#pragma once
#include "System/Timer.h"
#include "System/Input.h"
#include "TetrisPieces.h"
#include <queue>

class TetrisPlayer
{
public:
	TetrisPlayer(MinigameEngine::Vector2 worldStartPos);
	virtual ~TetrisPlayer();

    void BeginPlay();
    void Tick(float deltaTime, MinigameEngine::Input* input);
    void Draw();
    void DrawGhost(int ghostOffSetY);
    void Clear();

    void Spawn(PieceType t, int x, int y, int rot);

    void MoveHorizontal(bool isLeft);
    void MoveDown();

    void Rotate();


    PieceType GetPieceType() const { return type; }
    int GetRotation() const { return rotation; }
    int GetOffsetX() const { return offsetX; }
    int GetOffsetY() const { return offsetY; }
    void SetOffset(int offsetX, int offsetY);
    void SetRotation(int rotation);
    void InsertPieceQueue(PieceType t);
    PieceType GetNextPiece();
	void SetHoldPiece();
    void SetPieceQueue(std::deque<PieceType> pieceQueue);
    std::deque<PieceType> GetPieceQueue();

private:
    void DrawNextPieces();
    void DrawHoldPiece();

private:
    PieceType type;
    int rotation;

    int offsetX = -1;
    int offsetY = -1;

	std::deque<PieceType> pieceQueue;
	PieceType holdPiece = PieceType::EMPTY;

    MinigameEngine::Vector2 worldStartPos;
};

