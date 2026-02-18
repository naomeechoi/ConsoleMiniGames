#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Console.h"
#include "Util/Delete.h"

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
		Clear();

		const int width = screenSize.x;

		for (const RenderCommand& command : renderQueue)
		{
			const int length = static_cast<int>(command.text.size());
			if (length <= 0)
				continue;

			const int y = command.position.y;
			if (y < 0 || y >= screenSize.y)
				continue;

			const int startX = command.position.x;
			const int endX = startX + length - 1;

			if (endX < 0 || startX >= width)
				continue;

			const int visibleStart = startX < 0 ? 0 : startX;
			const int visibleEnd = endX >= width ? width - 1 : endX;

			const int rowStart = y * width;

			WORD attr =
				(static_cast<WORD>(command.color) & 0x0F) |
				(static_cast<WORD>(command.bgColor) << 4);

			const char* src = command.text.data() + (visibleStart - startX);

			CHAR_INFO* dstChar =
				&frame->charInfoArray[rowStart + visibleStart];

			int* dstSort =
				&frame->sortingOrderArray[rowStart + visibleStart];

			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				if (*dstSort <= command.sortingOrder)
				{
					dstChar->Char.AsciiChar = *src;
					dstChar->Attributes = attr;
					*dstSort = command.sortingOrder;
				}

				src++;
				dstChar++;
				dstSort++;
			}
		}

		GetCurScreenBuffer()->Draw(frame->charInfoArray);
		Present();
		renderQueue.clear();
	}

	void Renderer::Submit(
		const char* text,
		const Vector2& position,
		Color color,
		Color bgColor,
		int sortingOrder)
	{
		if (!text)
			return;

		int y = position.y;
		const char* lineStart = text;
		const char* ptr = text;

		while (*ptr)
		{
			if (*ptr == '\n')
			{
				renderQueue.emplace_back(RenderCommand{
					std::string(lineStart, ptr - lineStart),
					{ position.x, y },
					color,
					bgColor,
					sortingOrder
					});

				y++;
				lineStart = ptr + 1;
			}
			++ptr;
		}

		if (ptr != lineStart)
		{
			renderQueue.emplace_back(RenderCommand{
				std::string(lineStart, ptr - lineStart),
				{ position.x, y },
				color,
				bgColor,
				sortingOrder
				});
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