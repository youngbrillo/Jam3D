#include "transform2d.hpp"

Rectangle jam::components::Transform2D::getRectangle() const
{
    return Rectangle{ position.x, position.y, size.x * scale, size.y * scale };
}

Rectangle jam::components::Transform2D::getAnchoredRectangle() const
{
	Rectangle destination = getRectangle();
	Vector2 origin = getOrigin();
	destination.x -= (origin.x);
	destination.y -= (origin.y);
	return destination;
}

Vector2 jam::components::Transform2D::getOrigin() const
{
	return size * scale * origin;
}

Vector2 jam::components::Transform2D::getHeading() const
{
	return Vector2{ sinf(angle * DEG2RAD), cosf(angle * DEG2RAD) };
}
