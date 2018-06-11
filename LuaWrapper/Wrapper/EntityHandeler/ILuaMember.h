#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "../LuaWrapperDefined.h" // Only for DEBUG

class ILuaMember
{
private:
	static unsigned int freeID;
	unsigned int id;

public:

	ILuaMember() {id = freeID++;};
	~ILuaMember() {};

	inline unsigned int GetID() const { return id; };
};

template<typename ...Args>
class RetValues : public ILuaMember
{
public:
	RetValues() {};
	RetValues(Args&... args) {
		info = std::forward_as_tuple(std::forward<Args>(args)...);
		//info = std::make_tuple(args...);
	};
	~RetValues() {};

	void SetRetValues(Args&&... args) {
		info = std::forward_as_tuple(std::forward<Args>(args)...);
	}
	std::tuple<Args...> info;
};