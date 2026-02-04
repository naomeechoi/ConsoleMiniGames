#pragma once
#include "Common/Common.h"
#include "World/Level.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace MinigameEngine
{
	class Input;

	class NAOMI_API LevelManager
	{
	public:
		LevelManager();
		~LevelManager();

		// 복사 생성자/복사 대입 금지
		LevelManager(const LevelManager&) = delete;
		LevelManager& operator=(const LevelManager&) = delete;

		// 이동 생성자/이동 대입 가능
		LevelManager(LevelManager&&) = default;
		LevelManager& operator=(LevelManager&&) = default;

	public:
		void AddLevel(int id, std::unique_ptr<Level> state);
		void SetLevel(int id);
		Level* GetGameState() const { return current; };
		void Tick(float deltaTime, Input* input);
		void Draw();
		void ProcessAddAndDestroyActorsOnLevel();
		void ChangeLevel();

	private:
		std::unordered_map<int, std::unique_ptr<Level>> levels;
		Level* current = nullptr;
	};
}
