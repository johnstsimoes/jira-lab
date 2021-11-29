#include <string>
#include <ctime>
#include <libstein.h>
#include <fmt/core.h>

#include "../util.h"
#include "../settings.h"
#include "../json_to_lua.h"

#include "lua_jira.h"
#include "jira_jql.h"
#include "jira_components.h"
#include "jira_metadata.h"
#include "jira_changes.h"
#include "jira_ticket.h"


static int run_jql(lua_State* lua_state)
{
    // The argument is the JQL used in RAII.
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING))
    {
        print_warning("Usage: Jira.JQL(string: jql command)");
        return luaL_typeerror(lua_state, 1, "string");
    }

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
        print_successfully_loaded(index, "keys");
    }

    return 1;
}

static int get_components(lua_State* lua_state)
{
    // The argument is the project name used in RAII.
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING))
    {
        print_warning("Usage: Jira.Components(string: project name)");
        return luaL_typeerror(lua_state, 1, "string");
    }

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
        print_successfully_loaded(index, "components");
    }

    return 1;
}

static int get_fields(lua_State* lua_state)
{
    if (lua_gettop(lua_state) > 0)
    {
        print_warning("Usage: Jira.Fields()");
        return 0;
    }

    lua_newtable(lua_state);
    int index = 0;

    for (const auto& field : JiraMetadata::get_instance().fields_)
    {
        lua_pushinteger(lua_state, ++index);
        lua_pushstring(lua_state, field.second.c_str());
        lua_settable(lua_state, -3);
    }

    return 1;
}

static int get_changes(lua_State* lua_state)
{
    // The arguments are ticket key and field.
    if (    (lua_gettop(lua_state) != 2)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING)
        ||  (lua_type(lua_state, 2) != LUA_TSTRING))
    {
        print_warning("Usage: Jira.Changes(string: ticket key, string: field)");
        return luaL_typeerror(lua_state, 1, "string");
    }

    const char* luavalue_key = lua_tostring(lua_state, 1);
    const char* luavalue_field = lua_tostring(lua_state, 2);

    std::string field_name = "";

    // Retrieve field
    for (auto& entry : JiraMetadata::get_instance().fields_)
    {
        if (luavalue_field == entry.second)
        {
            field_name = entry.first;
            break;
        }
    }

    if (field_name.empty())
    {
        print_warning("Field not found.");
        return 0;
    }

    auto changes = JiraChanges(luavalue_key, field_name).get_results();

    lua_newtable(lua_state);
    int index = 0;
    for (auto& entry : changes)
    {
        lua_pushinteger(lua_state, ++index);

        lua_newtable(lua_state);

        // Timestamp entry
        lua_pushliteral(lua_state, "timestamp");
        lua_pushinteger(lua_state, std::mktime(&entry.timestamp));
        lua_settable(lua_state, -3);

        // New state
        lua_pushliteral(lua_state, "value");
        lua_pushstring(lua_state, entry.state.c_str());
        lua_settable(lua_state, -3);

        lua_settable(lua_state, -3);
    }

    return 1;
}

static int get_load(lua_State* lua_state)
{
    // The arguments are ticket key and field.
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING))
    {
        print_warning("Usage: Jira.Load(string: ticket key)");
        return luaL_typeerror(lua_state, 1, "string");
    }

    const char* luavalue_key = lua_tostring(lua_state, 1);

    JiraTicket ticket(luavalue_key);
    auto ticket_fields = ticket.get_fields();

    lua_newtable(lua_state);
    for (auto& entry : ticket_fields)
    {
        // Add field
        lua_pushstring(lua_state, entry.first.c_str());
        JsonToLua parser(lua_state, nlohmann::json::parse(entry.second), false, false);

        lua_settable(lua_state, -3);
    }

    return 1;
}

static int set_verbose(lua_State* lua_state)
{
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TBOOLEAN))
    {
        print_warning("Usage: Jira.Verbose(bool: interactive feedback)");
        return luaL_typeerror(lua_state, 1, "boolean");
    }

    bool enabled = lua_toboolean(lua_state, 1);

    bool old_value = Settings::get_instance().verbose;
    Settings::get_instance().verbose = enabled;

    lua_pushboolean(lua_state, old_value);

    return 1;
}

static int set_delay(lua_State* lua_state)
{
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TNUMBER))
    {
        print_warning("Usage: Jira.Delay(number: time in milisseconds)");
        return luaL_typeerror(lua_state, 1, "number");
    }

    int delay = lua_tonumber(lua_state, 1);

    libstein::CachedRest::is_delay_milisseconds = delay;

    return 0;
}

static int parse_date(lua_State* lua_state)
{
    if (    (lua_gettop(lua_state) != 1)
        ||  (lua_type(lua_state, 1) != LUA_TSTRING))
    {
        print_warning("Usage: Jira.ParseDate(string: date in format YYYY-MM-ddTHH:MM:SS)");
        return luaL_typeerror(lua_state, 1, "string");
    }

    const char* date = lua_tostring(lua_state, 1);

    std::tm parsed = jira_parse_date(date);

    lua_pushinteger(lua_state, std::mktime(&parsed));

    return 1;
}

void LuaJira::register_functions(lua_State* lua_state)
{
    const luaL_Reg table_definition[] =
    {
        {"JQL", run_jql},
        {"Components", get_components},
        {"Fields", get_fields},
        {"Changes", get_changes},
        {"Load", get_load},
        {"Verbose", set_verbose},
        {"Delay", set_delay},
        {"ParseDate", parse_date},
        {NULL, NULL}
    };

    lua_newtable(lua_state);
    luaL_newmetatable(lua_state, "Jira");
    luaL_setfuncs(lua_state, table_definition, 0);
    lua_pushliteral(lua_state, "__index");
    lua_pushvalue(lua_state, -2);
    lua_rawset(lua_state, -3);
    lua_setglobal(lua_state, "Jira");

    // Load Jira metadata.
    try
    {
        JiraMetadata::get_instance();
    }
    catch(const std::exception& e)
    {
        print_error(e.what());
        print_error("Could not load metadata - please check if environment settings are correct.\n");

        fmt::print("    {}: login name, usually an email address\n", color_highlight("JIRA_USER"));
        fmt::print("    {}: Jira API token\n", color_highlight("JIRA_TOKEN"));
        fmt::print("    {}: Jira instance address\n\n", color_highlight("JIRA_HOST"));
    }
}
