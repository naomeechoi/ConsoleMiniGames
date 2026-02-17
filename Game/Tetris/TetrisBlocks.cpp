#include "TetrisBlocks.h"

using MinigameEngine::Color;

const SBlockInfo BLOCK_INFO[BLOCKS_TYPE_SIZE] =
{
	// I
	{
		{
			{ {0,-1},{0,0},{0,1},{0,2} },
			{ {-1,0},{0,0},{1,0},{2,0} },
			{ {0,-1},{0,0},{0,1},{0,2} },
			{ {-1,0},{0,0},{1,0},{2,0} }
		},
		Color::Cyan,
	},

	// O
	{
		{
			{ {0,0},{1,0},{0,1},{1,1} },
			{ {0,0},{1,0},{0,1},{1,1} },
			{ {0,0},{1,0},{0,1},{1,1} },
			{ {0,0},{1,0},{0,1},{1,1} }
		},
		Color::Yellow,
	},

	// T
	{
		{
			{ {-1,0}, {0,0}, {1,0}, {0,-1} },
			{ {0,-1}, {0,0}, {0,1}, {-1,0} },
			{ {-1,0}, {0,0}, {1,0}, {0,1} },
			{ {0,-1}, {0,0}, {0,1}, {1,0} }
		},
		Color::Purple,
	},

	// L
	{
	  {
		{ {1,0}, {0,0}, {-1,0}, {-1, 1} },
		{ {0,1}, {0,0}, {0,-1}, {-1,-1} },
		{ { -1,0}, {0,0}, {1,0}, {1,-1} },
		{ {0,-1}, {0,0}, {0,1}, {1,1} }
	  },
	  Color::LightYellow,
	},

	// J
	{
		{
			{ {-1,-1}, {-1,0}, {0,0}, {1,0} },
			{ {0,-1}, {1,-1}, {0,0}, {0,1} },
			{ {-1,0}, {0,0}, {1,0}, {1,1} },
			{ {0,-1}, {0,0}, {-1,1}, {0,1} }
		},
		Color::Blue,
	},

	// S
	{
		{
			{ {0,0},{1,0},{-1,1},{0,1} },
			{ {0,-1},{0,0},{1,0},{1,1} },
			{ {0,0},{1,0},{-1,1},{0,1} },
			{ {0,-1},{0,0},{1,0},{1,1} }
		},
		Color::Green,
	},

	// Z
	{
		{
			{ {-1,0}, {0,0}, {0,1}, {1,1} },
			{ {1,-1}, {1,0}, {0,0}, {0,1} },
			{ {-1,0}, {0,0}, {0,1}, {1,1} },
			{ {1,-1}, {1,0}, {0,0}, {0,1} }
		},
		Color::Red,
	}

};

const char* BLOCK_STR = "[][]\n[][]\0";
const int BLOCK_SIZE_X = 4;
const int BLOCK_SIZE_Y = 2;

const char* SMALL_BLOCK_STR = "[]\0";
const int SMALL_BLOCK_SIZE_X = 2;
const int SMALL_BLOCK_SIZE_Y = 1;

const int SMALL_BOARD_EDGE_SIZE_X = 12;
const int SMALL_BOARD_EDGE_SIZE_Y = 6;
const int SMALL_BOARD_DRAW_PADDING_X = 42;
const int SMALL_BOARD_DRAW_GAP_Y = 9;
const int LAST_SMALL_BOARD_PADDING_Y = 34;