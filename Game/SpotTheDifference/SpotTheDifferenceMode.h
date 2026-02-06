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
	bool Check(int pos);
	bool IsGameClear();
	void SetAnswer(std::unordered_set<int>& answer);
	const std::unordered_set<int>& GetAnswer();
	const std::unordered_set<int>& GetUserAnswer();
	void Clear();

private:
	std::unordered_set<int> answerSet;
	std::unordered_set<int> userAnswerSet;
};

