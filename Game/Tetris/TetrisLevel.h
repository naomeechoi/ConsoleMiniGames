#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"
#include "TetrisPieces.h"

class TetrisPlayer;
class TetrisBoard;
class TetrisLevel : public MinigameEngine::Level
{
	enum class TetrisState
	{
		Falling,
		Locking,
	};

public:
	TetrisLevel();
	~TetrisLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, MinigameEngine::Input* input) override;
	virtual void Draw() override;

private:
	bool hasBeganPlay = false;

	TetrisPlayer* player = nullptr;
	TetrisBoard* board = nullptr;

	MinigameEngine::Timer playerDownTime;
	MinigameEngine::Timer softDropTimer;
	MinigameEngine::Timer horizontalMoveTimer;
	MinigameEngine::Timer lockDelayTimer;

	TetrisState currentState = TetrisState::Falling;

	bool isPlaceNow = false;

private:
	bool MoveDown();
	void Rotate();
	void HardDrop();
	void SpawnNewPiece();

	void TickFalling(float deltatime, MinigameEngine::Input* input);
	void TickLocking(float deltatime, MinigameEngine::Input* input);

	void MoveHorizontal(bool isLeft);
	void HandleHorizontalInput(float deltatime, MinigameEngine::Input* input);
	
	void EnterLockingState();

private:
	void StateGameOver();
	int GetGhostY() const;
};