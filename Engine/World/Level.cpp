#include "Level.h"
#include "System/Input.h"
#include "Core/Engine.h"

namespace MinigameEngine
{
	Level::Level()
	{
	}

	Level::~Level()
	{
	}

	void Level::OnExit()
	{
		result = -1;
	}

	void Level::BeginPlay()
	{
	}

	void Level::Tick(float deltaTime, Input* input)
	{
	}

	void Level::Draw()
	{
	}

	void Level::RequestChangeLevel(int levelID)
	{
		requestedLevel = levelID;
	}

	void Level::RequestShowResult(int result)
	{
		requestedShowResult = result;
	}

	std::optional<int> Level::ConsumeRequestedLevel()
	{
		auto temp = requestedLevel;
		requestedLevel.reset();
		return temp;
	}

	std::optional<int> Level::ConsumeRequestedShowResult()
	{
		auto temp = requestedShowResult;
		requestedShowResult.reset();
		return temp;
	}

	void Level::SetLevelDisplaySize(Vector2 displaySize)
	{
		this->displaySize = displaySize;
	}

	Vector2 Level::GetLevelDisplaySize()
	{
		return displaySize;
	}

	Color Level::GetEdgeColor()
	{
		return edgeColor;
	}

	void Level::SetResult(int result)
	{
		this->result = result;
	}
}