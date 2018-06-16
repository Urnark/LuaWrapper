#include "Wall.h"

Wall::Wall(float x, float y) : Entity(x, y)
{
	shape.setSize(sf::Vector2f(30.0f, 30.0f));
	shape.setFillColor(sf::Color(0, 0, 255, 255));
}

Wall::~Wall()
{
}