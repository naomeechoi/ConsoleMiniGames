#include "UICorrectCount.h"
#include "Render/Renderer.h"

const int PADDING = 3;

UICorrectCount::UICorrectCount()
{
}

void UICorrectCount::AddCount(int adding)
{
	curCount += adding;
	if (curCount > maxCount)
		curCount = maxCount;

	correctCountStr = std::to_string(curCount) + " / " + std::to_string(maxCount);
	position.x = limitX - (int)correctCountStr.size() - PADDING;
}

void UICorrectCount::Start(int y, int limitX, int maxCount)
{
	this->yPos = y;
	this->limitX = limitX;
	this->maxCount = maxCount;

	correctCountStr = "0 / " + std::to_string(maxCount);

	position.x = limitX - (int)correctCountStr.size() - PADDING;
	position.y = yPos;
}

void UICorrectCount::Clear()
{
	yPos = 0;
	position = Vector2::Zero;
	maxCount = 20;
	curCount = 0;
	limitX = 0;
	correctCountStr.clear();
}

void UICorrectCount::Draw()
{
	Renderer::Get().SubmitMultiLine(
		correctCountStr.c_str(),
		position,
		Color::White
	);
}