#if !defined(__lua_console__)
#define __lua_console__


#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <godot_cpp/classes/node.hpp>

extern "C" {
    #include "lua/lua.h"
    
    int _print_callback(lua_State *state);
    int _method_callback(lua_State *state);
}

struct BoundMethodInfo {
    godot::Callable callable;
    std::vector<godot::Variant::Type> arguments;
};

class LuaConsole: public godot::Node {
    GDCLASS(LuaConsole, godot::Node)

private:

    bool _dirty;
    lua_State *_lua;
    
protected:
    
    static void _bind_methods();
    
public:
    static const char *KEY;
    std::unordered_map<std::string, BoundMethodInfo> _method_map;

    LuaConsole();
    ~LuaConsole();

    void run_script(godot::String script_source);
    void bind_method(godot::Callable callable, godot::TypedArray<int> arguments);
    void reset();

    // Unexposed methods

    int call_method_and_return(const std::string& method_name);
};


#endif // __lua_console__
