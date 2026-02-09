#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "Render/Renderer.h"

using namespace MinigameEngine;

TetrisPlayer::TetrisPlayer(Vector2 worldStartPos)
    :worldStartPos(worldStartPos)
{
}

TetrisPlayer::~TetrisPlayer()
{
}

void TetrisPlayer::BeginPlay()
{

}

void TetrisPlayer::Tick(float deltaTime, Input* input)
{
}

void TetrisPlayer::Draw()
{
    const auto& piece = g_PieceInfo[(int)type];

    if (offsetX == -1 && offsetY == -1)
        return;

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        if (offsetY + piece.blocks[rotation][i].y < 0)
            continue;

        Vector2 pos;
        pos.x = worldStartPos.x + (offsetX + piece.blocks[rotation][i].x) * brickXSize;
        pos.y = worldStartPos.y + (offsetY + piece.blocks[rotation][i].y) * brickYSize;
        Renderer::Get().SubmitMultiLine(
            brick,
            pos,
            g_PieceInfo[(int)type].color,
            g_PieceInfo[(int)type].color,
            0
        );
    }
}

void TetrisPlayer::DrawGhost(int ghostOffsetY)
{
    const auto& piece = g_PieceInfo[(int)type];

    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        if (ghostOffsetY + piece.blocks[rotation][i].y < 0)
            continue;

        Vector2 pos;
        pos.x = worldStartPos.x +
            (offsetX + piece.blocks[rotation][i].x) * brickXSize;

        pos.y = worldStartPos.y +
            (ghostOffsetY + piece.blocks[rotation][i].y) * brickYSize;

        Renderer::Get().SubmitMultiLine(
            brick,
            pos,
            Color::Gray,
            Color::Gray,
            0
        );
    }
}

void TetrisPlayer::Clear()
{
    type = PieceType::EMPTY;
    rotation = 0;

    offsetX = -1;
    offsetY = -1;
}

void TetrisPlayer::Spawn(PieceType t, int x, int y, int rot)
{
    type = t;
    rotation = rot;

    offsetX = x;
    offsetY = y;
} 

void TetrisPlayer::MoveHorizontal(bool isLeft)
{
    if (isLeft)
        offsetX--;
    else
        offsetX++;
}

void TetrisPlayer::MoveDown()
{
    offsetY++;
}

void TetrisPlayer::Rotate()
{
    rotation = (rotation + 1) % ROTATION_COUNT;
}

void TetrisPlayer::SetOffset(int offsetX, int offsetY)
{
    this->offsetX = offsetX;
    this->offsetY = offsetY;
}

void TetrisPlayer::SetRotation(int rotation)
{
	this->rotation = rotation;
}