#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace LFW {

	class ILuaMember
	{
	private:
		static unsigned int freeID;
		unsigned int id;

	public:

		ILuaMember() { id = freeID++; };
		~ILuaMember() {};

		inline unsigned int GetID() const { return id; };
	};

	class Base_of_ReturnToLua {};

	template<typename ...Args>
	struct RetValuesToLua : Base_of_ReturnToLua
	{
		RetValuesToLua() {};
		RetValuesToLua(Args... args) {
			returnValues = std::make_tuple(args...);
		};

		std::tuple<Args...> returnValues;
	};

	template<typename ...Args>
	struct ReturnToLua : Base_of_ReturnToLua
	{
		std::tuple<Args...> returnValues;
		virtual void SetReturnVariables() = 0;
	};

};