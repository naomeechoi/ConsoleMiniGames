#pragma once
#include <string>
#include "Math/Vector2.h"

using namespace MinigameEngine;

class UICorrectCount
{
public:
	UICorrectCount(int y, int limitX, int maxCount);
	void Draw();
	void AddCount(int adding);

private:
	int yPos = 0;
	Vector2 position;
	int maxCount = 20;
	int curCount = 0;
	int limitX = 0;
	std::string correctCountStr;
};

