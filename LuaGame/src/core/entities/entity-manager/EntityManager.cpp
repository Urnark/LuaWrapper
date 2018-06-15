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

void EntityManager::CreatePlayer(float x, float y, float speed)
{
	player = new Player(x, y, speed);

	LW_RegisterCObjectFunctions(*player,
		LW_function2("GetPosition", Entity, Player::GetPosition),
		LW_function2("SetPosition", Entity, Player::SetPosition),
		LW_function("GetSpeed", Player::GetSpeed),
		LW_function("SetSpeed", Player::SetSpeed)
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