#pragma once
#include <optional>
#include "Math/Vector2.h"

enum class EBlockType;

class TetrisBoard
{
public:
    TetrisBoard(MinigameEngine::Vector2 worldPos);
    ~TetrisBoard();

    TetrisBoard(const TetrisBoard& other) = delete;
    TetrisBoard& operator=(const TetrisBoard& other) = delete;
    TetrisBoard(TetrisBoard&& other) = delete;
    TetrisBoard& operator=(TetrisBoard&& other) = delete;

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
    class Impl;
    Impl* impl;
};
