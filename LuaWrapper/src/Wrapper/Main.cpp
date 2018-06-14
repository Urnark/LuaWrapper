#include "FunctionWrapper\LuaFunctionsWrapper.h"

#include <iostream>

struct Point : LFW::ReturnToLua<int, int, float>
{
	int x;
	int y;
	float z;
	// Need to set which variables that shall be returned
	void SetReturnVariables() {
		LFW_SetReturnValues(x, y, z);
	};
};

class Test : public LFW::ILuaMember
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
		std::cout << "constFunc: void ConstFunc() const" << std::endl;
	};

	static void SFunc() {
		std::cout << "SFunc2: None" << std::endl;
	};

	Point p;
};

static void SFunc() {
	std::cout << "SFunc: None" << std::endl;
};
void Func() {
	std::cout << "Func: None" << std::endl;
};

// TODO: clean code

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1062);

	LFW::LuaManager::DEBUG_FLAGS = LFW::ERRORS | LFW::DEBUG_PRINTS | LFW::FUNCTION_CALLS | LFW::REGISTER_FUNCTIONS;
	LFW::LuaManager::InitLuaManager();
	LFW::LuaManager::CreateLuaState("TestLuaState");

	std::vector<Test> ts = { Test(), Test() };
	for (Test& t : ts)
	{
		LFW_RegisterCObjectFunctions(t,
			LFW_function("Print", Test::Print), 
			LFW_function("GetPoint", Test::GetPoint), 
			LFW_function("GetPoint2", Test::GetPoint2)
		);
	}

	LFW::LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);

	LFW::LuaManager::LoadScript("src/Wrapper/TestScripts/Test1.lua");
	LFW::LuaManager::LoadScript("TestLuaState", "src/Wrapper/TestScripts/Test2.lua");

	LFW::LuaFunctionsWrapper::RegisterCFunction("Testing3", &ts[0], &Test::Testing3);
	LFW::LuaFunctionsWrapper::RegisterCFunction("ConstFunc", &ts[0], &Test::ConstFunc);
	LFW::LuaFunctionsWrapper::RegisterCFunction("Func", &Func);
	LFW::LuaFunctionsWrapper::RegisterCFunction("SFunc", &SFunc);
	LFW::LuaFunctionsWrapper::RegisterCFunction("SFunc2", &Test::SFunc);
	

	LFW::LuaManager::UseLuaState("TestLuaState");
	LFW::LuaFunctionsWrapper::RegisterCFunction("Testing", &ts[0], &Test::Testing);
	LFW::LuaFunctionsWrapper::RegisterCFunction("Testing2", &ts[0], &Test::Testing2);
	
	// Call lua function
	LFW::LuaManager::UseLuaState("Init");
	LFW::LuaManager::CallLuaFunction<void>("HelloWorld");
	
	ts[0].p.z = 100.0f;
	LFW::LuaManager::CallLuaFunction<void>("Update", &ts[0]);
	LFW::LuaManager::CallLuaFunction<void>("Update", &ts[1]);

	int ret1;
	std::string ret2;
	if (LFW::LuaManager::CallLuaFunction<int, 2>("foo", 15) > 0)
	{
		ret2 = LFW::LuaManager::GetString();
		std::cout << "Returning: " << ret2 << std::endl;
	}
	else
		lua_pop(LFW::LuaManager::GetCurrentState(), -1);

	LFW::LuaManager::CallLuaFunction<2>("Testing2");
	LFW::LuaManager::GetAll(ret1, ret2);
	std::cout << "Returning: " << ret1 << ", " << ret2 << std::endl;

	LFW::LuaManager::UseLuaState("TestLuaState");
	std::cout << "Return form HelloWorld in Test2.lua: " << LFW::LuaManager::CallLuaFunction<std::string>("HelloWorld") << std::endl;

	std::system("pause");

	LFW::LuaManager::CloseLuaManager();
	return 0;
}