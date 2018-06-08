
#include "FunctionWrapper\LuaFunctionsWrapper.h"

static const bool DEBUG = true;

#include <iostream>

class Test : public ILuaMember
{
public:
	void Testing(int t) {
		std::cout << "Hello, value: " << t << std::endl;
	};
};

// TODO: Add more Lua_Sates

int main()
{
	LuaManager* luaManager = new LuaManager();
	luaManager->InitLuaManager();
	
	luaManager->LoadScript("Wrapper/TestScripts/Test1.lua");

	Test t;
	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing, _1);

	// Call lua function
	luaManager->CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	luaManager->CloseLuaManager();
	delete luaManager;
	return 0;
}