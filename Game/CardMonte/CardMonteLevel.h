#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"

#include <string>
#include <vector>

class UITop;
class UILoadingBar;
class UIMessage;
class UIColorEffect;
class CardMonteMode;
class CardMonteLevel : public MinigameEngine::Level
{
	using StateFunc = void (CardMonteLevel::*)(float);
	struct SCard
	{
		int num = -1;
		MinigameEngine::Vector2 pos;
		MinigameEngine::Vector2 originPos;
	};

	struct SShufflePair
	{
		int a = -1;
		int b = -1;
	};

public:
	CardMonteLevel();
	~CardMonteLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, MinigameEngine::Input* input) override;
	virtual void Draw() override;

private:
	void LoadSetting();
	void CardSetting();
	void SetAnswer();
	void Clear();
	bool ValidCheck();

private:
	CardMonteMode* mode = nullptr;
	// UI
	UITop* topUI = nullptr;
	UILoadingBar* loadingBarUI = nullptr;
	UIColorEffect* colorEffectUI = nullptr;
	UIMessage* messageUI = nullptr;

private:
	bool hasBeganPlay = false;

	int cardCount = 5;
	float playTime = 60;
	float suffleTime = 10;
	float suffleCount = 10;
	float cardFilpTime = 2;
	int cardWidth = 15;
	int cardHeight = 11;
	int cardMidIdx = 0;
	float showingTime = 2;
	std::string message;

	std::vector<std::string> cardSprites;
	std::vector<SCard> cards;

private:
	void ChangeState(StateFunc next, float duration);
	void StateShowing(float deltatime);
	void StateFilp(float deltatime);
	void StateShuffle(float deltatime);
	void StateChoose(float deltatime);
	void StateGameOver(float deltatime);
	void StateWaitToExit(float deltatime);


private:
	void SetShufflePairs();
	MinigameEngine::Vector2 GetCenter(const MinigameEngine::Vector2& card);
	MinigameEngine::Vector2 CenterToTopLeft(const MinigameEngine::Vector2& center);
	MinigameEngine::Vector2 CircularLerp(
		const MinigameEngine::Vector2& start,
		const MinigameEngine::Vector2& end,
		float t, bool topArc = true);
	void FlipCard(bool isOpen);
	std::vector<int> GetCurCardsOrder();
	void HandleChooseInput(MinigameEngine::Input* input);

private:
	StateFunc curState = nullptr;
	MinigameEngine::Timer stateTimer;

	int spriteIdx = 0;
	int currentShuffleIdx = 0;
	int selectedIdx = -1;
	bool isSuccess = false;

	std::vector<SShufflePair> shufflePairs;
};