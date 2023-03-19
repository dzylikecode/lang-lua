# function

```lua
x,y = foo2(), 20      -- x='a', y=20
```

当 function 不是最后一个时, 如`foo2()`, 它只会返回 return 的第一个值

`(foo2())`: 填上一个括号会强制返回一个值

## Closures

Technically speaking, what is a value in Lua is the closure, not the function. The function itself is just a prototype for closures.

> 相当于`类的私有变量`的概念, 可以用来构造类, prototype 设计

一个函数内部嵌套另一个函数定义时，内部的函数体可以访问外部的函数的局部变量。并且可以把局部的函数传递出去，并且在外部对函数内部的东西进行修改。

但如果进行重新调用返回出来的又是另一个环境。

```lua
local function fun(a, b)
    local x = 0;
    return function()
        print(a, b, x);
        x = x + 1;
    end
end

fun(1, 2);

local env1 = fun(1, 2); -- 此处是传参，没有进行函数调用
env1();                 --x = 0
env1();                 --x = 1
env1();                 --x = 2

local env2 = fun(1, 2);
env2(); --x = 0

env2(); --x = 1

local env2_copy = env2;
env2_copy(); --x = 2
```

lambda表达式，经典的闭包表现方式。 

## local

```lua
local fact = function (n)
    if n == 0 then return 1
    else return n*fact(n-1)   -- buggy
    end
end
```

`fact`要等函数表达式构建完才存在, 而内部需要, 所以会报错

```lua
local fact
fact = function (n)
    if n == 0 then return 1
    else return n*fact(n-1)
    end
end
```

提前声明可以解决这个问题

```lua
local function fact (n)
    if n == 0 then return 1
    else return n*fact(n-1)
    end
end
```

相当于上面的语法

## Tail Calls

A tail call happens when a function calls another as its last action, so it has nothing else to do.

```lua
function f (x)
    return g(x)
end
```

可以避免多次使用`stack`, 只用将 f 的参数和 g 的返回值构成`stack`即可

---

A subtle point when we use proper tail calls is what is a tail call.

As such, a quite useful application of proper tail calls in Lua is for programming state machines. **Such applications can represent each state by a function; to change state is to go to (or to call) a specific function.**

```lua
function room1 ()
    local move = io.read()
    if move == "south" then return room3()
    elseif move == "east" then return room2()
    else print("invalid move")
        return room1-- stay in the same room
    end
end

function room2 ()
    local move = io.read()
    if move == "south" then return room4()
    elseif move == "west" then return room1()
    else print("invalid move")
        return room2()
    end
end

function room3 ()
    local move = io.read()
    if move == "north" then return room1()
    elseif move == "east" then return room4()
    else print("invalid move")
        return room3()
    end
end

function room4 ()
    print("congratulations!")
end
```

![](assets/2023-03-07-11-05-27.png)

## 可变参数

```lua
function average(...)
   result = 0
   local arg={...}
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("总共传入 " .. #arg .. " 个数")
   return result/#arg
end
```

Lua 将函数的参数放在 `arg` 的表中，`#arg` 表示传入参数的个数。

```lua
function fun(fmt,...)  --固定参数在前，可变参数放最后
    select("#",...)    --获取可变参数列表中参数的个数
    select(3,...)      --返回可变参数列表第三个后的参数列表
end
```

- select函数的用法：
  
  通常在遍历变长参数的时候只需要使用 {…}，然而变长参数可能会包含一些 nil，那么就可以用 **select** 函数来访问变长参数了：**select('#', …)** 或者 **select(n, …)**
  
  - `select('#', …) `返回可变参数的长度。
  
  - `select(n, …) `用于返回从起点 **n** 开始到结束位置的所有参数列表。
  
  > select返回的是元组而不是table
  
  - 获取第i个参数的方法`arg[i] = (select("#",i))`

- 使用pack处理可变参
  
  ```lua
  --将可变参打包为一个table，在最后会多出一个key为n的键值对，其值为参数的个数
  table.pack(...)
  arg_count = table.pack(...).n
  --如果不需要知道参数个数，可以直接使用for进行遍历结课
  local function fun1(...)
      for i, v in ipairs { ... } do
          print(i, v);
      end
  end
  --使用unpack可以将table解包,i和j表示获取那几个值
  table.unpack(table,i,j);
  ```

## 函数返回值

- 可以返回局部变量值，并且拥有原本的块环境

- 可以返回函数，可以是局部函数，全局函数，或者匿名函数都可以，返回函数变量具有原本return时的环境。
