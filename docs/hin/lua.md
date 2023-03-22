
## 参考

- [dzylikecode/lang-lua (github.com)](https://github.com/dzylikecode/lang-lua)
- [Lua 教程 | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-tutorial.html)

## 数据类型

> 参考 [Lua 数据类型 | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-data-types.html)

### 字符串

使用 `[[` 和 `]]` 表示字符串块  
```lua
html = [[
<html>
<head></head>
<body></body>
</html>
]]

print(html)
```

使用运算符连接字符串时，会试图将字符串转换为数字：

```lua
print("2" + "6")
-- 输出 8

print("-2e2" * "6")
-- 输出 -1200.0
```

使用 `..` 连接字符串：

```lua
print("a" .. "b")  -- 输出 ab
print(123 .. 456)  -- 输出 123456
```

> 注意 `..` 两边需空格

使用 `#` 计算字符串的长度：
```lua
print(#"abcde")  -- 输出 5
```


Lua 有丰富的字符串函数，支持大小写转换、复制、连接、查找、替换、正则表达式匹配等。

详情可参考[Lua 字符串 | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-strings.html) 

转译字符参考[Lua 字符串 | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-strings.html)

总的来说，lua 的字符串操作非常强大，有字符串问题可以多翻翻文档。

### 表 (table)

在 Lua 中，表的索引默认以 **1** 开始。  
```lua
local table = {"a", "B", nil, "C"}
table["key1"] = 5
table["keyNil"] = nil
for key, value in pairs(table) do
    print("key", key, "value", value)
end

--[[
输出为：
key     1       value   a
key     2       value   B
key     4       value   C
key     key1    value   5
--]]
```

### 函数

这样定义：
```lua
function fun1(arg1, arg2)
    -- 内容
end
```

## 运算符

### 算术运算符

- 加减乘除 `+ - * /`   
- 取余 `%`
- 整除 `//`

### 比较运算符

不等于 `~=`

其它与 [C 语言](C%20语言.md) 和 [js](../Web/JavaScript.md) 等语言类似。

## 流程控制

### 条件

```lua
-- if 语句 --
if(0)  -- 0 为 true
then

end

-- if else 语句 --
if(true)
then
    -- if 内容
else
    -- else 内容
end

-- if elseif 语句 --
if(condition1)
then
    -- 代码块1
elseif(condition2)
then
    -- 代码块2
end
```

### 循环

```lua
-- while --
while(condition)
do
--
end

-- for --
for i=exp1, exp2, exp3 do  -- exp1 是初始值，exp2 是结束值，exp3 是步长
--
end

-- for in 迭代数组 --
local array1 = {1, 2, 3}
for i in ipairs(array1) do
    print(i)
end
--[[
1
2
3
--]]

-- repeat util --
repeat
    -- body
until(conditon)
```

### goto

lua 有其它语言很少用的 `goto` 语句，而且据说是用来实现 `continue` 的。

```lua
local a = 1
::label1::
a = a + 1
if a <= 3
```

## 函数

```lua
function fun1(arg1, ...)  -- ... 为可变参数
    local a1 = arg1
    local a2 = {...}  -- 将可变参数赋给一个表
end
```

## 迭代器

### 无状态迭代器
无状态迭代器是指不保留任何状态的迭代器，可以避免创建闭包而产生额外花销。

在 `for in` 中，参数分别是迭代函数、状态常量和控制变量：
```lua
local function iter(x, y)
    if y < x then
        y = y + 1
        return y, y * y
    end
end

for i, j in iter, 5, 0 do
    print(i, j)
end
```

### 多状态迭代器
多状态迭代器多用闭包和表来实现。

## 元表

元表用于定义表的一些操作。

`__index` 用于定义在表找不到元素时可以去其他表找对应索引的元素值。

```lua
local table1 = {"a", nil, "c"}
local table2 = {"A", "B", "C", "D"}
local metatable1 = {}
setmetatable(table1, metatable1)
metatable1.__index = table2  -- 设置 table1 的元表 index 为 table2

for i = 1, 4, 1 do
    print(table1[i])
end
--[[
a
B
c
D
--]]
```

`__newindex` 定义了赋值时的操作：

```lua
local table1 = { "a", nil, "c" }
local table2 = { "A", "B", "C", "D" }
local metatable1 = {}
setmetatable(table1, metatable1)
metatable1.__newindex = table2

table1[3] = "value3"
table1[4] = "value4"

print("table1")
for i, j in pairs(table1) do
    print(i, j)
end

print("table2")
for k, l in pairs(table2) do
    print(k, l)
end
--[[
table1
1       a
3       value3
table2
1       A
2       B
3       C
4       value4
--]]
```

还可以给元表定义加减乘除等操作符、调用方法 `__call`、输出格式 `__tostring`，可参考[Lua 元表(Metatable) | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-metatables.html) 

## 文件 IO

参考[Lua 文件 I/O | 菜鸟教程 (runoob.com)](https://www.runoob.com/lua/lua-file-io.html)

