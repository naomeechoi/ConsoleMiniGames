#include "Box.h"

Box::Box(const Vector2& newPosition)
	: super('B', newPosition, Color::Red)
{
	sortingOrder = 15;
}
