#pragma once

#include "Core/Engine.h"
#include "World/Level.h"
#include <vector>

using namespace MinigameEngine;

class Game : public Engine
{
public:
	Game();
	~Game();

	static Game* Get();
	inline const std::vector<std::pair<int, std::string>>& GetOnMenu() { return onMenu; }
private:
	static Game* instance;
	std::vector<std::pair<int, std::string>> onMenu;
};

