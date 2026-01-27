#pragma once
#include "Common/Common.h"

namespace Wanted
{
	class Input;

	class NAOMI_API Engine
	{
	public:
		Engine();
		~Engine();

		// 엔진 루프
		void Run();
		void QuitEngine();

		// 새 레벨 추가 함수
		void SetNewLevel(class Level* newLevel);

		static Engine& Get();

	private:
		void BeginPlay();
		void Tick(float deletaTime);
		void Draw();

	private:
		// 엔진 종료 플래그.
		bool isQuit = false;
		Input* input = nullptr;
		class Level* mainLevel = nullptr;

		static Engine* instance;
	};
}

