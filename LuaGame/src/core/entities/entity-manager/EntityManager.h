#pragma once

#include <SFML\Graphics.hpp>

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>

#include "../Entity.h"
#include "../player/Player.h"

#include <vector>

class EntityManager : public lw::ILuaMember
{
private:
	std::vector<Entity*> entities;
	Player* player;

private:
	void loadLua();
	void registerFunctions();

public:
	EntityManager();
	~EntityManager();

	void CreatePlayer(float x, float y);
	Player* GetPlayer();
	void Draw(sf::RenderTarget& target);
};

