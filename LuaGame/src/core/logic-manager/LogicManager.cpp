#include "LogicManager.h"

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>
#include "../../GameDef.h"

using namespace lw;

void LogicManager::init()
{
	LuaFunctionsWrapper::RegisterCFunction("CreatePlayer", &entityManager, &EntityManager::CreatePlayer);

	start();
}

void LogicManager::start()
{
	LuaManager::LoadScript(SCRIPTS_PATH_INIT + "init.lua");

	LuaManager::CallLuaFunction<void>("InitWorld", WORLD_WIDTH, WORLD_HEIGHT);

	LuaManager::LoadScript(SCRIPTS_PATH_LOGIC + "Test.lua");
}

LogicManager::LogicManager()
{
	LuaManager::DEBUG_FLAGS = lw::ERRORS;
	LuaManager::InitLuaManager();

	init();
}

LogicManager::~LogicManager()
{
	LuaManager::CloseLuaManager();
}

void LogicManager::Update(sf::RenderTarget& target)
{
	LuaManager::CallLuaFunction<void>("Update", entityManager.GetPlayer());
	entityManager.Draw(target);
}