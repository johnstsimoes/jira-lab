#include "lua_base.h"

#include <string>

static int exit(lua_State* lua_state)
{
    printf("\e[?25h"); // Display the cursor

    std::exit(EXIT_SUCCESS);
    return 0;
}

void LuaBase::register_functions(lua_State* lua_state)
{
    lua_pushcfunction(lua_state, exit);
    lua_setglobal(lua_state, "exit");
}
