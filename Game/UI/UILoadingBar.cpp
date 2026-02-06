#include "UILoadingBar.h"
#include "Render/Renderer.h"

UILoadingBar::UILoadingBar(Vector2 position, float maxWidth, float totalTime, char shapeChar)
	:position(position), maxWidth(maxWidth), totalTime(totalTime), shapeChar(shapeChar)
{
}

void UILoadingBar::Tick(float deltaTime)
{
	if (!isMove)
		return;

	timer.Tick(deltaTime);
	if (!timer.IsTimeOut())
		return;

	charDrawAccum += maxWidth / totalTime;
	int toDraw = (int)charDrawAccum;
	charDrawAccum -= toDraw;

	if(toDraw > 0)
		loadingBarStr += std::string(toDraw, shapeChar);

	timer.Reset();
}

void UILoadingBar::Draw()
{
	Renderer::Get().SubmitMultiLine(
		loadingBarStr.c_str(),
		position,
		Color::White
	);
}

void UILoadingBar::Start()
{
	if (isMove)
		return;

	timer.SetTargetTime(interval);
	isMove = true;
}

void UILoadingBar::Stop()
{
	isMove = false;
}

void UILoadingBar::Clear()
{
	loadingBarStr.clear();
}