#pragma once
#include <time.h>
#include <Windows.h>

namespace Random
{
	inline void SetRandomSeed()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	inline float Random(float min, float max)
	{
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float diff = max - min;
		return (random * diff) + min;
	}
}