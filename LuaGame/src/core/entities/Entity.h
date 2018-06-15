#pragma once

#include <SFML\Graphics.hpp>

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>

class Entity : public lw::ILuaMember
{
protected:
	sf::RectangleShape shape;
	sf::Vector2f position;

public:
	Entity(float x, float y);
	~Entity();

	LW_ReturnType GetPosition() const {
		return LW_ReturnValues(position.x, position.y);
	};

	void SetPosition(float x, float y) { 
		position.x = x; 
		position.y = y;
	}

	void Draw(sf::RenderTarget& target);
};

