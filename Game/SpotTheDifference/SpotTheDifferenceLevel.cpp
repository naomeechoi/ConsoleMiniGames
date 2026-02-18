#include "SpotTheDifferenceLevel.h"
#include "SpotTheDifferenceMode.h"
#include "UI/UITop.h"
#include "UI/UILoadingBar.h"
#include "UI/UICorrectCount.h"
#include "UI/UIColorEffect.h"
#include "Math/Vector2.h"
#include "System/Input.h"
#include "System/FileIO.h"
#include "Common/GameCommon.h"
#include "World/Actor.h"
#include "Render/Renderer.h"
#include "Util/Console.h"
#include "Util/Random.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Util/Delete.h>

using namespace MinigameEngine;
using std::string;
using std::pair;
using std::to_string;
using std::unordered_set;
using std::clamp;

const int UI_BOTTOM_OFFSET_Y = 3;
const float PLAYTIME = 120.0f;
const float BLANK_TIME = 1.0f;
const int BLANK_COUNT = 5;
const int DRAW_START_Y = 4;
const int GAP_BETWEEN_PAINT = 6;
const int PAINT_SIZE = 4;
const int PAINT_WIDTH = 65;
const int PAINT_HEIGHT = 41;
const int ANSWER_SIZE = 20;

void SpotTheDifferenceLevel::Cursor::Init(Vector2 topLeft)
{
	pos = topLeft;
	this->topLeft = topLeft;
}

void SpotTheDifferenceLevel::Cursor::Move(int dx, int dy)
{
	int x = pos.x + dx;
	int y = pos.y + dy;
	x = clamp(x, topLeft.x, topLeft.x + PAINT_WIDTH - 1);
	y = clamp(y, topLeft.y, topLeft.y + PAINT_HEIGHT - 1);
	pos.x = x;
	pos.y = y;
}

void SpotTheDifferenceLevel::Cursor::Tick(float deltaTime, Input* input)
{
	if (input->IsKeyPressed(VK_LEFT))
		Move(-1, 0);
	else if (input->IsKeyPressed(VK_RIGHT))
		Move(1, 0);
	else if (input->IsKeyPressed(VK_UP))
		Move(0, -1);
	else if (input->IsKeyPressed(VK_DOWN))
		Move(0, 1);
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
		LoadPaint();

	currentPaintIdx = Random::Random(0, PAINT_SIZE - 1);

	const int TOTAL_WIDTH = PAINT_WIDTH * 2 + GAP_BETWEEN_PAINT;
	leftPaintStartPosX = (displaySize.x - TOTAL_WIDTH) / 2;
	rightPaintStartPosX = leftPaintStartPosX + PAINT_WIDTH + GAP_BETWEEN_PAINT;

	if (!topUI)
		topUI = new UITop(
			displaySize.x,
			Vector2(UI_START_POS_X, UI_START_POS_Y),
			"Spot The Difference");

	const int UI_BUTTOM_Y = displaySize.y - UI_BOTTOM_OFFSET_Y;
	if (!loadingBarUI)
		loadingBarUI = new UILoadingBar(
			Vector2(UI_START_POS_X, UI_BUTTOM_Y),
			((float)displaySize.x / 10.0f) * 9.0f,
			PLAYTIME, '#');

	if (!correctCountUI)
		correctCountUI = new UICorrectCount();

	if (!colorEffectUI)
		colorEffectUI = new UIColorEffect(edgeColor, Color::LightRed, BLANK_TIME, BLANK_COUNT);

	if (!ValidCheck())
	{
		return;
	}

	MakeDifferences();
	timer.SetTargetTime(PLAYTIME);
	loadingBarUI->Start();
	correctCountUI->Start(UI_BUTTOM_Y, displaySize.x, ANSWER_SIZE);
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

	paints[currentPaintIdx].second = paints[currentPaintIdx].first;
	currentPaintIdx = 0;

	timer.Reset();
}

void SpotTheDifferenceLevel::Tick(float deltaTime, Input* input)
{
	if (!ValidCheck())
		return;

	timer.Tick(deltaTime);
	if (timer.IsTimeOut())
	{
		ShowGameResult(false);
		return;
	}

	if (input->IsKeyPressed(VK_ESCAPE))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}

	if (input->IsKeyPressed(VK_SPACE))
	{
		int idx = GetIndexAtPos(cursor.pos);
		if (mode->Check(idx))
		{
			correctCountUI->AddCount();
			colorEffectUI->Start();

			if (mode->IsGameClear())
			{
				ShowGameResult(true);
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

	string& paint2 = paints[currentPaintIdx].second;
	
	auto getCharAt = [&](int idx) -> char
	{
		return paint2[idx];
	};

	auto submitChar = [&](char ch, Vector2 pos, Color fg, Color bg = Color::Black, int z = 999)
	{
		char str[2] = { ch, '\0' };
		Renderer::Get().Submit(str, pos, fg, bg, z);
	};

	// user answer
	Vector2 paint1StartPos(leftPaintStartPosX, DRAW_START_Y);
	for (int idx : mode->GetUserAnswer())
	{
		Vector2 posLeft = GetPosAtIndex(idx, paint1StartPos);
		Vector2 posRight = GetPosAtIndex(idx, cursor.topLeft);

		char ch = getCharAt(idx);

		submitChar(ch, posLeft, Color::Green, Color::Yellow);
		submitChar(ch, posRight, Color::Green, Color::Yellow);
	}

	// cursor color
	int cursorIdx = GetIndexAtPos(cursor.pos);
	if (cursorIdx != -1)
	{
		submitChar(getCharAt(cursorIdx), cursor.pos, Color::Black, Color::White);
	}
}

void SpotTheDifferenceLevel::LoadPaint()
{
	for(int i = 0; i < PAINT_SIZE; i++)
	{
		string filePath = "../Assets/SpotTheDifference/" + to_string(i) + ".txt";
		
		string tempStr = FileIO::ReadFixedWidthText(filePath, (size_t)PAINT_WIDTH);
		pair<string, string> paintPair = { tempStr, tempStr };
		paints.push_back(paintPair);
	}
}

void SpotTheDifferenceLevel::MakeDifferences()
{
	string& paint2 = paints[currentPaintIdx].second;
	int paintLen = (int)paint2.size();
	unordered_set<int> differencesSet;

	auto isPrintable = [](char c) { return c != '\n'; };
	const int lineWidth = PAINT_WIDTH + 1;

	while ((int)differencesSet.size() < ANSWER_SIZE)
	{
		int idx = Random::Random(0, paintLen - 1);
		if (!isPrintable(paint2[idx])) continue;
		differencesSet.insert(idx);
	}

	unordered_set<int> answerSet;

	auto indexToPos = [lineWidth](int idx) -> Vector2
	{
		int x = idx % lineWidth;
		int y = idx / lineWidth;
		return { x, y };
	};

	auto randomChar = []() { return static_cast<char>(Random::Random(33, 126)); };
	for (int idx : differencesSet)
	{
		char newChar = randomChar();
		while (paint2[idx] == newChar)
			newChar = randomChar();
		paint2[idx] = newChar;

		Vector2 pos = indexToPos(idx);
		pos.x += rightPaintStartPosX;
		pos.y += DRAW_START_Y;

		answerSet.insert(idx);
	}

	cursor.Init(Vector2(rightPaintStartPosX, DRAW_START_Y));

	if (mode)
		mode->SetAnswer(answerSet);
}

void SpotTheDifferenceLevel::DrawPaint()
{
	// left paint
	Renderer::Get().Submit(
		paints[currentPaintIdx].first.c_str(),
		Vector2(leftPaintStartPosX, DRAW_START_Y),
		Color::Green
	);

	// right paint
	Renderer::Get().Submit(
		paints[currentPaintIdx].second.c_str(),
		Vector2(rightPaintStartPosX, DRAW_START_Y),
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
	const int pointSize = 5;
	void* checks[pointSize] = { mode, topUI, loadingBarUI, correctCountUI, colorEffectUI };
	for (int i = 0; i < pointSize; i++)
	{
		if (checks[i] == nullptr)
		{
			RequestChangeLevel((int)LevelType::Menu);
			return false;
		}
	}

	if (paints.empty())
	{
		RequestChangeLevel((int)LevelType::Menu);
		return false;
	}

	return true;
}

void SpotTheDifferenceLevel::ShowGameResult(bool isSuccess)
{
	if(isSuccess)
		RequestShowResult(EResult::success);
	else
		RequestShowResult(EResult::fail);
	RequestChangeLevel((int)LevelType::GameResult);
}
