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
    bool multi = false;
    std::string line;

    fmt::print("{}", (multi? ">>" : ">"));

    std::string multiline_buffer = "";

    while (std::getline(std::cin, line))
    {
        multiline_buffer += line;

        int error = luaL_loadstring(lua_state, multiline_buffer.c_str());

        if (error != LUA_ERRSYNTAX)
        {
            lua_pcall(lua_state, 0, LUA_MULTRET, 0);
            multi = false;
            multiline_buffer = "";
        }
        else
        {
            std::string error_message = lua_tostring(lua_state, -1);

            // Lua "waiting for more" errors always end with "<eof>".
            if (error_message.rfind("<eof>") == error_message.length() - 5)
            {
                multi = true;
                multiline_buffer += "\n";
            }
            else
            {
                fmt::print("len:{}, eof_found:{}\n", error_message.length(), error_message.rfind("<eof>"));
                multi = false;
                multiline_buffer = "";

                fmt::print(fg(fmt::color::red), "{}\n", error_message);
            }
        }

        fmt::print("{}", (multi? ">>" : ">"));
    }

    lua_close(lua_state);

    return EXIT_SUCCESS;
}