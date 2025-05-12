#include "Vector3Interop.h"

#include "Utilities/Error.h"
#include "Utilities/Formatter.h"
#include "LuaScript.h"

namespace Game {

	int Vector3Constructor(::lua_State* state)
	{
		auto wrappedState = LuaStateObjWrapper{ state };
		Ensure(::lua_isnumber(state, -3) == 1, "Stack index 1 not number:\n{}", wrappedState);
		const auto x = ::lua_tonumber(state, -3);
		Ensure(::lua_isnumber(state, -2) == 1, "Stack index 2 not number:\n{}", wrappedState);
		const auto y = ::lua_tonumber(state, -2);
		Ensure(::lua_isnumber(state, -1) == 1, "Stack index 3 not number:\n{}", wrappedState);
		const auto z = ::lua_tonumber(state, -1);

		::lua_pop(state, 3);

		::lua_newtable(state);

		::lua_pushnumber(state, x);
		::lua_setfield(state, -2, "x");

		::lua_pushnumber(state, y);
		::lua_setfield(state, -2, "y");

		::lua_pushnumber(state, z);
		::lua_setfield(state, -2, "z");

		return 1;
	}

}
