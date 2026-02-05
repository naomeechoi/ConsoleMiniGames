#pragma once
#include "Common/RTTI.h"
#include <vector>
#include <optional>
#include <Math/Vector2.h>

namespace MinigameEngine
{
	// 전방 선언
	class Actor;
	class Input;
	
	// 담당 임무: 레벨에 있는 모든 액터(물체) 관리.
	class NAOMI_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)
	public:
		Level();
		virtual ~Level();

		virtual void BeginPlay();
		virtual void Tick(float deletaTime, Input* input);
		virtual void Draw();
		virtual void OnExit() {};

		void AddNewActor(Actor* const newActor);

		// 액터 추가/제거 처리 함수
		void ProcessAddAndDestroyActors();

		void RequestChangeLevel(int levelID);

		std::optional<int> ConsumeRequestedLevel();

		void SetLevelDisplaySize(Vector2 displaySize);

	protected:
		std::vector<Actor*> actors;
		// dll 쓸때는 템플릿 사용하면 안된다.
		// 그래서 #pragma warning(disable: 4251) 이걸 써줘서 오류가 뜨지 않게 한다.

		std::vector<Actor*> addRequestedActors;

		std::optional<int> requestedLevel;
		Vector2 displaySize;
	};
}

