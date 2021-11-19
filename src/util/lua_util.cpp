#include "lua_util.h"
#include "../util.h"

#include <string>
#include <regex>

static int regex(lua_State* lua_state)
{
    if (    (lua_gettop(lua_state) != 2)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING)
        ||  (lua_type(lua_state, 2) != LUA_TSTRING))
    {
        print_warning("Usage: Util.Regex(string: to evaluate, string: regular expression)");
        return luaL_typeerror(lua_state, 1, "string");
    }

    const std::string string = lua_tostring(lua_state, 1);
    const std::regex regex(lua_tostring(lua_state, 2));

    lua_newtable(lua_state);
    int index = 0;

    for(std::sregex_iterator i = std::sregex_iterator(string.begin(), string.end(), regex);
                             i != std::sregex_iterator();
                             ++i)
    {
        std::smatch matches = *i;

        lua_pushinteger(lua_state, ++index);
        lua_newtable(lua_state);
        int column = 0;
        for (auto token: matches)
        {
            if (column > 0) // Ignore column 0, since it will be the entire group.
            {
                lua_pushinteger(lua_state, column);
                lua_pushstring(lua_state, token.str().c_str());
                lua_settable(lua_state, -3);
            }
            column++;
        }
        lua_settable(lua_state, -3);
    }

    return 1;
}

void LuaUtil::register_functions(lua_State* lua_state)
{
    const luaL_Reg table_definition[] =
    {
        {"Regex", regex},
        {NULL, NULL}
    };

    lua_newtable(lua_state);
    luaL_newmetatable(lua_state, "Util");
    luaL_setfuncs(lua_state, table_definition, 0);
    lua_pushliteral(lua_state, "__index");
    lua_pushvalue(lua_state, -2);
    lua_rawset(lua_state, -3);
    lua_setglobal(lua_state, "Util");
}
