#include "Engine.h"
#include <iostream>
#pragma comment(lib, "winmm.lib")
#include <Windows.h>

namespace Wanted
{
	Engine::Engine()
	{
	}

	Engine::~Engine()
	{
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
				ProcessInput();

				// 프레임 처리
				Tick(deltaTime);
				Draw();

				previousTime = currentTime;

				// 현재 입력 값을 이전 입력 값으로 저장
				for (int i = 0; i < 255; i++)
					keyStates[i].wasKeyDown = keyStates[i].isKeyDown;
			}
		}


		std::cout << "Engine has been shutdown...\n";
		// TODO: 정리 작업
	}

	void Engine::QuitEngine()
	{
		isQuit = true;
	}

	bool Engine::GetKeyDown(int keyConde)
	{
		return keyStates[keyConde].isKeyDown
			&& !keyStates[keyConde].wasKeyDown;
	}

	bool Engine::GetKeyUp(int keyConde)
	{
		return keyStates[keyConde].wasKeyDown
			&& !keyStates[keyConde].isKeyDown;
	}

	bool Engine::GetKey(int keyConde)
	{
		return keyStates[keyConde].isKeyDown;
	}

	void Engine::ProcessInput()
	{
		// 키 마다의 입력 읽기.
		// 운영체제가 제공하는 기능을 사용할 수 밖에 없다.
		for(int i = 0; i < 255; i++)
			keyStates[i].isKeyDown = GetAsyncKeyState(i) & 0x8000;
	}

	void Engine::Tick(float deletaTime)
	{
		std::cout << "deletaTime: " << deletaTime 
			<< ", FPS: " << (1.0f / deletaTime)
			<< std::endl;

		if (GetKeyDown(VK_ESCAPE))
			QuitEngine();
	}

	void Engine::Draw()
	{
	}
}
