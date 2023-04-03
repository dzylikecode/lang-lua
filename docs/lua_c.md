动态库.so 静态库.a 文件 - 简书 (jianshu.com)](https://www.jianshu.com/p/91fac87bd6d3)

- 在编译时需要指定lua的头文件目录`-I+头文件路径`
  
  ```cpp
  #include "lua.h"  
  #include "lauxlib.h"  
  #include "lualib.h"
  ```
  
  如果是c++的话需要加上extern ”C“{}，因为lua没有在接口上兼容cpp
  
  ```cpp
  extern "C" {
  #include <lauxlib.h>
  #include <lua.h>
  #include <lualib.h>
  ```

- 编译时链接静态库和动态库
  
  ```lua
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
  ```

## 在c++中调用lua

- 加lua脚本
  
  ```cpp
  //1.创建Lua状态，返回一个指向堆栈的指针
      lua_State *L = luaL_newstate();
      if (L == NULL)
      {
          return;
      }
  
      //2.加载lua文件
      int bRet = luaL_loadfile(L, "hello.lua");
      if (bRet)
      {
          cout << "load file error" << endl;
          return;
      }
  
      //3.运行lua文件
      bRet = lua_pcall(L, 0, 0, 0);
      if (bRet)
      {
          cout << "pcall error" << endl;
         return;
      }
  ```

- c和lua的互相调用都是通过虚拟环境state进行的，本质就是一个存储节点的栈
  
  对于栈的理解：
  
  - `lua_State *L = luaL_newstate();`
    
    在没有加载脚本文件或者chunk前，新创建的state是一个空的栈。
  
  - `void luaL_openlibs (lua_State *L);`
    
    调用该函数后会将环境变量全局表`_G`压入state中，此时state为(_G)
  
  - 使用`int luaL_loadbuffer (lua_State *L, const char *buff,size_t sz,const char *name);`或者`int luaL_loadfile (lua_State *L, const char *filename);`加载完chunk后，会生成一个函数压在栈顶，此时state为(_G, chunk)，但此时并没有运行chunk。所以目前脚本里的东西无法获取
  
  - 创建独立的包环境需要使用自己的表作为upvalue，步骤和代码如下所示：
    
    1. 生成chunk，push到栈顶
    
    2. 创建table，设置_G["pkg"] = { }
    
    3. 这个table设置元表,元表继承_G的访问域(__index)
    
    4. 执行chunk
  
  ```cpp
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
  }常用API(优先查找官方文档)
  ```

- > 时刻注意这些API对state进行了什么操作，是否弹出或者压入了什么
  
  - `lua_State *luaL_newstate (void);`
    
    用于创建一个state
    
    success：return lua_State *
    
    failed：return NULL
  
  - `void luaL_openlibs (lua_State *L);`
    
    将lua的标准环境加载到L中；
  
  - ```cpp
    int luaL_loadbuffer (lua_State *L,
                         const char *buff,
                         size_t sz,
                         const char *name);
    ```
    
    加载一个代码块但是不执行它，如果加载没有语法错误，会将该块编译成函数压到顶，需要加载这段脚本时可以调用栈顶进行执行。有语法错误的话会压入一条错误信息，根据函数返回值判断是否加载成功，失败可以进行打印错误信息，参数name就是用于定位出错时代码块位置
    
    ```cpp
    auto ret =luaL_loadbuffer(state, 
                            scriptBuffer, 
                            strlen(scriptBuffer), 
                            "script");
    if (ret != 0)
    {
         if (lua_isstring(state, -1))
         {
             auto msg = lua_tostring(state, -1); //-1表示从栈顶取出，因为出现错误信息会弹到栈顶
             printf("script load failed:%s\n", msg);
             lua_pop(state, 1);
         }
         return -1;
     }
    ```
  
  - `int lua_pcall (lua_State *L, int nargs, int nresults, int msgh);`
    
    于一次函数调用，或者执行一个chunk，因为chunk加载进来的时候会被封装成函数压在栈顶
  
  - `int lua_getglobal (lua_State *L, const char *name);`
    
    将name压到栈顶，用于查找全局表中的对应的key，返回值是value的类型
  
  - `lua_isxxx(lua_State *L, int index)`
    
    用于判断state中节点的类型
  
  - `lua_toxxx(lua_State *L, int index)`
    
    用于将state中的值转换为c类型的变量
  
  - `void lua_newtable (lua_State *L);`
    
    创建一个空表压到栈顶，等价于``lua_createtable(L, 0, 0)`
  
  - `const char *lua_pushstring (lua_State *L, const char *s);`
    
    压入一个s的拷贝到栈顶，并返回一个指向该拷贝的字符串，s为NULL则压入nil并返回NULL
  
  - `void lua_pushvalue (lua_State *L, int index);`
    
    Pushes a copy of the element at the given index onto the stack.
  
  - `void lua_rawset (lua_State *L, int index);
    
    和`lua_settable`相似，只是该函数是不考虑元方法的情况
    
    参数index是table在state中的节点位置，state栈顶为value，栈顶的下一个为key，
    
    函数调用后会将value和key弹出state
  
  - `int lua_rawget (lua_State *L, int index);`
    
    Pushes onto the stack the value `t[k]`, where `t` is the value at the given index and `k` is the value at the top of the stack.
  
  - `void lua_setmetatable (lua_State *L, int index);`
    
    Pops a table from the stack and sets it as the new metatable for the value at the given index.
  
  - `lua_pop(lua_State *L, int n)`
    
    Pops `n` elements from the stack.
  
  - `lua_close(lua_State *L);`
    
    Destroys all objects in the given Lua state and frees all dynamic memory used by this state.

## 注c函数给脚本调用

- 使用函数指针`int (*lua_CFunction) (lua_State *L);`,定义自己的函数。

- 函数中第一件事就是判断调用时输入的参数个数和类型是否正确。进行数据处理时得将lua的变量转换成c变量类型，最后将返回值压入栈中，返回值是返回值的个数，即压入state的返回值个数。

- 注册需要将函数名字和函数指针作为键值对添加到`_G`中。

- 使用函数`void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n); `将cclosure压入state，并使用`lua_rawset`将其添加到_G。后面lua调用时可以在全局变量中找到函数接口，并会创建一个闭包，一个独立的state，函数执行的state只有参数和返回值，不受外部的state影响。

- `int lua_gettop (lua_State *L);`可以获得state栈顶元素的index值，即获得state栈中的元素个数。

- 函数传参是在函数的state中，第一个传入的参数在栈底(即index = 1为第一个参数)，返回值会在c函数执行完压入栈中，回到lua脚本中接受返回值会去state中取值，根据c函数中的返回值取对应的个数。压入栈中返回值的顺序是和lua接收返回值的变量顺序一致，先压入栈的作为第一个返回值。

> 我们说用Lua可以调用C语言函数，但这并不意味着Lua可以调用所有的C函数。当C语言调用Lua函数时，该函数必须遵循一个简单的规则来传递参数和获取结果。同样，当Lua调用C函数时，这个C函数也必须遵循某种规则来获取参数和返回结果(如上)。此外，当Lua调用C函数时，我们必须注册该函数，即必须以一种恰当的方式为ua提供该C函数的地址。
>   Lua调用C函数时，也使用一个与C语言调用Lua函数时相同类型的栈state，C函数从栈中获取参数，并将结果压入栈中。（这个栈是闭包state）
> 
>         Lua中一个lua_State就对应一个lua stack，通常情况下一个lua stack就足够了，由此便产生了与C/C++不同的地方：在C/C++中每个函数调用都会有自己对应的stack，在函数调用完成之后，便将其释放掉，因而不需要担心stack会无限制地生长下去；而在C/C++中使用Lua，由于所有的操作都基于同一个stack，因此需要程序员自己在代码中做stack的清理工作。
>   此处的重点在于，这个栈不是一个全局结构；每个函数都有其私有的局部栈。当Lua调用一个C函数时，第一个参数总是位于这个局部栈中索引为1的位置。即使一个C函数调用了Lua代码，而且Lua代码又再次调用了同一个C函数，这些调用每一次都只会看到本次调用自己的私有栈，其中索引为1的位置上就是一个参数。

- example:

```cpp
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
include <iostream>
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
```

## lua本中使用c++类

- 概念：`LUA_REGISTRYINDEX`，这是一个用于注册的全局table，可以在里面存一些全局的变量，像类的metatable

- 构建userdata和userdata的metatable中的index（提供给lua操作类的方法，在这些方法中会将lua的userdata转为c++的object去调用类的方法去实现并给lua返回执行结果，也就是index中函数的返回值。）关键是创建对象时将metatable设置好，借助
  
  ```cpp
  //为GameObject类注册栈顶的metatable到LUA_REGISTRYINDEX表中
  //使用lua_rawgetp可以获取到这张metatable，用于创建该类的userdata时进行元表设置
  //&GameObject::registry_value为静态地址固定，所以可以用于定位获取metatable
  lua_rawsetp(state, LUA_REGISTRYINDEX, &GameObject::registry_value);
  
  lua_rawgetp(state, LUA_REGISTRYINDEX, &GameObject::registry_value); // userdata; metatable
  lua_setmetatable(state, -2); // userdata
  ```
  
  使用类的静态变量的地址获取到元表

- 总结：
  
  1. 在给lua提供创建userdata前需要先将提供给lua的方法存到metatable的index表中，并使用一个技巧即使用类的静态成员的地址去绑定这个metatale到`LUA_REGISTRYINDEX`中
     
     ```cpp
      //将metatable注册到LUA_REGISTRYINDEX中，并与&GameObject::registry_value绑定
     lua_rawsetp(state, LUA_REGISTRYINDEX, &GameObject::registry_value); //                                        
     ```
  
  2. 实现cpp接口给lua创建一个userdata,使用`lua_newuserdata`会分配一块内存，使用这块内存new一个GameObject，同时这个userdata会存在state，为其设置元表，让该userdata可以调用我们为其实现的cpp方法。
     
     ```cpp
     nt CreatGameObject(lua_State *state)
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
     ```
  
  3. 为lua实现那些注册在metatable的index表中的方法
     
     ```cpp
     lua_getglobal(state, "_G");                  //_G
     lua_pushstring(state, "create_game_object"); //_G,"id"
     lua_pushcclosure(state, CreatGameObject, 0); //_G,"id",closure
     lua_rawset(state, -3);                       //_G
     lua_pop(state, 1);                           //
     // set metatable{_index = indextable}
     lua_newtable(state);                       // metatable
     lua_pushstring(state, "__index");          // metatable,"__index"
     lua_newtable(state);                       // metatable，"__index",indextable
     ua_pushstring(state, "id");               // metatable,"__index",indextable,"id"
     lua_pushcclosure(state, GameObject_id, 0); // metatable,"__index",indextable,"id",closure
     lua_rawset(state, -3);
     lua_pushstring(state, "equal");                                     // metatable,"__index",indextable,"equal"
     lua_pushcclosure(state, GameObject_equal, 0);                       // metatable,"__index",indextable,"equal",closure
     lua_rawset(state, -3);                                             // metatable,"__index",indextable
     lua_rawset(state, -3); 
     ```

## lightuserdata和userdata

- lightuserdata没有元表，是c++传递给lua的一个类指针，它可以作为参数使用，一般不会把指向动态内存的指针作为lightuserdata传递给lua，因为如果该指针指向的动态内存被释放，那它成为野指针，具有危险，所以一般是将静态或是全局的类作为lightuserdata传递给lua。
  
  ```cpp
  void reg_light_userdata(lua_State *state)
  {
      lua_getglobal(state,"_G");
      lua_pushstring(state,"gobject");
      //gobject是一个全局的类对象
      lua_pushlightuserdata(state,&gobject);
      lua_rawset(state,-3);
      lua_pop(state,1);
  }
  ```
  
  > 无法使用gobject去访问元表中的方法。

- we can use light userdata to find C objects inside Lua.

- 在lua中，使用cpp提供的接口，但是是lua分配的内存具有自动回收创建了对象，返回lua能使用的userdata，并且给userdata提供了方法，因为这些方法是在cpp函数中实现注册给lua的，在cpp函数中便能对对象进行操作，操作后可以返回给c++使用

## Lua脚本使用c++库

- 使用lua提供的接口定义函数
  
  `typedef int (*lua_CFunction) (lua_State *L);`定义相同的函数类型

- 添加方法到 `luaL_Reg`类型的数组中,每个元素为一个struct，第一个成员为lua中调用时使用的函数名，第二个成员为函数指针，注意注册通过识别{NULL,NULL}进行终止
  
  ```cpp
  static luaL_Reg test_functions[] =
      {
          {"average", average},
          {"add", add},
          {"sub", sub},
          {NULL, NULL}};
  ```

- 使用`int luaopen_xxx(lua_State *L)`将上面的数组注册到lua中，"xxx"必须是库名，后面生成的库名得是xxx.so，不然lua脚本无法加载库函数为模块
  
  ```cpp
  int luaopen_testlib(lua_State *L)
  {
      luaL_newlib(L, test_functions); // Lua 5.2之后用luaL_newlib代替了luaL_register
      return 1;
  }
  ```

- 动态库生成命令,依赖lua静态库或者lua动态库（`-L.  -llua`）
  
  ```bash
  g++ -std=c++11 -shared -fPIC  testlib.cpp -o testlib.so -I/usr/local/lua5.3/lua-5.3.0/src -I. -L. -llua -ldl
  ```

- 遇到的问题
  
  lua需要使用的动态库名称必须和必须和包的名称一致，后面require “包名”，但是在进行c程序编译使gcc或g++字链接动态库时是会认为是在linux下面，会将libxxx.so，认为是xxx动态库，如果生成的动态库前面不带lib，则在链接动态库会链接不到，如果生成的带lib的动态库，则lua在使用包时需要require “libxxx”
  
  这种特殊情况只会在编译给lua的动态库才会出现，所以，可以在单独生成动态库使用能被gcc识别的名字（加lib），后面放在lua库路径下时将其改名为lua能识别的库名，也就是不带lib的。当然可以在为lua生成动态库时，为库的命名为`lib包名`这样后面即使是gcc需要加上lib也能正常编译并且lua也认得这个`lib包名.so`

- 动态库生成时添加的链接路径只是在生成程序时使用，程序执行需要指定库依赖的库路径。方法如下：
  
  - 修改/etc/ld.so.conf 使用sudo vim /etc/ls.so.conf，然后把动态库的绝对路径粘贴进去（里面会使用include包含其他的.config文件，可以新建一个属于自己动态库路径的文件，方便后续删库跑路）
    
    更新查找动态库的路径使配置文件生效 使用 `sudo ldconfig -v`
  
  - `export LD_LIBRARY_PATH=动态库路径:$LD_LIBRARY_PATH`,然后source 环境变量

- 总结：
  
  1. 使用lua官方生成的静态库或者动态库，在编译的时候进行链接，记得动态库的话得添加运行时的链接路径，编译时的链接路径在xmake或者makefile中使用-L指定
  
  2. 库名问题建议在生成lua的包就使用lib开头加库名作为实际包名，即`lib包名.so`，兼容gcc在linux链接动态库的需求。另一种方法是在链接完在xmake或者makefile中将动态库进行改名，改成lua可以识别的不带lib开头的名称。
  
  
