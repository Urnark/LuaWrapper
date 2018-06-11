
#include "FunctionWrapper\LuaFunctionsWrapper.h"

#include <iostream>

class Point : public RetValues<int, int, float>
{
public:
	Point() : RetValues(0.0f, 0, 0) {};
	Point(int x, int y, float z) : RetValues(x, y, z) { this->x = x; this->y = y; this->z = z; };
	int x;
	int y;
	float z;
};

class Test : public ILuaMember
{
public:
	void Testing(int t) {
		std::cout << "Testing: " << t << std::endl;
	};
	void Print(std::string str) {
		std::cout << "Print: " << str << std::endl;
	};
	void Print2(std::string str) {
		std::cout << "Print2: " << str << std::endl;
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
	Point GetPoint(int x, int y) {
		std::cout << "GetPoint: " << x << ", " << y << ", " << 4 << std::endl;
		return Point(x, y, 4);
	};
};

// TODO: Be able to call static functions and const functions, remove luaObject and use ID, clean code

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(189);

	LuaManager::InitLuaManager();
	LuaManager::AddLuaState("TestLuaState");

	std::vector<Test> ts = { Test(), Test() };
	for (Test& t : ts)
	{
		luaL_Reg mFuncList[] = {
			{ "Print", LuaFunctionsWrapper::GetRegisterFunction("Print", &t, &Test::Print) },
		{ NULL, NULL }
		};
		LuaFunctionsWrapper::RegisterCObject(&t, mFuncList);
	}

	LuaManager::LoadScript("Wrapper/TestScripts/Test1.lua");
	LuaManager::LoadScript("TestLuaState", "Wrapper/TestScripts/Test2.lua");

	LuaFunctionsWrapper::RegisterCFunction("Testing3", &ts[0], &Test::Testing3);
	LuaFunctionsWrapper::RegisterCFunction("GetPoint", &ts[0], &Test::GetPoint);

	LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);
	LuaManager::SetState("TestLuaState");
	LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);
	LuaFunctionsWrapper::RegisterCFunction("Testing2", &ts[0], &Test::Testing2);
	
	// Call lua function
	LuaManager::SetState("Init");
	LuaManager::CallLuaFunc<void>("HelloWorld");

	LuaManager::CallLuaFunc<void>("Update", &ts[0]);
	LuaManager::CallLuaFunc<void>("Update", &ts[1]);

	LuaManager::SetState("TestLuaState");
	LuaManager::CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}