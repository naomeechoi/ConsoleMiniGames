#pragma once
#include "Common/Common.h"
#include <vector>
#include <algorithm>

namespace MinigameEngine
{
	enum class LayoutArea
	{
		TopLeft, Top, TopRight,
		CenterLeft, Center, CenterRight,
		BottomLeft, Bottom, BottomRight,
	};

	struct LayoutElement
	{
		LayoutArea area;
		int preferredWidth = -1;
		int preferredHeight = -1;
		int margin = 0;
	};

	class NAOMI_API LayoutCalculator
	{
	public:
		struct Rect
		{
			int x = 0;
			int y = 0;
			int w = 0;
			int h = 0;
		};

		struct Insets
		{
			int left = 0;
			int top = 0;
			int right = 0;
			int bottom = 0;
		};

	public:
		void SetScreenSize(int w, int h);
		void SetBorder(int thickness);
		void Clear();
		int AddElement(const LayoutElement& elem);
		const Rect& GetRect(int id) const;
		void Recalculate();

	private:
		void CalculateTop();
		void CalculateBottom();
		void CalculateLeft();
		void CalculateRight();
		void CalculateCenter();

	private:
		int screenW = 0;
		int screenH = 0;
		Insets viewport;

		int usedTop = 0;
		int usedBottom = 0;
		int usedLeft = 0;
		int usedRight = 0;

		std::vector<LayoutElement> elements;
		std::vector<Rect> results;
	};

}
