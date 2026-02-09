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
#include <Util/Delete.h>

const int UI_START_POS_X = 3;
const int UI_START_POS_Y = 2;
const int UI_BOTTOM_OFFSET_Y = 3;
const float PLAYTIME = 30.0f; // TODO: 외부에서 플레이 시간 받 수 있게 수정
const float BLANK_TIME = 1.0f;
const int BLANK_COUNT = 5;
const int DRAW_START_Y = 4;
const int GAP_BETWEEN_PAINT = 6;
const int PAINT_SIZE = 2;

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

	if (!mode)
		mode = new SpotTheDifferenceMode();

	if(paints.empty())
		LoadText();

	int showingPaint = Random::Random(0, PAINT_SIZE - 1);
	paintStr = &paints[showingPaint].first;
	paintStr2 = &paints[showingPaint].second;
	MakeDifferences();

	hasBeganPlay = true;

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
	char ch = (*paintStr2)[GetIndexAtPos(cursor.pos)];
	char cursorChar[2] = { ch, '\0' };

	Renderer::Get().Submit(
		cursorChar,
		cursor.pos,
		Color::Black,
		Color::White,
		999
	);

	const int totalWidth = paintSize.x * 2 + GAP_BETWEEN_PAINT;
	Vector2 paint1StartPos((displaySize.x - totalWidth) / 2, DRAW_START_Y);

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
		tempStr.assign((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		// CR 제거
		tempStr.erase(
			std::remove(tempStr.begin(), tempStr.end(), '\r'),
			tempStr.end()
		);

		// paintSize 계산
		paintSize = Vector2(0, 0);
		lineLengths.clear();

		std::string line;
		std::istringstream iss(tempStr);
		while (std::getline(iss, line))
		{
			paintSize.x = std::max(paintSize.x, (int)line.size());
			++paintSize.y;

			lineLengths.push_back((int)line.size());
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

	const int totalWidth = paintSize.x * 2 + GAP_BETWEEN_PAINT;
	const int startX = (displaySize.x - totalWidth) / 2;
	const int rightStartX = startX + paintSize.x + GAP_BETWEEN_PAINT;

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
		debugSet.insert({ x + rightStartX, y + DRAW_START_Y });
		answerSet.insert(difference);
	}

	cursor.Init(Vector2(rightStartX, DRAW_START_Y), paintSize);
	Renderer::Get().SetDebugMode(debugSet);

	if (mode)
		mode->SetAnswer(answerSet);
}

void SpotTheDifferenceLevel::DrawPaint()
{
	const int totalWidth = paintSize.x * 2 + GAP_BETWEEN_PAINT;

	const int startX = (displaySize.x - totalWidth) / 2;

	// 왼쪽 그림
	Renderer::Get().SubmitMultiLine(
		(*paintStr).c_str(),
		Vector2(startX, DRAW_START_Y),
		Color::Green
	);

	// 오른쪽 그림
	Renderer::Get().SubmitMultiLine(
		(*paintStr2).c_str(),
		Vector2(startX + paintSize.x + GAP_BETWEEN_PAINT, DRAW_START_Y),
		Color::Green
	);

	Console::SetConsoleTextColor(Color::White);
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

	return Vector2(
		topLeft.x + x,
		topLeft.y + y
	);
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
