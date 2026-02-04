#include "ScreenBuffer.h"
#include <iostream>
#include "Renderer.h"

namespace MinigameEngine
{
	ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

		// 예외 처리
		if (buffer == INVALID_HANDLE_VALUE)
		{
			MessageBoxA(nullptr, "ScreenBuffer - Faild to create buffer.", "Buffer creation error", MB_OK);
			__debugbreak();
		}

		// 콘솔 창 크기 지정
		const SMALL_RECT rect = { 0, 0, (short)(screenSize.x - 1), (short)(screenSize.y - 1) };
		if (!SetConsoleWindowInfo(buffer, true, &rect))
		{
			std::cerr << "Faild to set console window info.\n";
			__debugbreak();
		}

		// 버퍼 크기
		if (!SetConsoleScreenBufferSize(buffer, screenSize))
		{
			std::cerr << "Faild to set console screen buffer size.\n";
			__debugbreak();
		}

		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(buffer, &info);
		info.bVisible = false;
		SetConsoleCursorInfo(buffer, &info);
	}

	ScreenBuffer::~ScreenBuffer()
	{
		if (buffer)
		{
			CloseHandle(buffer);
		}
	}

	void ScreenBuffer::Clear()
	{
		DWORD writtenCount = 0;
		FillConsoleOutputCharacterA(buffer, ' ', screenSize.x * screenSize.y, Vector2::Zero, &writtenCount);
	}

	void ScreenBuffer::Draw(CHAR_INFO* charInfo)
	{
		SMALL_RECT writeRegion = { 0, 0, (short)(screenSize.x - 1), (short)(screenSize.y - 1) };
		WriteConsoleOutputA(buffer, charInfo, screenSize, Vector2::Zero, &writeRegion);
	}
}