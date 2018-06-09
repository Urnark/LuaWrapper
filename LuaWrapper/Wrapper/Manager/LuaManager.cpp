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

std::vector<lua_State*> LuaManager::mLs;
std::vector<std::string> LuaManager::mMetaTables;
unsigned int LuaManager::mCurrentState = 0;
std::map<std::string, unsigned int> LuaManager::mLuaStateMap;

LuaManager::LuaManager()
{
}

LuaManager::~LuaManager()
{
}

void LuaManager::InitLuaManager(const std::string & pLuaStateName)
{
	CloseLuaManager();
	mLs.push_back(luaL_newstate());
	luaL_openlibs(mLs[mLs.size() - 1]);
	mCurrentState = 0;
	mLuaStateMap[pLuaStateName] = mLs.size() - 1;
}

void LuaManager::CloseLuaManager()
{
	for (int i = 0; i < mLs.size(); i++)
	{
		lua_close(mLs[i]);
	}
}

void LuaManager::SetState(unsigned int pLuaStateIndex)
{
	if (pLuaStateIndex >= 0 && pLuaStateIndex < mLs.size())
		mCurrentState = pLuaStateIndex;
	else
		std::cout << "ERROR: Index [" << pLuaStateIndex << "] out of bounds for the lua state" << std::endl;
}

void LuaManager::SetState(const std::string & pLuaStateName)
{
	if (mLuaStateMap.find(pLuaStateName) != mLuaStateMap.end())
		mCurrentState = mLuaStateMap[pLuaStateName];
	else
		std::cout << "ERROR: The Lua state [" << pLuaStateName << "] do not exist in the LuaManager" << std::endl;
}

void LuaManager::AddLuaState(const std::string & pLuaStateName)
{
	mLs.push_back(luaL_newstate());
	luaL_openlibs(mLs[mLs.size() - 1]);
	mLuaStateMap[pLuaStateName] = mLs.size() - 1;
}

void LuaManager::LoadScript(const std::string & pPath)
{
	int error = luaL_loadfile(GetCurrentState(), pPath.c_str()) || lua_pcall(GetCurrentState(), 0, 0, 0);
	if (error)
	{
		std::cout << "ERROR: could not load lua script [" << pPath << "]" << std::endl;
		lua_pop(GetCurrentState(), 1);
		PrintStackSize();
	}
	else
	{
		if (LuaWrapperDefined::DEBUG)
			std::cout << "[" << pPath << "] Script loaded successfully" << std::endl;
	}
}

void LuaManager::LoadScript(const std::string & pLuaStateName, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	SetState(pLuaStateName);
	LoadScript(pPath);
	SetState(luaStateIndex);
}

void LuaManager::LoadScript(unsigned int pLuaStateIndex, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	SetState(pLuaStateIndex);
	LoadScript(pPath);
	SetState(luaStateIndex);
}

const char * LuaManager::GetFunctionNameFromLua()
{
	lua_Debug ar;
	if (!lua_getstack(GetCurrentState(), 0, &ar))  /* no stack frame? */
		ar.name = "?"; /* Some default ? */
	else
	{
		lua_getinfo(GetCurrentState(), "n", &ar);
		if (ar.name == NULL)
			ar.name = "?"; /* <-- PUT DEFAULT HERE */
	}
	return ar.name;
}

void LuaManager::CallLuaFunction(const std::string & pFuncName)
{
	lua_getglobal(GetCurrentState(), pFuncName.c_str());
	CallLuaFunction(pFuncName, 0, 0);
}

void LuaManager::CallLuaFunction(const std::string & pFuncName, int pArg, int pResults)
{
	if (pArg == 0 && pResults != 0)
		lua_getglobal(GetCurrentState(), pFuncName.c_str());
	int error = lua_pcall(GetCurrentState(), pArg, pResults, 0);
	if (error)
	{
		std::cout << "ERROR: could not call function: [" << pFuncName << "]" << std::endl;
		lua_pop(GetCurrentState(), 1);
		PrintStackSize();
	}
}

lua_State * LuaManager::GetCurrentState()
{
	return mLs[mCurrentState];
}

unsigned int LuaManager::GetCurrentStateIndex()
{
	return mCurrentState;
}

void LuaManager::PushInteger(int pInteger)
{
	lua_pushinteger(GetCurrentState(), pInteger);
}

void LuaManager::PushFloat(float pFloat)
{
	lua_pushnumber(GetCurrentState(), pFloat);
}

void LuaManager::PushString(std::string pString)
{
	lua_pushstring(GetCurrentState(), pString.c_str());
}

void LuaManager::PushBool(bool pBool)
{
	lua_pushboolean(GetCurrentState(), pBool);
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
	LUA_GETTOP(integer, GetCurrentState());
	return integer;
}

float LuaManager::GetFloat()
{
	float number;
	LUA_GETTOP(number, GetCurrentState());
	return number;
}

std::string LuaManager::GetString()
{
	std::string string;
	LUA_GETTOP(string, GetCurrentState());
	return string;
}

bool LuaManager::GetBool()
{
	bool boolean;
	LUA_GETTOP(boolean, GetCurrentState());
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
	LUA_GETTOP_I(integer, GetCurrentState(), index);
	return integer;
}

float LuaManager::GetFloat(int params)
{
	int index = 0 - params;
	float number;
	LUA_GETTOP_I(number, GetCurrentState(), index);
	return number;
}

std::string LuaManager::GetString(int params)
{
	int index = 0 - params;
	std::string string;
	LUA_GETTOP_I(string, GetCurrentState(), index);
	return string;
}

bool LuaManager::GetBool(int params)
{
	int index = 0 - params;
	bool boolean;
	LUA_GETTOP_I(boolean, GetCurrentState(), index);
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
	if (luaL_newmetatable(GetCurrentState(), metatable.c_str()) == 0)
		std::cout << "ERROR: metatable with name [" << metatable << "] already exists" << std::endl;
	else
	{
		mMetaTables.push_back(metatable);
		luaL_setfuncs(GetCurrentState(), sMonsterRegs, 0);
		lua_pushvalue(GetCurrentState(), -1);
		lua_setfield(GetCurrentState(), -1, "__index");
		// Testing
		lua_pushstring(GetCurrentState(), "objName");
		lua_pushstring(GetCurrentState(), pObjectName.c_str());
		lua_settable(GetCurrentState(), -3);
		////////////
		lua_setglobal(GetCurrentState(), pObjectName.c_str());
	}
}

void LuaManager::PrintStackSize()
{
	std::cout << "Size of Lua stack: " << lua_gettop(GetCurrentState()) << std::endl;
}

const char * LuaManager::GetType()
{
	return GetType(-1);
}

const char * LuaManager::GetType(int index)
{
	return luaL_typename(LuaManager::GetCurrentState(), index);
}
