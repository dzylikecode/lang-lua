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
