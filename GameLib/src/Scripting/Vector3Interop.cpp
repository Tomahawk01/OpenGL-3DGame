#include "Vector3Interop.h"

#include "Utilities/Error.h"
#include "Utilities/Formatter.h"
#include "Math/Vector3.h"
#include "LuaScript.h"

namespace {
	
	Game::vec3 PopVector3(::lua_State* state)
	{
		Game::vec3 result{};
		Game::LuaStateObjWrapper wrappedState{ state };

		Game::Ensure(::lua_type(state, -1) == LUA_TTABLE, "No table at top of stack\n{}", wrappedState);

		Game::Ensure(::lua_getfield(state, -1, "x") == LUA_TNUMBER, "Could not get 'x' field\n{}", wrappedState);
		result.x = static_cast<float>(::lua_tonumber(state, -1));
		::lua_pop(state, 1);

		Game::Ensure(::lua_getfield(state, -1, "y") == LUA_TNUMBER, "Could not get 'y' field\n{}", wrappedState);
		result.y = static_cast<float>(::lua_tonumber(state, -1));
		::lua_pop(state, 1);

		Game::Ensure(::lua_getfield(state, -1, "z") == LUA_TNUMBER, "Could not get 'z' field\n{}", wrappedState);
		result.z = static_cast<float>(::lua_tonumber(state, -1));
		::lua_pop(state, 1);

		::lua_pop(state, 1);

		return result;
	}

	void PushVector3(::lua_State* state, const Game::vec3& v)
	{
		::lua_newtable(state);
		::luaL_setmetatable(state, "vec3");

		::lua_pushnumber(state, v.x);
		::lua_setfield(state, -2, "x");

		::lua_pushnumber(state, v.y);
		::lua_setfield(state, -2, "y");

		::lua_pushnumber(state, v.z);
		::lua_setfield(state, -2, "z");
	}

}

namespace Game {

	int Vector3Constructor(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState { state };
		Ensure(::lua_isnumber(state, -3) == 1, "Stack index 1 not number:\n{}", wrappedState);
		const auto x = static_cast<float>(::lua_tonumber(state, -3));
		Ensure(::lua_isnumber(state, -2) == 1, "Stack index 2 not number:\n{}", wrappedState);
		const auto y = static_cast<float>(::lua_tonumber(state, -2));
		Ensure(::lua_isnumber(state, -1) == 1, "Stack index 3 not number:\n{}", wrappedState);
		const auto z = static_cast<float>(::lua_tonumber(state, -1));

		::lua_pop(state, 3);

		PushVector3(state, { x, y, z });

		return 1;
	}

	int Vector3Add(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);
		
		const auto vec2 = PopVector3(state);
		const auto vec1 = PopVector3(state);
		PushVector3(state, vec1 + vec2);

		return 1;
	}

	int Vector3Sub(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);

		const auto vec2 = PopVector3(state);
		const auto vec1 = PopVector3(state);
		PushVector3(state, vec1 - vec2);

		return 1;
	}

	int Vector3Mul(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);

		const auto vec2 = PopVector3(state);
		const auto vec1 = PopVector3(state);
		PushVector3(state, vec1 * vec2);

		return 1;
	}

	int Vector3Unm(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);

		const auto vec = PopVector3(state);
		PushVector3(state, -vec);

		return 1;
	}

	int Vector3Eq(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);

		const auto vec2 = PopVector3(state);
		const auto vec1 = PopVector3(state);
		::lua_pushboolean(state, vec1 == vec2);

		return 1;
	}

	int Vector3ToString(::lua_State* state)
	{
		LuaStateObjWrapper wrappedState{ state };
		Ensure(::lua_gettop(state) >= 2, "No results to get!\n{}", wrappedState);

		const auto vec = PopVector3(state);
		::lua_pushstring(state, vec.to_string().c_str());

		return 1;
	}

}
