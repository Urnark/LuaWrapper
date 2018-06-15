#include "Player.h"

Player::Player(float x, float y, float speed) : Entity(x, y)
{
	shape.setSize(sf::Vector2f(30.0f, 30.0f));
	shape.setFillColor(sf::Color(0, 255, 0, 255));
	SetSpeed(speed);
}

Player::~Player()
{
}
