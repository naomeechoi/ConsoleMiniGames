#include "LevelManager.h"
#include "Math/Vector2.h"

namespace MinigameEngine
{
	LevelManager::LevelManager(Vector2 windowSize)
		:windowSize(windowSize)
	{
	}

	LevelManager::~LevelManager()
	{
	}

	void LevelManager::AddLevel(int id, std::unique_ptr<Level> state)
	{
		levels.emplace(id, std::move(state));

		if(levels[id])
			levels[id]->SetLevelDisplaySize(windowSize);
	}

	void LevelManager::SetLevel(int id)
	{
		if (current)
			current->OnExit();

		current = levels[id].get();
	}

	void LevelManager::BeginPlay()
	{
		if (!current)
			return;

		current->BeginPlay();
	}

	void LevelManager::Tick(float deltaTime, Input* input)
	{
		if (!current)
			return;
		
		current->Tick(deltaTime, input);
	}

	void LevelManager::Draw()
	{
		if (!current)
			return;

		current->Draw();
	}

	void LevelManager::ProcessAddAndDestroyActorsOnLevel()
	{
		if (!current)
			return;

		current->ProcessAddAndDestroyActors();
	}

	void LevelManager::ChangeLevel()
	{
		if (!current)
			return;

		if (auto nextLevel = current->ConsumeRequestedLevel())
		{
			current->OnExit();
			SetLevel(*nextLevel);
		}
	}
}