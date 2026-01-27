#include "Engine.h"
#include "Level/Level.h"
#include "Core/Input.h"

#include <iostream>
#include <Windows.h>

namespace Wanted
{
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		instance = this;
		input = new Input();
		// 힙에 생성되나 힙에 생성된 이 주소가
		// input 클래스의 Input* Input::instance = nullptr; 에 의해
		// 데이터 영역에 저장된다.
		// 데이터 영역에 input을 가르키는 주소가 저장되는 것!!
	}

	Engine::~Engine()
	{
		if (mainLevel)
		{
			delete mainLevel;
			mainLevel = nullptr;
		}

		if (input)
		{
			delete input;
			input = nullptr;
		}
	}

	void Engine::Run()
	{
		// 시계의 정밀도
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		int64_t currentTime = 0;
		int64_t previousTime = 0;

		// 기준 프레임 (단위: 초)
		float targetFrameRate = 120.0f;
		float oneFrameTime = 1.0f / targetFrameRate;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		// loop 진입 전에는 두 시간을 동일하게 맞춘다.
		currentTime = time.QuadPart;
		previousTime = currentTime;

		while (!isQuit)
		{
			// 현재 시간 계산
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			// 프레임 시간 계산
			float deltaTime = (float)(currentTime - previousTime);
			// 초단위 변환
			deltaTime /= (float)frequency.QuadPart;

			if (deltaTime >= oneFrameTime)
			{
				input->ProcessInput();

				// 프레임 처리
				BeginPlay();
				Tick(deltaTime);
				Draw();

				previousTime = currentTime;
				input->SavePreviousInputStates();
			}
		}


		std::cout << "Engine has been shutdown...\n";
		// TODO: 정리 작업
	}

	void Engine::QuitEngine()
	{
		isQuit = true;
	}

	void Engine::SetNewLevel(Level* newLevel)
	{
		// TODO: 임시 코드, 레벨 전환시 바로 제거하면 안 됌
		if (mainLevel)
		{
			delete mainLevel;
			mainLevel = nullptr;
		}

		mainLevel = newLevel;
	}

	Engine& Engine::Get()
	{
		if (!instance)
		{
			std::cout << "Error: Engine::Get(). instance is null\n";
			__debugbreak();
		}

		return *instance;
	}

	void Engine::BeginPlay()
	{
		if (!mainLevel)
		{
			std::cout << "Error: Engine::BeginPlay(). mainLevel is empty.\n";
			return;
		}

		mainLevel->BeginPlay();
	}

	void Engine::Tick(float deletaTime)
	{
		/*std::cout << "deletaTime: " << deletaTime 
			<< ", FPS: " << (1.0f / deletaTime)
			<< std::endl;
		*/

		if (input->GetKeyDown(VK_ESCAPE))
			QuitEngine();

		if (!mainLevel)
		{
			std::cout << "Error: Engine::Tick(). mainLevel is empty.\n";
			return;
		}
		
		mainLevel->Tick(deletaTime);
	}

	void Engine::Draw()
	{
		if (!mainLevel)
		{
			std::cout << "Error: Engine::Draw(). mainLevel is empty.\n";
			return;
		}

		mainLevel->Draw();
	}
}
