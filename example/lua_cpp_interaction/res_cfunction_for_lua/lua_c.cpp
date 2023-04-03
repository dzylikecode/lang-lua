#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>
using namespace std;

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

const char *scriptBuffer = R"(
    local val = pow_from_c(2,5);
    local val1 = add_from_c(1,2,3);
    print(val);
    print(val1)
    )";

int pow_from_c(lua_State *state)
{
    int param_count = lua_gettop(state); // 因为在调用函数前
    printf("param_count = %d\n", param_count);
    if (param_count != 2)
    {
        return 0;
    }
    if (lua_isinteger(state, -1) && lua_isinteger(state, -2))
    {
        auto x = lua_tointeger(state, -2); // 先传的参数在栈底
        auto y = lua_tointeger(state, -1);
        int rst = (int)pow(x, y);
        lua_pushinteger(state, rst);
        return 1; // 一个返回值
    }
    return 0;
}

int add_from_c(lua_State *state)
{
    int param_count = lua_gettop(state);
    printf("param_count = %d\n", param_count);
    if (param_count != 3)
    {
        return 0;
    }
    if (lua_isinteger(state, -1) && lua_isinteger(state, -2) && lua_isinteger(state, -3))
    {
        auto x = (int)lua_tointeger(state, -3);
        printf("%d\n", x);
        auto y = lua_tointeger(state, -2);
        auto z = lua_tointeger(state, -1);
        int rst = (int)(x + y + z);
        lua_pushinteger(state, rst);
        return 1; // 一个返回值
    }
    return 0;
}

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();
    int param_count = lua_gettop(state);
    printf("栈有%d个元素\n", param_count);
    luaL_openlibs(state); //
    {
        param_count = lua_gettop(state);
        printf("栈有%d个元素\n", param_count);
        lua_getglobal(state, "_G");          //_G
        lua_pushstring(state, "pow_from_c"); //_G,"pow_from_c"
        // 使用c闭包，相当于创建了一个新的栈存参数，如果调用lua_pushcfunction则需要把参数也给压入state,按标准进行调传参调用
        lua_pushcclosure(state, pow_from_c, 0); //_G,"pow_from_c",closure
        lua_rawset(state, -3);
        lua_pushstring(state, "add_from_c");    //_G,"add_from_c"
        lua_pushcclosure(state, add_from_c, 0); //_G,"add_from_c",closure
        lua_rawset(state, -3);
        param_count = lua_gettop(state);
        printf("栈有%d个元素\n", param_count);
        // lua_pop(state, 1);
        param_count = lua_gettop(state);
        printf("栈有%d个元素\n", param_count);
        auto ret = luaL_loadbuffer(state, scriptBuffer, strlen(scriptBuffer), "reg_c_for_lua"); // 里面有一个新的state？
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
        param_count = lua_gettop(state);
        printf("栈有%d个元素\n", param_count);
        if (lua_pcall(state, 0, 0, 0)) // 加载脚本
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