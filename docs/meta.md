# meta

元编程

## metatable

- 元表并不是一个普通的表，而是一套自定义的计算规则，用这些规则，可以实现表与表之间的运算而这些规则，都以函数的方式，写在元表中，所以又称为元方法（就是写在元表里面的方法），起到类似于c++中的运算符重载的作用

- 可以实现透明代理

---

`__index` 元方法

类似 js 的 getter

1. 在表中查找，如果找到，返回该元素，找不到则继续
2. 判断该表是否有元表，**如果没有元表，返回 nil**，有元表则继续。
3. 判断元表有没有 `__index` 方法，如果 `__index` 方法为 nil，则返回 nil；如果 `__index` 方法是一个表，则重复 1、2、3；如果 `__index` 方法是一个函数，则返回该函数的返回值。

---

`__newindex` 元方法

类似 js 的 setter

## overload operator

| method       | operator |
|:------------:|:--------:|
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

## 设置元表

- `setmetatable(table, metatable)`返回值为table
- 设置元表并实现元方法，example如下
  
  ```lua
  local t11 = { 1, 2, 3, nil };
  local t22 = { 1, 2, 3, 6 };
  local metat = {};
  
  setmetatable(t11, metat);
  setmetatable(t22, metat);
  
  metat.__add = function(t1, t2)
      local len1 = #t1;
      local len2 = #t2;
      local t = {};
      if len1 <= len2 then
          len1 = len2;
      end
      for i = 1, len1 do
          local a = t1[i] or 0;
          local b = t2[i] or 0;
          t[i] = a + b;
      end
      return t;
  end
  
  metat.__eq = function(t1, t2)
      local len1 = #t1;
      local len2 = #t2;
      if len1 ~= len2 then
          return false;
      end
      for i = 1, len1 do
          if t1[i] ~= t2[i] then
              return false;
          end
      end
      return true;
  end
  
  
  local t3 = t11 + t22;
  -- for index, value in pairs(t3) do
  --     print(index, value);
  -- end
  for k, v in ipairs(t3) do
      print(k, v);
  end
  
  print(t11 == t22);
  print(t11 == t11);
  ```
  
  
