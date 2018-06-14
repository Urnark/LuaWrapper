#include <Wrapper\FunctionWrapper\LuaFunctionsWrapper.h>

#include <iostream>

void foo() {
	std::cout << "Hello from function foo" << std::endl;
};

using namespace LFW;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1062);

	LuaManager::DEBUG_FLAGS = LFW::ERRORS | LFW::DEBUG_PRINTS | LFW::FUNCTION_CALLS | LFW::REGISTER_FUNCTIONS;
	LuaManager::InitLuaManager();

	LuaManager::LoadScript("src/logic/Test.lua");

	LuaFunctionsWrapper::RegisterCFunction("foo", &foo);

	LuaManager::CallLuaFunction<void>("Update");

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}