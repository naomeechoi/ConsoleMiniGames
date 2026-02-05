#include "UITop.h"
#include "System/Input.h"
#include "Render/Renderer.h"
#include "Math/Vector2.h"

UITop::UITop(int totalWidth, Vector2 position, std::string levelStr, std::string prefixStr)
	: totalWidth(totalWidth),
	position(position),
	levelStr(levelStr),
	prefixStr(prefixStr)
{
	int padding = (totalWidth - (int)levelStr.size()) / 2 - (int)prefixStr.size() - position.x;
	resultStr = prefixStr + std::string(padding, ' ') + levelStr;
}

void UITop::Tick(float deltaTime, Input* input)
{
}

void UITop::Draw()
{
	Renderer::Get().SubmitMultiLine(
		resultStr.c_str(),
		position,
		Color::White
	);
}
