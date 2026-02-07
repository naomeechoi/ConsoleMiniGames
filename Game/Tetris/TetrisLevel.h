#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"

using namespace MinigameEngine;

class TetrisLevel : public Level
{
public:
	TetrisLevel();
	~TetrisLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;
};