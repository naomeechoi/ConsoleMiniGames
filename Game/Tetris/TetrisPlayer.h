#pragma once
#include "System/Timer.h"
#include "TetrisBlocks.h"
#include <deque>

namespace MinigameEngine {
    class Input;
}

class TetrisPlayer
{
public:
	TetrisPlayer(MinigameEngine::Vector2 worldStartPos);

    void Draw();
    void DrawGhost(int ghostOffSetY);
    void Clear();

    void Spawn(EBlockType t, int x, int y, int rot);

    void MoveHorizontal(bool isLeft);
    void MoveDown();

    void Rotate();


    EBlockType GetBlockType() const { return type; }
    int GetRotation() const { return rotation; }
    int GetOffsetX() const { return offsetX; }
    int GetOffsetY() const { return offsetY; }
    void SetOffset(int offsetX, int offsetY);
    void SetRotation(int rotation);
    void InsertBlockQueue(EBlockType t);
    EBlockType GetNextBlock();
    bool SetHoldBlockAndCheckNeedSpawn();
    void SetBlockQueue(std::deque<EBlockType> blockQueue);
    std::deque<EBlockType> GetBlockQueue();

private:
    void DrawMiniBlock(EBlockType type, int boardX, int boardY);

private:
    EBlockType type;
    int rotation;

    int offsetX = -1;
    int offsetY = -1;

	std::deque<EBlockType> blockQueue;
	EBlockType holdBlock = EBlockType::EMPTY;

    MinigameEngine::Vector2 worldStartPos;
};

