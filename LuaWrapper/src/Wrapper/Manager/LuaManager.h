#pragma once

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

#include "../../Lua/lua.hpp"

#include "../EntityHandeler/ILuaMember.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <tuple>

#define LW_PRINT_ERROR(error)\
if (lw::LuaManager::DEBUG_FLAGS & lw::ERRORS)\
	std::cout << "ERROR: " << error << "[Line " << __LINE__ <<"]"<< std::endl;

#define LW_PRINT_DEBUG_TEXT(text, flags)\
if (lw::LuaManager::DEBUG_FLAGS & flags)\
	std::cout << text << std::endl;

namespace lw {

	enum FLAGS {
		ERRORS = 1 << 0,
		FUNCTION_CALLS = 1 << 1,
		REGISTER_FUNCTIONS = 1 << 2,
		DEBUG_PRINTS = 1 << 3
	};

	class LuaManager
	{
	public:
		static int DEBUG_FLAGS;
	private:
		/* For iterating through a tuple and push its values to Lua */
		template<int index, typename... Args>
		struct iteratePushTuple {
			void operator() (std::tuple<Args...>& t) {
				const auto size = std::tuple_size<std::tuple<Args...>>::value;
				Push(std::get<size - 1 - index>(t));
				iteratePushTuple<index - 1, Args...>{}(t);
			}
		};

		template<typename... Args>
		struct iteratePushTuple<0, Args...> {
			void operator() (std::tuple<Args...>& t) {
				const auto size = std::tuple_size<std::tuple<Args...>>::value;
				Push(std::get<size - 1>(t));
			}
		};

		template<typename... Args>
		static void PushFromTuple(std::tuple<Args...>& t) {
			const auto size = std::tuple_size<std::tuple<Args...>>::value;
			iteratePushTuple<size - 1, Args...>{}(t);
		}

		/* For iterating through a tuple and get its values from Lua */
		template<int index, typename... Args>
		struct iterateGetTuple {
			void operator() (std::tuple<Args...> t, std::tuple<Args&...>& tu) {
				typedef std::remove_reference<decltype(std::get<index>(t))>::type type;
				std::get<index>(tu) = Get<type>(GetCurrentState());
				iterateGetTuple<index - 1, Args...>{}(t, tu);
			}
		};

		template<typename... Args>
		struct iterateGetTuple<0, Args...> {
			void operator() (std::tuple<Args...> t, std::tuple<Args&...>& tu) {
				const auto size = std::tuple_size<std::tuple<Args...>>::value;
				typedef std::remove_reference<decltype(std::get<0>(t))>::type type;
				std::get<0>(tu) = Get<type>(GetCurrentState());
			}
		};

		template<typename... Args>
		static void GetFromTuple(std::tuple<Args...> t, std::tuple<Args&...>& tu) {
			const auto size = std::tuple_size<std::tuple<Args...>>::value;
			iterateGetTuple<size - 1, Args...>{}(t, tu);
		}

	private:
		inline static void _push(lua_State * pL, ILuaMember* luaMember) {
			//ILuaMember** ptr = reinterpret_cast<ILuaMember**>(lua_newuserdata(LuaManager::GetCurrentState(), sizeof(ILuaMember*)));
			//*ptr = luaMember;
			luaL_getmetatable(LuaManager::GetCurrentState(), LuaManager::GetMetaTable(std::to_string(luaMember->GetID())).c_str());
			//lua_setmetatable(LuaManager::GetCurrentState(), -2);
		}

		template<typename ...Args>
		inline static void _push(lua_State * pL, RetValuesToLua<Args...>&& returnValues) {
			PushFromTuple(returnValues.returnValues);
		}

		template<typename ...Args>
		inline static void _push(lua_State * pL, ReturnToLua<Args...>&& returnValues) {
			returnValues.SetReturnVariables();
			PushFromTuple(returnValues.returnValues);
		}

		inline static void _push(lua_State * pL, int arg) {
			LuaManager::PushInteger(pL, arg);
		}
		inline static void _push(lua_State * pL, bool arg) {
			LuaManager::PushBool(pL, arg);
		}
		inline static void _push(lua_State * pL, std::string arg) {
			LuaManager::PushString(pL, arg);
		}
		inline static void _push(lua_State * pL, float arg) {
			LuaManager::PushFloat(pL, arg);
		}
		inline static void _push(lua_State * pL, double arg) {
			LuaManager::PushDouble(pL, arg);
		}

		template <typename Ret>
		inline static Ret _get(lua_State * pL) {
			if (!std::is_base_of<ILuaMember, std::remove_pointer<Ret>::type>::value)
				LW_PRINT_ERROR("Argument for [_get] is not a ILuaMember");
			if constexpr (std::is_pointer<Ret>::value)
				return GetUserDataFromTable<std::remove_pointer<Ret>::type>();//LuaManager::GetUserData<std::remove_pointer<Ret>::type>();
			else
				return *GetUserDataFromTable<std::remove_pointer<Ret>::type>();//*LuaManager::GetUserData<std::remove_pointer<Ret>::type>();
		}

		template <>
		inline static int _get<int>(lua_State * pL) {
			return LuaManager::GetInteger(pL);
		}
		template <>
		inline static bool _get<bool>(lua_State * pL) {
			return LuaManager::GetBool(pL);
		}
		template <>
		inline static std::string _get<std::string>(lua_State * pL) {
			return LuaManager::GetString(pL);
		}
		template <>
		inline static float _get<float>(lua_State * pL) {
			return LuaManager::GetFloat(pL);
		}
		template <>
		inline static double _get<double>(lua_State * pL) {
			return LuaManager::GetDouble(pL);
		}

		template<typename Ret, typename... Args>
		struct Func {
			template<typename Ret, typename... Args> struct Function {
				static Ret CallLuaFunction(const std::string & pFuncName, int r, Args && ... args) {
					const int params = sizeof...(args);
					lua_getglobal(LuaManager::GetCurrentState(), pFuncName.c_str());
					PushAll(std::forward<Args>(args)...);
					//if (params == 0)
					//	lua_pop(LuaManager::GetCurrentState(), -1);
					if (r == 0)
						r = 1;
					LuaManager::CallLuaFun(pFuncName, params, r);
					return Get<Ret>(LuaManager::GetCurrentState());
				}
			};
			template<> struct Function<void, Args...> {
				static void CallLuaFunction(const std::string & pFuncName, int r, Args&& ... args) {
					const int params = sizeof...(args);
					lua_getglobal(LuaManager::GetCurrentState(), pFuncName.c_str());
					PushAll(std::forward<Args>(args)...);
					LuaManager::CallLuaFun(pFuncName, params, r);
				}
			};
		};


		static void _LoadScript(lua_State * pL, const std::string & pPath);

	public:
		// Push values to lua
		template <typename Arg>
		static void Push(Arg&& arg) {
			return _push(LuaManager::GetCurrentState(), std::forward<Arg>(arg));
		}
	
		template <typename... Args>
		static void PushAll(Args&&... args) {
			std::initializer_list<int>{(Push<Args>(std::forward<Args>(args)), 0)...};
		}

		// Get values from lua
		template <typename Ret>
		static Ret Get(lua_State * L) {
			return _get<Ret>(L);
		}

		/// <summary>
		/// Return values from Lua and places them in the variables that it takes as arguments 
		/// <param name="rets"> the variables that shall hold the returned values </param>
		/// </summary> 
		template <typename... Rets>
		static void GetAll(Rets& ... rets) {
			std::tuple<Rets&...> tu = std::forward_as_tuple(rets...);
			GetFromTuple(std::tuple<Rets...>(Rets()...), tu);
		}

		// If the lua function returns more than one values
		template <typename Ret, size_t nrOfRets, typename... Args>
		static Ret CallLuaFunction(const std::string & pFuncName, Args&& ... args) {
			return Func<Ret, Args...>::Function<Ret, Args...>::CallLuaFunction(pFuncName, nrOfRets, std::forward<Args>(args)...);
		}
		// If the lua function returns more than one values
		template <size_t nrOfRets, typename... Args>
		static void CallLuaFunction(const std::string & pFuncName, Args&& ... args) {
			return Func<void, Args...>::Function<void, Args...>::CallLuaFunction(pFuncName, nrOfRets, std::forward<Args>(args)...);
		}

		// If the lua function returns one or none values
		template <typename Ret, typename... Args>
		static Ret CallLuaFunction(const std::string & pFuncName, Args&& ... args) {
			return Func<Ret, Args...>::Function<Ret, Args...>::CallLuaFunction(pFuncName, 0, std::forward<Args>(args)...);
		}

	public:
		LuaManager();
		~LuaManager();

		static void InitLuaManager(const std::string & pLuaStateName = "Init");
		static void CloseLuaManager();
		static void UseLuaState(unsigned int pLuaStateIndex);
		static void UseLuaState(const std::string & pLuaStateName);
		static void CreateLuaState(const std::string & pLuaStateName);

		static void LoadLib(const std::string & pPath);
		static void LoadLib(const std::string & pLuaStateName, const std::string & pPath);
		static void LoadLib(unsigned int pLuaStateIndex, const std::string & pPath);

		static void LoadScript(const std::string & pPath);
		static void LoadScript(const std::string & pLuaStateName, const std::string & pPath);
		static void LoadScript(unsigned int pLuaStateIndex, const std::string & pPath);

		static lua_State * GetCurrentState();
		static unsigned int GetCurrentStateIndex();

		static void SetLuaBaseDirectory(const std::string& path);
		static void SetLuaLibsDirectory(const std::string& path);

		static void Pop(int index = -1);
		static void RemoveFromStack(int index);

		static void PushInteger(int pInteger);
		static void PushFloat(float pFloat);
		static void PushDouble(double pDouble);
		static void PushString(std::string pString);
		static void PushBool(bool pBool);

		static void PushInteger(lua_State *& pL, int pInteger);
		static void PushFloat(lua_State *& pL, float pFloat);
		static void PushDouble(lua_State *& pL, double pDouble);
		static void PushString(lua_State *& pL, std::string pString);
		static void PushBool(lua_State *& pL, bool pBool);

		static int GetInteger();
		static float GetFloat();
		static double GetDouble();
		static std::string GetString();
		static bool GetBool();

		static int GetInteger(lua_State *& pL);
		static float GetFloat(lua_State *& pL);
		static double GetDouble(lua_State *& pL);
		static std::string GetString(lua_State *& pL);
		static bool GetBool(lua_State *& pL);

		static int GetInteger(int params);
		static float GetFloat(int params);
		static double GetDouble(int params);
		static std::string GetString(int params);
		static bool GetBool(int params);

		template<typename T>
		static T* GetUserDataFromTable() {
			int index = -lua_gettop(LuaManager::GetCurrentState());
			lua_getfield(LuaManager::GetCurrentState(), index, "ptr");
			T* obj = (T*)lua_touserdata(LuaManager::GetCurrentState(), -1);
			lua_remove(LuaManager::GetCurrentState(), -1);
			LuaManager::RemoveFromStack(index);
			return obj;
		};

		template<typename T>
		static T* GetUserData() {
			return GetUserData<T>(-lua_gettop(GetCurrentState()));
		};
		template<typename T>
		static T* GetUserData(int index) {
			T* obj = *(T **)lua_touserdata(LuaManager::GetCurrentState(), index);
			lua_remove(LuaManager::GetCurrentState(), index);
			return obj;
		};
		template<typename T>
		static T* GetUserData(const std::string & metaTable) {
			T* obj = *(T **)luaL_testudata(LuaManager::GetCurrentState(), 1, metaTable.c_str());
			lua_remove(LuaManager::GetCurrentState(), -1);
			return obj;
		};

		static std::string GetMetaTable(const std::string & pObjectName);
		static std::string GetMetaTableAndCheck(const std::string & pObjectName);

		static void PrintStackSize();
		static int StackSize();
		static const char* GetType();
		static const char* GetType(int index);

		/* C++ functions */
		// Need to be called before the script that has the functions is loaded
		//static void RegisterObjectFunctions(ILuaMember* member, luaL_Reg sMonsterRegs[]);
		static void RegisterObjectFunctions(ILuaMember* member, luaL_Reg sMonsterRegs[]);

		/* complementary functions for C++ functions in Lua */
		// Retrun nullptr if <T> not exists
		template<typename T>
		static T* GetObjectPtr(const std::string & pObjectName, int pIndex = 1);
		template<typename T>
		static T* GetObjectPtrEmpty(const std::string & pLuaObjectName, const std::string & pObjectName, int pIndex = 1);

		static const char* GetFunctionNameFromLua();

	private:
		static std::vector<lua_State*> mLs;
		static unsigned int mCurrentState;
		static std::vector<std::string> mMetaTables;
		
		static std::map<std::string, unsigned int> mLuaStateMap;
		static std::string mLibsPath;
		static std::string mLuaBasePath;
	private:
		static void CallLuaFun(const std::string & pFuncName);
		static void CallLuaFun(const std::string & pFuncName, int pArg, int pResults);
	};
};

template<typename T>
inline T * lw::LuaManager::GetObjectPtr(const std::string & pObjectName, int pIndex)
{
	T* objectPtr = nullptr;

	void* ptr = luaL_testudata(GetCurrentState(), pIndex, GetMetaTable(pObjectName).c_str());
	if (ptr != nullptr)
		objectPtr = *(T**)ptr;

	return objectPtr;
}

template<typename T>
inline T * lw::LuaManager::GetObjectPtrEmpty(const std::string & pLuaObjectName, const std::string & pObjectName, int pIndex)
{
	lua_getglobal(GetCurrentState(), pLuaObjectName.c_str());
	T* ptr = LuaManager::GetObjectPtr<Enemy>(pObjectName.c_str());
	lua_pop(GetCurrentState(), -1);
	return ptr;
}
