#pragma once
#include <string>
#include "Math/Vector2.h"

using namespace MinigameEngine;

class UITop
{
	class Input;
public:
	UITop(int totalWidth,
		Vector2 position,
		std::string levelStr,
		std::string prefixStr = "<- Press ESC to return to menu");
	void Tick(float deltaTime, Input* input);
	void Draw();

private:
	std::string prefixStr;
	std::string levelStr;
	std::string padding;
	std::string resultStr;

	int totalWidth = 0;
	Vector2 position;
};

