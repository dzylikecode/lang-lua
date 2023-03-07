# meta

元编程

## metatable

可以实现透明代理

---

`__index` 元方法

类似 js 的 getter

1. 在表中查找，如果找到，返回该元素，找不到则继续
2. 判断该表是否有元表，如果没有元表，返回 nil，有元表则继续。
3. 判断元表有没有 `__index` 方法，如果 `__index` 方法为 nil，则返回 nil；如果 `__index` 方法是一个表，则重复 1、2、3；如果 `__index` 方法是一个函数，则返回该函数的返回值。

---

`__newindex` 元方法

类似 js 的 setter

## overload operator

| method       | operator |
| ------------ | -------- |
| `__add`      | a `+` b  |
| `__sub`      | a `-` b  |
| `__mul`      | a `*` b  |
| `__div`      | a `/` b  |
| `__mod`      | a `%` b  |
| `__unm`      | a `-` b  |
| `__concat`   | a `..` b |
| `__eq`       | a `==` b |
| `__lt`       | a `<` b  |
| `__le`       | a `<=` b |
| `__call`     | a`()`    |
| `__tostring` | a        |
