#pragma once

#include "Math/Vector2.h"
#include "Color.h"

namespace MinigameEngine
{
	class NAOMI_API Console
	{
	public:
		static void SetConsolePosition(const Vector2& position);
		static void SetConsoleTextColor(Color color);
		static void SetCursorVisible(bool bVisible);
	};
}
