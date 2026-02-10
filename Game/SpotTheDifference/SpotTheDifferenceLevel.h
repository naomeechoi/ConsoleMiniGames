#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"
#include <string>

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

		void Init(Vector2 topLeft);
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
	int GetIndexAtPos(Vector2 pos) const;
	Vector2 GetPosAtIndex(int idx, Vector2 topLeft) const;
	bool ValidCheck();

private:
	bool hasBeganPlay = false;

	SpotTheDifferenceMode* mode = nullptr;
	
	// UI
	UITop* topUI = nullptr;
	UILoadingBar* loadingBarUI = nullptr;
	UICorrectCount* correctCountUI = nullptr;
	UIColorEffect* colorEffectUI = nullptr;

	// Paint
	std::vector<std::pair<std::string, std::string>> paints;
	std::string* paintStr = nullptr;
	std::string* paintStr2 = nullptr;

private:
	Cursor cursor;
	Timer timer;
	int answerCount = 20;
	int totalWidth = 0;
	int startXPos = 0;
	int rightStartXPos = 0;
};