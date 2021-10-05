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

#include <libstein.h>

#include "util.h"
#include "jira/jira_metadata.h"
#include "lua_jira.h"
#include "settings.h"

#define COMMAND_HISTORY_FILE ".jira-lab-history"

char* get_prompt(lua_State *lua_state, bool multi)
{
    static char buffer[256];

    int used_memory = lua_gc(lua_state, LUA_GCCOUNT, 0);

    sprintf(buffer, "[%d kb]%s", used_memory, (multi? ">>": ">"));

    return buffer;
}

void load_library(lua_State *lua_state, const char* name, lua_CFunction function)
{
    lua_pushcfunction(lua_state, function);
    lua_pushstring(lua_state, name);
    lua_call(lua_state, 1, 0);
}

void lua_loop()
{
    Settings &settings = Settings::get_instance();

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
    load_library(lua_state, LUA_LOADLIBNAME, luaopen_package);
    load_library(lua_state, LUA_DBLIBNAME, luaopen_debug);

    if (settings.localmode)
    {
        // luaopen_coroutine
        // luaopen_io
        // luaopen_os
        // luaopen_utf8
    }

    LuaJira::register_functions(lua_state);

    if (settings.autorun)
    {
        fmt::print("Loading {}...\n", settings.autorun_filename);

        if (LUA_OK != luaL_loadfile(lua_state, settings.autorun_filename.c_str()) ||
                      lua_pcall(lua_state, 0, 0, 0))
        {
            print_error(lua_tostring(lua_state, -1));
        }
    }

    /*
     * Read standard input and execute each line in Lua VM.
     */
    bool multi = false;

    std::string multiline_buffer = "";

    char* readline_buffer;

    while ( (readline_buffer = readline(get_prompt(lua_state, multi))) != nullptr)
    {
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

std::string highlight(std::string text)
{
    return fmt::format(fg(fmt::color::gray) | fmt::emphasis::bold, text);
}

int main(int argc, char **argv)
{
    libstein::Arguments args(argc, argv);
    libstein::CommandLine cmd;

    auto results = cmd.description("Jira-Lab, a Lua-powered sandbox to analyse Jira tickets.")
        .parameter("a,autorun","Initial script to load.", false)
        .parameter("s,scriptmode","Disable interactions and run on script-only mode.", false)
        .parameter("l,localmode","Enable default libraries - UNSAFE in servers.", false)
        .eval(args);

    if (!results.valid)
    {
        for (auto& line : results.output)
            fmt::print("{}\n", line);

        exit(EXIT_FAILURE);
    }

    Settings &settings = Settings::get_instance();

    if (cmd.hasValue("s"))
    {
        settings.autorun = true;
        settings.autorun_filename = cmd.getValue("s");
    }

    fmt::print(fg(fmt::color::light_green) |
               bg(fmt::color::green) |
               fmt::emphasis::bold,
               "Jira Lab v0.1");

    fmt::print("\n2021 John Simoes - Vancouver, BC\n\n");

    try
    {
        auto &metadata = JiraMetadata::get_instance();

        lua_loop();
    }
    catch(const std::exception& e)
    {
        print_error(e.what());
        print_error("Could not load metadata - please check if environment settings are correct.\n");

        fmt::print("    {}: login name, usually an email address\n", highlight("JIRA_USER"));
        fmt::print("    {}: Jira API token\n", highlight("JIRA_TOKEN"));
        fmt::print("    {}: Jira instance address\n\n", highlight("JIRA_HOST"));
    }

    return EXIT_SUCCESS;
}
