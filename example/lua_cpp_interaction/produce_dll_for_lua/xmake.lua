---@diagnostic disable: undefined-global
--API Manual (Description Scope)：https://xmake.io/#/manual/project_target

add_rules("mode.debug","mode.release")

-- target("lua_library")
--     set_kind("static")
--     add_files("/usr/local/lua5.3/lua-5.3.0/src/*.c")
--     add_cflags( "-g", 
--                 "-O2",
--                 "-DLUA_USE_LINUX",
--                 "-I/usr/local/lua5.3/lua-5.3.0/src"
                -- "-Wl",
                -- "-E",
                -- "-lreadline",
                -- "-DLUA_USE_DLOPEN"
                -- "-ldl"
                -- )

target("testlib")
    set_kind("shared")
    add_files("testlib.cpp")
    add_includedirs("/usr/local/lua5.3/lua-5.3.0/src","./")
    add_shflags("-fPIC",
                "-ldl"
                )
    add_cxxflags( "-g",
                "-O2"
                -- "-I/usr/local/lua5.3/lua-5.3.0/src",
                -- "-I./"
                -- "-fPIC",
                -- "-ldl"
                )
target_end()

target("lua_c")
    set_kind("binary")
    add_includedirs("/usr/local/lua5.3/lua-5.3.0/src")
    add_files("*.cpp")
    add_linkdirs("./","/home/yong/ws/lua_c/produce_so_for_lua/build/linux/x86_64/release")
    add_deps("testlib") --相当于-ltestlib
    add_links("lua")  --相当于-llua
    add_cxxflags( "-g",
            "-O2"
            )
target_end()
