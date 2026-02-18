#define NOMINMAX
#include "StartLevel.h"
#include "Util/Console.h"
#include "Util/Color.h"
#include "Common/GameCommon.h"
#include "System/Input.h"
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
	Level::OnExit();
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
	std::string mainStr;
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

	std::string temp;
	size_t start = 0;
	size_t pos = 0;

	while((pos = mainStr.find('7', start)) != std::string::npos)
	{
		int tempHeight = 0;

		size_t lineStart = start;
		size_t lineEnd;
		while ((lineEnd = mainStr.find('\n', lineStart)) != std::string::npos && lineEnd < pos)
		{
			int len = (int)(lineEnd - lineStart);
			width = std::max(width, len);
			tempHeight++;
			lineStart = lineEnd + 1;
		}

		if (lineStart < pos)
		{
			int len = (int)(pos - lineStart);
			width = std::max(width, len);
			tempHeight++;
		}

		height += tempHeight;
		temp = mainStr.substr(start, pos - start);
		mainStrLines.push_back(make_pair(temp, tempHeight));
		start = pos + 1;
	}
}

void StartLevel::DrawStarStr()
{
	int startX = (displaySize.x - width) / 2;
	int startY = (displaySize.y - height) / 2;
	for(int i = 0; i < (int)mainStrLines.size(); i++)
	{
		Color color = Color::Green;
		if (i == 1 || i == 2)
		{
			color = Color::Cyan;
		}
		Renderer::Get().Submit(
			mainStrLines[i].first.c_str(),
			Vector2(startX, startY),
			color
		);
		startY += mainStrLines[i].second;
	}
}