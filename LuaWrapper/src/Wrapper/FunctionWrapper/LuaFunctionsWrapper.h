#pragma once

#include "../Manager/LuaManager.h"

#include <vector>
#include <functional>
#include <map>
#include <iostream>

#define LFW_CALL_ERROR(ret, args, retV)\
if (MapHolder<ret, args>::Functions.find(name) == MapHolder<ret, args>::Functions.end())\
{\
	LFW_PRINT_ERROR("C Function [" << name << "] is not a registered function")\
	return retV;\
}\

#define LFW_CALL_EXISTS(ret, args)\
if (MapHolder<ret, args>::Functions.find(name) != MapHolder<ret, args>::Functions.end())\
{\
	LFW_PRINT_ERROR("C Function [" << name << "] is allready a registered function")\
	return false;\
}\

#define LFW_WRONG_CLASS(pointer)\
ILuaMember* ptr = dynamic_cast<ILuaMember*>(pointer);\
if (ptr == nullptr) {\
	LFW_PRINT_ERROR("wrong class in [" << __func__ << "]")\
	return false;\
}\

#define LFW_CALL_RET_ERROR(ret, args) LFW_CALL_ERROR(ret, args, ret())

#define LFW_RegisterCObjectFunctions(obj, ...)\
decltype(obj) LFW__obj = obj;\
luaL_Reg mFuncList[] = { __VA_ARGS__, { NULL, NULL } };\
LFW::LuaFunctionsWrapper::RegisterCObject(&##obj, mFuncList);

#define LFW_function(name, function) { ##name, LFW::LuaFunctionsWrapper::GetRegisterFunction(##name, &LFW__obj, &##function) }
#define LFW_ReturnValues(...) LFW::LuaFunctionsWrapper::ReturnValuesToLua(__VA_ARGS__)
#define LFW_ReturnType auto
#define LFW_SetReturnValues(...) returnValues = std::make_tuple(__VA_ARGS__)


template<int N>
struct my_placeholder { static my_placeholder ph; };

template<int N>
my_placeholder<N> my_placeholder<N>::ph;

namespace std {
	template<int N>
	struct std::is_placeholder<::my_placeholder<N>> : std::integral_constant<int, N> { };
}

namespace LFW {
	class LuaFunctionsWrapper
	{
	private:
		template <typename Ret, typename... Args>
		struct MapHolder {
			static std::map<std::string, std::function<Ret(Args...)>> Functions;
		};

	private:
		// For checking if two classes is the same
		template <class A, class B>
		struct eq {
			static const bool result = false;
		};
		// For checking if two classes is the same
		template<class A>
		struct eq<A, A> {
			static const bool result = true;
		};

		/*----------------------- Function for Lua to c functions -----------------------------*/
		template<typename Ret, typename... Args>
		struct CallAndRet {
			template<typename Ret, typename... Args> struct Function {
				static Ret CallMemFunc(const std::string &name, Args &&... args) {
					LFW_CALL_RET_ERROR(Ret, Args...)
					LFW_PRINT_DEBUG_TEXT("Called C function [" << name << "]", LFW::FUNCTION_CALLS)
					return MapHolder<Ret, Args...>::Functions[name](std::forward<Args>(args)...);
				}
			};
			template<> struct Function<void, Args...> {
				static void CallMemFunc(const std::string &name, Args &&... args) {
					LFW_CALL_RET_ERROR(void, Args...)
					LFW_PRINT_DEBUG_TEXT("Called C function [" << name << "]", LFW::FUNCTION_CALLS)
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
					LuaManager::Push<Ret>(LuaManager::GetCurrentState(), LuaFunctionsWrapper::CallMemFunction<Ret>(name, LuaManager::Get<Args>(LuaManager::GetCurrentState())...));
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
			if (!eq<void, Ret>::result)
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
			Clazz* luaMember = LuaManager::GetUserData<Clazz>(-lua_gettop(LuaManager::GetCurrentState()));
			std::string functionName = LuaFunctionsWrapper::GenerateFuncName(LuaManager::GetFunctionNameFromLua(), luaMember);

			int stackSize = LuaManager::StackSize() - sizeof...(Args);
			CallAndPush<Ret, Args...>(functionName);
			if (!eq<void, Ret>::result)
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
			LFW_CALL_EXISTS(Ret, Args...)
			if (!luaMember) {
				LFW_PRINT_ERROR("Class of C function [" << name << "] is not a Lua member")
				return false;
			}
			LFW_PRINT_DEBUG_TEXT("Register C function [" << name << "] with [" << params << "] arguments", LFW::REGISTER_FUNCTIONS)
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
			LFW_CALL_EXISTS(Ret, Args...)
			LFW_PRINT_DEBUG_TEXT("Register a static C function [" << name << "] with [" << params << "] arguments", LFW::REGISTER_FUNCTIONS)
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
			LFW_WRONG_CLASS(pClass);
			bool retur = LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
			LuaFunctionsWrapper::AddCFunction<Ret, Args...>(pClass, name);
			return retur;
		}

		/* --------------- Register a const member function ---------------- */
		template<typename Ret, typename Clazz, typename ...Args>
		static bool RegisterCFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...) const) {
			LFW_WRONG_CLASS(pClass);
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
			LuaManager::RegisterObjectFunctions(std::to_string(member->GetID()), functions);
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
std::map<std::string, std::function<Ret(Args...)>> LFW::LuaFunctionsWrapper::MapHolder<Ret, Args...>::Functions;