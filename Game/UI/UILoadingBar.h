#pragma once
#include <string>
#include "Math/Vector2.h"
#include "System/Timer.h"

using namespace MinigameEngine;

class UILoadingBar
{
public:
	UILoadingBar(Vector2 position, float maxWidth, float totalTime, char shapeChar);
	void Tick(float deltaTime);
	void Draw();

	void Start();
	void Stop();
	void Clear();

private:
	Timer timer;

	Vector2 position;
	float maxWidth;

	std::string loadingBarStr;
	char shapeChar = '#';
	float charDrawAccum = 0.0f;

	float totalTime = 60.0f;
	float interval = 1.0f;

	bool isMove = false;
};

