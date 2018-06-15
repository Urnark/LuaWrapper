#include "Keyboard.h"

bool Keyboard::pressedKeys[sf::Keyboard::KeyCount] = { false };

void Keyboard::Update()
{
	for (int key = 0; key < sf::Keyboard::KeyCount; key++)
		Keyboard::pressedKeys[key] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key);
}

bool Keyboard::IsKeyClicked(int key)
{
	bool clicked = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key) && !Keyboard::pressedKeys[key];
	Keyboard::pressedKeys[key] = false;
	return clicked;
}

bool Keyboard::IsKeyPressed(int key)
{
	return Keyboard::pressedKeys[key];
}

bool Keyboard::IsKeyReleased(int key)
{
	return !Keyboard::pressedKeys[key];
}
