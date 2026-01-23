#pragma once

namespace Wanted
{
	class Engine
	{
		struct KeyState
		{
			bool isKeyDown = false;
			bool wasKeyDown = false;
		};

	public:
		Engine();
		~Engine();

		// 엔진 루프
		void Run();
		void QuitEngine();

		// 입력 확인 함수
		bool GetKeyDown(int keyConde);
		bool GetKeyUp(int keyConde);
		bool GetKey(int keyConde);

	private:
		void ProcessInput();
		void Tick(float deletaTime);
		void Draw();

	private:
		// 엔진 종료 플래그.
		bool isQuit = false;

		KeyState keyStates[255] = { };
	};
}

