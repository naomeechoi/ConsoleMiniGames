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

	float Timer::GetRatio() const
	{
		if (targetTime <= 0.0f)
			return 1.0f;
		return elapsedTime / targetTime; // 0.0 ~ 1.0
	}
}

