#pragma once

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

class LuaUtil
{
public:
    static void register_functions(lua_State* lua_state);
};
