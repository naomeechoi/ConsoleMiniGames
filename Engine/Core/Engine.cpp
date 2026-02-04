#include "Engine.h"
#include "LevelManager.h"
#include "Util/Console.h"
#include "Render/Renderer.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include "Util/LayoutCalculator.h"

namespace MinigameEngine
{
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		instance = this;
		LoadSetting();
		renderer = new Renderer(Vector2(setting.width, setting.height));

		levelManager = std::make_unique<LevelManager>();
		// 커서 끄기
		Console::SetCursorVisible(false);

		layoutCalculator = new LayoutCalculator();
	}

	Engine::~Engine()
	{
		delete layoutCalculator;
		layoutCalculator = nullptr;

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

		// 프레임 계산용 변수.
		int64_t currentTime = 0;
		int64_t previousTime = 0;

		// 하드웨어 타이머로 시간 구하기.
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		// 엔진 시작 직전에는 두 시간 값을 같게 맞춤.
		currentTime = time.QuadPart;
		previousTime = currentTime;

		setting.framerate
			= setting.framerate == 0.0f ? 60.0f : setting.framerate;
		float oneFrameTime = 1.0f / setting.framerate;

		// 엔진 루프(게임 루프).
		// !->Not -> bool값 뒤집기.
		while (!isQuit)
		{
			// 현재 시간 구하기.
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			// 프레임 시간 계산.
			float deltaTime
				= static_cast<float>(currentTime - previousTime);

			// 초단위 변환.
			deltaTime = deltaTime
				/ static_cast<float>(frequency.QuadPart);

			// 고정 프레임 기법.
			if (deltaTime >= oneFrameTime)
			{
				input.Update();
				Update(deltaTime);
				Draw();
				previousTime = currentTime;

				ProcessActorsOnLevel();
				ChangeLevel();
			}
		}
	}

	void Engine::Update(float deltaTime)
	{
		if (levelManager)
		{
			levelManager->Tick(deltaTime, &input);
		}
	}

	void Engine::Draw()
	{
		if (levelManager && !isQuit)
		{
			levelManager->Draw();
		}
		renderer->Draw();
	}

	void Engine::ProcessActorsOnLevel()
	{
		if (!levelManager)
			return;

		levelManager->ProcessAddAndDestroyActorsOnLevel();
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
			// 줄 앞뒤 공백 제거
			line = trim(line);

			// '=' 위치 찾기
			size_t equalPos = line.find('=');
			if (equalPos != std::string::npos)
			{
				std::string key = line.substr(0, equalPos);     // '=' 앞부분 (key)
				std::string value = line.substr(equalPos + 1);  // '=' 뒷부분 (value)

				key = trim(key);
				value = trim(value);

				// 설정 값 파싱
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
}
