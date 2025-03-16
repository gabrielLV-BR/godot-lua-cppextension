#include "register_types.hpp"

#include "lua_console.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_luaconsole_module(ModuleInitializationLevel initializationLevel) {
    if (initializationLevel != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

    GDREGISTER_CLASS(LuaConsole);
}

void uninitialize_luaconsole_module(ModuleInitializationLevel initializationLevel) {
    if (initializationLevel != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
    GDExtensionBool GDE_EXPORT luaconsole_library_init(
        GDExtensionInterfaceGetProcAddress getProcAddress,
        const GDExtensionClassLibraryPtr classLibraryPtr,
        GDExtensionInitialization *extensionInitialization
    ) {
        godot::GDExtensionBinding::InitObject init_obj(getProcAddress, classLibraryPtr, extensionInitialization);

        init_obj.register_initializer(initialize_luaconsole_module);
        init_obj.register_terminator(uninitialize_luaconsole_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
