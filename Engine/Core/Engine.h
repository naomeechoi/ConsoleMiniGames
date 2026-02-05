#pragma once
#include "Common/Common.h"
#include "LevelManager.h"
#include "System/Input.h"

#include <memory>
#include <string>

namespace MinigameEngine
{
	class LevelManger;
	class LayoutCalculator;
	class Renderer;

	class NAOMI_API Engine
	{
		struct EngineSetting
		{
			// 프레임 속도.
			float framerate = 0.0f;
			int width = 0;
			int height = 0;
		};

	public:
		Engine();
		~Engine();
		void Run();
		LevelManager* GetLevelManager() const { return levelManager.get(); };
		void Quit() { isQuit = true; };

	protected:
		Renderer* renderer = nullptr;
		std::unique_ptr<LevelManager> levelManager;
		static Engine* instance;
		bool isQuit = false;

	private:
		EngineSetting setting;
		Input input;

		std::string edgeStr;

	private:
		void LoadSetting();
		std::string trim(const std::string& str);
		void BeginPlay();
		void Tick(float deltaTime);
		void Draw();
		void ProcessActorsOnLevel();
		void ChangeLevel();

	private:
		void LoadEdgeTxt();
		void DrawEdge();

	public:
		LayoutCalculator* layoutCalculator;
	};
}

