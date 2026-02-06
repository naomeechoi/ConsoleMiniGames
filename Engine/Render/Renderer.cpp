#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Console.h"
#include "Util/Delete.h"
#include <sstream>   // std::istringstream
#include <string>  
#include <vector>

namespace MinigameEngine
{
	Renderer* Renderer::instance = nullptr;

	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, -1, sizeof(int) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}


	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		const int width = screenSize.x;
		const int height = screenSize.y;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				const int idx = i * width + j;

				CHAR_INFO& info = charInfoArray[idx];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;
				sortingOrderArray[idx] = -1;
			}
		}
	}

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);
		frame->Clear(screenSize);

		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		Present();
	}

	Renderer::~Renderer()
	{
		delete frame;
		frame = nullptr;

		delete screenBuffers[0];
		screenBuffers[0] = nullptr;

		delete screenBuffers[1];
		screenBuffers[1] = nullptr;
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(nullptr, "Renderer::Get() - instance is null", "Error", MB_OK);
			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Draw()
	{
		// 화면 지우기
		Clear();

		// 전제조건: 레벨의 모든 액터가 렌더러에 Submit을 완료
		// 렌더큐 순회하면서 프레임 채우기
		for (const RenderCommand& command : renderQueue)
		{
			const int length = static_cast<int>(command.text.size());
			if (length <= 0)
				continue;

			if (command.position.y < 0 || command.position.y > screenSize.y)
				continue;

			const int startX = command.position.x;
			const int endX = command.position.x + length - 1;

			if (endX < 0 || startX >= screenSize.x)
			{
				continue;
			}

			// 시작 위치
			const int visibleStart = startX < 0 ? 0 : startX;
			const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

			for (int x = visibleStart; x <= visibleEnd; x++)
			{
				// 문자열 안에 문자 인덱스
				const int sourceIndex = x - startX;

				// 프레임에서 인덱스
				const int index = (command.position.y * screenSize.x) + x;

				if (frame->sortingOrderArray[index] > command.sortingOrder)
					continue;

				WORD color = (WORD)command.color;
				WORD bg = (WORD)command.bgColor;
				if (!debugSet.empty() && debugSet.count(Vector2(x, command.position.y)))
				{
					bg = 4;
				}
				frame->charInfoArray[index].Char.AsciiChar = command.text[sourceIndex];
				frame->charInfoArray[index].Attributes = (color & 0x0F) | (bg << 4);
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		// 그리기
		GetCurScreenBuffer()->Draw(frame->charInfoArray);

		// 버퍼 교환
		Present();

		// 렌더큐 비우기
		renderQueue.clear();
	}

	void Renderer::Submit(
		const char* text,
		const Vector2& position,
		Color color,
		int sortingOrder)
	{
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;
		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		const char* text,
		const Vector2& position,
		Color color,
		Color bgColor,
		int sortingOrder)
	{
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.bgColor = bgColor;
		command.sortingOrder = sortingOrder;
		renderQueue.emplace_back(command);
	}

	void Renderer::SubmitMultiLine(
		const char* text,
		const Vector2& position,
		Color color,
		Color bgColor,
		int sortingOrder)
	{
		if (!text)
			return;

		std::istringstream iss(text);
		std::string line;
		int y = position.y;

		while (std::getline(iss, line))
		{
			// 빈 줄도 렌더링 가능하도록 line.c_str() 그대로 전달
			Submit(line.c_str(), { position.x, y }, color, bgColor, sortingOrder);
			y++;
		}
	}

	void Renderer::Clear()
	{
		frame->Clear(screenSize);
		GetCurScreenBuffer()->Clear();
	}

	void Renderer::Present()
	{
		// 버퍼 교환
		SetConsoleActiveScreenBuffer(GetCurScreenBuffer()->GetBuffer());
		curBufferIdx = 1 - curBufferIdx;
	}

	ScreenBuffer* Renderer::GetCurScreenBuffer()
	{
		return screenBuffers[curBufferIdx];
	}
}