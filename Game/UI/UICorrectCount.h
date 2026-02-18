#pragma once
#include <string>
#include "Math/Vector2.h"

class UICorrectCount
{
public:
	UICorrectCount();
	void Draw();
	void AddCount();

	void Start(int y, int limitX, int maxCount);
	void Clear();

private:
	int yPos = 0;
	MinigameEngine::Vector2 position;
	int maxCount = 20;
	int curCount = 0;
	int limitX = 0;
	std::string correctCountStr;
};

