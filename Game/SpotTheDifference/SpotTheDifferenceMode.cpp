#include "SpotTheDifferenceMode.h"
#include <unordered_set>

using std::unordered_set;

void SpotTheDifferenceMode::SetAnswer(unordered_set<int>& answer)
{
	answerSet.insert(answer.begin(), answer.end());
}

const unordered_set<int>& SpotTheDifferenceMode::GetAnswer()
{
	return answerSet;
}

const unordered_set<int>& SpotTheDifferenceMode::GetUserAnswer()
{
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

bool SpotTheDifferenceMode::IsGameClear()
{
	return answerSet.size() == userAnswerSet.size();
}

void SpotTheDifferenceMode::Clear()
{
	answerSet.clear();
	userAnswerSet.clear();
}