#pragma once

#include "../Entity.h"

class Player : public Entity
{
private:
	float speed;

public:
	Player(float x, float y, float speed);
	~Player();

	float GetSpeed() const { return speed; }
	void SetSpeed(float speed) { this->speed = speed; }
};

