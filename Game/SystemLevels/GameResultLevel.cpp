#include "GameResultLevel.h"
#include "Common/LevelType.h"
#include "Util/Console.h"
#include "Render/Renderer.h"
#include "System/Input.h"
#include <iostream>

void GameResultLevel::BeginPlay()
{
	hasBeganPlay = true;
	if (result)
	{
		mainText = "YOU DID GOOD JOB";
	}
	else
	{
		mainText = "ALMOST! TRY AGAIN?";
	}
}

void GameResultLevel::OnExit()
{
	Level::OnExit();
	hasBeganPlay = false;
}

void GameResultLevel::Tick(float deltaTime, Input* input)
{
	if (input->IsKeyPressed(VK_ESCAPE))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}
}

void GameResultLevel::Draw()
{
	Level::Draw();
	DrawResultStr();
}

void GameResultLevel::SetMainText(std::string& str)
{
	mainText = str;
}

void GameResultLevel::SetSubText(std::string& str)
{
	subText = str;
}

void GameResultLevel::DrawResultStr()
{

	int startY = (displaySize.y - 2) / 2;
	int startX = (displaySize.x - (int)mainText.size()) / 2;

	Renderer::Get().Submit(
		mainText.c_str(),
		Vector2(startX, startY),
		Color::White, 0);

	startX = (displaySize.x - (int)subText.size()) / 2;
	Renderer::Get().Submit(
		subText.c_str(),
		Vector2(startX, startY + 2),
		Color::Green, 0);

	Console::SetConsoleTextColor(Color::White);
}