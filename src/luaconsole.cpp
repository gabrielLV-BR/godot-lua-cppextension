#include "luaconsole.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

static const std::string CLASS_KEY = "LuaConsole";

namespace Signals {
    const char *PRINT = "print\0";
    const char *CALLED_METHOD = "called_method\0";
    const char *ERROR = "error\0";
}

namespace GlobalLuaFunctions {
    const char *PRINT = "print\0";
}

extern "C" {
    #include "lua/lua.h"
    #include "lua/lualib.h"
    #include "lua/lauxlib.h"

    static int _print_callback(lua_State *state) {
        const char *message  = lua_tostring(state, -1);

        lua_getfield(state, LUA_REGISTRYINDEX, CLASS_KEY.c_str());
        auto instance = static_cast<godot::LuaConsole*>(lua_touserdata(state, -1));

        instance->emit_signal(Signals::PRINT, message);

        return 0;
    }

    static int _method_callback(lua_State *state) {
        const char *method_name = lua_tostring(state, lua_upvalueindex(1));

        lua_getfield(state, LUA_REGISTRYINDEX, CLASS_KEY.c_str());
        auto instance = static_cast<godot::LuaConsole*>(lua_touserdata(state, -1));

        instance->emit_signal(Signals::CALLED_METHOD, method_name);

        return 0;
    }
}

LuaConsole::LuaConsole() {
    _lua_state = nullptr;
}

LuaConsole::~LuaConsole() {
    if (_lua_state != nullptr)  {
        lua_close(_lua_state);
        _lua_state = nullptr;
    }
}

void LuaConsole::setup_lua() {
    if (_lua_state != nullptr) {
        lua_close(_lua_state);
    }

    _lua_state = luaL_newstate();

    luaL_openlibs(_lua_state);

    lua_pushcfunction(_lua_state, _print_callback);
    lua_setglobal(_lua_state, GlobalLuaFunctions::PRINT);

    lua_pushlightuserdata(_lua_state, this);
    lua_setfield(_lua_state, LUA_REGISTRYINDEX, CLASS_KEY.c_str());

    for (const auto &entry : _method_name_callback) {
        const char* method_name_str = entry.c_str();

        lua_pushstring(_lua_state, method_name_str);
        lua_pushcclosure(_lua_state, &_method_callback, 1);
        lua_setglobal(_lua_state, method_name_str);
    }
}

void LuaConsole::bind_method(String method_name) {
    auto method_name_str = method_name.utf8();

    _method_name_callback.emplace(method_name_str);
}

void LuaConsole::run_script(String script_source) {
    setup_lua();

    const char *source = script_source.utf8();

    if (luaL_dostring(_lua_state, source) != LUA_OK) {
        const char *error_message = lua_tostring(_lua_state, -1);
        emit_signal(Signals::ERROR, error_message);
    }
}

void LuaConsole::_bind_methods() {
    ClassDB::bind_method(D_METHOD("bind_method", "method_name"), &LuaConsole::bind_method);
    ClassDB::bind_method(D_METHOD("run_script", "script_source"), &LuaConsole::run_script);

    ADD_SIGNAL(
        MethodInfo(
            Signals::ERROR,
            PropertyInfo(Variant::STRING, "message")
        )
    );

    ADD_SIGNAL(
        MethodInfo(
            Signals::PRINT,
            PropertyInfo(Variant::STRING, "value")
        )
    );

    ADD_SIGNAL(
        MethodInfo(
            Signals::CALLED_METHOD, 
            PropertyInfo(Variant::STRING, "method_name"),
            PropertyInfo(Variant::ARRAY, "arguments")
        )
    );
}