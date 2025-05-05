#include "Vector3Interop.h"

namespace Game {

	int Vector3Constructor(::lua_State* state)
	{
		const auto x = ::luaL_checknumber(state, 1);
		const auto y = ::luaL_checknumber(state, 2);
		const auto z = ::luaL_checknumber(state, 3);

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
