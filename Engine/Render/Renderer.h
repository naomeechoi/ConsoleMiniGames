#pragma once
#include "Math/Vector2.h"
#include "Util/Color.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <unordered_set>

namespace MinigameEngine
{
	class ScreenBuffer;

	class NAOMI_API Renderer
	{
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			void Clear(const Vector2& screenSize);
			CHAR_INFO* charInfoArray = nullptr;
			int* sortingOrderArray = nullptr;
		};

		struct RenderCommand
		{
			std::string text;
			Vector2 position = Vector2::Zero;
			Color color = Color::White;
			int sortingOrder = 0;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();
		void Draw();
		void Submit(const char* text, const Vector2& position, Color color = Color::White, int sortingOrder = 0);
		void SubmitMultiLine(const char* text, const Vector2& position, Color color = Color::White, int sortingOrder = 0);
		static Renderer& Get();

	public:
		void SetDebugMode(std::unordered_set<Vector2> debugSet) { this->debugSet = debugSet; };
		void OffDebugMode() { debugSet.clear(); };

	private:
		void Clear();
		void Present();
		ScreenBuffer* GetCurScreenBuffer();

	private:
		Vector2 screenSize = Vector2::Zero;
		Frame* frame = nullptr;
		ScreenBuffer* screenBuffers[2] = {};
		int curBufferIdx = 0;
		std::vector<RenderCommand> renderQueue;

		static Renderer* instance;
		std::unordered_set<Vector2> debugSet;
	};
}
