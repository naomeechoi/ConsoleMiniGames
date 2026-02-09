#pragma once
#include "System/Timer.h"
#include "System/Input.h"
#include "TetrisPieces.h"

class TetrisPlayer
{
public:
	TetrisPlayer(MinigameEngine::Vector2 worldStartPos);
	~TetrisPlayer();

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

private:
    PieceType type;
    int rotation;

    int offsetX = -1;
    int offsetY = -1;

    MinigameEngine::Vector2 worldStartPos;
};

