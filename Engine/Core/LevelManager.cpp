#include "LevelManager.h"

namespace MinigameEngine
{
	LevelManager::LevelManager()
	{
	}

	LevelManager::~LevelManager()
	{
	}

	void LevelManager::AddLevel(int id, std::unique_ptr<Level> state)
	{
		levels.emplace(id, std::move(state));
	}

	void LevelManager::SetLevel(int id)
	{
		if (current)
			current->OnExit();

		current = levels[id].get();
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