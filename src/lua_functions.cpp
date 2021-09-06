#include <string>
#include <fmt/color.h>

#include "lua_functions.h"
#include "settings.h"
#include "jira/jira.h"

static int lua_print(lua_State *lua_state)
{
    std::string str = lua_tostring(lua_state, 1);
    fmt::print("{}\n", str);
    return 0;
}

static void print_error(const std::string &error_message)
{
    fmt::print(fg(fmt::color::red), "{}\n", error_message);
}

static int lua_exit(lua_State *lua_state)
{
    exit(EXIT_SUCCESS);
    return 0;
}

namespace JiraTable
{
    int create(lua_State* lua_state)
    {
        // The argument is the JQL used in RAII.
        // if(1 != lua_isstring(lua_state, 1))
        if (lua_type(lua_state, 1) != LUA_TSTRING)
        {
            fmt::print(fg(fmt::color::light_salmon), "Usage: Jira.Create(string: jql command)\n");
            return luaL_typeerror(lua_state, 1, "string");
        }

        // Initialize a new resource shared pointer.
        try
        {
            const char* jql = lua_tostring(lua_state, 1);
            auto jira = std::make_shared<Jira>(jql);

            // The shared space with Lua needs to be specifically allocated this way:
            void *user_data = lua_newuserdata(lua_state, sizeof(std::shared_ptr<Jira>));

            // Throw an allocation if memory
            if (!user_data)
            {
                fmt::print(fg(fmt::color::red), "Not enough memory.");
                return 0;
            }

            // The "placement new operator" will create the object on the preallocated memory.
            new(user_data) std::shared_ptr<Jira>(jira);

            // Now just set the metatable on this new object
            luaL_getmetatable(lua_state, "Jira");
            lua_setmetatable(lua_state, -2);

            if (Settings::get_instance().verbose)
            {
                fmt::print(fg(fmt::color::light_green),
                           "Successfully loaded {} keys.", jira->get_keys().size());
                fmt::print("\n");
            }

            return 1;
        }
        catch(const std::invalid_argument& e)
        {
            fmt::print(fg(fmt::color::red), "{}\n", e.what());
        }

        return 0;
    }

    int destroy(lua_State* lua_state)
    {
        void* jira_reference = luaL_checkudata(lua_state, 1, "Jira");

        if (jira_reference)
        {
            auto resource = static_cast<std::shared_ptr<Jira>*>(jira_reference);
            resource->reset();
        }

        return 0;
    }

    int get_keys(lua_State* lua_state)
    {
        void* jira_reference = luaL_checkudata(lua_state, 1, "Jira");

        if (jira_reference)
        {
            auto jira = static_cast<std::shared_ptr<Jira>*>(jira_reference);

            lua_newtable(lua_state);

            int index = 0;

            auto keys = (*jira)->get_keys();

            for (const auto& key : keys)
            {
                // lua_pushstring(L, row->date);
                // lua_setfield(L, -2, "date");

                lua_pushinteger(lua_state, index++);
                lua_pushstring(lua_state, key.c_str());
                lua_settable(lua_state, -3);
            }

            return 1;
        }
        else
        {
            print_error("Object not found.");
        }

        return 0;
    }

    int get_count(lua_State* lua_state)
    {
        void* jira_reference = luaL_checkudata(lua_state, 1, "Jira");

        if (jira_reference)
        {
            auto jira = static_cast<std::shared_ptr<Jira>*>(jira_reference);

            lua_pushinteger(lua_state, (*jira)->get_count());

            return 1;
        }
        else
        {
            print_error("Object not found.");
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
        {"Keys", JiraTable::get_keys},
        {"Count", JiraTable::get_count},
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
