#pragma once

#include "Common/Common.h"
#include <Windows.h>

namespace Wanted
{
	// 콘솔에 텍스트 색상 등을 지정할 때 사용
	// 색상 강타입 열거형
	enum class NAOMI_API Color : unsigned short
	{
		Black = 0,
		Red = FOREGROUND_RED,
		Green = FOREGROUND_GREEN,
		Blue = FOREGROUND_BLUE,
		Yellow = Red | Green,
		White = Blue | Green | Red,
	};
}
