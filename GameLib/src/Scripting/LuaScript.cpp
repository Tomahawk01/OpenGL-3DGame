#include "LuaScript.h"

#include "Utilities/Error.h"

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
		Ensure(m_Lua, "Failed to create lua state!");

		::luaL_openlibs(m_Lua.get());

		LoadData loadData{ source, 0 };

		Ensure(::lua_load(m_Lua.get(), &LoadStringView, reinterpret_cast<void*>(&loadData), "lua_script", "t") == LUA_OK, "Failed to load lua source!");

		Ensure(::lua_pcall(m_Lua.get(), 0, 0, 0) == LUA_OK, "Failed to initialize script!");
	}

	void LuaScipt::SetFunction(const std::string& name) const
	{
		const auto retType = ::lua_getglobal(m_Lua.get(), name.c_str());
		Ensure(retType == LUA_TFUNCTION, "Missing function {}", name);
	}

	void LuaScipt::Execute(uint32_t numArgs, uint32_t numResults) const
	{
		if (::lua_pcall(m_Lua.get(), numArgs, numResults, 0) != LUA_OK)
		{
			const auto res = ::lua_tostring(m_Lua.get(), -1);
			::lua_pop(m_Lua.get(), 1);

			throw Exception(std::format("Failed to execute ({})", res));
		}
	}

	void LuaScipt::SetArgument(std::string_view value) const
	{
		::lua_pushlstring(m_Lua.get(), value.data(), value.size());
	}

}
