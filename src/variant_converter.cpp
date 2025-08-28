#include "variant_converter.hpp"

using namespace godot;

Variant LuaIntegration::lua_value_to_variant(lua_State *lua, int idx) {
    int value_type = lua_type(lua, idx);

    switch (value_type) {
        case LUA_TSTRING:
            return String(lua_tostring(lua, idx));
        case LUA_TNUMBER:
            if (lua_isinteger(lua, idx)) {
                return (int) lua_tointeger(lua, idx);
            } else {
                return (double) lua_tonumber(lua, idx);
            }
        case LUA_TBOOLEAN:
            return (bool) lua_toboolean(lua, idx);
        case LUA_TTABLE: {
            Array arr;
            int abs_idx = lua_absindex(lua, idx);
            lua_pushnil(lua);

            while (lua_next(lua, abs_idx) != 0) {
                arr.append(lua_value_to_variant(lua, -1));
                lua_pop(lua, 1);
            }
            return arr;
        }
        default:
            return Variant(); 
    }
}

bool LuaIntegration::variant_to_lua_value(lua_State *lua, const Variant &variant) {
    switch (variant.get_type()) {
        case Variant::Type::STRING:
            lua_pushstring(lua, ((String)variant).utf8());
            break;
        case Variant::Type::INT:
            lua_pushinteger(lua, (int)variant);
            break;
        case Variant::Type::FLOAT:
            #if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
                lua_pushnumber(lua, (float)variant);
            #else
                lua_pushnumber(lua, (double)variant);
            #endif
            break;
        case Variant::Type::BOOL:
            lua_pushboolean(lua, (bool)variant);
            break;
        case Variant::Type::ARRAY: {
            Array arr = variant;
            lua_newtable(lua);
            for (int i = 0; i < arr.size(); i++) {
                lua_pushinteger(lua, i + 1);
                variant_to_lua_value(lua, arr[i]);
                lua_settable(lua, -3);
            }
            break;
        }
        default:
            return false; 
    }
    return true;
}

Variant LuaIntegration::get_from_lua(lua_State *lua, Variant::Type variant_type, int idx) {
    switch (variant_type) {
        case Variant::Type::STRING:
        case Variant::Type::INT:
        case Variant::Type::FLOAT:
        case Variant::Type::BOOL:
        case Variant::Type::ARRAY:
            return lua_value_to_variant(lua, idx);
        default:
            return Variant();
    }
}

bool LuaIntegration::push_to_lua(lua_State *lua, Variant variant) {
    return variant_to_lua_value(lua, variant);
}
