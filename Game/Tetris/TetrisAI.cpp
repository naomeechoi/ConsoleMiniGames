#include "TetrisAI.h"

TetrisAI::TetrisAI(MinigameEngine::Vector2 worldStartPos)
	: TetrisPlayer(worldStartPos)
{
}

TetrisAI::~TetrisAI()
{
}

void TetrisAI::SetActive(bool active)
{
	this->active = active;
}

bool TetrisAI::IsActive()
{
	return active;
}
