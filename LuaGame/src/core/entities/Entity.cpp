#include "Entity.h"

Entity::Entity(float x, float y)
{
	position.x = x;
	position.y = y;
}

Entity::~Entity()
{
}

void Entity::Draw(sf::RenderTarget & target)
{
	shape.setPosition(position);
	target.draw(shape);
}
