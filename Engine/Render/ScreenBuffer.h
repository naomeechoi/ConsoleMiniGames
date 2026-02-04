#pragma once
#include "Math/Vector2.h"
#include <Windows.h>

namespace MinigameEngine
{
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		void Clear();
		void Draw(CHAR_INFO* charInfo);
		inline HANDLE GetBuffer() const { return buffer; }

	private:
		HANDLE buffer = nullptr;
		Vector2 screenSize;
	};
}

