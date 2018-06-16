#include "EntityManager.h"

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>

#include "../../../GameDef.h"

void EntityManager::registerFunctions()
{
	//lw::LuaFunctionsWrapper::RegisterCFunction("GetEntity", this, &EntityManager::GetEntity);
	//lw::LuaFunctionsWrapper::RegisterCFunction("GetNrOfEntities", this, &EntityManager::GetNrOfEntities);
}

EntityManager::EntityManager()
{
	registerFunctions();
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

	LW_RegisterCObjectFunctions(player,
		LW_baseClassFunction("GetPosition", Entity, Player::GetPosition),
		LW_baseClassFunction("SetPosition", Entity, Player::SetPosition),
		LW_function("GetSpeed", Player::GetSpeed),
		LW_function("SetSpeed", Player::SetSpeed)
	);

	entities.push_back(player);
}

Player* EntityManager::GetPlayer()
{
	return player;
}

void EntityManager::CreateWall(float x, float y)
{
	entities.push_back(new Wall(x, y));
}

Entity * EntityManager::GetEntity(int index) const
{
	return entities[index];
}

int EntityManager::GetNrOfEntities() const
{
	return entities.size();
}

void EntityManager::Update(float dt)
{
	for (Entity* entity : entities)
	{
		if (dynamic_cast<Player*>(entity))
		{
			//lw::LuaManager::CallLuaFunction<void>("");
		}
	}
}

void EntityManager::Draw(sf::RenderTarget& target)
{
	for (Entity* entity : entities)
	{
		entity->Draw(target);
	}
}