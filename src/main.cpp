#include <string>
#include <fmt/color.h>
#include <iostream>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <readline/readline.h>
#include <readline/history.h>

#include <libstein.h>

#include "util.h"
#include "jira/lua_jira.h"
#include "util/lua_util.h"
#include "base/lua_base.h"
#include "settings.h"

#define COMMAND_HISTORY_FILE ".jira-lab-history"

char* get_prompt(lua_State *lua_state, bool multi, bool verbose)
{
    static char buffer[256];
    static char empty[] = "";

    int used_memory = lua_gc(lua_state, LUA_GCCOUNT, 0);
    sprintf(buffer, "[%d kb]%s", used_memory, (multi? ">>": ">"));

    return verbose ? buffer : empty;
}

void load_library(lua_State *lua_state, const char* name, lua_CFunction function)
{
    luaL_requiref(lua_state, name, function, 1);
    lua_pop(lua_state, 1);
}

void lua_loop(lua_State *lua_state)
{
    Settings &settings = Settings::get_instance();

    /*
     * Read standard input and execute each line in Lua VM.
     */

    bool multi = false;
    char* readline_buffer = nullptr;
    std::string multiline_buffer = "";

    while (true)
    {
        printf("\e[?25h"); // Display the cursor

        // Build prompt.
        static char prompt_buffer[64];
        static char empty[] = "";
        int used_memory = lua_gc(lua_state, LUA_GCCOUNT, 0);
        sprintf(prompt_buffer, "[%d kb]>%s", used_memory, (multi? ">": ""));

        std::cout << (settings.verbose? prompt_buffer : empty) << std::flush;
        readline_buffer = readline("");//settings.verbose ? prompt_buffer : empty);

        printf("\e[?25l"); // Hide the cursor
        if (readline_buffer == nullptr)
            break;

        // Use readline history and delete the allocated memory.
        std::string line = readline_buffer;
        if (strlen(readline_buffer) == 0) continue;

        add_history(readline_buffer);
        write_history(COMMAND_HISTORY_FILE);
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

            // Perform garbage collecting.
            lua_gc(lua_state, LUA_GCCOLLECT, 0);
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
}

int main(int argc, char **argv)
{
    libstein::CachedRest::is_delay_milisseconds = 0;
    libstein::Arguments args(argc, argv);
    libstein::CommandLine cmd;

    auto results = cmd.description("Jira-Lab, a Lua-powered sandbox to analyse Jira tickets.")
        .parameter("a,autorun","Initial script to load.", false)
        .parameter("s,scriptmode","Disable interactions and run on script-only mode.", false)
        .parameter("l,localmode","Enable default libraries - UNSAFE in servers.", false)
        .parameter("n,nocache","Disable caching in local Redis.", false)
        .eval(args);

    if (!results.valid)
    {
        for (auto& line : results.output)
            fmt::print("{}\n", line);

        exit(EXIT_FAILURE);
    }

    Settings &settings = Settings::get_instance();

    libstein::CachedRest::is_redis_enabled = !cmd.isSet("n");

    if (cmd.isSet("s"))
    {
        settings.verbose = false;
    }

    if (cmd.hasValue("a"))
    {
        settings.autorun = true;
        settings.autorun_filename = cmd.getValue("a");
    }

    if (settings.verbose)
    {
        fmt::print(fg(fmt::color::light_green) |
                bg(fmt::color::green) |
                fmt::emphasis::bold,
                "Jira Lab v0.2");

        fmt::print("\n2021 John Simoes - Vancouver, BC\n\n");
    }

    try
    {
        // Avoid file tab completion.
        rl_bind_key('\t', rl_insert);

        // Use command history.
        read_history(COMMAND_HISTORY_FILE);

        // Initialize Lua VM context.
        lua_State *lua_state = luaL_newstate();

        // Load libraries on Lua.
        load_library(lua_state, "", luaopen_base);
        load_library(lua_state, LUA_TABLIBNAME, luaopen_table);
        load_library(lua_state, LUA_STRLIBNAME, luaopen_string);
        load_library(lua_state, LUA_MATHLIBNAME, luaopen_math);
        // load_library(lua_state, LUA_DBLIBNAME, luaopen_debug);
        load_library(lua_state, LUA_UTF8LIBNAME, luaopen_utf8);

        // If in local mode, load more 'dangerous' libraries (that could give access to the file system remotely, for example).
        if (settings.localmode)
        {
            load_library(lua_state, LUA_OSLIBNAME, luaopen_os);
            // load_library(lua_state, LUA_IOLIBNAME, luaopen_io);  // Security breach if left on
            load_library(lua_state, LUA_LOADLIBNAME, luaopen_package);
        }

        LuaJira::register_functions(lua_state);
        LuaUtil::register_functions(lua_state);
        LuaBase::register_functions(lua_state);

        Settings &settings = Settings::get_instance();
        if (settings.autorun)
        {
            if (settings.verbose)
                fmt::print("Loading {}...\n", settings.autorun_filename);

            if (LUA_OK != luaL_loadfile(lua_state, settings.autorun_filename.c_str()) ||
                        lua_pcall(lua_state, 0, 0, 0))
            {
                print_error(lua_tostring(lua_state, -1));
            }
        }

        lua_loop(lua_state);
    }
    catch(const std::exception& e)
    {
        print_error(e.what());
    }

    return EXIT_SUCCESS;
}
