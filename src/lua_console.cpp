#include "lua_console.hpp"
#include "lua_constants.hpp"
#include "variant_converter.hpp"

using namespace godot;

extern "C" {
    #include "lua/lua.h"
    #include "lua/lualib.h"
    #include "lua/lauxlib.h"
}

const char* LuaConsole::KEY = "LuaConsole";

LuaConsole::LuaConsole() {
    _lua = nullptr;
    _dirty = true;
}

LuaConsole::~LuaConsole() {
    if (_lua != nullptr)  {
        lua_close(_lua);
        _lua = nullptr;
    }
}

void LuaConsole::reset() {
    print_line(String("Resetting"));

    if (_lua != nullptr) {
        lua_close(_lua);
    }

    _lua = luaL_newstate();

    luaL_openlibs(_lua);

    lua_pushcfunction(_lua, _print_callback);
    lua_setglobal(_lua, GlobalLuaFunctions::PRINT);

    lua_pushlightuserdata(_lua, this);
    lua_setfield(_lua, LUA_REGISTRYINDEX, LuaConsole::KEY);

    for (const auto &[name, callback] : _method_map) {
        register_method_in_lua(name);
    }

    _dirty = false;
}

void LuaConsole::bind_method(Callable callable, TypedArray<int> arguments) {
    auto method_name = std::string(callable.get_method().c_escape().utf8());

    BoundMethodInfo bound_method;

    bound_method.callable = callable;

    for (int i = 0; i < arguments.size(); i++) {
        bound_method.arguments.push_back((Variant::Type)(int)arguments[i]);
    }

    _method_map[method_name] = bound_method;

    if (_lua != nullptr) {
        register_method_in_lua(method_name);
    }
    else {
        _dirty = true;
    }
}

void LuaConsole::register_method_in_lua(const std::string& name) {
    const auto* name_str = name.c_str();
    lua_pushstring(_lua, name_str);
    lua_pushcclosure(_lua, &_method_callback, 1);
    lua_setglobal(_lua, name_str);
}

void LuaConsole::run_script(String script_source) {
    if (_dirty) {
        reset();
    }

    const char *source = script_source.utf8();

    if (luaL_dostring(_lua, source) != LUA_OK) {
        const char *error_message = lua_tostring(_lua, -1);
        emit_signal(Signals::ERROR, String(error_message));
    }
}

int LuaConsole::call_method_and_return(const std::string& method_name) {
    emit_signal(Signals::CALLED_METHOD, String(method_name.c_str()));

    if (_method_map.find(method_name) == _method_map.end()) {
        return 0;
    }

    auto found_method = _method_map.at(method_name);

    Variant v;
    Array arguments;
    int argument_count = found_method.arguments.size();

    for (int i = 0; i < argument_count; i++) {
        int idx = - argument_count + i;

        v = LuaIntegration::get_from_lua(_lua, (Variant::Type)found_method.arguments[i], idx);

        if (v.get_type() == Variant::Type::NIL) {
            emit_signal(Signals::ERROR, String(Messages::UNEXPECTED_ARGUMENT_TYPE));
        }
        
        arguments.push_back(v);
    }

    if (arguments.size() != argument_count) {
        emit_signal(Signals::ERROR, String(Messages::WRONG_NUMBER_OF_ARGUMENTS));
    }

    auto return_value = found_method.callable.callv(arguments);

    if (LuaIntegration::push_to_lua(_lua, return_value)) {
        return 1;
    }

    return 0;
}
