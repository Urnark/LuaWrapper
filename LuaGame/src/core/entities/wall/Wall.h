#pragma once

#include "../Entity.h"

class Wall : public Entity
{

public:
	Wall(float x, float y);
	virtual ~Wall();
};