#pragma once
#include "World/Actor.h"
#include "System/Timer.h"
#include "TetrisPieces.h"

using namespace MinigameEngine;

class TetrisBoard;
class TetrisPlayer : public Actor
{
public:
	TetrisPlayer();
	~TetrisPlayer();

    virtual void Tick(float deltaTime, Input* input) override;

    void Spawn(PieceType type);

    void MoveLeft(const TetrisBoard& board);
    void MoveRight(const TetrisBoard& board);
    bool MoveDown(const TetrisBoard& board);

    void RotateCW(const TetrisBoard& board);
    void HardDrop(const TetrisBoard& board);


    PieceType GetPieceType() const { return type; }
    int GetRotation() const { return rotation; }
    int GetOffsetX() const { return offsetX; }
    int GetOffsetY() const { return offsetY; }

private:
    PieceType type;
    int rotation;

    int offsetX;
    int offsetY;

};

