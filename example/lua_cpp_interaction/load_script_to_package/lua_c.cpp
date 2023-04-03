#include <assert.h>
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
    )";

const char *scriptBuffer2 = R"(print("hello world!"))";

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();

    luaL_openlibs(state); // _G
    {
        // luaL_loadbuffer只加载不运行，所以得先调用一次lua_pcall运行加载的代码，不然后面取不到脚本中的东西
        auto ret =
            luaL_loadbuffer(state, scriptBuffer, strlen(scriptBuffer), "script"); //_G,chunk
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
        lua_getglobal(state, "_G"); //_G，chunk,_G
        if (lua_istable(state, -1))
        {
            lua_newtable(state);          // L：_G,chunk,_G,new_table
            lua_pushstring(state, "pkg"); // L：_G,chunk,_G,new_table,"pkg"
            lua_pushvalue(state, -2);     // L：_G,chunk,_G,new_table,"pkg",new_table(copy but still itself)
            lua_rawset(state, -4);        // 将pkg = {}添加到_G L：_G,chunk,_G,new_table
            /*设置栈顶值为upvalue，-3为chunk在state中的index，
            即lua_setupvalue的第二个参数funindex,返回值upvalueName = "_ENV"
            可以理解为闭包环境_ENV为这个new_table，也就是pkg={}*/
            char const *upvalueName = lua_setupvalue(state, -3, 1); // L：_G,chunk,_G
            // printf("%s\n",upvalueName);
            /*接下来为package设置元表的__index为_G,此时upvalue为一个空表，即pkg = {};
            在_G中也能找到，设置_G为pkg的元表__index方法后就可以使用print，否则环境为一个
            空表是无法使用print的，重在理解可以创建一个独立的环境*/
            assert(strcmp(upvalueName, "_ENV") == 0 );
            lua_newtable(state);              // L：_G,chunk,_G,metatable
            lua_pushstring(state, "__index"); // L：_G,chunk,_G,metatable,"__index"
            lua_pushvalue(state, -3);         // L：_G,chunk,_G,metatable,"__index",_G
            // 实现metatable = {__index = _G}
            lua_rawset(state, -3);        // L：_G,chunk,_G,metatable
            lua_pushstring(state, "pkg"); // L：_G,chunk,_G,metatable,"pkg"(string)
            // 将"pkg"(string)弹出，取_G["pkg"]放到栈顶
            lua_rawget(state, -3);       // L：_G,chunk,_G,metatable,"pkg"(table)
            lua_pushvalue(state, -2);    // L：_G,chunk,_G,metatable,"pkg"(table),metatable
            lua_setmetatable(state, -2); // L：_G,chunk,_G,metatable,"pkg"(table)
            lua_pop(state, 3);           // chunk
        }
        // call the code chunk
        if (lua_pcall(state, 0, 0, 0)) // 加载脚本
        {
            auto msg = lua_tostring(
                state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
            printf("call function chunk failed:%s", msg);
            lua_pop(state, 1);
        }
        ret =
            luaL_loadbuffer(state, scriptBuffer2, strlen(scriptBuffer2), "script2"); //_G,chunk,chunk2
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
            printf("call function chunk2 failed:%s", msg);
            lua_pop(state, 1);
        }
    }
    // 4.关闭state
    lua_close(state);
    return 0;
}