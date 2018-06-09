
#include "FunctionWrapper\LuaFunctionsWrapper.h"

static const bool DEBUG = true;

#include <iostream>

class Test : public ILuaMember
{
public:
	void Testing(int t) {
		std::cout << "Hello, value: " << t << std::endl;
	};
	void Print(std::string str) {
		std::cout << "Hello, value: " << str << std::endl;
	};
};

static int luaPrintC(lua_State* L)
{
	std::string function = LuaManager::GetString();
	std::string luaObject = LuaManager::GetString();

	std::string str = LuaManager::GetString();

	ILuaMember* luaMember = LuaManager::GetObjectPtr<ILuaMember>(luaObject);
	LuaFunctionsWrapper::CallMemFunc<void>(std::to_string(LuaManager::GetCurrentStateIndex()) + LuaFunctionsWrapper::GenerateFuncName(function, luaMember), std::forward<std::string>(str));
	return 0;
}

// TODO: Add more Lua_Sates

int main()
{
	LuaManager* luaManager = new LuaManager();
	luaManager->InitLuaManager();

	Test t;
	LuaFunctionsWrapper::RegisterObject(&t);
	luaL_Reg functionList[] = {
		{ "CPrint", luaPrintC },
		{ NULL, NULL }
	};
	LuaManager::RegisterObjectFunctions(t.GetLuaObject(), functionList);

	luaManager->LoadScript("Wrapper/TestScripts/Test1.lua");

	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing, _1);
	LuaFunctionsWrapper::RegisterCFunction(LuaFunctionsWrapper::GenerateFuncName("Print", &t), &t, &Test::Print, _1);

	// Call lua function
	luaManager->CallLuaFunc<void>("HelloWorld");
	luaManager->CallLuaFunc<void>("Update", &t, t.GetLuaObject());

	luaManager->AddLuaState("TestLuaState");
	luaManager->SetState("TestLuaState");

	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing, _1);

	luaManager->LoadScript("Wrapper/TestScripts/Test2.lua");
	luaManager->CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	luaManager->CloseLuaManager();
	delete luaManager;
	return 0;
}