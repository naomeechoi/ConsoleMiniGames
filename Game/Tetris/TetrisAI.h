#pragma once
#include "TetrisPlayer.h"

class TetrisAI : public TetrisPlayer
{
public:
    TetrisAI(MinigameEngine::Vector2 worldStartPos);
    ~TetrisAI();

public:
    void SetActive(bool active);
    bool IsActive();

private:
	bool active = false;
};

