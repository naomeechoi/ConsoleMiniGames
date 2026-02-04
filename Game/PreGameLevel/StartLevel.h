#pragma once
#include "World/Level.h"
#include <vector>
#include <string>

using namespace MinigameEngine;

class StartLevel : public Level
{
public:
	StartLevel();
public:
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	//virtual void Draw() override;

private:
	void Load(const char* fileName);

private:
	std::vector<std::string> lines;
	bool isDrawn = false;
};

