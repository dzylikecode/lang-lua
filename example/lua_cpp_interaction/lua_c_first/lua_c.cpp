#include <string.h>

#include <iostream>
using namespace std;

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

const char *scriptBuffer = R"(print("hello");)";

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    {
        auto ret =
            luaL_loadbuffer(state, scriptBuffer, strlen(scriptBuffer), "hello");
        if (ret != 0)
        {
            if (lua_isstring(state, -1))
            {
                auto msg = lua_tostring(
                    state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
                printf("script load failed:%s", msg);
                lua_pop(state, 1);
            }
            return -1;
        }
        if (lua_pcall(state, 0, 0, 0))
        {
            auto msg = lua_tostring(
                state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
            printf("call function chunk failed:%s", msg);
            lua_pop(state, 1);
        }
    }
    // 4.关闭state
    lua_close(state);
    return 0;
}