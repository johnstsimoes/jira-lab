#include <iostream>
#include <string>
#include <fmt/color.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

static int lua_print(lua_State *lua_state)
{
    std::string str = lua_tostring(lua_state, 1);
    fmt::print("{}\n", str);
    return 0;
}

static int lua_exit(lua_State *lua_state)
{
    exit(EXIT_SUCCESS);
    return 0;
}

int main(void)
{
    fmt::print(fg(fmt::color::light_green) |
               bg(fmt::color::green) |
               fmt::emphasis::bold,
               "Jira Lab v0.1");
    fmt::print("\n2021 John Simoes\n\n");

    lua_State *lua_state = luaL_newstate();

    static const luaL_Reg lualibs[] = {
        { "base", luaopen_base },
        { NULL, NULL}
    };

    lua_pushcfunction(lua_state, lua_print);
    lua_setglobal(lua_state, "print");

    lua_pushcfunction(lua_state, lua_exit);
    lua_setglobal(lua_state, "exit");

    for (std::string line; std::getline(std::cin, line);)
    {
        int error = luaL_loadbuffer(lua_state, line.c_str(), line.length(), "line") ||
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
