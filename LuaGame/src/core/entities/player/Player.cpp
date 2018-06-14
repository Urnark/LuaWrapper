#include "Player.h"

Player::Player(float x, float y) : Entity(x, y)
{
	shape.setSize(sf::Vector2f(30.0f, 30.0f));
	shape.setFillColor(sf::Color(0, 255, 0, 255));
}

Player::~Player()
{
}

void Player::Print()
{
	std::cout << "Hello I am the player" << std::endl;
}
