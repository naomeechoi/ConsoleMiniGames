#pragma once
#include "World/Mode.h"
#include <unordered_set>
#include <vector>
#include "Math/Vector2.h"

using namespace MinigameEngine;

class SpotTheDifferenceMode : public Mode
{
public:
	SpotTheDifferenceMode();
	bool Check(Vector2 pos);
	void SetAnswer(std::unordered_set<Vector2>& answer);
	const std::unordered_set<Vector2>& GetAnswer();
	void Clear();

private:
	std::unordered_set<Vector2> answerSet;
};

