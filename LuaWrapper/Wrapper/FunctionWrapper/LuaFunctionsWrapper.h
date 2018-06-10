#pragma once

#include "../Manager/LuaManager.h"

#include <vector>
#include <functional>
#include <map>
#include <iostream>

#define ILuaMember_CALL_ERROR(ret, args, retV)\
if (MapHolder<ret, args>::CallbackMap.find(name) == MapHolder<ret, args>::CallbackMap.end())\
{\
	std::cout << "ERROR: C Function [" << name << "] is not a registered function" << std::endl;\
	return retV;\
}\

#define ILuaMember_CALL_EXISTS(ret, args)\
if (MapHolder<ret, args>::CallbackMap.find(name) != MapHolder<ret, args>::CallbackMap.end())\
{\
	std::cout << "ERROR: C Function [" << name << "] is allready a registered function" << std::endl;\
	return false;\
}\

#define CALL_RET_ERROR(ret, args) ILuaMember_CALL_ERROR(ret, args, ret())

template<int N>
struct my_placeholder { static my_placeholder ph; };

template<int N>
my_placeholder<N> my_placeholder<N>::ph;

namespace std {
	template<int N>
	struct std::is_placeholder<::my_placeholder<N>> : std::integral_constant<int, N> { };
}

class LuaFunctionsWrapper
{

private:
	static unsigned int freeLuaObject;

	template <typename Ret, typename... Args>
	struct MapHolder {
		static std::map<std::string, std::function<Ret(Args...)>> CallbackMap;
	};

private:
	// For checking if two classes is the same
	template <class A, class B>
	struct eq {
		static const bool result = false;
	};

	template<class A>
	struct eq<A, A> {
		static const bool result = true;
	};

	/*----------------------- Function for Lua to c functions -----------------------------*/
	template<typename Ret, typename... Args>
	struct CallAndRet {
		template<typename Ret, typename... Args> struct Function {
			static Ret CallMemFunc(const std::string &name, Args &&... args) {
				CALL_RET_ERROR(Ret, Args...)
					if (LuaWrapperDefined::DEBUG)
						std::cout << "Called C function [" << name << "]" << std::endl;
				return MapHolder<Ret, Args...>::CallbackMap[name](std::forward<Args>(args)...);
			}
		};
		template<> struct Function<void, Args...> {
			static void CallMemFunc(const std::string &name, Args &&... args) {
				CALL_RET_ERROR(void, Args...)
					if (LuaWrapperDefined::DEBUG)
						std::cout << "Called C function [" << name << "]" << std::endl;
				MapHolder<void, Args...>::CallbackMap[name](std::forward<Args>(args)...);
			}
		};
	};

public:
	template <typename Ret, typename... Args>
	static Ret CallMemFunc(const std::string &name, Args &&... args) {
		return CallAndRet<Ret, Args...>::Function<Ret, Args...>::CallMemFunc(name, std::forward<Args>(args)...);
	}

private:
	/*-------------------------- For Lua to call c functions --------------------------------*/
	// Call a member function and push to the lua stack if the function returns a value
	template<typename Ret, typename... Args>
	struct CallFunc {
		template<typename Ret, typename... Args> struct Function {
			static void CallAndPush(const std::string &name) {
				LuaManager::Push<Ret>(LuaManager::GetCurrentState(), LuaFunctionsWrapper::CallMemFunc<Ret>(name, LuaManager::Get<Args>(LuaManager::GetCurrentState())...));
			}
		};
		template<> struct Function<void, Args...> {
			static void CallAndPush(const std::string &name) {
				LuaFunctionsWrapper::CallMemFunc<Ret>(name, LuaManager::Get<Args>(LuaManager::GetCurrentState())...);
			}
		};
	};

	template <typename Ret, typename... Args>
	static void CallAndPush(const std::string &name) {
		CallFunc<Ret, Args...>::Function<Ret, Args...>::CallAndPush(name);
	}

	template<typename Ret, typename Clazz, typename ...Args>
	inline static int FunctionWrapper(lua_State * L) {
		Clazz* luaMember = reinterpret_cast<Clazz*>(lua_touserdata(LuaManager::GetCurrentState(), lua_upvalueindex(1)));
		std::string function = lua_tostring(LuaManager::GetCurrentState(), lua_upvalueindex(2));
		
		int stackSize = LuaManager::StackSize() - sizeof...(Args);
		CallAndPush<Ret, Args...>(function);
		if (!eq<void, Ret>::result)
		{
			if (std::is_base_of<ILuaMember, Ret>::value) {
				return LuaManager::StackSize() - stackSize;
			}
			else
				return 1;
		}
		return 0;
	}

	/*----------------------- Register functions -----------------------------*/
	template<typename Ret, typename Clazz, typename ...Args, int... Is>
	static bool RegisterCaller(std::string name, Clazz*& pClass, Ret(Clazz::*Callback)(Args...), std::integer_sequence<int, Is...>) {
		const int params = sizeof...(Args);
		ILuaMember* luaMember = dynamic_cast<ILuaMember*>(pClass);
		name = std::to_string(LuaManager::GetCurrentStateIndex()) + LuaFunctionsWrapper::GenerateFuncName(name, pClass);
		ILuaMember_CALL_EXISTS(Ret, Args...)
		if (!luaMember) {
			std::cout << "ERROR: Class of C function [" << name << "] is not a Lua member" << std::endl;
			return false;
		}
		if (LuaWrapperDefined::DEBUG)
			std::cout << "Register C function [" << name << "] with [" << params << "] arguments" << std::endl;
		MapHolder<Ret, Args...>::CallbackMap[name] = std::bind(Callback, pClass, my_placeholder<Is + 1>::ph...);
		return true;
	}

	template<typename Ret, typename Clazz, typename ...Args>
	static bool AddCFunction(Clazz* pPointer, std::string luafuncname) {
		ILuaMember* ptr = dynamic_cast<ILuaMember*>(pPointer);
		if (ptr == nullptr) {
			std::cout << "ERROR: wrong class in [" << __func__ << "]" << std::endl;
			return false;
		}
		std::string newName = std::to_string(LuaManager::GetCurrentStateIndex()) + LuaFunctionsWrapper::GenerateFuncName(luafuncname, pPointer);
		lua_pushlightuserdata(LuaManager::GetCurrentState(), pPointer);
		lua_pushstring(LuaManager::GetCurrentState(), newName.c_str());
		lua_pushcclosure(LuaManager::GetCurrentState(), LuaFunctionsWrapper::FunctionWrapper<Ret, Clazz, Args...>, 2);
		lua_setglobal(LuaManager::GetCurrentState(), luafuncname.c_str());
		return true;
	}

	static void RegisterObject(ILuaMember* member) {
		unsigned int luaObjectIndex = freeLuaObject++;
		member->SetLuaObject(std::to_string(luaObjectIndex));
	};

public:

	template<typename Ret, typename Clazz, typename ...Args>
	static bool RegisterCFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...)) {
		bool retur = LuaFunctionsWrapper::RegisterCaller(name, pClass, Callback, std::make_integer_sequence<int, sizeof...(Args)>());
		LuaFunctionsWrapper::AddCFunction<Ret, Clazz, Args...>(pClass, name);
		return retur;
	}

	static std::string GenerateFuncName(const std::string & funcName, ILuaMember* luaMember) {
		return  "_" + std::to_string(luaMember->GetID()) + "_" + funcName;
	}


	template<typename Ret, typename Clazz, typename ...Args>
	inline static int FunctionWrapper2(lua_State * L) {
		Clazz* luaMember = LuaManager::GetUserData<Clazz>(-lua_gettop(LuaManager::GetCurrentState()));
		std::string functionName = std::to_string(LuaManager::GetCurrentStateIndex()) + LuaFunctionsWrapper::GenerateFuncName(LuaManager::GetFunctionNameFromLua(), luaMember);
		
		CallAndPush<Ret, Args...>(functionName);
		if (!eq<void, Ret>::result)
			return 1;
		return 0;
	}

	template<typename Ret, typename Clazz, typename ...Args>
	static lua_CFunction GetRegisterFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...)) {
		LuaFunctionsWrapper::RegisterCFunction(name, pClass, Callback);
		return LuaFunctionsWrapper::FunctionWrapper2<Ret, Clazz, Args...>;
	};

	static void RegisterCObject(ILuaMember* member, luaL_Reg functions[]) {
		RegisterObject(member);
		LuaManager::RegisterObjectFunctions(member->GetLuaObject(), functions);
	};
};

template <typename Ret, typename... Args>
std::map<std::string, std::function<Ret(Args...)>> LuaFunctionsWrapper::MapHolder<Ret, Args...>::CallbackMap;