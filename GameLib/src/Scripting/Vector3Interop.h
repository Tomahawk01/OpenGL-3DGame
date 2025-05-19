#pragma once

#include <lua.hpp>

namespace Game {

	int Vector3Constructor(::lua_State* state);
	int Vector3Add(::lua_State* state);
	int Vector3Sub(::lua_State* state);
	int Vector3Mul(::lua_State* state);
	int Vector3Unm(::lua_State* state);
	int Vector3Eq(::lua_State* state);
	int Vector3ToString(::lua_State* state);
	int Vector3Distance(::lua_State* state);

}
