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

constexpr const char* BLOCK_STR = "[][]\n[][]\0";
constexpr int BLOCK_SIZE_X = 4;
constexpr int BLOCK_SIZE_Y = 2;

constexpr const char* SMALL_BLOCK_STR = "[]\0";
constexpr int SMALL_BLOCK_SIZE_X = 2;
constexpr int SMALL_BLOCK_SIZE_Y = 1;

constexpr int SMALL_BOARD_EDGE_SIZE_X = 12;
constexpr int SMALL_BOARD_EDGE_SIZE_Y = 6;
constexpr int SMALL_BOARD_DRAW_PADDING_X = 42;
constexpr int SMALL_BOARD_DRAW_GAP_Y = 9;
constexpr int LAST_SMALL_BOARD_PADDING_Y = 34;

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;