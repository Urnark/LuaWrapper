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

#include "../Manager/LuaManager.h"

#include <vector>
#include <functional>
#include <map>
#include <iostream>

#define LW_CALL_ERROR(ret, args, retV)\
if (MapHolder<ret, args>::Functions.find(name) == MapHolder<ret, args>::Functions.end())\
{\
	LW_PRINT_ERROR("C Function [" << name << "] is not a registered function")\
	return retV;\
}\

#define LW_CALL_EXISTS(ret, args)\
if (MapHolder<ret, args>::Functions.find(name) != MapHolder<ret, args>::Functions.end())\
{\
	LW_PRINT_ERROR("C Function [" << name << "] is allready a registered function")\
	return false;\
}\

#define LW_WRONG_CLASS(pointer)\
ILuaMember* ptr = dynamic_cast<ILuaMember*>(pointer);\
if (ptr == nullptr) {\
	LW_PRINT_ERROR("wrong class in [" << __func__ << "]")\
	return false;\
}\

#define LW_CALL_RET_ERROR(ret, args) LW_CALL_ERROR(ret, args, ret())

#define LW_RegisterCObjectFunctions(obj, ...)\
decltype(obj) LW__obj = obj;\
luaL_Reg mFuncList[] = { __VA_ARGS__, { NULL, NULL } };\
lw::LuaFunctionsWrapper::RegisterCObject(##obj, mFuncList);

#define LW_function(name, function) { ##name, lw::LuaFunctionsWrapper::GetRegisterFunction(##name, LW__obj, &##function) }
#define LW_baseClassFunction(name, type, function) { ##name, lw::LuaFunctionsWrapper::GetRegisterFunction(##name, (##type*&)LW__obj, &##function) }
#define LW_ReturnValues(...) lw::LuaFunctionsWrapper::ReturnValuesToLua(__VA_ARGS__)
#define LW_ReturnType auto
#define LW_SetReturnValues(...) returnValues = std::make_tuple(__VA_ARGS__)


template<int N>
struct my_placeholder { static my_placeholder ph; };

template<int N>
my_placeholder<N> my_placeholder<N>::ph;

namespace std {
	template<int N>
	struct std::is_placeholder<::my_placeholder<N>> : std::integral_constant<int, N> { };
}

namespace lw {
	class LuaFunctionsWrapper
	{
	private:
		template <typename Ret, typename... Args>
		struct MapHolder {
			static std::map<std::string, std::function<Ret(Args...)>> Functions;
		};

	private:
		/*----------------------- Function for Lua to c functions -----------------------------*/
		template<typename Ret, typename... Args>
		struct CallAndRet {
			template<typename Ret, typename... Args> struct Function {
				static Ret CallMemFunc(const std::string &name, Args &&... args) {
					LW_CALL_RET_ERROR(Ret, Args...)
					LW_PRINT_DEBUG_TEXT("Called C function [" << name << "]", FUNCTION_CALLS)
					return MapHolder<Ret, Args...>::Functions[name](std::forward<Args>(args)...);
				}
			};
			template<> struct Function<void, Args...> {
				static void CallMemFunc(const std::string &name, Args &&... args) {
					LW_CALL_RET_ERROR(void, Args...)
					LW_PRINT_DEBUG_TEXT("Called C function [" << name << "]", FUNCTION_CALLS)
					MapHolder<void, Args...>::Functions[name](std::forward<Args>(args)...);
				}
			};
		};

	private:
		/*-------------------------- For Lua to call a c functions --------------------------------*/
		// Call a member function and push to the lua stack if the function returns a value
		template<typename Ret, typename... Args>
		struct CallFunc {
			template<typename Ret, typename... Args> struct Function {
				static void CallAndPush(const std::string &name) {
					LuaManager::Push<Ret>(LuaFunctionsWrapper::CallMemFunction<Ret>(name, LuaManager::Get<Args>(LuaManager::GetCurrentState())...));
				}
			};
			template<> struct Function<void, Args...> {
				static void CallAndPush(const std::string &name) {
					LuaFunctionsWrapper::CallMemFunction<Ret>(name, LuaManager::Get<Args>(LuaManager::GetCurrentState())...);
				}
			};
		};

		template <typename Ret, typename... Args>
		static void CallAndPush(const std::string &name) {
			CallFunc<Ret, Args...>::Function<Ret, Args...>::CallAndPush(name);
		}

		/* --------------- Lua C function for calling a function ---------------- */
		template<typename Ret, typename ...Args>
		inline static int FunctionWrapper(lua_State * L) {
			std::string function = lua_tostring(LuaManager::GetCurrentState(), lua_upvalueindex(1));

			int stackSize = LuaManager::StackSize() - sizeof...(Args);
			CallAndPush<Ret, Args...>(function);
			if (!std::is_same<void, Ret>::value)
			{
				if (std::is_base_of<Base_of_ReturnToLua, Ret>::value) {
					return LuaManager::StackSize() - stackSize;
				}
				else
					return 1;
			}
			return 0;
		}

		/* --------------- Lua C function for calling a member function with self as a parameter ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		inline static int FunctionWrapper2(lua_State * L) {
			//Clazz* luaMember = LuaManager::GetUserData<Clazz>(-lua_gettop(LuaManager::GetCurrentState()));
			std::string functionName;// = LuaFunctionsWrapper::GenerateFuncName(LuaManager::GetFunctionNameFromLua(), luaMember);
			//std::cout << "type: " << LuaManager::GetType(-lua_gettop(LuaManager::GetCurrentState())) << std::endl;
			if (LuaManager::GetType(-lua_gettop(LuaManager::GetCurrentState())) == "table")
			{
				int index = -lua_gettop(LuaManager::GetCurrentState());
				lua_getfield(LuaManager::GetCurrentState(), index, "id");
				std::string id = LuaManager::GetString();
				LuaManager::RemoveFromStack(index);
				functionName = std::to_string(LuaManager::GetCurrentStateIndex()) + "_" + id + "_" + LuaManager::GetFunctionNameFromLua();
			}
			else
			{
				LW_PRINT_ERROR("Not a table on the stack");
			}
			
			int stackSize = LuaManager::StackSize() - sizeof...(Args);
			CallAndPush<Ret, Args...>(functionName);
			if (!std::is_same<void, Ret>::value)
			{
				if (std::is_base_of<Base_of_ReturnToLua, Ret>::value) {
					return LuaManager::StackSize() - stackSize;
				}
				else
					return 1;
			}
			return 0;
		}

		/*----------------------- Register functions -----------------------------*/
		template<typename Ret, typename Clazz, typename ...Args>
		static bool PrepareToRegisterCaller(std::string& name, Clazz*& pClass) {
			const int params = sizeof...(Args);
			ILuaMember* luaMember = dynamic_cast<ILuaMember*>(pClass);
			name = LuaFunctionsWrapper::GenerateFuncName(name, pClass);
			LW_CALL_EXISTS(Ret, Args...)
			if (!luaMember) {
				LW_PRINT_ERROR("Class of C function [" << name << "] is not a Lua member")
				return false;
			}
			LW_PRINT_DEBUG_TEXT("Register C function [" << name << "] with [" << params << "] arguments", REGISTER_FUNCTIONS)
			return true;
		}

		/*----------------------- Add a member function to the mapping for functions -----------------------------*/
		template<typename Ret, typename Clazz, typename ...Args, int... Is>
		static bool RegisterCaller(std::string name, Clazz*& pClass, Ret(Clazz::*Callback)(Args...), std::integer_sequence<int, Is...>) {
			bool retur = PrepareToRegisterCaller<Ret, Clazz, Args...>(name, pClass);
			MapHolder<Ret, Args...>::Functions[name] = std::bind(Callback, pClass, my_placeholder<Is + 1>::ph...);
			return retur;
		}

		/*----------------------- Add a const member function to the mapping for functions -----------------------------*/
		template<typename Ret, typename Clazz, typename ...Args, int... Is>
		static bool RegisterCaller(std::string name, Clazz*& pClass, Ret(Clazz::*Callback)(Args...) const, std::integer_sequence<int, Is...>) {
			bool retur = PrepareToRegisterCaller<Ret, Clazz, Args...>(name, pClass);
			MapHolder<Ret, Args...>::Functions[name] = std::bind(Callback, pClass, my_placeholder<Is + 1>::ph...);
			return retur;
		}

		/*----------------------- Add a static function to the mapping for functions -----------------------------*/
		template<typename Ret, typename ...Args, int... Is>
		static bool RegisterCallerStatic(std::string name, Ret(*Callback)(Args...), std::integer_sequence<int, Is...>) {
			const int params = sizeof...(Args);
			name = LuaFunctionsWrapper::GenerateFuncName(name, nullptr);
			LW_CALL_EXISTS(Ret, Args...)
			LW_PRINT_DEBUG_TEXT("Register a static C function [" << name << "] with [" << params << "] arguments", REGISTER_FUNCTIONS)
			MapHolder<Ret, Args...>::Functions[name] = std::bind(Callback, my_placeholder<Is + 1>::ph...);
			return true;
		}

		/*----------------------- For creating a Lua C function and closure for the function -----------------------------*/
		template<typename Ret, typename ...Args>
		static bool AddCFunction(ILuaMember* pPointer, std::string luafuncname) {
			std::string newName = LuaFunctionsWrapper::GenerateFuncName(luafuncname, pPointer);
			lua_pushstring(LuaManager::GetCurrentState(), newName.c_str());
			lua_pushcclosure(LuaManager::GetCurrentState(), LuaFunctionsWrapper::FunctionWrapper<Ret, Args...>, 1);
			lua_setglobal(LuaManager::GetCurrentState(), luafuncname.c_str());
			return true;
		}

	public:
		/* --------------- Register a member function ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		static bool RegisterCFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...)) {
			LW_WRONG_CLASS(pClass);
			bool retur = LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			LuaFunctionsWrapper::AddCFunction<Ret, Args...>(pClass, name);
			return retur;
		}

		/* --------------- Register a const member function ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		static bool RegisterCFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...) const) {
			LW_WRONG_CLASS(pClass);
			bool retur = LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			LuaFunctionsWrapper::AddCFunction<Ret, Args...>(pClass, name);
			return retur;
		}
		/* --------------- Register a static function ---------------- */
		template<typename Ret, typename ...Args>
		static bool RegisterCFunction(std::string name, Ret(*Callback)(Args...)) {
			bool retur = LuaFunctionsWrapper::RegisterCallerStatic(name, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			LuaFunctionsWrapper::AddCFunction<Ret, Args...>(nullptr, name);
			return retur;
		}

		/* --------------- Generate function name for mapping the functions ---------------- */
		static std::string GenerateFuncName(const std::string & funcName, ILuaMember* luaMember) {
			if (luaMember == nullptr)
				return  std::to_string(LuaManager::GetCurrentStateIndex()) + "_S_" + funcName;
			else
				return  std::to_string(LuaManager::GetCurrentStateIndex()) + "_" + std::to_string(luaMember->GetID()) + "_" + funcName;
		}

		/* --------------- Register a member function and return the Lua CFunction ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		static lua_CFunction GetRegisterFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...)) {
			LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			return LuaFunctionsWrapper::FunctionWrapper2<Ret, Clazz, Args...>;
		};

		/* --------------- Register a const member function and return the Lua CFunction ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		static lua_CFunction GetRegisterFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...) const) {
			LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			return LuaFunctionsWrapper::FunctionWrapper2<Ret, Clazz, Args...>;
		};

		/* --------------- Register functions for a C/C++ object to be used in Lua ---------------- */
		static void RegisterCObject(ILuaMember* member, luaL_Reg functions[]) {
			LuaManager::RegisterObjectFunctions(member, functions);
		};

		/* --------------- Call a member function ---------------- */
		template <typename Ret, typename... Args>
		static Ret CallMemFunction(const std::string &name, Args &&... args) {
			return CallAndRet<Ret, Args...>::Function<Ret, Args...>::CallMemFunc(name, std::forward<Args>(args)...);
		}

		/* --------------- Create a instance of RetValuesToLua with specific parameters ---------------- */
		template<typename ...Args>
		inline static RetValuesToLua<Args...> ReturnValuesToLua(Args... args) {
			return RetValuesToLua<Args...>(args...);
		};
	};
};
template <typename Ret, typename... Args>
std::map<std::string, std::function<Ret(Args...)>> lw::LuaFunctionsWrapper::MapHolder<Ret, Args...>::Functions;