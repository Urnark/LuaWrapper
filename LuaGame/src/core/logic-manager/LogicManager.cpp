#include "LogicManager.h"

#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>
#include "../../GameDef.h"
#include "../utils/Keyboard.h"

using namespace lw;

void LogicManager::init()
{
	LuaFunctionsWrapper::RegisterCFunction("CreatePlayer", &entityManager, &EntityManager::CreatePlayer);
	LuaFunctionsWrapper::RegisterCFunction("GetPlayer", &entityManager, &EntityManager::GetPlayer);

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
	LuaManager::DEBUG_FLAGS = lw::ERRORS;
	LuaManager::InitLuaManager();
	LuaManager::SetLuaBasePath(SCRIPTS_DIRECTORY_PATH);

	init();
}

LogicManager::~LogicManager()
{
	LuaManager::CloseLuaManager();
}

void LogicManager::Update(float dt)
{
	Player* p = LuaManager::CallLuaFunction<Player*>("Update", dt);
	//std::cout << "Player: " << std::get<0>(p->GetPosition().returnValues) << ", " << std::get<1>(p->GetPosition().returnValues) << std::endl;

	Keyboard::Update();
}

void LogicManager::Draw(sf::RenderTarget& target)
{
	entityManager.Draw(target);
}