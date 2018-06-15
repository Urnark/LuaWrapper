#include <SFML\Graphics.hpp>
#include <Wrapper\Manager\LuaManager.h>

#include "core\logic-manager\LogicManager.h"
#include "GameDef.h"

#include <iostream>

// In LuaWrapper
// TODO: clean code

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1062);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	std::string title = "Lua Test Game!";
	sf::RenderWindow window(sf::VideoMode(WORLD_WIDTH, WORLD_HEIGHT), title, sf::Style::Default, settings);

	sf::Clock clock;

	LogicManager logicManager;
	float dt = 0;
	float time = 0;
	double accumulator = 0.0;

	window.setFramerateLimit(FPS);
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

		dt = clock.restart().asSeconds();
		float fps = 1.f / dt;
		time += dt;

		if (time >= 1.0f) {
			time = 0.0f;
			window.setTitle(title + ", FPS: " + std::to_string(fps));
		}

		logicManager.Update(dt);
		logicManager.Draw(window);

		window.display();
	}

	return 0;
}