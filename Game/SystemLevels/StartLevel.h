#pragma once
#include "World/Level.h"
#include <vector>
#include <string>

using namespace MinigameEngine;

class StartLevel : public Level
{
public:
	StartLevel();
public:
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

private:
	void LoadText();
	void DrawStarStr();

private:
	std::vector<std::pair<std::string, int>> mainStrLines;
	bool isDrawn = false;
	int width = 0;
	int height = 0;
};

