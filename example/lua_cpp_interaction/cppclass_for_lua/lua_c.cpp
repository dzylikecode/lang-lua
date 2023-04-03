#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <new>
using namespace std;

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

const char *scriptBuffer = R"(
    local obj_1 = create_game_object(1);
    local obj_2 = create_game_object(1);
    local obj_3 = create_game_object(2);
    local rst1 = obj_1:equal(obj_2)
    local rst2 = obj_1:equal(obj_3)
    print(rst1,";",rst2)
    print(""..obj_1:id())
)";

class GameObject
{
private:
    uint32_t _id;

public:
    static size_t registry_value;

public:
    GameObject(uint32_t id) : _id(id)
    {
    }

    uint32_t id() const
    {
        return _id;
    }
    bool equal(GameObject *obj)
    {
        return _id == obj->_id;
    }
};
size_t GameObject::registry_value = 0;

int GameObject_equal(lua_State *state)
{
    int param_count = lua_gettop(state);
    if (param_count != 2)
    {
        return 0;
    }
    if (lua_isuserdata(state, -1) && lua_isuserdata(state, -2))
    {
        auto obj1 = (GameObject *)lua_touserdata(state, -2);
        auto obj2 = (GameObject *)lua_touserdata(state, -1);
        bool rst = obj1->equal(obj2);
        lua_pushboolean(state, rst);
        return 1;
    }
    return 0;
}

int GameObject_id(lua_State *state)
{
    int param_count = lua_gettop(state);
    if (param_count != 1)
    {
        return 0;
    }
    if (lua_isuserdata(state, -1))
    {
        auto obj = (GameObject *)lua_touserdata(state, -1);
        uint32_t rst = obj->id();
        lua_pushinteger(state, rst);
        return 1;
    }
}

int CreatGameObject(lua_State *state)
{
    int param_count = lua_gettop(state);
    if (param_count != 1)
    {
        return 0;
    }
    if (lua_isinteger(state, -1))
    {
        auto id = lua_tointeger(state, -1);
        auto p = lua_newuserdata(state, sizeof(GameObject)); // userdata
        GameObject *obj = new (p) GameObject(id);
        lua_rawgetp(state, LUA_REGISTRYINDEX, &GameObject::registry_value); // userdata,metatable
        lua_setmetatable(state, -2);                                        // userdata
        return 1;
    }
    return 0;
}

int main()
{
    // 1.创建一个state
    //  luaL_newstate返回一个指向堆栈的指针
    lua_State *state = luaL_newstate();
    luaL_openlibs(state); //
    {
        lua_getglobal(state, "_G");                  //_G
        lua_pushstring(state, "create_game_object"); //_G,"id"
        lua_pushcclosure(state, CreatGameObject, 0); //_G,"id",closure
        lua_rawset(state, -3);                       //_G
        lua_pop(state, 1);                           //
        // set metatable{_index = indextable}
        lua_newtable(state);                       // metatable
        lua_pushstring(state, "__index");          // metatable,"__index"
        lua_newtable(state);                       // metatable，"__index",indextable
        lua_pushstring(state, "id");               // metatable,"__index",indextable,"id"
        lua_pushcclosure(state, GameObject_id, 0); // metatable,"__index",indextable,"id",closure
        lua_rawset(state, -3);
        lua_pushstring(state, "equal");                                     // metatable,"__index",indextable,"equal"
        lua_pushcclosure(state, GameObject_equal, 0);                       // metatable,"__index",indextable,"equal",closure
        lua_rawset(state, -3);                                              // metatable,"__index",indextable
        lua_rawset(state, -3);                                              // metatable
        lua_rawsetp(state, LUA_REGISTRYINDEX, &GameObject::registry_value); //

        auto ret = luaL_loadbuffer(state, scriptBuffer, strlen(scriptBuffer), "script"); // 里面有一个新的state？
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
    }
    // 4.关闭state
    lua_close(state);
    return 0;
}