#pragma once
#include <string>
#include "Math/Vector2.h"

using namespace MinigameEngine;

class UIMessage
{
public:
	UIMessage();
	void Draw();
	void Start(int totalWidth, Vector2 position, std::string levelStr, std::string prefixStr);
	void Clear();

private:
	Vector2 pos;
	std::string message;
};

