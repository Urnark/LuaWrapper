#include <SFML\Graphics.hpp>
#include <Wrapper\Manager\LuaManager.h>

#include "core\logic-manager\LogicManager.h"
#include "GameDef.h"

#include <iostream>

// In LuaWrapper
// TODO: clean code, use _get to give Lua an instance of a ILuaMember

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1062);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WORLD_WIDTH, WORLD_HEIGHT), "Lua Test Game!", sf::Style::Default, settings);

	LogicManager logicManager;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		}

		window.clear();

		logicManager.Update(window);

		window.display();
	}

	return 0;
}