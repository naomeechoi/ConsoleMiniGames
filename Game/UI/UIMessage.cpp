#include "UIMessage.h"
#include "Render/Renderer.h"

using namespace MinigameEngine;

UIMessage::UIMessage()
{
}

void UIMessage::Draw()
{
    Renderer::Get().Submit(
        message.c_str(),
        pos,
        Color::White,
        Color::Black,
        0
    );
}

void UIMessage::Start(int totalWidth, Vector2 position, std::string levelStr, std::string prefixStr)
{
    int padding = (totalWidth - (int)levelStr.size()) / 2 - (int)prefixStr.size() - position.x;
    message = prefixStr + std::string(padding, ' ') + levelStr;
    pos = position;
}



void UIMessage::Clear()
{
    if(message.empty())
		return;

    message.clear();
}