#pragma once
#include "World/Level.h"
#include "Util/Color.h"
#include <vector>

using namespace MinigameEngine;

class MenuLevel : public Level
{
public:
	// GameState을(를) 통해 상속됨
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

private:
	void DrawMenuStr();

private:
	std::vector<int> items;
	int curIdx = 0;

	Color selectedColor = Color::LightGreen;
	Color color = Color::LightYellow;
};

