
#include "FunctionWrapper\LuaFunctionsWrapper.h"

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

// TODO: remove placeholders from function call

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(189);

	LuaManager::InitLuaManager();
	LuaManager::AddLuaState("TestLuaState");

	Test t;
	Test t2;
	luaL_Reg mFuncList[] = {
		{ "Print", LuaFunctionsWrapper::GetRegisterFunction("Print", &t, &Test::Print, _1) },
		{ NULL, NULL }
	};
	LuaFunctionsWrapper::RegisterCObject(&t, mFuncList);

	luaL_Reg mFuncList2[] = {
		{ "Print", LuaFunctionsWrapper::GetRegisterFunction("Print", &t2, &Test::Print, _1) },
		{ NULL, NULL }
	};
	LuaFunctionsWrapper::RegisterCObject(&t2, mFuncList2);

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
	LuaManager::CallLuaFunc<void>("Update", &t2);

	LuaManager::SetState("TestLuaState");
	LuaManager::CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}