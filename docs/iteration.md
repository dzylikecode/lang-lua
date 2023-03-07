# iteration

the closure and a factory

```lua
function list_iter (t)
    local i = 0
    local n = table.getn(t)
    return function ()
            i = i + 1
            if i <= n then return t[i] end
            end
end
```

`list_iter` is the factory

```lua
for var_1, ..., var_n in explist
  do
    block
  end
```

```lua
do
  local _f, _s, _var = explist
  while true do
    local var_1, ... , var_n = _f(_s, _var)
    _var = var_1
    if _var == nil then break end
    block
  end
end
```

?> 闭包相当于构造类

```python
next, obj, i = list_iter(t) # i: state
while True
    i = next(obj, i) # update state
    if i == None:
        break
    block()
```

```python
obj, i = list_iter(t) # next, obj 可以合并为 obj
while True
    i = obj.next(i) # update state
    if i == None:
        break
    block()
```

## 无状态的迭代器

用两个变量（状态常量和控制变量）的值作为参数被调用

```lua
function iter (a, i)
    i = i + 1
    local v = a[i]
    if v then
       return i, v
    end
end

function ipairs (a)
    return iter, a, 0
end
```

当 Lua 调用 ipairs(a) 开始循环时，他获取三个值：迭代函数 iter、状态常量 a、控制变量初始值 0；然后 Lua 调用 iter(a,0) 返回 1, `a[1]`（除非 `a[1]`=nil）；第二次迭代调用 iter(a,1) 返回 2, `a[2]`......直到第一个 nil 元素。

## 多状态的迭代器
