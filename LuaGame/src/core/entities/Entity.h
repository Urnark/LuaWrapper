#pragma once

#include <SFML\Graphics.hpp>

#include "../utils/point/Point2f.h"

class Entity : public lw::ILuaMember
{
protected:
	sf::RectangleShape shape;
	Point2f position;

public:
	Entity(float x, float y);
	virtual ~Entity();

	Point2f GetPosition() const {
		return position;
	};

	void SetPosition(float x, float y) { 
		position.x = x; 
		position.y = y;
	}

	void Draw(sf::RenderTarget& target);
};

