#include "UICorrectCount.h"
#include "Render/Renderer.h"

const int PADDING = 3;

UICorrectCount::UICorrectCount(int yPos, int limitX, int maxCount)
	: yPos(yPos), limitX(limitX), maxCount(maxCount)
{
	correctCountStr = "0 / " + std::to_string(maxCount);

	position.x = limitX - (int)correctCountStr.size() - PADDING;
	position.y = yPos;
}

void UICorrectCount::AddCount(int adding)
{
	curCount += adding;
	if (curCount > maxCount)
		curCount = maxCount;

	correctCountStr = std::to_string(curCount) + " / " + std::to_string(maxCount);
	position.x = limitX - (int)correctCountStr.size() - PADDING;
}

void UICorrectCount::Draw()
{
	Renderer::Get().SubmitMultiLine(
		correctCountStr.c_str(),
		position,
		Color::White
	);
}