#include "Actor.h"
#include <iostream>
#include <Windows.h>

namespace Wanted
{
	Actor::Actor(const char image, const Vector2& position)
		:image(image), position(position)
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
	}

	void Actor::Tick(float deletaTime)
	{

	}

	void Actor::Draw()
	{
		COORD coord = {static_cast<SHORT>(position.x), static_cast<SHORT>(position.y)};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		std::cout << image;
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		COORD coord = { static_cast<SHORT>(position.x), static_cast<SHORT>(position.y) };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

		std::cout << ' ';
		position = newPosition;
	}
}