#pragma once

#include <SFML\Window\Keyboard.hpp>

class Keyboard
{
private:
	static bool pressedKeys[sf::Keyboard::KeyCount];

public:
	static void Update();
	static bool IsKeyClicked(int key);
	static bool IsKeyPressed(int key);
	static bool IsKeyReleased(int key);

};