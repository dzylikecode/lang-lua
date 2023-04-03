#include <string.h>

#include <iostream>
using namespace std;

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

const char *scriptBuffer = R"(
    function say()
        print("hello");
    end
    function my_pow(x,y)
        return x^y;
    end
    say();
    )";

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    {
        //luaL_loadbuffer只加载不运行，所以得先调用一次lua_pcall运行加载的代码，不然后面取不到脚本中的东西
        auto ret =
            luaL_loadbuffer(state, scriptBuffer, strlen(scriptBuffer), "script");
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
        if (lua_pcall(state, 0, 0, 0)) //加载脚本
        {
            auto msg = lua_tostring(
                state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
            printf("call function chunk failed:%s", msg);
            lua_pop(state, 1);
        }
        lua_getglobal(state, "my_pow");
        if (!lua_isfunction(state, -1))
        {
            // 栈顶不是函数错误处理
            printf("function name is not found\n");
            return -1;
        }
        lua_pushnumber(state, 2);
        lua_pushnumber(state, 3);
        ret = lua_pcall(state, 2, 1, 0);
        if (ret != 0)
        {
            if (lua_isstring(state, -1))
            {
                auto msg = lua_tostring(
                    state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
                printf("function call failed:%s\n", msg);
                lua_pop(state, 1);
            }
            return -1;
        }
        if (lua_isnumber(state, -1))
        {
            lua_Number result = lua_tonumber(state, -1);
            printf("pow result is:%f", (float)result);
        }
    }
    // 4.关闭state
    lua_close(state);
    return 0;
}