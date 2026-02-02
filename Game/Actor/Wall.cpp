#include "Wall.h"

Wall::Wall(const Vector2& position)
	:super('#', position, Color::White)
{
	// 그리기 우선순위 설정
	sortingOrder = 0;

}

Wall::~Wall()
{
}
