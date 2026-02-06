#pragma once
#include "World/Mode.h"

using namespace MinigameEngine;

class CardMonteMode : public Mode
{
public:
	CardMonteMode();
	void SetAnswer(int num);
	bool Check(int selectedNum);
	void Clear();

private:
	int answer = -1;
};

