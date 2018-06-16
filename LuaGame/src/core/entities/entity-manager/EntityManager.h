#pragma once

#include <SFML\Graphics.hpp>

#include "../Entity.h"
#include "../player/Player.h"
#include "../wall/Wall.h"

#include <vector>

class EntityManager : public lw::ILuaMember
{
private:
	std::vector<Entity*> entities;
	Player* player;

private:
	void registerFunctions();

public:
	EntityManager();
	~EntityManager();

	void CreatePlayer(float x, float y, float speed);
	Player* GetPlayer();

	void CreateWall(float x, float y);

	Entity* GetEntity(int index) const;
	int GetNrOfEntities() const;

	void Update(float dt);
	void Draw(sf::RenderTarget& target);
};

