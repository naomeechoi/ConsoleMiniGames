#include "TetrisLevel.h"
#include "World/Level.h"
#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "Common/LevelType.h"
#include "System/Input.h"
#include "TetrisRotationSystem.h"
#include "Util/Random.h"

using namespace MinigameEngine;

const int MAX_LOCK_RESET = 15;

TetrisLevel::TetrisLevel()
{
}

TetrisLevel::~TetrisLevel()
{
	OnExit();
	delete player;
	player = nullptr;

	delete board;
	board = nullptr;
}

void TetrisLevel::BeginPlay()
{
	if (hasBeganPlay)
		return;

	hasBeganPlay = true;

	Vector2 startPos;
	startPos.x = (displaySize.x / 2 - 20) / 2;
	startPos.y = (displaySize.y - 40) / 2;
	if(!player)
		player = new TetrisPlayer(startPos);

	if(!board)
		board = new TetrisBoard(startPos);

	if (!player || !board)
	{
		RequestChangeLevel((int)LevelType::Menu);
		return;
	}

	int randomType = Random::Random((int)PieceType::I, (int)PieceType::Z);
	player->Spawn((PieceType)randomType);
	playerDownTime.SetTargetTime(1.0f);
	softDropTimer.SetTargetTime(0.05f);
	horizontalMoveTimer.SetTargetTime(0.05f);
}

void TetrisLevel::OnExit()
{
	hasBeganPlay = false;
	playerDownTime.Reset();
	softDropTimer.Reset();
	horizontalMoveTimer.Reset();
	board->Clear();
	bool isLocking = false;
	int lockResetCount = 0;
}

void TetrisLevel::Tick(float deltaTime, MinigameEngine::Input* input)
{
	if (input->IsKeyPressed(VK_ESCAPE))
	{
		RequestChangeLevel((int)LevelType::Menu);
	}

	board->Tick(deltaTime);

	// 자동 낙하
	playerDownTime.Tick(deltaTime);
	if (playerDownTime.IsTimeOut())
	{
		MoveDownOrFix();
	}

	// Down 키 연속 낙하
	if (input->IsKeyHeld(VK_DOWN))
	{
		softDropTimer.Tick(deltaTime);
		if (softDropTimer.IsTimeOut())
		{
			MoveDownOrFix();
			softDropTimer.Reset();
		}
	}
	else if(input->IsKeyReleased(VK_DOWN))
	{
		softDropTimer.Reset(); // 키를 떼면 초기화
	}

	// Left
	if (input->IsKeyHeld(VK_LEFT))
	{
		horizontalMoveTimer.Tick(deltaTime);
		if (horizontalMoveTimer.IsTimeOut())
		{
			MoveHorizontal(true);
			horizontalMoveTimer.Reset();
		}
	}
	else if (input->IsKeyReleased(VK_LEFT))
	{
		horizontalMoveTimer.Reset();
	}

	// Right
	if (input->IsKeyHeld(VK_RIGHT))
	{
		horizontalMoveTimer.Tick(deltaTime);
		if (horizontalMoveTimer.IsTimeOut())
		{
			MoveHorizontal(false);
			horizontalMoveTimer.Reset();
		}
	}
	else if (input->IsKeyReleased(VK_RIGHT))
	{
		horizontalMoveTimer.Reset();
	}

	if (input->IsKeyPressed(VK_UP))
	{
		Rotate();
	}

	if (input->IsKeyPressed(VK_SPACE))
	{
		HardDrop();
	}
	
	LockCheck(deltaTime);

}

void
TetrisLevel::Draw()
{
	if (!player || !board)
		return;

	board->Draw();


	player->DrawGhost(GetGhostY());
	player->Draw();
}

void TetrisLevel::MoveDownOrFix()
{
	PieceType type = player->GetPieceType();
	int rotation = player->GetRotation();
	int x = player->GetOffsetX();
	int nextY = player->GetOffsetY() + 1;

	if (board->CanPlace(type, rotation, x, nextY))
	{
		player->MoveDown();
	}

	playerDownTime.Reset();
}

void TetrisLevel::MoveHorizontal(bool isLeft)
{
	PieceType type = player->GetPieceType();
	int rotation = player->GetRotation();
	int nextX = player->GetOffsetX();
	int y = player->GetOffsetY();

	if (isLeft)
		nextX--;
	else
		nextX++;

	if (board->CanPlace(type, rotation, nextX, y))
	{
		player->MoveHorizontal(isLeft);

		if (isLocking && lockResetCount < MAX_LOCK_RESET)
		{
			lockDelayTimer.Reset();
			lockResetCount++;
		}
	}
}

void TetrisLevel::Rotate()
{
	PieceType type = player->GetPieceType();
	int from = player->GetRotation();
	int to = (from + 1) % ROTATION_COUNT;

	int x = player->GetOffsetX();
	int y = player->GetOffsetY();

	const int (*kick)[2];

	if (type == PieceType::I)
		kick = I_KICK[from];
	else
		kick = JLSTZ_KICK[from];

	for (int i = 0; i < offsetsForRotation; ++i)
	{
		int nx = x + kick[i][0];
		int ny = y + kick[i][1];

		if (board->CanPlace(type, to, nx, ny))
		{
			player->SetOffset(nx, ny);
			player->SetRotation(to);

			if (isLocking && lockResetCount < MAX_LOCK_RESET)
			{
				lockDelayTimer.Reset();
				lockResetCount++;
			}

			return;
		}
	}
}

void TetrisLevel::HardDrop()
{
	PieceType type = player->GetPieceType();
	int rotation = player->GetRotation();
	int x = player->GetOffsetX();
	int y = player->GetOffsetY();

	// 더 이상 내려갈 수 없을 때까지 이동
	while (board->CanPlace(type, rotation, x, y + 1))
	{
		y++;
	}

	// 최종 위치 적용
	player->SetOffset(x, y);

	// 즉시 고정
	board->PlacePiece(type, rotation, x, y);

	// 다음 블록 생성
	int randomType = Random::Random((int)PieceType::I, (int)PieceType::Z);
	player->Spawn((PieceType)randomType);

	// Lock 상태 초기화
	isLocking = false;
	lockDelayTimer.Reset();
}

void TetrisLevel::LockCheck(float deltatime)
{
	bool canMoveDown = board->CanPlace(player->GetPieceType(),
		player->GetRotation(),
		player->GetOffsetX(),
		player->GetOffsetY() + 1);

	if (canMoveDown)
	{
		// 아래로 이동 가능 → Lock 상태 초기화
		isLocking = false;
		lockDelayTimer.Reset();
		lockResetCount = 0;
	}
	else
	{
		// 아래로 이동 불가 → Lock Delay 시작/진행
		if (!isLocking)
		{
			isLocking = true;
			lockResetCount = 0;
			lockDelayTimer.SetTargetTime(0.5f);
			lockDelayTimer.Reset();
		}
		else
		{
			lockDelayTimer.Tick(deltatime);
			if (lockDelayTimer.IsTimeOut())
			{
				board->PlacePiece(player->GetPieceType(),
					player->GetRotation(),
					player->GetOffsetX(),
					player->GetOffsetY());

				int randomType = Random::Random((int)PieceType::I, (int)PieceType::Z);
				player->Spawn((PieceType)randomType);

				isLocking = false;
				lockDelayTimer.Reset();
			}
		}
	}
}

int TetrisLevel::GetGhostY() const
{
	int ghostY = player->GetOffsetY();

	while (board->CanPlace(
		player->GetPieceType(),
		player->GetRotation(),
		player->GetOffsetX(),
		ghostY + 1))
	{
		ghostY++;
	}

	return ghostY;
}