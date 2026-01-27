#include "Input.h"
#include <Windows.h>
#include <iostream>

namespace Wanted
{
	const int KEY_RANGE = 255;
	Input* Input::instance = nullptr;

	Input::Input()
	{
		instance = this;
	}

	Input::~Input()
	{
	}

	bool Input::GetKeyDown(int keyConde)
	{
		return keyStates[keyConde].isKeyDown
			&& !keyStates[keyConde].wasKeyDown;
	}

	bool Input::GetKeyUp(int keyConde)
	{
		return keyStates[keyConde].wasKeyDown
			&& !keyStates[keyConde].isKeyDown;
	}

	bool Input::GetKey(int keyConde)
	{
		return keyStates[keyConde].isKeyDown;
	}

	Input& Input::Get()
	{
		// 엔진에서 초기화 하는데
		// 널이다? 약간 답이 없는 이상한 상태
		if (!instance)
		{
			std::cout << "Error: Input::Get(). instance is null\n";
			
			// 디버그 모드에서만 동작함
			// 자동으로 중단점 걸린다.
			__debugbreak();
		}

		return *instance;
	}

	void Input::ProcessInput()
	{
		// 키 마다의 입력 읽기.
		// 운영체제가 제공하는 기능을 사용할 수 밖에 없다.
		for (int i = 0; i < KEY_RANGE; i++)
			keyStates[i].isKeyDown = GetAsyncKeyState(i) & 0x8000;
	}

	void Input::SavePreviousInputStates()
	{
		// 현재 입력 값을 이전 입력 값으로 저장
		for (int i = 0; i < KEY_RANGE; i++)
			keyStates[i].wasKeyDown = keyStates[i].isKeyDown;
	}

}