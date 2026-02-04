#pragma once
#include "World/Level.h"

using namespace MinigameEngine;

class LoadingLevel : public Level
{
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;
};

