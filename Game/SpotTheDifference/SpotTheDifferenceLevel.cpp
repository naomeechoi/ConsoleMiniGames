#define NOMINMAX
#include "SpotTheDifferenceLevel.h"
#include "SpotTheDifferenceMode.h"
#include "Util/Console.h"
#include "UI/UITop.h"
#include "Math/Vector2.h"
#include "System/Input.h"
#include "Common/LevelType.h"
#include "World/Actor.h"
#include "Render/Renderer.h"
#include "Util/Random.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

SpotTheDifferenceLevel::SpotTheDifferenceLevel()
{
}

SpotTheDifferenceLevel::~SpotTheDifferenceLevel()
{
	if (topUI)
	{
		delete topUI;
		topUI = nullptr;
	}

	if (mode)
	{
		delete mode;
		mode = nullptr;
	}
}

void SpotTheDifferenceLevel::BeginPlay()
{
	if (hasBeganPlay)
		return;

	if (!topUI)
		topUI = new UITop(displaySize.x, Vector2(3, 2), "Spot The Difference");

	if (!mode)
		mode = new SpotTheDifferenceMode();

	LoadText();
	MakeDifferences(); // 틀린 곳 만들어주고 모드에 전송

	hasBeganPlay = true;
}

void SpotTheDifferenceLevel::OnExit()
{
	hasBeganPlay = false;

	if (mode)
		mode->Clear();

	Renderer::Get().OffDebugMode();
}

void SpotTheDifferenceLevel::Tick(float deltaTime, Input* input)
{
	if (input->IsKeyPressed(VK_ESCAPE))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}
}

void SpotTheDifferenceLevel::Draw()
{
	Level::Draw();

	if (topUI)
		topUI->Draw();

	DrawPaint();
}

void SpotTheDifferenceLevel::LoadText()
{
	std::ifstream file("../Assets/SpotTheDifference/0.txt", std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "Fail to open file: 0.txt" << std::endl;
		__debugbreak();
		return;
	}

	paintStr.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	// CR 제거
	paintStr.erase(
		std::remove(paintStr.begin(), paintStr.end(), '\r'),
		paintStr.end()
	);

	// paintSize 계산
	paintSize = Vector2(0, 0);
	lineLengths.clear();

	std::string line;
	std::istringstream iss(paintStr);
	while (std::getline(iss, line))
	{
		paintSize.x = std::max(paintSize.x, (int)line.size());
		++paintSize.y;

		lineLengths.push_back((int)line.size());
	}

	paintStr2 = paintStr;
}

void SpotTheDifferenceLevel::MakeDifferences()
{
	const int COUNT = 20;

	int paintLen = (int)paintStr2.size();
	std::unordered_set<int> differencesSet;

	// 차이 인덱스 뽑기 (줄바꿈 인덱스는 제외)
	while ((int)differencesSet.size() < COUNT)
	{
		int random = Random::Random(0, paintLen - 1);

		if (paintStr2[random] == '\n')
			continue;

		differencesSet.insert(random);
	}

	const int GAP = 6;
	const int totalWidth = paintSize.x * 2 + GAP;
	const int startX = (displaySize.x - totalWidth) / 2;
	const int rightStartX = startX + paintSize.x + GAP;
	const int startY = 4;

	std::unordered_set<Vector2> answerSet;

	for (int difference : differencesSet)
	{
		int random = Random::Random(33, 126);
		while (paintStr2[difference] == (char)random)
			random = Random::Random(33, 126);

		paintStr2[difference] = char(random);

		int x = 0, y = 0;
		for (int i = 0; i < difference; i++)
		{
			if (paintStr2[i] == '\n')
			{
				y++;
				x = 0;
			}
			else
			{
				x++;
			}
		}

		// 화면 좌표로 변환
		answerSet.insert({ x + rightStartX, y + startY });
	}

	Renderer::Get().SetDebugMode(answerSet);

	if (mode)
		mode->SetAnswer(answerSet);
}

void SpotTheDifferenceLevel::DrawPaint()
{
	const int GAP = 6;

	const int startY = 4; // TODO: 하드코딩 나중에 수정

	const int totalWidth = paintSize.x * 2 + GAP;

	const int startX = (displaySize.x - totalWidth) / 2;

	// 왼쪽 그림
	Renderer::Get().SubmitMultiLine(
		paintStr.c_str(),
		Vector2(startX, startY),
		Color::Green
	);

	// 오른쪽 그림
	Renderer::Get().SubmitMultiLine(
		paintStr2.c_str(),
		Vector2(startX + paintSize.x + GAP, startY),
		Color::Green
	);

	Console::SetConsoleTextColor(Color::White);
}
