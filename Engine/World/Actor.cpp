#include "Actor.h"
#include "Util/Console.h"
#include "Render/Renderer.h"

#include <iostream>
#include <Windows.h>

namespace MinigameEngine
{
	Actor::Actor(const char* image, const Vector2& position, Color color)
		: position(position), color(color)
	{
		width = (int)strlen(image);
		this->image = new char[width + 1];
		strcpy_s(this->image, width + 1, image);
	}

	Actor::~Actor()
	{
		if (image)
		{
			delete[] image;
			image = nullptr;
		}
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
		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		if (position == newPosition)
			return;

		position = newPosition;
	}
}