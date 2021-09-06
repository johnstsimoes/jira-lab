#include <string>
#include <fmt/color.h>

#include "lua_jql.h"
#include "util.h"
#include "settings.h"

#include "jira/jira.h"

namespace JQLTable
{
    int create(lua_State* lua_state)
    {
        // The argument is the JQL used in RAII.
        // if(1 != lua_isstring(lua_state, 1))
        if (lua_type(lua_state, 1) != LUA_TSTRING)
        {
            fmt::print(fg(fmt::color::light_salmon), "Usage: JQL.Create(string: jql command)\n");
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
            luaL_getmetatable(lua_state, "JQL");
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
            print_error(e.what());
        }

        return 0;
    }

    int destroy(lua_State* lua_state)
    {
        void* jira_reference = luaL_checkudata(lua_state, 1, "JQL");

        if (jira_reference)
        {
            auto resource = static_cast<std::shared_ptr<Jira>*>(jira_reference);
            resource->reset();
        }

        return 0;
    }

    int get_keys(lua_State* lua_state)
    {
        void* jira_reference = luaL_checkudata(lua_state, 1, "JQL");

        if (jira_reference)
        {
            auto jira = static_cast<std::shared_ptr<Jira>*>(jira_reference);

            lua_newtable(lua_state);

            // Conventionally, Lua indexes start at 1.
            int index = 1;

            auto keys = (*jira)->get_keys();

            for (const auto& key : keys)
            {
                lua_pushinteger(lua_state, index++);
                lua_pushstring(lua_state, key.c_str());
                lua_settable(lua_state, -3);
            }

            return 1;
        }
        else
        {
            fmt::print("jira_reference found\n");

            print_error("Object not found.");
        }

        return 0;
    }
}

void LuaJQL::register_functions(lua_State* lua_state)
{
    const luaL_Reg jira_table_definition[] =
    {
        {"Create", JQLTable::create},
        {"__gc", JQLTable::destroy},
        {"Keys", JQLTable::get_keys},
        {NULL, NULL}
    };

    lua_newtable(lua_state);
    luaL_newmetatable(lua_state, "JQL");
    luaL_setfuncs(lua_state, jira_table_definition, 0);
    lua_pushliteral(lua_state, "__index");
    lua_pushvalue(lua_state, -2);
    lua_rawset(lua_state, -3);
    lua_setglobal(lua_state, "JQL");

}
