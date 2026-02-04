#pragma once
#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Util/Color.h"

namespace MinigameEngine
{
	// 전방 선언
	class Level;
	class NAOMI_API Actor : public RTTI
	{
		// RTTI 코드 추가
		RTTI_DECLARATIONS(Actor, RTTI)
	public:
		Actor(const char* image = " ", const Vector2& position = Vector2::Zero, Color color = Color::White);
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deletaTime);
		virtual void Draw();

		// 위치 변경 및 읽기 함수
		void SetPosition(const Vector2& newPosition);
		Vector2 GetPosition() const { return position; }

		// 오너십 추가/읽기 함수
		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !destroyRequested; }
		inline bool DestroyRequested() const { return destroyRequested; }

		inline int GetSortingOrder() const { return sortingOrder; }

	protected:
		bool hasBeganPlay = false;
		bool isActive = true;
		bool destroyRequested = false;

		char* image = nullptr;
		int width = 0;

		Color color = Color::White;

		// 오너십
		Level* owner = nullptr;

		// 그리기 우선 순위(값이 크면 우선순위가 높음)
		int sortingOrder = 0;

	private:
		Vector2 position;
	};
}

