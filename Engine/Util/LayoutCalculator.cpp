#include "LayoutCalculator.h"

namespace MinigameEngine
{
	void LayoutCalculator::SetScreenSize(int w, int h)
	{
		screenW = w;
		screenH = h;
	}

	void LayoutCalculator::SetBorder(int thickness)
	{
		viewport.left = thickness;
		viewport.top = thickness;
		viewport.right = thickness;
		viewport.bottom = thickness;
	}

	void LayoutCalculator::Clear()
	{
		elements.clear();
		results.clear();

		usedTop = 0;
		usedBottom = 0;
		usedLeft = 0;
		usedRight = 0;
	}

	int LayoutCalculator::AddElement(const LayoutElement& elem)
	{
		elements.emplace_back(elem);
		results.emplace_back();
		return static_cast<int>(elements.size() - 1);
	}

	const LayoutCalculator::Rect& LayoutCalculator::GetRect(int id) const
	{
		return results[id];
	}

	void LayoutCalculator::Recalculate()
	{
		usedTop = viewport.top;
		usedLeft = viewport.left;
		usedBottom = viewport.bottom;
		usedRight = viewport.right;

		CalculateTop();
		CalculateBottom();
		CalculateLeft();
		CalculateRight();
		CalculateCenter();
	}

	void LayoutCalculator::CalculateTop()
	{
		int maxHeight = 0;

		for (const auto& e : elements)
		{
			if (e.area == LayoutArea::TopLeft ||
				e.area == LayoutArea::Top ||
				e.area == LayoutArea::TopRight)
			{
				if (e.preferredHeight > 0)
					maxHeight = std::max(maxHeight, e.preferredHeight);
			}
		}

		if (maxHeight == 0)
			return;

		for (size_t i = 0; i < elements.size(); ++i)
		{
			const auto& e = elements[i];
			auto& r = results[i];

			if (e.area == LayoutArea::TopLeft)
			{
				r.x = usedLeft + e.margin;
				r.y = usedTop + e.margin;
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 4);
				r.h = maxHeight;
			}
			else if (e.area == LayoutArea::Top)
			{
				r.x = usedLeft;
				r.y = usedTop + e.margin;
				r.w = screenW - usedLeft - usedRight;
				r.h = maxHeight;
			}
			else if (e.area == LayoutArea::TopRight)
			{
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 4);
				r.x = screenW - usedRight - r.w - e.margin;
				r.y = usedTop + e.margin;
				r.h = maxHeight;
			}
		}

		usedTop += maxHeight;
	}

	// =========================
	// Bottom 영역
	// =========================
	void LayoutCalculator::CalculateBottom()
	{
		int maxHeight = 0;

		for (const auto& e : elements)
		{
			if (e.area == LayoutArea::BottomLeft ||
				e.area == LayoutArea::Bottom ||
				e.area == LayoutArea::BottomRight)
			{
				if (e.preferredHeight > 0)
					maxHeight = std::max(maxHeight, e.preferredHeight);
			}
		}

		if (maxHeight == 0)
			return;

		for (size_t i = 0; i < elements.size(); ++i)
		{
			const auto& e = elements[i];
			auto& r = results[i];

			if (e.area == LayoutArea::BottomLeft)
			{
				r.x = usedLeft + e.margin;
				r.h = maxHeight;
				r.y = screenH - usedBottom - r.h - e.margin;
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 4);
			}
			else if (e.area == LayoutArea::Bottom)
			{
				r.x = usedLeft;
				r.h = maxHeight;
				r.y = screenH - usedBottom - r.h - e.margin;
				r.w = screenW - usedLeft - usedRight;
			}
			else if (e.area == LayoutArea::BottomRight)
			{
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 4);
				r.h = maxHeight;
				r.x = screenW - usedRight - r.w - e.margin;
				r.y = screenH - usedBottom - r.h - e.margin;
			}
		}

		usedBottom += maxHeight;
	}

	// =========================
	// CenterLeft 영역
	// =========================
	void LayoutCalculator::CalculateLeft()
	{
		for (size_t i = 0; i < elements.size(); ++i)
		{
			const auto& e = elements[i];
			if (e.area == LayoutArea::CenterLeft)
			{
				auto& r = results[i];
				r.x = usedLeft + e.margin;
				r.y = usedTop + e.margin;
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 6);
				r.h = screenH - usedTop - usedBottom;
				usedLeft += r.w;
			}
		}
	}

	// =========================
	// CenterRight 영역
	// =========================
	void LayoutCalculator::CalculateRight()
	{
		for (size_t i = 0; i < elements.size(); ++i)
		{
			const auto& e = elements[i];
			if (e.area == LayoutArea::CenterRight)
			{
				auto& r = results[i];
				r.w = (e.preferredWidth > 0 ? e.preferredWidth : screenW / 6);
				r.x = screenW - usedRight - r.w - e.margin;
				r.y = usedTop + e.margin;
				r.h = screenH - usedTop - usedBottom;
				usedRight += r.w;
			}
		}
	}

	// =========================
	// Center 영역 (남은 공간 전부)
	// =========================
	void LayoutCalculator::CalculateCenter()
	{
		for (size_t i = 0; i < elements.size(); ++i)
		{
			const auto& e = elements[i];
			if (e.area == LayoutArea::Center)
			{
				auto& r = results[i];
				r.x = usedLeft;
				r.y = usedTop;
				r.w = screenW - usedLeft - usedRight;
				r.h = screenH - usedTop - usedBottom;
			}
		}
	}
}