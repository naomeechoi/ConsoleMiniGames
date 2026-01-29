#pragma once
#include "Common/RTTI.h"
#include <vector>

namespace Wanted
{
	// 전방 선언
	class Actor;
	// 담당 임무: 레벨에 있는 모든 액터(물체) 관리.
	class NAOMI_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)
	public:
		Level();
		virtual ~Level();

		virtual void BeginPlay();
		virtual void Tick(float deletaTime);
		virtual void Draw();

		void AddNewActor(Actor* newActor);

	protected:
		std::vector<Actor*> actors;
		// dll 쓸때는 템플릿 사용하면 안된다.
		// 그래서 #pragma warning(disable: 4251) 이걸 써줘서 오류가 뜨지 않게 한다.
	};
}

