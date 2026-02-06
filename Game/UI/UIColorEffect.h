#include "Util/Color.h"
#include "System/Timer.h"

using namespace MinigameEngine;

class UIColorEffect
{
public:
	UIColorEffect(Color& controlledColor, Color effectColor, float totalTime, int blankCount);
	void Tick(float deltaTime);
	void Start();
	void Stop();

private:
	void Reset();

private:
	Timer totalTimer;
	Timer blankTimer;

	float totalTime = 0.0f;
	int blackCount = 0;

	Color& controlledColor;
	Color originColor = Color::Green;
	Color effectColor = Color::LightRed;

	bool isStarted = false;
};

