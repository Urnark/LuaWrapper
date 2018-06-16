#pragma once

#include <SFML\Graphics.hpp>

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>

class Point2f : public lw::ReturnToLua<float, float>
{
public:
	float x, y;
	Point2f() { x = 0.0f; y = 0.0F; }
	Point2f(float x, float y) { this->x = x; this->y = y; };

	sf::Vector2f getPos() const { return sf::Vector2f(x, y); };

	void SetReturnVariables() {
		LW_SetReturnValues(x, y);
	}
};