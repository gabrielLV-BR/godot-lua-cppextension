#include "variant_converter.hpp"

using namespace godot;

Variant LuaIntegration::get_from_lua(lua_State *lua, Variant::Type variant_type, int idx) {
    switch (variant_type)
    {
    case Variant::Type::STRING:
        if (lua_isstring(lua, idx)) {
            return String(lua_tostring(lua, idx));
        }
        break;
    case Variant::Type::INT:
        if (lua_isinteger(lua, idx)) {
            return lua_tointeger(lua, idx);
        }
        break;
    case Variant::Type::FLOAT:
        if (lua_isnumber(lua, idx)) {
            #if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
                return (float) lua_tonumber(lua, idx);
            #else
                return (double) lua_tonumber(lua, idx);
            #endif
        }
        break;
    case Variant::Type::BOOL:
        if (lua_isboolean(lua, idx)) {
            return (bool) lua_toboolean(lua, idx);
        }
        break;
    default:
        return Variant();
    }
}

bool LuaIntegration::push_to_lua(lua_State *lua, Variant variant) {
    Variant::Type variant_type = variant.get_type();

    switch (variant_type)
    {
    case Variant::Type::STRING:
        lua_pushstring(lua, ((String)variant).utf8());
        break;
    case Variant::Type::INT:
        lua_pushinteger(lua, (int) variant);
        break;
    case Variant::Type::FLOAT:
        #if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
            lua_pushnumber(lua, (float) variant);
        #else
            lua_pushnumber(lua, (double) variant);
        #endif
        break;
    case Variant::Type::BOOL:
        lua_pushboolean(lua, (bool) variant);
        break;
    default:
        return false;
    }

    return true;
}
