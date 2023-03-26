#include <iostream>
#include <string.h>
// #include "testlib.h"
using namespace std;

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// print(mylib1.mysin(3.14 / 2));
const char *script = R"(
    package.cpath = package.cpath .. ";/home/yong/ws/lua_c/produce_so_for_lua/?.so"..";./?.so";
    local testlib = require "libtestlib";
    print(testlib.add(1,2));
    print("hello world");
    )";

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    auto ret = luaL_loadbuffer(state, script, strlen(script), "script"); // 里面有一个新的state
    if (ret != 0)
    {
        if (lua_isstring(state, -1))
        {
            auto msg = lua_tostring(
                state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
            printf("script load failed:%s\n", msg);
            lua_pop(state, 1);
        }
        return -1;
    }
    if (lua_pcall(state, 0, 0, 0)) // 加载脚本
    {
        auto msg = lua_tostring(
            state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
        printf("call function chunk failed:%s", msg);
        lua_pop(state, 1);
    }
    // 4.关闭state
    lua_close(state);

    return 0;
}