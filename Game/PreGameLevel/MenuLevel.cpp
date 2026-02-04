#include "MenuLevel.h"
#include "Game/Game.h"
#include "Util/Console.h"
#include <iostream>

void MenuLevel::OnExit()
{
	curIdx = 0;
	isDrawn = false;
}

void MenuLevel::Tick(float deltaTime, Input* input)
{
	int len = (int)Game::Get()->GetOnMenu().size();
	if (input->IsKeyPressed(VK_UP))
	{
		curIdx = (curIdx - 1 + len) % len;
		isDrawn = false;
	}
	else if (input->IsKeyPressed(VK_DOWN))
	{
		curIdx = (curIdx + 1) % len;
		isDrawn = false;
	}
	else if (input->IsKeyPressed(VK_RETURN))
	{
		if (curIdx == len - 1)
		{
			OnExit();
			Game::Get()->Quit();
			return;
		}
		RequestChangeLevel(Game::Get()->GetOnMenu()[curIdx].first);
	}
}

void MenuLevel::Draw()
{
	if (isDrawn)
		return;

	system("cls");

	for (int i = 0; i < Game::Get()->GetOnMenu().size(); i++)
	{
		Console::SetConsoleTextColor(i == curIdx ? selectedColor : color);
		std::cout << Game::Get()->GetOnMenu()[i].second << std::endl;
	}

	Console::SetConsoleTextColor(Color::White);
	isDrawn = true;
}