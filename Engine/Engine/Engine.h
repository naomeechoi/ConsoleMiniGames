#pragma once
#include "Common/Common.h"

namespace Wanted
{
	class Input;

	class NAOMI_API Engine
	{
		struct EngineSetting
		{
			float framerate = 0.0f;
		};

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
		// 설정 파일 로드 함수
		void LoadSetting();

		void BeginPlay();
		void Tick(float deletaTime);
		void Draw();

	private:
		// 엔진 종료 플래그.
		bool isQuit = false;

		// 엔진 설정 값.
		EngineSetting setting;

		// 입력 관리자.
		Input* input = nullptr;

		// 메인 레벨.
		class Level* mainLevel = nullptr;

		static Engine* instance;
	};
}

