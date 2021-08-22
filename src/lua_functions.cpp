#include "lua_functions.h"
#include "jira/jira.h"

#include <fmt/color.h>
#include <string>

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

namespace JiraTable
{
    int create(lua_State* state)
    {
        // The argument is the JQL used in RAII.
        if(!lua_isstring(state, 1))
        {
            return luaL_typeerror(state, 1, "string");
        }

        // Initialize a new resource shared pointer.
        try
        {
            const char* jql = lua_tostring(state, 1);
            auto jira = std::make_shared<Jira>(jql);

            // The shared space with Lua needs to be specifically allocated this way:
            void *user_data = lua_newuserdata(state, sizeof(std::shared_ptr<Jira>));

            // TODO allocation failure should issue an jira_error.
            if (!user_data)
            {
                return 0;
            }

            // The "placement new operator" will create the object on the preallocated memory.
            new(user_data) std::shared_ptr<Jira>(jira);

            // Now just set the metatable on this new object
            luaL_getmetatable(state, "Jira");
            lua_setmetatable(state, -2);

            return 1;
        }
        catch(const std::exception& e)
        {
            fmt::print(fg(fmt::color::red), "{}\n", e.what());
        }

        return 0;
    }

    int destroy(lua_State* state)
    {
        void* jira_reference = luaL_checkudata(state, 1, "Jira");

        if (jira_reference)
        {
            auto resource = static_cast<std::shared_ptr<Jira>*>(jira_reference);
            resource->reset();
        }

        return 0;
    }

    int get_keys(lua_State* state)
    {
        void* jira_reference = luaL_checkudata(state, 1, "Jira");

        if (jira_reference)
        {
            auto jira = static_cast<std::shared_ptr<Jira>*>(jira_reference);
            lua_pushstring(state, (*jira)->get_keys().c_str());
            return 1;
        }

        return 0;
    }
}

static void helper_register_lua_function(lua_State *lua_state, const char *name, lua_CFunction function)
{
    lua_pushcfunction(lua_state, function);
    lua_setglobal(lua_state, name);
}

void register_lua_functions (lua_State *lua_state)
{
    // Register common functions.
    helper_register_lua_function (lua_state, "Print", lua_print);
    helper_register_lua_function (lua_state, "Exit", lua_exit);

    const luaL_Reg jira_table_definition[] =
    {
        {"Create", JiraTable::create},
        {"__gc", JiraTable::destroy},
        {"GetKeys", JiraTable::get_keys},
        {NULL, NULL}
    };

    lua_newtable(lua_state);
    luaL_newmetatable(lua_state, "Jira");
    luaL_setfuncs(lua_state, jira_table_definition, 0);
    lua_pushliteral(lua_state, "__index");
    lua_pushvalue(lua_state, -2);
    lua_rawset(lua_state, -3);
    lua_setglobal(lua_state, "Jira");
}
