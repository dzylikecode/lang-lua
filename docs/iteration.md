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
