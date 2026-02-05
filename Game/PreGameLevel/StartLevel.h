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
	std::string mainStr;
	bool isDrawn = false;
	int startStrWidth = 0;
	int startStrHeight = 0;
};

