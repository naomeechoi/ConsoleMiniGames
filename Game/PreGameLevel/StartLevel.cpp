#include "StartLevel.h"

#define NOMINMAX
#include "Util/Console.h"
#include "Util/Color.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "TextActor/Pound.h"
#include "World/Actor.h"

#include <fstream>
#include <iostream>
#include <algorithm>

StartLevel::StartLevel()
{
	Load("Main.txt");

	for (int i = 0; i < 149; i++)
	{
		AddNewActor(new Pound(Vector2(i, 0)));
		AddNewActor(new Pound(Vector2(i, 49)));
	}

	for (int i = 0; i < 49; i++)
	{
		AddNewActor(new Pound(Vector2(0, i)));
		AddNewActor(new Pound(Vector2(149, i)));
	}
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

/*void StartLevel::Draw()
{
	if (isDrawn)
		return;

	system("cls");

	for (const auto& line : lines)
	{
		for (char c : line)
		{
			// 문자별로 색상 분류
			if (c == ',' || c == '|')
				Console::SetConsoleTextColor(Color::LightYellow);
			else if (c == '\\' || c == '/' || c == '-')
				Console::SetConsoleTextColor(Color::LightBlue);
			else if (c == '.' || c == '\'' || c == '_')
				Console::SetConsoleTextColor(Color::LightGreen);
			else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				Console::SetConsoleTextColor(Color::White);
			else
				Console::SetConsoleTextColor(Color::LightRed);

			std::cout << c;
		}
		std::cout << "\n";
	}

	isDrawn = true;
}*/

/*
void StartLevel::Load(const char* fileName)
{
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", fileName);

	std::ifstream file(path);
	if (!file.is_open())
	{
		lines.emplace_back("FAILED TO LOAD Assets/Main.txt");
		return;
	}

	std::string line;
	while (std::getline(file, line))
		lines.emplace_back(line);

	file.close();
}*/

void StartLevel::Load(const char* fileName)
{
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", fileName);

	std::ifstream file(path);
	if (!file.is_open())
	{
		AddNewActor(new Actor(
			"FAILED TO LOAD Assets/Main.txt",
			Vector2(0, 0),
			Color::Red
		));
		return;
	}

	// 1. 파일 전체 읽기
	std::vector<std::string> lines;
	std::string line;

	int artWidth = 0;
	while (std::getline(file, line))
	{
		artWidth = std::max(artWidth, (int)line.size());
		lines.emplace_back(line);
	}

	int artHeight = (int)lines.size();
	file.close();

	// 2. 중앙 시작 좌표 계산
	Vector2 screenSize = { 150, 50 }; // 엔진에서 가져오는 값
	int startX = (screenSize.x - artWidth) / 2;
	int startY = (screenSize.y - artHeight) / 2;

	// 3. Actor 생성
	for (int y = 0; y < artHeight; y++)
	{
		const std::string& row = lines[y];
		for (int x = 0; x < (int)row.size(); x++)
		{
			char image[2] = { row[x], '\0' };

			AddNewActor(new Actor(
				image,
				Vector2(startX + x, startY + y),
				Color::White
			));
		}
	}
}
