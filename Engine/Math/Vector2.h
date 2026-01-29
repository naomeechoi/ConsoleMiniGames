#pragma once
#include "Common/Common.h"

namespace Wanted
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

