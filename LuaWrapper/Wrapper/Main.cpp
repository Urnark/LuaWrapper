
#define LUA_WRAPPER_DEBUG

#include "FunctionWrapper\LuaFunctionsWrapper.h"

#include <iostream>

struct Point : ReturnToLua<int, int, float>
{
	int x;
	int y;
	float z;
	// Need to set which variables that shall be returned
	void SetReturnVariables() {
		LFW_SetReturnValues(x, y, z);
	};
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

	LFW_ReturnType GetPoint(int x, int y) {
		std::cout << "GetPoint: " << x << ", " << y << ", " << 4 << std::endl;
		return LFW_ReturnValues(x, y, 4.0f);
	};
	Point GetPoint2(int x, int y) {
		std::cout << "GetPoint: " << x << ", " << y << ", " << 4 << std::endl;
		p.x = x;
		p.y = y;
		return p;
	};

	void ConstFunc() const {
		std::cout << "constFunc: None" << std::endl;
	};

	static void SFunc() {
		std::cout << "SFunc2: None" << std::endl;
	};

	Point p;
};

static void SFunc() {
	std::cout << "SFunc: None" << std::endl;
};

// TODO: clean code

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1062);

	LuaManager::InitLuaManager();
	LuaManager::AddLuaState("TestLuaState");

	std::vector<Test> ts = { Test(), Test() };
	for (Test& t : ts)
	{
		luaL_Reg mFuncList[] = {
			LFW_function("Print", t, Test::Print),
			LFW_function("GetPoint", t, Test::GetPoint),
			LFW_function("GetPoint2", t, Test::GetPoint2),
			{ NULL, NULL }
		};
		LuaFunctionsWrapper::RegisterCObject(&t, mFuncList);
	}

	LuaManager::LoadScript("Wrapper/TestScripts/Test1.lua");
	LuaManager::LoadScript("TestLuaState", "Wrapper/TestScripts/Test2.lua");

	LuaFunctionsWrapper::RegisterCFunction("Testing3", &ts[0], &Test::Testing3);
	LuaFunctionsWrapper::RegisterCFunction("ConstFunc", &ts[0], &Test::ConstFunc);
	LuaFunctionsWrapper::RegisterCFunction("SFunc", &SFunc);
	LuaFunctionsWrapper::RegisterCFunction("SFunc2", &Test::SFunc);
	
	LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);

	LuaManager::SetState("TestLuaState");
	LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);
	LuaFunctionsWrapper::RegisterCFunction("Testing2", &ts[0], &Test::Testing2);
	
	// Call lua function
	LuaManager::SetState("Init");
	LuaManager::CallLuaFunc<void>("HelloWorld");
	
	ts[0].p.z = 100.0f;
	LuaManager::CallLuaFunc<void>("Update", &ts[0]);
	LuaManager::CallLuaFunc<void>("Update", &ts[1]);

	LuaManager::SetState("TestLuaState");
	LuaManager::CallLuaFunc<void>("HelloWorld");

	std::system("pause");

	LuaManager::CloseLuaManager();
	return 0;
}