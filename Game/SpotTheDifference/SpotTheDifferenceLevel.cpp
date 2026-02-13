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
#include "Common/Common.h"
#include "World/Actor.h"
#include "Render/Renderer.h"
#include "Util/Random.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Util/Delete.h>

const int UI_BOTTOM_OFFSET_Y = 3;
const float PLAYTIME = 60.0f; // TODO: 외부에서 플레이 시간 받 수 있게 수정
const float BLANK_TIME = 1.0f;
const int BLANK_COUNT = 5;
const int DRAW_START_Y = 4;
const int GAP_BETWEEN_PAINT = 6;
const int PAINT_SIZE = 4;
const int PAINT_WIDTH = 65;
const int PAINT_HEIGHT = 41;

void SpotTheDifferenceLevel::Cursor::Init(Vector2 topLeft)
{
	pos = topLeft;
	this->topLeft = topLeft;
}

void SpotTheDifferenceLevel::Cursor::Move(int dx, int dy)
{
	int x = pos.x + dx;
	int y = pos.y + dy;
	x = std::clamp(x, topLeft.x, topLeft.x + PAINT_WIDTH - 1);
	y = std::clamp(y, topLeft.y, topLeft.y + PAINT_HEIGHT - 1);
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
	SafeDelete(topUI);
	SafeDelete(loadingBarUI);
	SafeDelete(correctCountUI);
	SafeDelete(colorEffectUI);
	SafeDelete(mode);
}

void SpotTheDifferenceLevel::BeginPlay()
{
	if (hasBeganPlay)
		return;
	hasBeganPlay = true;

	if (!mode)
		mode = new SpotTheDifferenceMode();

	if(paints.empty())
		LoadText();

	int showingPaint = Random::Random(0, PAINT_SIZE - 1);
	paintStr = &paints[showingPaint].first;
	paintStr2 = &paints[showingPaint].second;
	*paintStr2 = *paintStr;

	totalWidth = PAINT_WIDTH * 2 + GAP_BETWEEN_PAINT;
	startXPos = (displaySize.x - totalWidth) / 2;
	rightStartXPos = startXPos + PAINT_WIDTH + GAP_BETWEEN_PAINT;

	MakeDifferences();

	int uiBottomY = displaySize.y - UI_BOTTOM_OFFSET_Y;

	if (!topUI)
		topUI = new UITop(displaySize.x, Vector2(UI_START_POS_X, UI_START_POS_Y), "Spot The Difference");

	if (!loadingBarUI)
		loadingBarUI = new UILoadingBar(Vector2(UI_START_POS_X, uiBottomY), ((float)displaySize.x / 10.0f) * 9.0f, PLAYTIME, '#');

	if (!correctCountUI)
		correctCountUI = new UICorrectCount();

	if (!colorEffectUI)
		colorEffectUI = new UIColorEffect(edgeColor, Color::LightRed, BLANK_TIME, BLANK_COUNT);

	if (!ValidCheck())
		return;

	timer.SetTargetTime(PLAYTIME);

	// 로딩바 시작
	loadingBarUI->Start();
	correctCountUI->Start(uiBottomY, displaySize.x, answerCount);
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
	if (!ValidCheck())
		return;

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

	if (input->IsKeyPressed(VK_SPACE))
	{
		// 정답 체크
		int idx = GetIndexAtPos(cursor.pos);
		if (mode->Check(idx))
		{
			correctCountUI->AddCount();
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
	
	loadingBarUI->Tick(deltaTime);
	colorEffectUI->Tick(deltaTime);
}

void SpotTheDifferenceLevel::Draw()
{
	Level::Draw();
	if (!ValidCheck())
		return;

	topUI->Draw();
	loadingBarUI->Draw();
	correctCountUI->Draw();

	DrawPaint();

	// 커서 위치 배경 바꾸기
	int cursorIdx = GetIndexAtPos(cursor.pos);
	if (cursorIdx != -1)
	{
		char ch = (*paintStr2)[GetIndexAtPos(cursor.pos)];
		char cursorChar[2] = { ch, '\0' };

		Renderer::Get().Submit(
			cursorChar,
			cursor.pos,
			Color::Black,
			Color::White,
			999
		);
	}

	Vector2 paint1StartPos(startXPos, DRAW_START_Y);

	for (int idx : mode->GetUserAnswer())
	{
		Vector2 pos = GetPosAtIndex(idx, cursor.topLeft);

		char oneCharStr[2] = { (*paintStr2)[idx], '\0' };
		Renderer::Get().Submit(
			oneCharStr,
			pos,
			Color::Green,
			Color::Yellow,
			999
		);

		pos = GetPosAtIndex(idx, paint1StartPos);
		char oneCharStr2[2] = { (*paintStr2)[idx], '\0' };
		Renderer::Get().Submit(
			oneCharStr2,
			pos,
			Color::Green,
			Color::Yellow,
			999
		);
	}
}

void SpotTheDifferenceLevel::LoadText()
{
	for(int i = 0; i < PAINT_SIZE; i++)
	{
		std::string filePath = "../Assets/SpotTheDifference/" + std::to_string(i) + ".txt";
		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "Fail to open Path: " << filePath << std::endl;
			__debugbreak();
			return;
		}

		std::string tempStr;
		std::string line;

		while (std::getline(file, line))
		{
			line.erase(
				std::remove(line.begin(), line.end(), '\r'),
				line.end()
			);

			if (line.size() > PAINT_WIDTH)
			{
				line = line.substr(0, PAINT_WIDTH);
			}

			else if (line.size() < PAINT_WIDTH)
			{
				line.append(PAINT_WIDTH - line.size(), ' ');
			}

			tempStr += line;
			tempStr += '\n';
		}

		std::pair<std::string, std::string> paintPair = { tempStr, tempStr };
		paints.push_back(paintPair);
	}
}

void SpotTheDifferenceLevel::MakeDifferences()
{
	int paintLen = (int)(*paintStr2).size();
	std::unordered_set<int> differencesSet;

	// 차이 인덱스 뽑기 (줄바꿈 인덱스는 제외)
	while ((int)differencesSet.size() < answerCount)
	{
		int random = Random::Random(0, paintLen - 1);

		if ((*paintStr2)[random] == '\n')
			continue;

		differencesSet.insert(random);
	}

	std::unordered_set<Vector2> debugSet;
	std::unordered_set<int> answerSet;

	for (int difference : differencesSet)
	{
		int random = Random::Random(33, 126);
		while ((*paintStr2)[difference] == (char)random)
			random = Random::Random(33, 126);

		(*paintStr2)[difference] = char(random);

		int x = 0, y = 0;
		for (int i = 0; i < difference; i++)
		{
			if ((*paintStr2)[i] == '\n')
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
		debugSet.insert({ x + rightStartXPos, y + DRAW_START_Y });
		answerSet.insert(difference);
	}

	cursor.Init(Vector2(rightStartXPos, DRAW_START_Y));
	//디버그 보고 싶으면 주석 해제
	//Renderer::Get().SetDebugMode(debugSet);

	if (mode)
		mode->SetAnswer(answerSet);
}

void SpotTheDifferenceLevel::DrawPaint()
{
	// 왼쪽 그림
	Renderer::Get().SubmitMultiLine(
		(*paintStr).c_str(),
		Vector2(startXPos, DRAW_START_Y),
		Color::Green
	);

	// 오른쪽 그림
	Renderer::Get().SubmitMultiLine(
		(*paintStr2).c_str(),
		Vector2(startXPos + PAINT_WIDTH + GAP_BETWEEN_PAINT, DRAW_START_Y),
		Color::Green
	);

	Console::SetConsoleTextColor(Color::White);
}

int SpotTheDifferenceLevel::GetIndexAtPos(Vector2 pos) const
{
	int localX = pos.x - cursor.topLeft.x;
	int localY = pos.y - cursor.topLeft.y;

	if (localX < 0 || localX >= PAINT_WIDTH)
		return -1;

	if (localY < 0 || localY >= PAINT_HEIGHT)
		return -1;

	// 한 줄 = 65 + '\n'
	return localY * (PAINT_WIDTH + 1) + localX;
}

Vector2 SpotTheDifferenceLevel::GetPosAtIndex(int idx, Vector2 topLeft) const
{
	int lineWidth = PAINT_WIDTH + 1;

	int y = idx / lineWidth;
	int x = idx % lineWidth;

	return Vector2(topLeft.x + x, topLeft.y + y);
}

bool SpotTheDifferenceLevel::ValidCheck()
{
	if (!mode
		|| !topUI
		|| !loadingBarUI
		|| !correctCountUI
		|| !colorEffectUI
		|| paints.empty()
		|| !paintStr
		|| !paintStr2)
	{
		// 게임 시작 못하는 상황
		RequestChangeLevel((int)LevelType::Menu);
		return false;
	}

	return true;
}
