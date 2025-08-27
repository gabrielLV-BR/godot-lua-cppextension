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
        case Variant::Type::ARRAY:
        if (lua_istable(lua, idx)) {
            Array arr;
            int abs_idx = lua_absindex(lua, idx); 
            lua_pushnil(lua);

            while (lua_next(lua, abs_idx) != 0) {
                int value_type = lua_type(lua, -1);

                Variant element;
                switch (value_type) {
                    case LUA_TSTRING:
                        element = String(lua_tostring(lua, -1));
                        break;
                    case LUA_TNUMBER:
                        if (lua_isinteger(lua, -1)) {
                            element = (int) lua_tointeger(lua, -1);
                        } else {
                            element = (double) lua_tonumber(lua, -1);
                        }
                        break;
                    case LUA_TBOOLEAN:
                        element = (bool) lua_toboolean(lua, -1);
                        break;
                    default:
                        element = Variant();
                        break;
                }

                arr.append(element);
                lua_pop(lua, 1); 
            }
            return arr;
        }
        break;
        default:
            return Variant();
    }
    return Variant();
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
    case Variant::Type::ARRAY: {
        Array arr = variant;  
        lua_newtable(lua);    

        for (int i = 0; i < arr.size(); i++) {
            lua_pushinteger(lua, i + 1); 
            push_to_lua(lua, arr[i]);    
            lua_settable(lua, -3);        
        }
        break;
    }
    default:
        return false;
    }

    return true;
}
