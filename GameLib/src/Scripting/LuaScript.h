#pragma once

#include <lua.hpp>

#include <memory>
#include <string_view>

namespace Game {

	class LuaScipt
	{
	public:
		LuaScipt(std::string_view source);

		void SetFunction(const std::string& name) const;
		void Execute(uint32_t numArgs, uint32_t numResults) const;
		void SetArgument(std::string_view value) const;

	private:
		std::unique_ptr<::lua_State, decltype(&::lua_close)> m_Lua;
	};

}
