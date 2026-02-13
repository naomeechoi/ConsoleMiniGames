#pragma once

enum class LevelType : int
{
	Quit = -1,
	Start = 0,
	Menu,
	Loading,
	SpotTheDifference,
	CardMonte,
	Tetris,
	GameResult,
};

enum EResult
{
	fail = 0,
	success,
};

constexpr int UI_START_POS_X = 3;
constexpr int UI_START_POS_Y = 2;