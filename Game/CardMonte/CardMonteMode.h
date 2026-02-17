#pragma once
#include "World/Mode.h"

class CardMonteMode : public MinigameEngine::Mode
{
public:
	CardMonteMode();
	void SetAnswer(int num);
	bool Check(int selectedNum);
	void Clear();

private:
	int answer = -1;
};

