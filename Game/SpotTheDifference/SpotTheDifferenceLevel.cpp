#define NOMINMAX
#include "SpotTheDifferenceLevel.h"
#include "SpotTheDifferenceMode.h"
#include "Util/Console.h"
#include "UI/UITop.h"
#include "UI/UILoadingBar.h"
#include "UI/UICorrectCount.h"
#include "UI/UIColorEffect.h"
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

enum EResult
{
	fail = 0,
	success,
};

float const PLAYTIME = 30.0f; // TODO: 외부에서 플레이 시간 받 수 있게 수정

void SpotTheDifferenceLevel::Cursor::Init(Vector2 topLeft, Vector2 size)
{
	pos = topLeft;
	this->topLeft = topLeft;
	this->size = size;
}

void SpotTheDifferenceLevel::Cursor::Move(int dx, int dy)
{
	int x = pos.x + dx;
	int y = pos.y + dy;
	x = std::clamp(x, topLeft.x, topLeft.x + size.x - 1);
	y = std::clamp(y, topLeft.y, topLeft.y + size.y - 1);
	pos.x = x;
	pos.y = y;
}

void SpotTheDifferenceLevel::Cursor::Tick(float deltaTime, Input* input)
{
	
	if (input->IsKeyPressed(VK_LEFT))
	{
		Move(-1, 0);
	}
	if (input->IsKeyPressed(VK_RIGHT))
	{
		Move(1, 0);
	}
	if (input->IsKeyPressed(VK_UP))
	{
		Move(0, -1);
	}
	if (input->IsKeyPressed(VK_DOWN))
	{
		Move(0, 1);
	}
}

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

	if (loadingBarUI)
	{
		delete loadingBarUI;
		loadingBarUI = nullptr;
	}

	if (correctCountUI)
	{
		delete correctCountUI;
		correctCountUI = nullptr;
	}

	if (colorEffectUI)
	{
		delete colorEffectUI;
		colorEffectUI = nullptr;
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

	if (!mode)
		mode = new SpotTheDifferenceMode();

	LoadText();
	MakeDifferences(); // 틀린 곳 만들어주고 모드에 전송

	hasBeganPlay = true;

	if (!topUI)
		topUI = new UITop(displaySize.x, Vector2(3, 2), "Spot The Difference");

	if (!loadingBarUI)
		loadingBarUI = new UILoadingBar(Vector2(3, paintSize.y + 6), ((float)displaySize.x / 10.0f) * 9.0f, PLAYTIME, '#');

	if (!correctCountUI)
		correctCountUI = new UICorrectCount(); // TODO: 하드 코딩 수정 필요

	if (!colorEffectUI)
		colorEffectUI = new UIColorEffect(edgeColor, Color::LightRed, 1.0f, 5); // TODO: 하드 코딩 수정 필요

	if (!mode
		|| !topUI
		|| !loadingBarUI
		|| !correctCountUI
		|| !colorEffectUI)
	{
		// 게임 시작 못하는 상황
		RequestChangeLevel((int)LevelType::Menu);
		return;
	}

	timer.SetTargetTime(PLAYTIME);

	// 로딩바 시작
	loadingBarUI->Start();
	correctCountUI->Start(paintSize.y + 6, displaySize.x, 20);
}

void SpotTheDifferenceLevel::OnExit()
{
	Level::OnExit();

	hasBeganPlay = false;

	if (loadingBarUI)
	{
		loadingBarUI->Stop();
		loadingBarUI->Clear();
	}

	if (colorEffectUI)
		colorEffectUI->Stop();

	if (correctCountUI)
		correctCountUI->Clear();

	if (mode)
		mode->Clear();

	timer.Reset();

	Renderer::Get().OffDebugMode();
}

void SpotTheDifferenceLevel::Tick(float deltaTime, Input* input)
{
	timer.Tick(deltaTime);
	if (timer.IsTimeOut())
	{
		// 끝내기
		RequestShowResult(EResult::fail);
		RequestChangeLevel((int)LevelType::GameResult);
		return;
	}

	if (input->IsKeyPressed(VK_ESCAPE))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}

	if (input->IsKeyPressed(VK_SPACE) && mode)
	{
		// 정답 체크
		int idx = GetIndexAtPos(cursor.pos);
		if (mode->Check(idx))
		{
			if(correctCountUI)
				correctCountUI->AddCount(1);
			if (colorEffectUI)
				colorEffectUI->Start();

			// 정답을 다 맞췄을 때
			if (mode->IsGameClear())
			{
				RequestShowResult(EResult::success);
				RequestChangeLevel((int)LevelType::GameResult);
			}
		}
	}

	cursor.Tick(deltaTime, input);
	
	if (loadingBarUI)
	{
		loadingBarUI->Tick(deltaTime);
	}

	if (colorEffectUI)
	{
		colorEffectUI->Tick(deltaTime);
	}
}

void SpotTheDifferenceLevel::Draw()
{
	Level::Draw();

	if (topUI)
		topUI->Draw();

	if (loadingBarUI)
		loadingBarUI->Draw();

	if (correctCountUI)
		correctCountUI->Draw();

	DrawPaint();

	// 커서 위치 배경 바꾸기
	char ch = GetCharAtCursor();
	char cursorChar[2] = { ch, '\0' };

	Renderer::Get().Submit(
		cursorChar,
		cursor.pos,
		Color::Black,
		Color::White,
		999
	);

	// 맞춘 정답들 표시해주기
	if (mode)
	{
		const int totalWidth = paintSize.x * 2 + 6;
		Vector2 paint1StartPos((displaySize.x - totalWidth) / 2, 4);

		for (int idx : mode->GetUserAnswer())
		{
			//Vector2 topLeft(cursor.pos.x - cursor.topLeft.x, cursor.pos.y - cursor.topLeft.y);
			Vector2 pos = GetPosAtIndex(idx, cursor.topLeft);

			char oneCharStr[2] = { paintStr2[idx], '\0' };
			Renderer::Get().Submit(
				oneCharStr,
				pos,
				Color::Green,
				Color::Yellow,
				999
			);

			pos = GetPosAtIndex(idx, paint1StartPos);
			char oneCharStr2[2] = { paintStr[idx], '\0' };
			Renderer::Get().Submit(
				oneCharStr2,
				pos,
				Color::Green,
				Color::Yellow,
				999
			);
		}
	}
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

	std::unordered_set<Vector2> debugSet;
	std::unordered_set<int> answerSet;

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
		debugSet.insert({ x + rightStartX, y + startY });
		answerSet.insert(difference);
	}

	cursor.Init(Vector2(rightStartX, startY), paintSize);
	Renderer::Get().SetDebugMode(debugSet);

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

char SpotTheDifferenceLevel::GetCharAtCursor() const
{
	int localX = cursor.pos.x - cursor.topLeft.x;
	int localY = cursor.pos.y - cursor.topLeft.y;

	if (localY < 0 || localY >= (int)lineLengths.size())
		return ' ';

	int lineLen = lineLengths[localY];
	if (localX < 0 || localX >= lineLen)
		return ' ';

	// paintStr2에서 해당 줄 시작 인덱스 계산
	int idx = 0;
	for (int y = 0; y < localY; y++)
		idx += lineLengths[y] + 1; // +1 : '\n'

	idx += localX;
	return paintStr2[idx];
}

int SpotTheDifferenceLevel::GetIndexAtPos(Vector2 pos) const
{
	int localX = pos.x - cursor.topLeft.x;
	int localY = pos.y - cursor.topLeft.y;

	if (localY < 0 || localY >= (int)lineLengths.size())
		return -1;

	if (localX < 0 || localX >= lineLengths[localY])
		return -1;

	int idx = 0;
	for (int i = 0; i < localY; i++)
		idx += lineLengths[i] + 1;

	return idx + localX;
}

Vector2 SpotTheDifferenceLevel::GetPosAtIndex(int idx, Vector2 topLeft) const
{
	int x = 0;
	int y = 0;

	for (int i = 0; i < idx; i++)
	{
		if (paintStr[i] == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			x++;
		}
	}

	return Vector2(
		topLeft.x + x,
		topLeft.y + y
	);
}