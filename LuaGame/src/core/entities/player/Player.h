#pragma once

#include "../Entity.h"

class Player : public Entity
{
public:
	Player(float x, float y);
	~Player();

	void Print();
};

