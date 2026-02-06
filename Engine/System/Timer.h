#pragma once
#include "Common/Common.h"

namespace MinigameEngine
{
	class NAOMI_API Timer
	{
	public:
		Timer(float targetTime = 1.0f);
		void Tick(float deltaTime);
		void Reset();
		bool IsTimeOut() const;
		void SetTargetTime(float newTargetTime);
		float GetRatio() const;

	private:
		float elapsedTime = 0.0f;
		float targetTime = 0.0f;
	};
}

