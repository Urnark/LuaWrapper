#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <functional>

#include "../Defined.h" // Only for DEBUG

using namespace std::placeholders;

#define ILuaMember_CALL_ERROR(ret, args, retV)\
if (MapHolder<ret, args>::CallbackMap.find(name) == MapHolder<ret, args>::CallbackMap.end())\
{\
	std::cout << "Function [" << name << "] is not a registered function" << std::endl;\
	return retV;\
}\

#define CALL_RET_ERROR(ret, args) ILuaMember_CALL_ERROR(ret, args, ret())
#define CALL_VOID_ERROR(args) ILuaMember_CALL_ERROR(void, args, )

class ILuaMember
{
private:
	template <typename Ret, typename... Args>
	struct MapHolder {
		static std::map<std::string, std::function<Ret(Args...)>> CallbackMap;
	};

	static unsigned int freeID;
	unsigned int id;
	std::string luaObject;

public:
	ILuaMember() {id = freeID++;};
	~ILuaMember() {};

	unsigned int GetID() const { return id; };
	void SetLuaObject(const std::string & luaObject) { this->luaObject = luaObject; };
	std::string GetLuaObject() const { return luaObject; };

	template<typename Ret, typename Clazz, typename ...Args, typename ...T>
	static void RegisterCaller(std::string name, Clazz*& pClass, Ret(Clazz::*Callback)(Args...), T... p) {
		const int params = sizeof...(Args);
		ILuaMember* luaMember = dynamic_cast<ILuaMember*>(pClass);
		if (!luaMember)
		{
			std::cout << "Class of function [" << name << "] is not a Lua member" << std::endl;
			return;
		}
		if (Defined::DEBUG)
			std::cout << "Register function [" << name << "] with [" << params << "] arguments" << std::endl;
		MapHolder<Ret, Args...>::CallbackMap[name] = std::bind(Callback, pClass, p...);
	}

	template<typename Ret, typename... Args>
	struct Func{
		template<typename Ret, typename... Args> struct Function {
			static Ret CallMemFunc(const std::string &name, Args &&... args) {
				CALL_RET_ERROR(Ret, Args...)
				if (Defined::DEBUG)
					std::cout << "Called function [" << name << "]" << std::endl;
				return MapHolder<Ret, Args...>::CallbackMap[name](std::forward<Args>(args)...);
			}
		};
		template<> struct Function<void, Args...> {
			static void CallMemFunc(const std::string &name, Args &&... args) {
				CALL_RET_ERROR(void, Args...)
				if (Defined::DEBUG)
					std::cout << "Called function [" << name << "]" << std::endl;
				MapHolder<void, Args...>::CallbackMap[name](std::forward<Args>(args)...);
			}
		};
	};

	template <typename Ret, typename... Args>
	static Ret CallMemFunc(const std::string &name, Args &&... args) {
		return Func<Ret, Args...>::Function<Ret, Args...>::CallMemFunc(name, std::forward<Args>(args)...);
	}
};

template <typename Ret, typename... Args>
std::map<std::string, std::function<Ret(Args...)>> ILuaMember::MapHolder<Ret, Args...>::CallbackMap;