#pragma once
#include "Math/Vector2.h"
#include "Util/Color.h"
using namespace MinigameEngine;

constexpr int PIECE_COUNT = 7;
constexpr int ROTATION_COUNT = 4;
constexpr int BLOCK_COUNT = 4;

enum class PieceType
{
    I = 0,
    O,
    T,
    L,
    J,
    S,
    Z
};

struct PieceInfo
{
    Vector2 blocks[ROTATION_COUNT][BLOCK_COUNT];
    Color color;
};


extern const PieceInfo g_PieceInfo[PIECE_COUNT];