#define NOMINMAX
#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "System/Input.h"
#include "Render/Renderer.h"
#include <algorithm>

using namespace MinigameEngine;
using std::deque;
using std::swap;
using std::min;
using std::max;

const int Y_PADDING = 1;
const int SHOWING_NEXT_BLOCKS_SIZE = 3;
const int INVALID = -1;

TetrisPlayer::TetrisPlayer(Vector2 worldStartPos)
	:worldStartPos(worldStartPos)
{
}

void TetrisPlayer::Draw()
{
    // NEXT BLOCKS
    for (int i = 0; i < SHOWING_NEXT_BLOCKS_SIZE && i < blockQueue.size(); i++)
    {
        int smallBoardX = worldStartPos.x + SMALL_BOARD_DRAW_PADDING_X;
        int smallBoardY = worldStartPos.y + Y_PADDING + (i * SMALL_BOARD_DRAW_GAP_Y);

        DrawMiniBlock(blockQueue[i], smallBoardX, smallBoardY);
    }

    // HOLD BlOCKS
    int smallBoardX = worldStartPos.x + SMALL_BOARD_DRAW_PADDING_X;
    int smallBoardY = worldStartPos.y + LAST_SMALL_BOARD_PADDING_Y;
    DrawMiniBlock(holdBlock, smallBoardX, smallBoardY);

    const auto& block = BLOCK_INFO[(int)type];

    if (offsetX == INVALID && offsetY == INVALID)
        return;

    for (int i = 0; i < CELL_SIZE; i++)
    {
        if (offsetY + block.shapes[rotation][i].y < 0)
            continue;

        Vector2 pos;
        pos.x = worldStartPos.x + (offsetX + block.shapes[rotation][i].x) * BLOCK_SIZE_X;
        pos.y = worldStartPos.y + (offsetY + block.shapes[rotation][i].y) * BLOCK_SIZE_Y;
        Renderer::Get().SubmitMultiLine(
            BLOCK_STR,
            pos,
            block.color,
            block.color,
            0
        );
    }
}

void TetrisPlayer::DrawMiniBlock(EBlockType type, int boardX, int boardY)
{
    if (type == EBlockType::EMPTY)
        return;

    const auto& block = BLOCK_INFO[(int)type];

    // 안전하게 첫 값으로 초기화 (매직넘버 제거)
    int minX = block.shapes[0][0].x;
    int maxX = minX;
    int minY = block.shapes[0][0].y;
    int maxY = minY;

    for (int j = 0; j < CELL_SIZE; j++)
    {
        int bx = block.shapes[0][j].x;
        int by = block.shapes[0][j].y;

        minX = min(minX, bx);
        maxX = max(maxX, bx);
        minY = min(minY, by);
        maxY = max(maxY, by);
    }

    int blockWidth = (maxX - minX + 1) * SMALL_BLOCK_SIZE_X;
    int blockHeight = (maxY - minY + 1) * SMALL_BLOCK_SIZE_Y;

    int centerXOffset = (SMALL_BOARD_EDGE_SIZE_X - blockWidth) / 2;
    int centerYOffset = (SMALL_BOARD_EDGE_SIZE_Y - blockHeight) / 2;

    for (int j = 0; j < CELL_SIZE; j++)
    {
        Vector2 pos;

        pos.x = boardX + centerXOffset +
            (block.shapes[0][j].x - minX) * SMALL_BLOCK_SIZE_X;

        pos.y = boardY + centerYOffset +
            (block.shapes[0][j].y - minY) * SMALL_BLOCK_SIZE_Y;

        Renderer::Get().SubmitMultiLine(
            SMALL_BLOCK_STR,
            pos,
            block.color,
            block.color,
            0
        );
    }
}

void TetrisPlayer::DrawGhost(int ghostOffsetY)
{
    const auto& block = BLOCK_INFO[(int)type];

    for (int i = 0; i < CELL_SIZE; i++)
    {
        if (ghostOffsetY + block.shapes[rotation][i].y < 0)
            continue;

        Vector2 pos;
        pos.x = worldStartPos.x +
            (offsetX + block.shapes[rotation][i].x) * BLOCK_SIZE_X;

        pos.y = worldStartPos.y +
            (ghostOffsetY + block.shapes[rotation][i].y) * BLOCK_SIZE_Y;

        Renderer::Get().SubmitMultiLine(
            BLOCK_STR,
            pos,
            Color::Gray,
            Color::Gray,
            0
        );
    }
}

void TetrisPlayer::Clear()
{
    type = EBlockType::EMPTY;
    rotation = 0;

    offsetX = INVALID;
    offsetY = INVALID;

	holdBlock = EBlockType::EMPTY;
	blockQueue.clear();
}

void TetrisPlayer::Spawn(EBlockType t, int x, int y, int rot)
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
    rotation = (rotation + 1) % ROTATION_SIZE;
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

void TetrisPlayer::InsertBlockQueue(EBlockType t)
{
    if((int)blockQueue.size() < 5)
        blockQueue.push_back(t);
}

EBlockType TetrisPlayer::GetNextBlock()
{
    if (blockQueue.empty())
        return EBlockType::EMPTY;
    
	EBlockType nextBlock = blockQueue.front();
	blockQueue.pop_front();
    return nextBlock;
}

bool TetrisPlayer::SetHoldBlockAndCheckNeedSpawn()
{
    if (holdBlock == EBlockType::EMPTY)
    {
        holdBlock = type;
        return true;
    }
    swap(type, holdBlock);
    return false;
}

void TetrisPlayer::SetBlockQueue(deque<EBlockType> blockQueue)
{
    this->blockQueue = blockQueue;
}

deque<EBlockType> TetrisPlayer::GetBlockQueue()
{
    return blockQueue;
}
