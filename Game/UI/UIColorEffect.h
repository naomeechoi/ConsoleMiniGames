#include "System/Timer.h"
#include "Util/Color.h"

class UIColorEffect
{
public:
	UIColorEffect(MinigameEngine::Color& controlledColor, MinigameEngine::Color effectColor, float totalTime, int blankCount);
	void Tick(float deltaTime);
	void Start();
	void Stop();

private:
	void Reset();

private:
	MinigameEngine::Timer totalTimer;
	MinigameEngine::Timer blankTimer;

	float totalTime = 0.0f;
	int blackCount = 0;

	MinigameEngine::Color& controlledColor;
	MinigameEngine::Color originColor = MinigameEngine::Color::Green;
	MinigameEngine::Color effectColor = MinigameEngine::Color::LightRed;

	bool isStarted = false;
};

