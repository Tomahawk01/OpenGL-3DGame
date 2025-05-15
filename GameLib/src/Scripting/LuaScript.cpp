#include "LuaScript.h"

#include "Utilities/Error.h"
#include "Vector3Interop.h"

#include <sstream>
#include <ranges>

namespace {

	struct LoadData
	{
		std::string_view source;
		uint32_t counter;
	};

	const char* LoadStringView(::lua_State*, void* data, size_t* size)
	{
		auto* loadData = reinterpret_cast<LoadData*>(data);
		if (loadData->counter != 0u)
			return nullptr;

		*size = loadData->source.size();
		loadData->counter++;
		return loadData->source.data();
	}

}

namespace Game {

	LuaScipt::LuaScipt(std::string_view source)
		: m_Lua(::luaL_newstate(), &::lua_close)
	{
		Ensure(m_Lua, "Failed to create lua state!\n{}", *this);

		::luaL_openlibs(m_Lua.get());
		lua_register(m_Lua.get(), "vec3", &Vector3Constructor);

		LoadData loadData{ source, 0 };

		Ensure(::lua_load(m_Lua.get(), &LoadStringView, reinterpret_cast<void*>(&loadData), "lua_script", "t") == LUA_OK, "Failed to load lua source!\n{}", *this);

		Ensure(::lua_pcall(m_Lua.get(), 0, 0, 0) == LUA_OK, "Failed to initialize script!\n{}", *this);
	}

	void LuaScipt::SetFunction(const std::string& name) const
	{
		const auto retType = ::lua_getglobal(m_Lua.get(), name.c_str());
		Ensure(retType == LUA_TFUNCTION, "Missing function {}\n{}", name, *this);
	}

	void LuaScipt::Execute(uint32_t numArgs, uint32_t numResults) const
	{
		Ensure(::lua_gettop(m_Lua.get()) >= static_cast<int>(numArgs), "Arg count mismatch {}\n{}", numArgs, *this);
		if (::lua_pcall(m_Lua.get(), numArgs, numResults, 0) != LUA_OK)
		{
			const auto res = ::lua_tostring(m_Lua.get(), -1);
			::lua_pop(m_Lua.get(), 1);

			throw Exception(std::format("Failed to execute ({})", res));
		}
	}

	void LuaScipt::SetArgument(int64_t value) const
	{
		::lua_pushinteger(m_Lua.get(), value);
	}

	void LuaScipt::SetArgument(float value) const
	{
		::lua_pushnumber(m_Lua.get(), value);
	}

	void LuaScipt::SetArgument(std::string_view value) const
	{
		::lua_pushlstring(m_Lua.get(), value.data(), value.size());
	}

	void LuaScipt::SetArgument(const vec3& value) const
	{
		SetArgument(value.x);
		SetArgument(value.y);
		SetArgument(value.z);

		Vector3Constructor(m_Lua.get());
	}

	void LuaScipt::GetResult(int64_t& result) const
	{
		Ensure(::lua_gettop(m_Lua.get()) != 0, "No results to get!\n{}", *this);
		Ensure(::lua_isinteger(m_Lua.get(), -1) == 1, "Result not an integer!\n{}", *this);
		result = ::lua_tointeger(m_Lua.get(), -1);
		::lua_pop(m_Lua.get(), 1);
	}

	void LuaScipt::GetResult(float& result) const
	{
		Ensure(::lua_gettop(m_Lua.get()) != 0, "No results to get!\n{}", *this);
		Ensure(::lua_isnumber(m_Lua.get(), -1) == 1, "Result not a float!\n{}", *this);
		result = static_cast<float>(::lua_tonumber(m_Lua.get(), -1));
		::lua_pop(m_Lua.get(), 1);
	}

	void LuaScipt::GetResult(std::string& result) const
	{
		Ensure(::lua_gettop(m_Lua.get()) != 0, "No results to get!\n{}", *this);
		Ensure(::lua_isstring(m_Lua.get(), -1) == 1, "Result not a string!\n{}", *this);
		result = ::lua_tostring(m_Lua.get(), -1);
		::lua_pop(m_Lua.get(), 1);
	}

	void LuaScipt::GetResult(vec3& result) const
	{
		Ensure(::lua_gettop(m_Lua.get()) != 0, "No results to get!\n{}", *this);
		Ensure(::lua_type(m_Lua.get(), -1) == LUA_TTABLE, "No table at top of stack\n{}", *this);

		Ensure(::lua_getfield(m_Lua.get(), -1, "x") == LUA_TNUMBER, "Could not get 'x' field\n{}", *this);
		result.x = static_cast<float>(::lua_tonumber(m_Lua.get(), -1));
		::lua_pop(m_Lua.get(), 1);

		Ensure(::lua_getfield(m_Lua.get(), -1, "y") == LUA_TNUMBER, "Could not get 'y' field\n{}", *this);
		result.y = static_cast<float>(::lua_tonumber(m_Lua.get(), -1));
		::lua_pop(m_Lua.get(), 1);

		Ensure(::lua_getfield(m_Lua.get(), -1, "z") == LUA_TNUMBER, "Could not get 'z' field\n{}", *this);
		result.z = static_cast<float>(::lua_tonumber(m_Lua.get(), -1));
		::lua_pop(m_Lua.get(), 1);

		::lua_pop(m_Lua.get(), 1);
	}

	std::string LuaScipt::to_string() const
	{
		return Game::to_string({ m_Lua.get() });
	}

	std::string to_string(LuaStateObjWrapper obj)
	{
		auto* state = obj.state;
		std::stringstream strm{};

		const int stackSize = ::lua_gettop(state);
		if (stackSize == 0)
			return "<empty stack>";

		for (const int index : std::views::iota(1, stackSize + 1) | std::views::reverse)
		{
			const int type = ::lua_type(state, index);
			switch (type)
			{
			case LUA_TNIL: strm << "LUA_TNIL"; break;
			case LUA_TNUMBER:
			{
				if (::lua_isinteger(state, index) == 1)
				{
					strm << std::format("LUA_TNUMBER (int) {}", ::lua_tointeger(state, index));
				}
				else
				{
					strm << std::format("LUA_TNUMBER (float) {}", ::lua_tonumber(state, index));
				}
				break;
			}
			case LUA_TBOOLEAN: strm << std::format("LUA_TBOOLEAN {}", ::lua_toboolean(state, index)); break;
			case LUA_TSTRING: strm << std::format("LUA_TSTRING '{}'", ::lua_tostring(state, index)); break;
			case LUA_TTABLE: strm << "LUA_TTABLE"; break;
			case LUA_TFUNCTION: strm << "LUA_TFUNCTION"; break;
			case LUA_TUSERDATA: strm << "LUA_TUSERDATA"; break;
			case LUA_TTHREAD: strm << "LUA_TTHREAD"; break;
			case LUA_TLIGHTUSERDATA: strm << "LUA_TLIGHTUSERDATA"; break;
			default: strm << "unknown"; break;
			}

			strm << "\n";
		}

		return strm.str();
	}

}
