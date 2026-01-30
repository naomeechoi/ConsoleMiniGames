#include "Player.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Actor/Box.h"
#include "Level/Level.h"

#include <iostream>
#include <Windows.h>

Player::Player()
	: super('T', Wanted::Vector2(5,5), Wanted::Color::Red)
{
	sortingOrder = 10;
}

void Player::BeginPlay()
{
	Actor::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
	if (Wanted::Input::Get().GetKeyDown('Q'))
	{
		Wanted::Engine::Get().QuitEngine();
	}
	/*std::cout << "TestActor::Tick(). deltaTime: "
		<< deltaTime << ", FPS: "
		<< (1.0f / deltaTime) << std::endl;*/


	if (Wanted::Input::Get().GetKeyDown(VK_SPACE))
	{
		if (owner)
		{
			owner-> AddNewActor(new Box(GetPosition()));
		}
	}

	if (Wanted::Input::Get().GetKeyDown(VK_RIGHT) && GetPosition().x < 20)
	{
		Wanted::Vector2 newPosition = GetPosition();
		newPosition.x += 1;
		SetPosition(newPosition);
	}

	if (Wanted::Input::Get().GetKeyDown(VK_LEFT) && GetPosition().x > 0)
	{
		Wanted::Vector2 newPosition = GetPosition();
		newPosition.x -= 1;
		SetPosition(newPosition);
	}

	if (Wanted::Input::Get().GetKeyDown(VK_UP) && GetPosition().y > 0)
	{
		Wanted::Vector2 newPosition = GetPosition();
		newPosition.y -= 1;
		SetPosition(newPosition);
	}

	if (Wanted::Input::Get().GetKeyDown(VK_DOWN) && GetPosition().y < 20)
	{
		Wanted::Vector2 newPosition = GetPosition();
		newPosition.y += 1;
		SetPosition(newPosition);
	}
}

void Player::Draw()
{
	Actor::Draw();
}
