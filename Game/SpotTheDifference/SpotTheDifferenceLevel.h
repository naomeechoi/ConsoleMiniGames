#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"
#include <vector>
#include <string>

class SpotTheDifferenceMode;
class UITop;
class UILoadingBar;
class UICorrectCount;
class UIColorEffect;

class SpotTheDifferenceLevel : public MinigameEngine::Level
{
	struct Cursor
	{
		MinigameEngine::Vector2 pos;
		MinigameEngine::Vector2 topLeft;

		void Init(MinigameEngine::Vector2 topLeft);
		void Move(int dx, int dy);
		void Tick(float deltaTime, MinigameEngine::Input* input);
	};
public:
	~SpotTheDifferenceLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, MinigameEngine::Input* input) override;
	virtual void Draw() override;

private:
	void LoadPaint();
	void MakeDifferences();
	void DrawPaint();
	int GetIndexAtPos(MinigameEngine::Vector2 pos) const;
	MinigameEngine::Vector2 GetPosAtIndex(int idx, MinigameEngine::Vector2 topLeft) const;
	bool ValidCheck();
	void ShowGameResult(bool isSuccess);

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
	int currentPaintIdx = 0;

private:
	Cursor cursor;
	MinigameEngine::Timer timer;
	int leftPaintStartPosX = 0;
	int rightPaintStartPosX = 0;
};