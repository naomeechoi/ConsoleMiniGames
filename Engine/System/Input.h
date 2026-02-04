#pragma once
#include "Common/Common.h"
#include <Windows.h>
#include <unordered_map>

namespace MinigameEngine
{
	class NAOMI_API Input
	{
	public:
		void Update();

		// 키보드
		bool IsKeyPressed(int key) const;
		bool IsKeyHeld(int key) const;
		bool IsKeyReleased(int key) const;

		// 마우스
		bool IsMousePressed(int button) const;
		bool IsMouseHeld(int button) const;
		bool IsMouseReleased(int button) const;

		COORD GetMousePosition() const;

	private:
		std::unordered_map<int, bool> curKeys;
		std::unordered_map<int, bool> prevKeys;

		std::unordered_map<int, bool> curMouse;
		std::unordered_map<int, bool> prevMouse;

		COORD mousePos{ 0, 0 };
	};
}

