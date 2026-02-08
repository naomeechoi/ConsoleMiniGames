#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"

class TetrisPlayer;
class TetrisBoard;
class TetrisLevel : public MinigameEngine::Level
{
public:
	TetrisLevel();
	~TetrisLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, MinigameEngine::Input* input) override;
	virtual void Draw() override;

private:
	TetrisPlayer* player = nullptr;
	TetrisBoard* board = nullptr;
	bool hasBeganPlay = false;
	bool isLocking = false;
	int lockResetCount = 0;

	MinigameEngine::Timer playerDownTime;
	MinigameEngine::Timer softDropTimer;
	MinigameEngine::Timer horizontalMoveTimer;
	MinigameEngine::Timer lockDelayTimer;

private:
	void MoveDownOrFix();
	void MoveHorizontal(bool isLeft);
	void Rotate();
	void HardDrop();
	void LockCheck(float deltatime);
	int GetGhostY() const;
};