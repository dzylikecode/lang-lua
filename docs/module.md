# module

## export

```lua
-- 文件名为 module.lua
-- 定义一个名为 module 的模块
module = {}

-- 定义一个常量
module.constant = "这是一个常量"

-- 定义一个函数
function module.func1()
    io.write("这是一个公有函数！\n")
end

local function func2()
    print("这是一个私有函数！")
end

function module.func3()
    func2()
end

return module
```

## import

```lua
-- test_module.lua 文件
-- module 模块为上文提到到 module.lua
require("module")

print(module.constant)

module.func3()
```

---

alias

```lua
-- test_module2.lua 文件
-- module 模块为上文提到到 module.lua
-- 别名变量 m
local m = require("module")

print(m.constant)

m.func3()
```

## 加载机制

1. Lua 文件: 读取环境变量`LUA_PATH`, 存放在全局变量 package.path 中
   
   ```bash
   #LUA_PATH
   export LUA_PATH="~/lua/?.lua;;"
   ```
   
   文件路径以 ";" 号分隔，最后的 2 个 ";;" 表示新加的路径后面加上原来的默认路径
   
   !> 记得 source 生效
   
   调用 require("module") 时就会尝试打开这些文件目录去搜索目标
   
   调用 package.loadfile 来加载模块

2. C 程序库: 环境变量 `LUA_CPATH` -> 全局变量 `package.cpath`
   
   搜索的是 so 或 dll 类型的文件
   
   通过 package.loadlib 来加载它

## C package

C 包在使用以前必须首先加载并连接，在大多数系统中最容易的实现方式是通过动态连接库机制

Lua 在一个叫 loadlib 的函数内提供了所有的动态连接的功能

```lua
local path = "/usr/local/lua/lib/libluasocket.so"
-- 或者 path = "C:\\windows\\luasocket.dll"，这是 Window 平台下
local f = assert(loadlib(path, "luaopen_socket"))
f()  -- 真正打开库
```

期望二进制的发布库包含一个与前面代码段相似的 stub 文件，安装二进制库的时候可以随便放在某个目录，只需要修改 stub 文件对应二进制库的实际路径即可

将 stub 文件所在的目录加入到 LUA_PATH，这样设定后就可以使用 require 函数加载 C 库了





## 补充

require所使用的路径与传统的路径不同，require采用的路径是一连串的模式，其中每项都是一种将模块名转换为文件名的方式。require会用模块名来替换每个“？”，然后根据替换的结果来检查是否存在这样一个文件，如果不存在，就会尝试下一项。路径中的每一项都是以分号隔开，比如路径为以下字符串：  

```lua
?;?.lua;c:\windows\?;/usr/local/lua/?/?.lua
```

那么，当我们require “mod”时，就会尝试着打开以下文件：

```lua
mod
mod.lua
c:\windows\mod
/usr/local/lua/mod/mod.lua
```
