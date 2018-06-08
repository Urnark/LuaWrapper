#pragma once

#include "../Manager/LuaManager.h"
#include <vector>

class LuaFunctionsWrapper
{
private:
	template <class A, class B>
	struct eq {
		static const bool result = false;
	};

	template<class A>
	struct eq<A, A> {
		static const bool result = true;
	};

	template<typename Ret, typename... Args>
	struct Func {
		template<typename Ret, typename... Args> struct Function {
			static void PushToStack(const std::string &name) {
				LuaManager::push<Ret>(LuaManager::GetCurrentState(), ILuaMember::CallMemFunc<Ret>(name, LuaManager::get<Args>(LuaManager::GetCurrentState())...));
			}
		};
		template<> struct Function<void, Args...> {
			static void PushToStack(const std::string &name) {
				ILuaMember::CallMemFunc<Ret>(name, LuaManager::get<Args>(LuaManager::GetCurrentState())...);
			}
		};
	};

	template <typename Ret, typename... Args>
	static void PushToStack(const std::string &name) {
		Func<Ret, Args...>::Function<Ret, Args...>::PushToStack(name);
	}

	static unsigned int freeLuaObject;

public:

	template<typename Ret, typename Clazz, typename ...Args>
	inline static int FunctionWrapper(lua_State * L) {
		const int params = sizeof...(Args);
		Clazz* luaMember = reinterpret_cast<Clazz*>(lua_touserdata(LuaManager::GetCurrentState(), lua_upvalueindex(1)));
		std::string function = lua_tostring(LuaManager::GetCurrentState(), lua_upvalueindex(1));

		PushToStack<Ret, Args...>(function);
		if (!eq<void, Ret>::result)
			return 1;
		return 0;
	}

	template<typename Ret, typename Clazz, typename ...Args>
	static void AddCFunction(Clazz* pPointer, std::string luafuncname) {
		ILuaMember* ptr = dynamic_cast<ILuaMember*>(pPointer);
		if (ptr == nullptr)
			std::cout << "ERROR: wrong class in [" << __func__ << "]" << std::endl;
		else
		{
			lua_pushlightuserdata(LuaManager::GetCurrentState(), pPointer);
			lua_pushstring(LuaManager::GetCurrentState(), luafuncname.c_str());
			lua_pushcclosure(LuaManager::GetCurrentState(), LuaFunctionsWrapper::FunctionWrapper<Ret, Clazz, Args...>, 1);
			lua_setglobal(LuaManager::GetCurrentState(), luafuncname.c_str());
			lua_remove(LuaManager::GetCurrentState(), -1);
		}
	}

	template<typename Ret, typename Clazz, typename ...Args, typename ...T>
	static void RegisterCFunction(std::string name, Clazz* pClass, Ret(Clazz::*Callback)(Args...), T... p) {
		ILuaMember::RegisterCaller(name, pClass, Callback, p...);
		LuaFunctionsWrapper::AddCFunction<Ret, Clazz, Args...>(pClass, name);
	}

	static std::string GenerateFuncName(const std::string & funcName, ILuaMember* luaMember) {
		return funcName + std::to_string(luaMember->GetID());
	}

	static void RegisterObject(ILuaMember* member) {
		unsigned int luaObjectIndex = freeLuaObject++;
		member->SetLuaObject(std::to_string(luaObjectIndex));
	};
};