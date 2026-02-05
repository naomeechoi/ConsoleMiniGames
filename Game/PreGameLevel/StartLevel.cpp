#define NOMINMAX
#include "StartLevel.h"
#include "Util/Console.h"
#include "Util/Color.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "World/Actor.h"
#include "Render/Renderer.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

StartLevel::StartLevel()
{
	LoadText();
}

void StartLevel::OnExit()
{
	isDrawn = false;
}

void StartLevel::Tick(float deltaTime, Input* input)
{
	Level::Tick(deltaTime, input);
	if (input->IsKeyPressed(VK_RETURN))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}
}

void StartLevel::Draw()
{
	Level::Draw();
	DrawStarStr();
}

void StartLevel::LoadText()
{
	std::ifstream file("../Assets/Main.txt", std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "Fail to open file: Main.txt" << std::endl;
		__debugbreak();
		return;
	}

	mainStr.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	mainStr.erase(
		std::remove(mainStr.begin(), mainStr.end(), '\r'),
		mainStr.end()
	);

	// width/height 계산을 따로 하려면 아래처럼
	startStrWidth = 0;
	startStrHeight = 0;
	std::string line;
	std::istringstream iss(mainStr);
	while (std::getline(iss, line))
	{
		startStrWidth = std::max(startStrWidth, (int)line.size());
		++startStrHeight;
	}
}

void StartLevel::DrawStarStr()
{
	int startX = (displaySize.x - startStrWidth) / 2;
	int startY = (displaySize.y - startStrHeight) / 2;
	Renderer::Get().SubmitMultiLine(
		mainStr.c_str(),
		Vector2(startX, startY),
		Color::Green
	);
}