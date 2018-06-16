#include "LuaManager.h"

/*
MIT License

Copyright (c) 2018 Urnark

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// PE = Print Error
#define PE_LUA_TOP(type) LW_PRINT_ERROR("not a " << typeid(type).name() << " on top of the stack [" << __func__ << "]")

#define LUA_GETTOP_I(var, L, i){\
	if (!lua_is##var(L, i))\
		PE_LUA_TOP(var);\
	var = (decltype(var))lua_to##var(L, i);\
	lua_remove(L, i);\
	return var;\
}\

#define LUA_GETTOP(var, L) LUA_GETTOP_I(var, L, -1)

namespace lw {

std::vector<lua_State*> LuaManager::mLs;
std::vector<std::string> LuaManager::mMetaTables;
unsigned int LuaManager::mCurrentState = 0;
std::map<std::string, unsigned int> LuaManager::mLuaStateMap;
std::string LuaManager::mLibsPath = "";
std::string LuaManager::mLuaBasePath = "";
int LuaManager::DEBUG_FLAGS = 0;

void LuaManager::_LoadScript(lua_State * pL, const std::string & pPath)
{
	int error = luaL_loadfile(pL, pPath.c_str()) || lua_pcall(pL, 0, 0, 0);
	if (error)
	{
		LW_PRINT_ERROR("could not load lua script [" << pPath << "]");
		lua_pop(pL, 1);
		PrintStackSize();
	}
	else
	{
		LW_PRINT_DEBUG_TEXT("[" << pPath << "] Script loaded successfully", DEBUG_PRINTS)
	}
}

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
	for (unsigned int i = 0; i < mLs.size(); i++)
	{
		lua_close(mLs[i]);
	}
}

void LuaManager::UseLuaState(unsigned int pLuaStateIndex)
{
	if (pLuaStateIndex >= 0 && pLuaStateIndex < mLs.size())
		mCurrentState = pLuaStateIndex;
	else
		LW_PRINT_ERROR("Index[" << pLuaStateIndex << "] out of bounds for the lua state")
}

void LuaManager::UseLuaState(const std::string & pLuaStateName)
{
	if (mLuaStateMap.find(pLuaStateName) != mLuaStateMap.end())
		mCurrentState = mLuaStateMap[pLuaStateName];
	else
		LW_PRINT_ERROR("The Lua state [" << pLuaStateName << "] do not exist in the LuaManager")
}

void LuaManager::CreateLuaState(const std::string & pLuaStateName)
{
	mLs.push_back(luaL_newstate());
	luaL_openlibs(mLs[mLs.size() - 1]);
	mLuaStateMap[pLuaStateName] = mLs.size() - 1;
}

void LuaManager::LoadLib(const std::string & pPath)
{
	_LoadScript(GetCurrentState(), mLibsPath + "/" + pPath.c_str());
}

void LuaManager::LoadLib(const std::string & pLuaStateName, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	UseLuaState(pLuaStateName);
	LoadLib(pPath);
	UseLuaState(luaStateIndex);
}

void LuaManager::LoadLib(unsigned int pLuaStateIndex, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	UseLuaState(pLuaStateIndex);
	LoadLib(pPath);
	UseLuaState(luaStateIndex);
}

void LuaManager::LoadScript(const std::string & pPath)
{
	_LoadScript(GetCurrentState(), mLuaBasePath + "/" + pPath.c_str());
}

void LuaManager::LoadScript(const std::string & pLuaStateName, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	UseLuaState(pLuaStateName);
	LoadScript(pPath);
	UseLuaState(luaStateIndex);
}

void LuaManager::LoadScript(unsigned int pLuaStateIndex, const std::string & pPath)
{
	unsigned int luaStateIndex = mCurrentState;
	UseLuaState(pLuaStateIndex);
	LoadScript(pPath);
	UseLuaState(luaStateIndex);
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

void LuaManager::CallLuaFun(const std::string & pFuncName)
{
	lua_getglobal(GetCurrentState(), pFuncName.c_str());
	CallLuaFun(pFuncName, 0, 0);
}

void LuaManager::CallLuaFun(const std::string & pFuncName, int pArg, int pResults)
{
	//if (pArg == 0 && pResults != 0)
		//lua_getglobal(GetCurrentState(), pFuncName.c_str());
	int error = lua_pcall(GetCurrentState(), pArg, pResults, 0);
	if (error)
	{
		LW_PRINT_ERROR("could not call function: [" << pFuncName << "]")
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

void LuaManager::SetLuaBaseDirectory(const std::string & path)
{
	mLuaBasePath = path;
	lua_getglobal(GetCurrentState(), "package");
	lua_getfield(GetCurrentState(), -1, "path");
	std::string cur_path = GetString();
	cur_path.append(";./");
	cur_path.append(path.c_str());
	cur_path.append("/?.lua");
	PushString(cur_path.c_str());
	lua_setfield(GetCurrentState(), -2, "path");
	lua_pop(GetCurrentState(), 1);
}

void LuaManager::SetLuaLibsDirectory(const std::string & path)
{
	mLibsPath = path;
	lua_getglobal(GetCurrentState(), "package");
	lua_getfield(GetCurrentState(), -1, "path");
	std::string cur_path = GetString();
	cur_path.append(";./");
	cur_path.append(path.c_str());
	cur_path.append("/?.lua");
	PushString(cur_path.c_str());
	lua_setfield(GetCurrentState(), -2, "path");
	lua_pop(GetCurrentState(), 1);
}

void LuaManager::Pop(int index)
{
	lua_pop(GetCurrentState(), index);
}

void LuaManager::RemoveFromStack(int index)
{
	lua_remove(GetCurrentState(), index);
}

void LuaManager::PushInteger(int pInteger)
{
	lua_pushinteger(GetCurrentState(), pInteger);
}

void LuaManager::PushFloat(float pFloat)
{
	lua_pushnumber(GetCurrentState(), pFloat);
}

void LuaManager::PushDouble(double pDouble)
{
	lua_pushnumber(GetCurrentState(), pDouble);
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

void LuaManager::PushDouble(lua_State *& pL, double pDouble)
{
	lua_pushnumber(pL, pDouble);
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

double LuaManager::GetDouble()
{
	double number;
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

double LuaManager::GetDouble(lua_State *& pL)
{
	double number;
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

double LuaManager::GetDouble(int params)
{
	int index = 0 - params;
	double number;
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

void LuaManager::RegisterObjectFunctions(ILuaMember* member, luaL_Reg sMonsterRegs[])
{
	std::string metatable = "Meta" + std::to_string(member->GetID());
	if (luaL_newmetatable(GetCurrentState(), metatable.c_str()) == 0) {
		LW_PRINT_ERROR("metatable with name [" << metatable << "] already exists")
	}
	else
	{
		mMetaTables.push_back(metatable);
		luaL_setfuncs(GetCurrentState(), sMonsterRegs, 0);
		lua_pushvalue(GetCurrentState(), -1);
		lua_setfield(GetCurrentState(), -1, "__index");
		// The id of the ILuaMember
		lua_pushstring(GetCurrentState(), "id");
		lua_pushstring(GetCurrentState(), std::to_string(member->GetID()).c_str());
		lua_settable(GetCurrentState(), -3);
		////////////
		// For holding the ptr to the ILuaMember
		lua_pushstring(GetCurrentState(), "ptr");
		lua_pushlightuserdata(LuaManager::GetCurrentState(), member);
		lua_settable(GetCurrentState(), -3);
		////////////
		lua_setglobal(GetCurrentState(), std::to_string(member->GetID()).c_str());
	}
}

void LuaManager::PrintStackSize()
{
	LW_PRINT_DEBUG_TEXT("Size of Lua stack: " << StackSize(), DEBUG_PRINTS)
}

int LuaManager::StackSize()
{
	return lua_gettop(GetCurrentState());
}

const char * LuaManager::GetType()
{
	return GetType(-1);
}

const char * LuaManager::GetType(int index)
{
	return luaL_typename(LuaManager::GetCurrentState(), index);
}

};