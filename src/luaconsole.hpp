#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <godot_cpp/classes/sprite2d.hpp>

extern "C" {
    #include "lua/lua.h"
}

namespace godot {

    class LuaConsole: public Node2D {
        GDCLASS(LuaConsole, Node2D)

        private:

        lua_State* _lua_state;
        std::unordered_set<std::string> _method_name_callback;

        protected:

        static void _bind_methods();

        public:

        LuaConsole();
        ~LuaConsole();

        void bind_method(String method_name);
        void run_script(String script_source);

        private:

        void setup_lua();
    };

}