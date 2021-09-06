#include <iostream>
#include <string>
#include <fmt/color.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"
#include "lua_jira.h"

int main(void)
{
    fmt::print(fg(fmt::color::light_green) |
               bg(fmt::color::green) |
               fmt::emphasis::bold,
               "Jira Lab v0.1");
    fmt::print("\n2021 John Simoes - Vancouver, BC\n\n");

    // Avoid file tab completion.
    rl_bind_key('\t', rl_insert);

    // Initialize Lua VM context.
    lua_State *lua_state = luaL_newstate();

    // Load all default libraries on Lua.
    luaL_openlibs (lua_state);

    LuaJira::register_functions(lua_state);

    // register_lua_functions (lua_state);

    /*
     * Read standard input and execute each line in Lua VM.
     */
    bool multi = false;

    std::string multiline_buffer = "";

    char* readline_buffer;

    while ( (readline_buffer = readline(multi? ">>" : ">")) != nullptr)
    {
        // Use readline history and delete the allocated memory.
        std::string line = readline_buffer;
        if (strlen(readline_buffer) > 0)
            add_history(readline_buffer);
        free(readline_buffer);

        multiline_buffer += line;

        // If starts with "=", wrap with print().
        if (!multi && multiline_buffer.find("=") == 0)
        {
            multiline_buffer.replace(0, 1, "print(");
            multiline_buffer += ")";
        }

        int error = luaL_loadstring(lua_state, multiline_buffer.c_str());

        if (error != LUA_ERRSYNTAX)
        {
            int result = lua_pcall(lua_state, 0, LUA_MULTRET, 0);
            if (result != LUA_OK)
            {
                print_error(lua_tostring(lua_state, -1));
            }

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
                multi = false;
                multiline_buffer = "";

                print_error(error_message);
            }
        }

        lua_settop(lua_state, 0);
    }

    lua_close(lua_state);

    return EXIT_SUCCESS;
}