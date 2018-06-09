#pragma once

#include <string>
#include <vector>

#include "../LuaWrapperDefined.h" // Only for DEBUG

class ILuaMember
{
private:
	static unsigned int freeID;
	unsigned int id;
	std::string luaObject;

public:
	ILuaMember() {id = freeID++;};
	~ILuaMember() {};

	inline unsigned int GetID() const { return id; };
	inline std::string GetLuaObject() const { return luaObject; };
	inline void SetLuaObject(const std::string & luaObject) { this->luaObject = luaObject; };
};