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

	LuaManager::DEBUG_FLAGS = LFW::ERRORS;
	LuaManager::InitLuaManager();

	LuaManager::LoadScript("src/logic/Test.lua");

	LuaFunctionsWrapper::RegisterCFunction("foo", &foo);

	LuaManager::CallLuaFunction<3>("Update");
	int ret1;
	float ret2;
	std::string ret3;
	LuaManager::GetAll(ret1, ret2, ret3);
	std::cout << "foo returning: " << ret1 << ", " << ret2 << ", " << ret3 << std::endl;

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}