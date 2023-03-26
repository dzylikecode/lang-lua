//testlib.h
 
#pragma once
 
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
 
extern "C" int luaopen_libtestlib(lua_State *L);