#include "LogicManager.h"

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>
#include "../../GameDef.h"
#include "../utils/keyboard/Keyboard.h"

using namespace lw;

void LogicManager::init()
{
	LuaFunctionsWrapper::RegisterCFunction("CreatePlayer", &entityManager, &EntityManager::CreatePlayer);
	LuaFunctionsWrapper::RegisterCFunction("GetPlayer", &entityManager, &EntityManager::GetPlayer);
	LuaFunctionsWrapper::RegisterCFunction("GetEntity", &entityManager, &EntityManager::GetEntity);
	LuaFunctionsWrapper::RegisterCFunction("GetNrOfEntities", &entityManager, &EntityManager::GetNrOfEntities);
	LuaFunctionsWrapper::RegisterCFunction("CreateWall", &entityManager, &EntityManager::CreateWall);

	// Register keyboard input
	LuaFunctionsWrapper::RegisterCFunction("IsKeyPressed", &Keyboard::IsKeyPressed);
	LuaFunctionsWrapper::RegisterCFunction("IsKeyReleased", &Keyboard::IsKeyReleased);
	LuaFunctionsWrapper::RegisterCFunction("IsKeyClicked", &Keyboard::IsKeyClicked);

	start();
}

void LogicManager::start()
{
	LuaManager::LoadScript("init/init.lua");

	LuaManager::CallLuaFunction<void>("InitWorld", WORLD_WIDTH, WORLD_HEIGHT);

	LuaManager::LoadScript("logic/test.lua");
}

LogicManager::LogicManager()
{
	LuaManager::DEBUG_FLAGS = ERRORS | DEBUG_PRINTS;
	LuaManager::InitLuaManager();
	LuaManager::SetLuaBaseDirectory(SCRIPTS_DIRECTORY_PATH);
	LuaManager::SetLuaLibsDirectory(LUA_LIBS_DIRECTORY_PATH);

	// Open the utils library
	LuaManager::LoadLib("utils.lua");

	init();

}

LogicManager::~LogicManager()
{
	LuaManager::CloseLuaManager();
}

void LogicManager::Update(float dt)
{
	LuaManager::CallLuaFunction<void>("WorldUpdate", dt);
	//Player* p = LuaManager::CallLuaFunction<Player*>("WorldUpdate", dt);
	//std::cout << "Player: " << p->GetPosition().x << ", " << p->GetPosition().y << std::endl;
	
	Keyboard::Update();
}

void LogicManager::Draw(sf::RenderTarget& target)
{
	entityManager.Draw(target);
}