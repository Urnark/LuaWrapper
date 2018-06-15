#pragma once

#include "../entities/entity-manager/EntityManager.h"

#include <vector>

class LogicManager
{
private:
	EntityManager entityManager;

private:
	void init();
	void start();

public:
	LogicManager();
	~LogicManager();

	void Update(float dt);
	void Draw(sf::RenderTarget& target);
};

