#pragma once

#include <nlohmann/json.hpp>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

class JsonToLua
{
public:
    JsonToLua(lua_State *lua_state, const nlohmann::json &json, const bool is_table, const bool is_index);
};
