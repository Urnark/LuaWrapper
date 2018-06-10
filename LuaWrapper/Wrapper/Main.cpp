
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
	void Testing3() {
		std::cout << "Testing3: void"<< std::endl;
	};
	void Testing4(int x, int y) {
		std::cout << "Testing4: " << x << ", " << y << std::endl;
	};
};

// TODO: registerObject can register a function that has already been registered

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(189);

	LuaManager::InitLuaManager();
	LuaManager::AddLuaState("TestLuaState");

	Test t;
	Test t2;
	luaL_Reg mFuncList[] = {
		{ "Print", LuaFunctionsWrapper::GetRegisterFunction("Print", &t, &Test::Print) },
		{ NULL, NULL }
	};
	LuaFunctionsWrapper::RegisterCObject(&t, mFuncList);

	luaL_Reg mFuncList2[] = {
		{ "Print", LuaFunctionsWrapper::GetRegisterFunction("Print", &t2, &Test::Print) },
		{ NULL, NULL }
	};
	LuaFunctionsWrapper::RegisterCObject(&t2, mFuncList2);

	LuaManager::LoadScript("Wrapper/TestScripts/Test1.lua");
	LuaManager::LoadScript("TestLuaState", "Wrapper/TestScripts/Test2.lua");

	LuaFunctionsWrapper::RegisterCFunction("Testing3", &t, &Test::Testing3);
	LuaFunctionsWrapper::RegisterCFunction("Testing4", &t, &Test::Testing4);

	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing);
	LuaManager::SetState("TestLuaState");
	LuaFunctionsWrapper::RegisterCFunction("Testing", &t, &Test::Testing);
	LuaFunctionsWrapper::RegisterCFunction("Testing2", &t, &Test::Testing2);

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