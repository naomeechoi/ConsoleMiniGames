#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"

#include <string>
#include <vector>

using namespace MinigameEngine;

class UITop;
class CardMonteLevel : public Level
{
	using StateFunc = void (CardMonteLevel::*)(float);
	struct Card
	{
		int num = -1;
		Vector2 pos;
		Vector2 originPos;
	};

	struct ShufflePair
	{
		int a = -1;
		int b = -1;
	};

public:
	CardMonteLevel();
	~CardMonteLevel();

	virtual void BeginPlay() override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime, Input* input) override;
	virtual void Draw() override;

private:
	void LoadSetting();
	void CardSetting();
	void Clear();

private:
	Vector2 GetCenter(const Vector2& card);
	Vector2 CenterToTopLeft(const Vector2& center);

private:
	// UI
	UITop* topUI = nullptr;

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

	std::vector<std::string> cardSprites;
	std::vector<Card> cards;

private:
	void ChangeState(StateFunc next, float duration);
	void StateShowing(float deltatime);
	void StateFilp(float deltatime);
	void StateShuffle(float deltatime);
	void StateChoose(float deltatime);
	void StateGameOver(float deltatime);


private:
	void SetShufflePairs();
	Vector2 CircularLerp(const Vector2& start, const Vector2& end, float t, bool topArc = true);

private:
	StateFunc curState = nullptr;
	Timer stateTimer;

private:
	int spriteIdx = 0;
	int currentShuffleIdx = 0;


private:
	std::vector<ShufflePair> shufflePairs;
};