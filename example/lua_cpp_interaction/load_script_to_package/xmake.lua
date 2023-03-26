---@diagnostic disable: undefined-global
add_rules("mode.debug","mode.release")

target("library")
    set_kind("static")
    add_files("/usr/local/lua5.3/lua-5.3.0/src/*.c")

target("lua_c")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("library")

add_cxxflags( "-g", 
            "-O2",
            "-I/usr/local/lua5.3/include",
            "-L/usr/local/lua5.3/lua-5.3.0/src",
            "-llua")
