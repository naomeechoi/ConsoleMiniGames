#include "SpotTheDifferenceMode.h"
#include <unordered_set>

SpotTheDifferenceMode::SpotTheDifferenceMode()
{
}

void SpotTheDifferenceMode::SetAnswer(std::unordered_set<Vector2>& answer)
{
	answerSet.insert(answer.begin(), answer.end());
}

const std::unordered_set<Vector2>& SpotTheDifferenceMode::GetAnswer()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return answerSet;
}

bool SpotTheDifferenceMode::Check(Vector2 pos)
{
	return answerSet.count(pos);
}

void SpotTheDifferenceMode::Clear()
{
	answerSet.clear();
}