#pragma once
#include <string>
#include "Math/Vector2.h"

class UIMessage
{
public:
	UIMessage();
	void Draw();
	void Start(int totalWidth, MinigameEngine::Vector2 position, std::string levelStr, std::string prefixStr);
	void Clear();

private:
	MinigameEngine::Vector2 pos;
	std::string message;
};

