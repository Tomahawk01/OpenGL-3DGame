#pragma once

#include "Math/Vector3.h"

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
		void SetArgument(int64_t value) const;
		void SetArgument(float value) const;
		void SetArgument(std::string_view value) const;
		void SetArgument(const vec3& value) const;
		void GetResult(int64_t& result) const;
		void GetResult(float& result) const;
		void GetResult(std::string& result) const;

	private:
		std::unique_ptr<::lua_State, decltype(&::lua_close)> m_Lua;
	};

}
