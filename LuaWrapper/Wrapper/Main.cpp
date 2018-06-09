
#include "FunctionWrapper\LuaFunctionsWrapper.h"

static const bool DEBUG = true;

#include <iostream>

class Test : public ILuaMember
{
public:
	void Testing(int t) {
		std::cout << "Testing: " << t << std::endl;
	};
	void Print(std::string str) {
		std::cout << "Print: " << str << std::endl;
	};
	bool Testing2(std::string str) {
		std::cout << "Testing2: " << str << std::endl;
		return true;
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
	LuaManager::InitLuaManager();
	LuaManager::AddLuaState("TestLuaState");

	Test t;
	/*LuaFunctionsWrapper::RegisterObject(&t);
	luaL_Reg functionList[] = {
		{ "CPrint", luaPrintC },
		{ NULL, NULL }
	};
	LuaManager::RegisterObjectFunctions(t.GetLuaObject(), functionList);*/

	LuaFunctionsWrapper::RegisterCFunction("Print", &t, &Test::Print, _1);
	LuaFunctionsWrapper::RegisterCObject("Print", &t, &Test::Print);

	LuaManager::LoadScript("Wrapper/TestScripts/Test1.lua");
	LuaManager::LoadScript("TestLuaState", "Wrapper/TestScripts/Test2.lua");

	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing, _1);
	LuaManager::SetState("TestLuaState");
	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing, _1);
	LuaFunctionsWrapper::RegisterCFunction("Testing2", &t, &Test::Testing2, _1);

	// Call lua function
	LuaManager::SetState("Init");
	LuaManager::CallLuaFunc<void>("HelloWorld");
	//LuaManager::CallLuaFunc<void>("Update", &t/*, t.GetLuaObject()*/);
	LuaManager::CallLuaFunc<void>("Update", &t);

	LuaManager::SetState("TestLuaState");
	LuaManager::CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}