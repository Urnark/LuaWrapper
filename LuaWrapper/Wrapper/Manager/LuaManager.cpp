#include "LuaManager.h"

// PE = Print Error
#define PE_LUA_TOP(type) std::cout << "ERROR: not a " << typeid(type).name() << " on top of the stack [" << __func__ << "]" << std::endl;

#define LUA_GETTOP_I(var, L, i){\
	if (!lua_is##var(L, i))\
		PE_LUA_TOP(var);\
	var = lua_to##var(L, i);\
	lua_remove(L, i);\
	return var;\
}\

#define LUA_GETTOP(var, L) LUA_GETTOP_I(var, L, -1)

lua_State * LuaManager::mL = nullptr;
std::vector<std::string> LuaManager::mMetaTables;

LuaManager::LuaManager()
{
}

LuaManager::~LuaManager()
{
}

void LuaManager::InitLuaManager()
{
	mL = luaL_newstate();
	luaL_openlibs(mL);
}

void LuaManager::CloseLuaManager()
{
	lua_close(mL);
}

void LuaManager::LoadScript(const std::string & pPath)
{
	int error = luaL_loadfile(mL, pPath.c_str()) || lua_pcall(mL, 0, 0, 0);
	if (error)
	{
		std::cout << "ERROR: could not load lua script [" << pPath << "]" << std::endl;
		lua_pop(mL, 1);
		PrintStackSize();
	}
	else
	{
		if (Defined::DEBUG)
			std::cout << "[" << pPath << "] Script loaded successfully" << std::endl;
	}
}

void LuaManager::CallLuaFunction(const std::string & pFuncName)
{
	lua_getglobal(mL, pFuncName.c_str());
	CallLuaFunction(pFuncName, 0, 0);
}

void LuaManager::CallLuaFunction(const std::string & pFuncName, int pArg, int pResults)
{
	if (pArg == 0 && pResults != 0)
		lua_getglobal(mL, pFuncName.c_str());
	int error = lua_pcall(mL, pArg, pResults, 0);
	if (error)
	{
		std::cout << "ERROR: could not call function: [" << pFuncName << "]" << std::endl;
		lua_pop(mL, 1);
		PrintStackSize();
	}
}

lua_State * LuaManager::GetCurrentState()
{
	return mL;
}

void LuaManager::PushInteger(int pInteger)
{
	lua_pushinteger(mL, pInteger);
}

void LuaManager::PushFloat(float pFloat)
{
	lua_pushnumber(mL, pFloat);
}

void LuaManager::PushString(std::string pString)
{
	lua_pushstring(mL, pString.c_str());
}

void LuaManager::PushBool(bool pBool)
{
	lua_pushboolean(mL, pBool);
}

void LuaManager::PushInteger(lua_State *& pL, int pInteger)
{
	lua_pushinteger(pL, pInteger);
}

void LuaManager::PushFloat(lua_State *& pL, float pFloat)
{
	lua_pushnumber(pL, pFloat);
}

void LuaManager::PushString(lua_State *& pL, std::string pString)
{
	lua_pushstring(pL, pString.c_str());
}

void LuaManager::PushBool(lua_State *& pL, bool pBool)
{
	lua_pushboolean(pL, pBool);
}

int LuaManager::GetInteger()
{
	int integer;
	LUA_GETTOP(integer, mL);
	return integer;
}

float LuaManager::GetFloat()
{
	float number;
	LUA_GETTOP(number, mL);
	return number;
}

std::string LuaManager::GetString()
{
	std::string string;
	LUA_GETTOP(string, mL);
	return string;
}

bool LuaManager::GetBool()
{
	bool boolean;
	LUA_GETTOP(boolean, mL);
	return boolean;
}

int LuaManager::GetInteger(lua_State *& pL)
{
	int integer;
	LUA_GETTOP(integer, pL);
	return integer;
}

float LuaManager::GetFloat(lua_State *& pL)
{
	float number;
	LUA_GETTOP(number, pL);
	return number;
}

std::string LuaManager::GetString(lua_State *& pL)
{
	std::string string;
	LUA_GETTOP(string, pL);
	return string;
}

bool LuaManager::GetBool(lua_State *& pL)
{
	bool boolean;
	LUA_GETTOP(boolean, pL);
	return boolean;
}

/////////////////////////////////////////////////////////
int LuaManager::GetInteger(int params)
{
	int index = 0 - params;
	int integer;
	LUA_GETTOP_I(integer, mL, index);
	return integer;
}

float LuaManager::GetFloat(int params)
{
	int index = 0 - params;
	float number;
	LUA_GETTOP_I(number, mL, index);
	return number;
}

std::string LuaManager::GetString(int params)
{
	int index = 0 - params;
	std::string string;
	LUA_GETTOP_I(string, mL, index);
	return string;
}

bool LuaManager::GetBool(int params)
{
	int index = 0 - params;
	bool boolean;
	LUA_GETTOP_I(boolean, mL, index);
	return boolean;
}

std::string LuaManager::GetMetaTable(const std::string & pObjectName)
{
	return "Meta" + pObjectName;
}

std::string LuaManager::GetMetaTableAndCheck(const std::string & pObjectName)
{
	std::string ret = "";
	for (std::string str : mMetaTables)
	{
		if (str.find(pObjectName) != std::string::npos)
		{
			ret = str;
		}
	}
	return ret;
}

void LuaManager::RegisterObjectFunctions(const std::string & pObjectName, luaL_Reg sMonsterRegs[])
{
	std::string metatable = "Meta" + pObjectName;
	if (luaL_newmetatable(mL, metatable.c_str()) == 0)
		std::cout << "ERROR: metatable with name [" << metatable << "] already exists" << std::endl;
	else
	{
		mMetaTables.push_back(metatable);
		luaL_setfuncs(mL, sMonsterRegs, 0);
		lua_pushvalue(mL, -1);
		lua_setfield(mL, -1, "__index");
		lua_setglobal(mL, pObjectName.c_str());
	}
}

void LuaManager::PrintStackSize()
{
	std::cout << "Size of Lua stack: " << lua_gettop(mL) << std::endl;
}
