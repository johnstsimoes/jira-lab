#include "json_to_lua.h"

JsonToLua::JsonToLua(lua_State *lua_state, const nlohmann::json &json, bool is_table, bool is_index)
{
    int index = 1;

    if (json.is_array())
    {
        lua_newtable(lua_state);
        is_index = true;
    }
    else if (json.is_object())
    {
        lua_newtable(lua_state);
        is_table = true;
    }
    else if (json.is_null())
    {
        lua_pushnil(lua_state);
    }

    for (const auto &it : json.items())
    {
        if (is_index)
            lua_pushinteger(lua_state, index++);
        else if (is_table)
            lua_pushstring(lua_state, it.key().c_str());

        if (it.value().is_array())
        {
            JsonToLua(lua_state, it.value(), false, true);
        }
        if (it.value().is_object())
        {
            JsonToLua(lua_state, it.value(), true, false);
        }
        else
        {
            switch(it.value().type())
            {
                case nlohmann::json::value_t::null:
                {
                    lua_pushnil(lua_state);
                    break;
                }
                case nlohmann::json::value_t::boolean:
                {
                    lua_pushboolean(lua_state, it.value());
                    break;
                }
                case nlohmann::json::value_t::string:
                {
                    std::string value = it.value();
                    lua_pushstring(lua_state, value.c_str());
                    break;
                }
                case nlohmann::json::value_t::number_integer:
                {
                    lua_pushinteger(lua_state, it.value());
                    break;
                }
                case nlohmann::json::value_t::number_unsigned:
                {
                    lua_pushinteger(lua_state, it.value());
                    break;
                }
                case nlohmann::json::value_t::number_float:
                {
                    lua_pushnumber(lua_state, it.value());
                    break;
                }
                case nlohmann::json::value_t::object:
                    break;
                case nlohmann::json::value_t::array:
                    break;
                case nlohmann::json::value_t::binary:
                    break;
                case nlohmann::json::value_t::discarded:
                    break;
            }
        }

        if (is_table || is_index)
            lua_settable(lua_state, -3);
    }
}
