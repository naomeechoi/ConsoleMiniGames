#define NOMINMAX
#include "UIMessage.h"
#include "Render/Renderer.h"


UIMessage::UIMessage()
{
}

void UIMessage::Draw()
{
    Renderer::Get().SubmitMultiLine(
        message.c_str(),
        pos,
        Color::White,
        Color::Black,
        1
    );
}

void UIMessage::Start(int totalWidth, Vector2 position, std::string levelStr, std::string prefixStr)
{
    int padding = (totalWidth - (int)levelStr.size()) / 2 - (int)prefixStr.size() - position.x;
    message = prefixStr + std::string(padding, ' ') + levelStr;
    pos.y = position.y;
}



void UIMessage::Clear()
{
    message.clear();
}