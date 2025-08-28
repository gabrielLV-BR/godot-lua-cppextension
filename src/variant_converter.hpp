#ifndef __variant_converter__
#define __variant_converter__

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/variant.hpp>

extern "C" {
    #include "lua/lua.h"
}

namespace godot {

    class LuaIntegration {
    public:
        static Variant get_from_lua(lua_State *lua, Variant::Type variant_type, int idx);
        static bool push_to_lua(lua_State *lua, Variant variant);

    private:
        static Variant lua_value_to_variant(lua_State *lua, int idx);
        static bool variant_to_lua_value(lua_State *lua, const Variant &variant);
    };

};

#endif // __variant_converter__
