#pragma once
#include "Math/Vector2.h"
#include "Util/Color.h"

constexpr int BLOCKS_TYPE_SIZE = 7;
constexpr int ROTATION_SIZE = 4;
constexpr int CELL_SIZE = 4;

enum class EBlockType
{
    EMPTY = -1,
    I = 0,
    O,
    T,
    L,
    J,
    S,
    Z,
    DUMMY,
};

struct SBlockInfo
{
    MinigameEngine::Vector2 shapes[ROTATION_SIZE][CELL_SIZE];
    MinigameEngine::Color color;
};

extern const SBlockInfo BLOCK_INFO[BLOCKS_TYPE_SIZE];
extern const char* SMALL_BLOCK_STR;
extern const int SMALL_BLOCK_SIZE_X;
extern const int SMALL_BLOCK_SIZE_Y;

extern const char* BLOCK_STR;
extern const int BLOCK_SIZE_X;
extern const int BLOCK_SIZE_Y;

extern const int SMALL_BOARD_EDGE_SIZE_X;
extern const int SMALL_BOARD_EDGE_SIZE_Y;
extern const int SMALL_BOARD_DRAW_PADDING_X;
extern const int SMALL_BOARD_DRAW_GAP_Y;
extern const int LAST_SMALL_BOARD_PADDING_Y;