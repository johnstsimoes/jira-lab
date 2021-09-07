#include <string>
#include <fmt/color.h>

#include "lua_jira.h"
#include "util.h"
#include "settings.h"

#include "jira/jira_jql.h"
#include "jira/jira_components.h"

static int run_jql(lua_State* lua_state)
{
    // The argument is the JQL used in RAII.
    if (lua_type(lua_state, 1) != LUA_TSTRING)
    {
        fmt::print(fg(fmt::color::light_salmon), "Usage: Jira.JQL(string: jql command)\n");
        return luaL_typeerror(lua_state, 1, "string");
    }

    try
    {
        const char* jql_statement = lua_tostring(lua_state, 1);

        JiraJQL jql(jql_statement);

        lua_newtable(lua_state);

        // Conventionally, Lua indexes start at 1.
        int index = 0;

        for (const auto& key : jql.get_keys())
        {
            lua_pushinteger(lua_state, ++index);
            lua_pushstring(lua_state, key.c_str());
            lua_settable(lua_state, -3);
        }

        if (Settings::get_instance().verbose)
        {
            fmt::print(fg(fmt::color::light_green), "Successfully loaded {} keys.", index);
            fmt::print("\n");
        }

        return 1;
    }
    catch(const std::invalid_argument& e)
    {
        print_error(e.what());
    }

    return 0;
}

static int get_components(lua_State* lua_state)
{
    // The argument is the project name used in RAII.
    if (lua_type(lua_state, 1) != LUA_TSTRING)
    {
        fmt::print(fg(fmt::color::light_salmon), "Usage: Jira.Components(string: project name)\n");
        return luaL_typeerror(lua_state, 1, "string");
    }

    try
    {
        const char* project_name = lua_tostring(lua_state, 1);

        JiraComponents components(project_name);

        lua_newtable(lua_state);

        // Conventionally, Lua indexes start at 1.
        int index = 0;

        for (const auto& component : components.get_results())
        {
            lua_pushinteger(lua_state, ++index);
            lua_pushstring(lua_state, component.c_str());
            lua_settable(lua_state, -3);
        }

        if (Settings::get_instance().verbose)
        {
            fmt::print(fg(fmt::color::light_green),
                        "Successfully loaded {} components.", index);
            fmt::print("\n");
        }

        return 1;
    }
    catch(const std::invalid_argument& e)
    {
        print_error(e.what());
    }

    return 0;
}

void LuaJira::register_functions(lua_State* lua_state)
{
    const luaL_Reg table_definition[] =
    {
        {"JQL", run_jql},
        {"Components", get_components},
        {NULL, NULL}
    };

    lua_newtable(lua_state);
    luaL_newmetatable(lua_state, "Jira");
    luaL_setfuncs(lua_state, table_definition, 0);
    lua_pushliteral(lua_state, "__index");
    lua_pushvalue(lua_state, -2);
    lua_rawset(lua_state, -3);
    lua_setglobal(lua_state, "Jira");
}
