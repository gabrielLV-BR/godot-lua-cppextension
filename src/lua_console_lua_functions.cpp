#include "lua_console.hpp"
#include "lua_constants.hpp"

extern "C" {
    #include <stdio.h>

    #include "lua/lua.h"
    #include "lua/lualib.h"
    #include "lua/lauxlib.h"

    int _print_callback(lua_State *state) {
        const char *message  = lua_tostring(state, -1);

        lua_getfield(state, LUA_REGISTRYINDEX, LuaConsole::KEY);
        auto instance = static_cast<LuaConsole*>(lua_touserdata(state, -1));

        instance->emit_signal(Signals::PRINT, message);

        return 0;
    }

    int _method_callback(lua_State *state) {
        int argument_count = lua_gettop(state);

        const char *method_name_str = lua_tostring(state, lua_upvalueindex(1));
        std::string method_name(method_name_str);

        lua_getfield(state, LUA_REGISTRYINDEX, LuaConsole::KEY);
        LuaConsole *instance = static_cast<LuaConsole*>(lua_touserdata(state, -1));

        lua_pop(state, 1);

        return instance->call_method_and_return(method_name);
    }
}