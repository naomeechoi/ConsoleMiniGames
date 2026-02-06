#include "MenuLevel.h"
#include "Game/Game.h"
#include "Util/Console.h"
#include "Render/Renderer.h"
#include <iostream>

void MenuLevel::OnExit()
{
	Level::OnExit();
	curIdx = 0;
}

void MenuLevel::Tick(float deltaTime, Input* input)
{
	int len = (int)Game::Get()->GetOnMenu().size();
	if (input->IsKeyPressed(VK_UP))
	{
		curIdx = (curIdx - 1 + len) % len;
	}
	else if (input->IsKeyPressed(VK_DOWN))
	{
		curIdx = (curIdx + 1) % len;
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
	Level::Draw();
	DrawMenuStr();
}

void MenuLevel::DrawMenuStr()
{
	const int Y_MARGIN = 2;
	auto& menu = Game::Get()->GetOnMenu();
	int menuSize = (int)menu.size();

	int startY = (displaySize.y - menuSize) / 2;

	for (int i = 0; i < menuSize; i++)
	{
		const std::string& menuText = menu[i].second;
		int startX = (displaySize.x - (int)menuText.size()) / 2;
	
		Renderer::Get().SubmitMultiLine(
			menuText.c_str(),
			Vector2(startX, startY),
			(i == curIdx ? selectedColor : color)
		);
		startY += Y_MARGIN;
	}

	Console::SetConsoleTextColor(Color::White);
}