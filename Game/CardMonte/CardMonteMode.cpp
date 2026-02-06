#include "CardMonteMode.h"

CardMonteMode::CardMonteMode()
{
}

void CardMonteMode::SetAnswer(int num)
{
	answer = num;
}

bool CardMonteMode::Check(int selectedNum)
{
	return answer == selectedNum;
}

void CardMonteMode::Clear()
{
	answer = -1;
}