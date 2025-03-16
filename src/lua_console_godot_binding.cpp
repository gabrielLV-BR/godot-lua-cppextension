#include "lua_console.hpp"
#include "lua_constants.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LuaConsole::_bind_methods() {

    ClassDB::bind_method(
        D_METHOD("bind_method", "method", "method_arguments"),
        &LuaConsole::bind_method,
        DEFVAL(Callable()),
        DEFVAL(TypedArray<int>())
    );

    ClassDB::bind_method(D_METHOD("run_script", "script_source"), &LuaConsole::run_script);
    ClassDB::bind_method(D_METHOD("reset"), &LuaConsole::reset);

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