#pragma once

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

void register_lua_functions (lua_State *lua_state);
