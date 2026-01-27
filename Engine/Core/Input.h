#pragma once

#include "Common/Common.h"

namespace Wanted
{
	class NAOMI_API Input
	{
		// 생성자 소멸자를 private로 막아놨다.
		// 이제 이 클래스의 생성은 Engine에서만 가능한다.
		// 싱글턴을 간단하게 관리할 수 있다.
		friend class Engine;

		struct KeyState
		{
			bool isKeyDown = false;
			bool wasKeyDown = false;
		};

	// 생성을 다른 함수를 통해 하거나
	// 생성을 특정 클래스에게만 제한하거나
	private:
		Input();
		~Input();

	public:
		bool GetKeyDown(int keyConde);
		bool GetKeyUp(int keyConde);
		bool GetKey(int keyConde);
		static Input& Get();

	private:
		void ProcessInput();
		void SavePreviousInputStates();

	private:
		KeyState keyStates[255] = { };
		static Input* instance;
	};
}

