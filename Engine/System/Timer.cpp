#include "Timer.h"

namespace MinigameEngine {

	Timer::Timer(float targetTime)
		:targetTime(targetTime)
	{
	}

	void Timer::Tick(float deltaTime)
	{
		elapsedTime += deltaTime;
	}

	void Timer::Reset()
	{
		elapsedTime = 0.0f;
	}

	bool Timer::IsTimeOut() const
	{
		return elapsedTime >= targetTime;
	}


	void Timer::SetTargetTime(float newTargetTime)
	{
		targetTime = newTargetTime;
	}
}

