#include "Input.h"

void MinigameEngine::Input::Update()
{
    prevKeys = curKeys;
    prevMouse = curMouse;

    for (int key = 0x08; key <= 0xFE; key++)
    {
        curKeys[key] = (GetAsyncKeyState(key) & 0x8000);
    }

    curMouse[VK_LBUTTON] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
    curMouse[VK_RBUTTON] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
    curMouse[VK_MBUTTON] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);

    POINT screenPos;
    GetCursorPos(&screenPos);

    ScreenToClient(GetConsoleWindow(), &screenPos);
    CONSOLE_FONT_INFO font;
    GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), false, &font);
    COORD fontSize = font.dwFontSize;
    if (fontSize.X > 0 && fontSize.Y > 0)
    {
        mousePos.X = (short)(screenPos.x / fontSize.X);
        mousePos.Y = (short)(screenPos.y / fontSize.Y);
    }
}

bool MinigameEngine::Input::IsKeyPressed(int key) const
{
    return curKeys.at(key) && !prevKeys.at(key);
}

bool MinigameEngine::Input::IsKeyHeld(int key) const
{
    return curKeys.at(key) && prevKeys.at(key);
}

bool MinigameEngine::Input::IsKeyReleased(int key) const
{
    return !curKeys.at(key) && prevKeys.at(key);
}

bool MinigameEngine::Input::IsMousePressed(int button) const
{
    return curMouse.at(button) && !prevMouse.at(button);
}

bool MinigameEngine::Input::IsMouseHeld(int button) const
{
    return curMouse.at(button) && prevMouse.at(button);
}

bool MinigameEngine::Input::IsMouseReleased(int button) const
{
    return !curMouse.at(button) && prevMouse.at(button);
}

COORD MinigameEngine::Input::GetMousePosition() const
{
    return mousePos;
}
