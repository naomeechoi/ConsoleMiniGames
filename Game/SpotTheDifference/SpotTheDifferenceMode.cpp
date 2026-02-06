#include "SpotTheDifferenceMode.h"
#include <unordered_set>

SpotTheDifferenceMode::SpotTheDifferenceMode()
{
}

void SpotTheDifferenceMode::SetAnswer(std::unordered_set<int>& answer)
{
	answerSet.insert(answer.begin(), answer.end());
}

const std::unordered_set<int>& SpotTheDifferenceMode::GetAnswer()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return answerSet;
}

const std::unordered_set<int>& SpotTheDifferenceMode::GetUserAnswer()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return userAnswerSet;
}


bool SpotTheDifferenceMode::Check(int pos)
{
	if (!answerSet.count(pos))
		return false;

	if (userAnswerSet.count(pos))
		return false;

	userAnswerSet.insert(pos);
	return true;
}

void SpotTheDifferenceMode::Clear()
{
	answerSet.clear();
}