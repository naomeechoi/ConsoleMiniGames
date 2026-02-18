#include "Engine.h"
#include "LevelManager.h"
#include "Util/Console.h"
#include "Render/Renderer.h"
#include "Util/Random.h"
#include "Math/Vector2.h"
#include "System/FileIO.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

namespace MinigameEngine
{
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		instance = this;
		LoadSetting();
		edgeStr = FileIO::ReadFile("../Assets/Edge.txt");

		renderer = new Renderer(Vector2(setting.width, setting.height));

		levelManager = std::make_unique<LevelManager>(Vector2(setting.width -1, setting.height - 1));
		Console::SetCursorVisible(false);
		Random::SetRandomSeed();
	}

	Engine::~Engine()
	{
		if (renderer)
		{
			delete renderer;
			renderer = nullptr;
		}
	}

	void Engine::Run()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		int64_t currentTime = 0;
		int64_t previousTime = 0;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		currentTime = time.QuadPart;
		previousTime = currentTime;

		setting.framerate
			= setting.framerate == 0.0f ? 60.0f : setting.framerate;
		float oneFrameTime = 1.0f / setting.framerate;

		while (!isQuit)
		{
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			float deltaTime
				= static_cast<float>(currentTime - previousTime);

			deltaTime = deltaTime
				/ static_cast<float>(frequency.QuadPart);

			if (deltaTime >= oneFrameTime)
			{
				input.Update();

				BeginPlay();
				Tick(deltaTime);
				Draw();
				previousTime = currentTime;

				ChangeLevel();
			}
		}
	}

	void Engine::BeginPlay()
	{
		if (levelManager)
		{
			levelManager->BeginPlay();
		}
	}

	void Engine::Tick(float deltaTime)
	{
		if (levelManager)
		{
			levelManager->Tick(deltaTime, &input);
		}
	}

	void Engine::Draw()
	{
		DrawEdge();
		if (levelManager && !isQuit)
		{
			levelManager->Draw();
		}
		renderer->Draw();
	}

	void Engine::ChangeLevel()
	{
		if (!levelManager)
			return;

		levelManager->ChangeLevel();
	}

	void Engine::LoadSetting()
	{
		std::ifstream file("../Config/Setting.txt");
		if (!file.is_open())
		{
			std::cout << "Fail to open engine setting file." << std::endl;
			__debugbreak();
			return;
		}

		std::string line;
		while (std::getline(file, line))
		{
			line = trim(line);

			size_t equalPos = line.find('=');
			if (equalPos != std::string::npos)
			{
				std::string key = line.substr(0, equalPos);
				std::string value = line.substr(equalPos + 1);

				key = trim(key);
				value = trim(value);

				if (key == "framerate")
					sscanf_s(value.c_str(), "%f", &setting.framerate);
				else if (key == "width")
					sscanf_s(value.c_str(), "%d", &setting.width);
				else if (key == "height")
					sscanf_s(value.c_str(), "%d", &setting.height);
			}
		}

		file.close();
	}

	std::string Engine::trim(const std::string& str)
	{
		size_t start = str.find_first_not_of(" \t");
		size_t end = str.find_last_not_of(" \t");

		return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
	}

	void Engine::DrawEdge()
	{
		Color edgeColor = levelManager->GetLevelEdgeColor();
		Renderer::Get().Submit(
			edgeStr.c_str(),
			Vector2(0, 0),
			edgeColor
		);
	}
}
