#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include <string>

using namespace MinigameEngine;
class SpotTheDifferenceMode;
class UITop;

class SpotTheDifferenceLevel : public Level
{
public:
	SpotTheDifferenceLevel();
	~SpotTheDifferenceLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

private:
	void LoadText();
	void MakeDifferences();
	void DrawPaint();

private:
	SpotTheDifferenceMode* mode = nullptr;
	UITop* topUI = nullptr;
	bool hasBeganPlay = false;
	Vector2 paintSize;
	std::string paintStr;
	std::string paintStr2;
	std::vector<int> lineLengths;
};