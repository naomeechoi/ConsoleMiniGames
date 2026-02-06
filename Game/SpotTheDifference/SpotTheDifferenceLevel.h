#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"

#include <string>
#include <algorithm>

using namespace MinigameEngine;
class SpotTheDifferenceMode;
class UITop;
class UILoadingBar;
class UICorrectCount;
class UIColorEffect;

class SpotTheDifferenceLevel : public Level
{
	struct Cursor
	{
		Vector2 pos;
		Vector2 topLeft;
		Vector2 size;

		void Init(Vector2 topLeft, Vector2 size);
		void Move(int dx, int dy);
		void Tick(float deltaTime, Input* input);
	};
public:
	SpotTheDifferenceLevel();
	~SpotTheDifferenceLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

private:
	void LoadText();
	void MakeDifferences();
	void DrawPaint();
	char GetCharAtCursor() const;
	int GetIndexAtPos(Vector2 pos) const;
	Vector2 GetPosAtIndex(int idx, Vector2 topLeft) const;

private:
	bool hasBeganPlay = false;

	SpotTheDifferenceMode* mode = nullptr;
	
	// UI
	UITop* topUI = nullptr;
	UILoadingBar* loadingBarUI = nullptr;
	UICorrectCount* correctCountUI = nullptr;
	UIColorEffect* colorEffectUI = nullptr;

	// Paint
	Vector2 paintSize;
	std::string paintStr;
	std::string paintStr2;
	std::vector<int> lineLengths;

private:
	Cursor cursor;
	Timer timer;
};