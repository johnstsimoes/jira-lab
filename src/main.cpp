#include <iostream>
#include <string>
#include <fmt/color.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "lua_functions.h"

int main(void)
{
    fmt::print(fg(fmt::color::light_green) |
               bg(fmt::color::green) |
               fmt::emphasis::bold,
               "Jira Lab v0.1");
    fmt::print("\n2021 John Simoes\n\n");

    // Initialize Lua VM context.
    lua_State *lua_state = luaL_newstate();

    // Load all default libraries on Lua.
    luaL_openlibs (lua_state);

    register_lua_functions (lua_state);

    /*
     * Read standard input and execute each line in Lua VM.
     */
    for (std::string line; std::getline(std::cin, line);)
    {
        int error = luaL_loadstring(lua_state, line.c_str()) ||
                    lua_pcall(lua_state, 0, 0, 0);

        if (error)
        {
            fmt::print(fg(fmt::color::red), "{}\n", lua_tostring(lua_state, -1));
            lua_pop(lua_state, 1);
        }
    }

    lua_close(lua_state);

    return EXIT_SUCCESS;
}