#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <godot_cpp/classes/node.hpp>

extern "C" {
    #include "lua/lua.h"
}

using CallableArgumentTypeList = std::vector<int>;

namespace godot {
    class LuaConsole: public Node {
        GDCLASS(LuaConsole, Node)

        private:

        lua_State *_lua;
        
        protected:
        
        static void _bind_methods();
        
        public:
        std::unordered_map<std::string, std::tuple<Callable, CallableArgumentTypeList>> _method_map;

        LuaConsole();
        ~LuaConsole();

        void run_script(String script_source);
        void bind_method(Callable callable, TypedArray<int> arguments);
        void reset();

        // Unexposed methods

        bool get_callable(const std::string& name, Callable& callable, CallableArgumentTypeList& argument_list);
    };

}