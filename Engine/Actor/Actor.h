#pragma once
#include "Common/RTTI.h"
#include "Math/Vector2.h"

namespace Wanted
{
	class NAOMI_API Actor : public RTTI
	{
		// RTTI 코드 추가
		RTTI_DECLARATIONS(Actor, RTTI)
	public:
		Actor(const char image = ' ', const Vector2& position = Vector2::Zero);
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deletaTime);
		virtual void Draw();

		// 위치 변경 및 읽기 함수
		void SetPosition(const Vector2& newPosition);
		Vector2 GetPosition() const { return position; }

		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !destroyRequested; }
		inline bool DestroyRequested() const { return destroyRequested; }

	protected:
		bool hasBeganPlay = false;
		bool isActive = true;
		bool destroyRequested = false;

		char image = ' ';

	private:
		Vector2 position;
	};
}

