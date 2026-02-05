#pragma once
#include "Common/Common.h"
#include <Windows.h>
#include <functional>

namespace MinigameEngine
{
	class NAOMI_API Vector2
	{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();

		const char* ToString();

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;
		
		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		// 형변환 연산자 오버로딩
		operator COORD() const;

		static Vector2 Zero;
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Right;

	public:
		int x = 0;
		int y = 0;

	private:
		char* string = nullptr;
	};
}

namespace std {
	template<>
	struct hash<MinigameEngine::Vector2>
	{
		size_t operator()(const MinigameEngine::Vector2& v) const noexcept
		{
			return (std::hash<int>()(v.x) * 397) ^ std::hash<int>()(v.y);
		}
	};
}