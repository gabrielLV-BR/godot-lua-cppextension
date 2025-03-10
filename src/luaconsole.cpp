#include "luaconsole.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

using VariantType = unsigned int;

static const std::string CLASS_KEY = "LuaConsole";

namespace Signals {
    const char *PRINT = "print";
    const char *CALLED_METHOD = "called_method";
    const char *ERROR = "error";
}

namespace GlobalLuaFunctions {
    const char *PRINT = "print";
}

extern "C" {
    #include <stdio.h>

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
        int argument_count = lua_gettop(state);

        const char *method_name_str = lua_tostring(state, lua_upvalueindex(1));
        std::string method_name(method_name_str);

        lua_getfield(state, LUA_REGISTRYINDEX, CLASS_KEY.c_str());
        LuaConsole *instance = static_cast<LuaConsole*>(lua_touserdata(state, -1));

        lua_pop(state, 1);

        Callable callable;
        CallableArgumentTypeList method_arguments;

        if (instance->get_callable(method_name, callable, method_arguments)) {
            std::vector<Variant> arguments;

            for (int i = 0; i < argument_count; i++) {
                int idx = - argument_count + i;

                Variant v;

                switch ((Variant::Type)method_arguments[i])
                {
                case Variant::Type::STRING:
                    if (lua_isstring(state, idx)) {
                        v = String(lua_tostring(state, idx));
                    }
                    break;
                case Variant::Type::INT:
                    if (lua_isinteger(state, idx)) {
                        v = lua_tointeger(state, idx);
                    }
                    break;
                case Variant::Type::FLOAT:
                    if (lua_isnumber(state, idx)) {
                        #if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
                            v = (float) lua_tonumber(state, idx);
                        #else
                            v = (double) lua_tonumber(state, idx);
                        #endif
                    }
                    break;
                case Variant::Type::BOOL:
                    if (lua_isboolean(state, idx)) {
                        v = (bool) lua_toboolean(state, idx);
                    }
                    break;
                }

                if (v.get_type() == Variant::Type::NIL) {
                    instance->emit_signal(Signals::ERROR, String("Unexpected argument type"));
                }
                
                arguments.push_back(v);
            }

            if (arguments.size() != method_arguments.size()) {
                instance->emit_signal(Signals::ERROR, String("Insufficient amount of parameters"));
            }

            for (int i = arguments.size() - 1; i >= 0; i--) {
                callable = callable.bind(arguments[i]);
            }

            callable.call();
        }
            
        instance->emit_signal(Signals::CALLED_METHOD, String(method_name_str));

        return 0;
    }
}

LuaConsole::LuaConsole() {
    _lua = nullptr;
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
    lua_setfield(_lua, LUA_REGISTRYINDEX, CLASS_KEY.c_str());

    for (const auto &[name, callback] : _method_map) {
        const auto* name_str = name.c_str();

        lua_pushstring(_lua, name_str);
        lua_pushcclosure(_lua, &_method_callback, 1);
        lua_setglobal(_lua, name_str);
    }
}

void LuaConsole::bind_method(Callable callable, TypedArray<int> arguments) {
    auto method_name = std::string(callable.get_method().c_escape().utf8());

    CallableArgumentTypeList method_types;

    for (int i = 0; i < arguments.size(); i++) {
        auto arg_type = (int)arguments[i];

        method_types.push_back(arg_type);
    }

    _method_map[method_name] = std::make_tuple(callable, method_types);
}

void LuaConsole::run_script(String script_source) {
    reset();
    
    const char *source = script_source.utf8();

    if (luaL_dostring(_lua, source) != LUA_OK) {
        const char *error_message = lua_tostring(_lua, -1);
        emit_signal(Signals::ERROR, String(error_message));
    }
}

bool LuaConsole::get_callable(const std::string& name, Callable& callable, CallableArgumentTypeList& argument_list) {
    auto entry = _method_map.find(name);

    if (entry == _method_map.end()) {
        return false;
    }

    callable = std::get<0>(entry->second);
    argument_list = std::get<1>(entry->second);

    return true;
}

// Godot binding

void LuaConsole::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("bind_method", "method", "method_arguments"),
        &LuaConsole::bind_method,
        DEFVAL(Callable()),
        DEFVAL(TypedArray<int>())
    );

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