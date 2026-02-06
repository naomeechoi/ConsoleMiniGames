#pragma once
#include "World/Level.h"
#include <string>

using namespace MinigameEngine;

class GameResultLevel : public Level
{
public:
	// GameState을(를) 통해 상속됨
	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

	void SetMainText(std::string& str);
	void SetSubText(std::string& str);

private:
	void DrawResultStr();
	
private:
	std::string mainText;
	std::string subText = "Press ESC to return to menu";

	bool hasBeganPlay = false;

};

