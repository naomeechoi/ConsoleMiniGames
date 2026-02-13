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
    // next 작은 블록 그리기
    DrawNextPieces();
    DrawHoldPiece();

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

void TetrisPlayer::DrawNextPieces()
{
    int boxWidth = 12;
    int boxHeight = 6;

    for (int i = 0; i < 3; i++)
    {
        if (i >= pieceQueue.size()) break;
        PieceType nextType = pieceQueue[i];
        const auto& piece = g_PieceInfo[(int)nextType];

        // 1. 블록의 실제 차지 범위 계산 (중앙 정렬용)
        int minX = 99, maxX = -99, minY = 99, maxY = -99;
        for (int j = 0; j < 4; j++)
        {
            int bx = piece.blocks[0][j].x;
            int by = piece.blocks[0][j].y;
            if (bx < minX) minX = bx; if (bx > maxX) maxX = bx;
            if (by < minY) minY = by; if (by > maxY) maxY = by;
        }

        // 2. 블록의 실제 출력 크기
        int pieceWidth = (maxX - minX + 1) * smallBrickXSize;
        int pieceHeight = (maxY - minY + 1) * smallBrickYSize;

        // 3. 박스 내 중앙 시작점 계산 (여백을 반으로 나눔)
        float centerXOffset = (boxWidth - pieceWidth) / 2.0f;
        float centerYOffset = (boxHeight - pieceHeight) / 2 + (boxHeight - pieceHeight) % 2;

        float boxX = worldStartPos.x + 42;
        float boxY = worldStartPos.y + 1 + (i * 9);

        for (int j = 0; j < 4; j++)
        {
            Vector2 pos;
            // (블록 좌표 - 최소값)을 해서 0부터 시작하게 만든 뒤, 중앙 오프셋을 더함
            pos.x = boxX + centerXOffset + (piece.blocks[0][j].x - minX) * smallBrickXSize;
            pos.y = boxY + centerYOffset + (piece.blocks[0][j].y - minY) * smallBrickYSize;

            Renderer::Get().SubmitMultiLine(smallBrick, pos, piece.color, piece.color, 0);
        }
    }
}

void TetrisPlayer::DrawHoldPiece()
{
    int boxWidth = 12;
    int boxHeight = 6;

    const auto& piece = g_PieceInfo[(int)holdPiece];

    // 1. 블록의 실제 차지 범위 계산 (중앙 정렬용)
    int minX = 99, maxX = -99, minY = 99, maxY = -99;
    for (int j = 0; j < 4; j++)
    {
        int bx = piece.blocks[0][j].x;
        int by = piece.blocks[0][j].y;
        if (bx < minX) minX = bx; if (bx > maxX) maxX = bx;
        if (by < minY) minY = by; if (by > maxY) maxY = by;
    }

    // 2. 블록의 실제 출력 크기
    int pieceWidth = (maxX - minX + 1) * smallBrickXSize;
    int pieceHeight = (maxY - minY + 1) * smallBrickYSize;

    // 3. 박스 내 중앙 시작점 계산 (여백을 반으로 나눔)
    float centerXOffset = (boxWidth - pieceWidth) / 2.0f;
    float centerYOffset = (boxHeight - pieceHeight) / 2 + (boxHeight - pieceHeight) % 2;

    float boxX = worldStartPos.x + 42;
    float boxY = worldStartPos.y + 42 - 8;

    for (int j = 0; j < 4; j++)
    {
        Vector2 pos;
        // (블록 좌표 - 최소값)을 해서 0부터 시작하게 만든 뒤, 중앙 오프셋을 더함
        pos.x = boxX + centerXOffset + (piece.blocks[0][j].x - minX) * smallBrickXSize;
        pos.y = boxY + centerYOffset + (piece.blocks[0][j].y - minY) * smallBrickYSize;

        Renderer::Get().SubmitMultiLine(smallBrick, pos, piece.color, piece.color, 0);
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

	holdPiece = PieceType::EMPTY;
	pieceQueue.clear();
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

void TetrisPlayer::InsertPieceQueue(PieceType t)
{
    if((int)pieceQueue.size() < 5)
    pieceQueue.push_back(t);
}

PieceType TetrisPlayer::GetNextPiece()
{
    if (pieceQueue.empty())
        return PieceType::EMPTY;
    
	PieceType nextPiece = pieceQueue.front();
	pieceQueue.pop_front();
    return nextPiece;
}

void TetrisPlayer::SetHoldPiece()
{
	PieceType prevHopdPiece = holdPiece;
    holdPiece = type;

    if (prevHopdPiece != PieceType::EMPTY)
    {
        pieceQueue.push_front(prevHopdPiece);
    }
}