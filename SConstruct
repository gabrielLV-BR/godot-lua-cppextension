#!/usr/bin/env python
import os
import sys

extension_name="luaconsole"

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "lib{}.{}.{}.framework/lib{}.{}.{}".format(
            extension_name, env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "lib{}.{}.{}.simulator.a".format(extension_name, env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "lib{}.{}.{}.a".format(extension_name, env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "lib{}{}{}".format(extension_name, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources
    )

lua_sources = Glob("src/lua/*.c")
lua_lib = env.StaticLibrary(
    "lua_static",
    source=lua_sources
)

Default(lua_lib)

env.Append(LIBS = [ lua_lib ])

Default(library)
