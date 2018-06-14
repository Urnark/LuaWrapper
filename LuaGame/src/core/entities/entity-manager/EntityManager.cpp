#include "EntityManager.h"

#include "../../../GameDef.h"

void EntityManager::loadLua()
{
	registerFunctions();
}

void EntityManager::registerFunctions()
{
}

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
	for (Entity* entity : entities)
	{
		delete entity;
	}
	entities.clear();
}

void EntityManager::CreatePlayer(float x, float y)
{
	player = new Player(x, y);

	LW_RegisterCObjectFunctions(*player,
		LW_function("Print", Player::Print),
		LW_function2("GetPosition", (Entity)*player, Player::GetPosition)
	);

	entities.push_back(player);
}

Player* EntityManager::GetPlayer()
{
	return player;
}

void EntityManager::Draw(sf::RenderTarget& target)
{
	for (Entity* entity : entities)
	{
		entity->Draw(target);
	}
}