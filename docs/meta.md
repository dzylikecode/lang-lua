# meta

元编程

## metatable

- 元表并不是一个普通的表，而是一套自定义的计算规则，用这些规则，可以实现表与表之间的运算而这些规则，都以函数的方式，写在元表中，所以又称为元方法（就是写在元表里面的方法），起到类似于c++中的运算符重载的作用

- 可以实现透明代理

---

## `__index` 元方法

类似 js 的 getter

1. 在本表中查找，如果找到，返回该元素，找不到则继续
2. 判断该表是否有元表，**如果没有元表，返回 nil**，有元表则继续。
3. 判断元表有没有 `__index` 方法，如果 `__index` 方法为 nil，则返回 nil；如果 `__index` 方法是一个表，则重复 1、2、3；如果 `__index` 方法是一个函数，则返回该函数的返回值。
- __index   
  
  1. 当在一个表中，去取某个key
  
  2. 如果该表中有对应的key,就直接返回
  
  3. 如果没有,则看有没有元表
  
  4. 没有元表，返回nil；
  
  5. 有元表则看元表中有没有__index
  
  6. 没有，返回nil
  
  7. 有，如果_index是个表，并且有key,返回值，没有key,返回nil
  
  8. 如果index是个function，则调用该function，且表和key会作为该函数的参数

```lua
local t1={};
print(t1.phone); --nil

local meta = {};

setmetatable(t1,meta);

meta.__index = {phone = "apple"};

print(t1.phone);  --apple
--------------
--__index为function时
local t2 = {};
print(t2.phone);

local meta = {};

meta.__index = function (t,k)
    t[k] = "apple"; --使用t,k参数进行操作
    return "index-phone";
end

setmetatable(t2,meta);
print(t2.phone); --使用返回值返回hongmi
print(t2.phone); --使用设置的apple
```

> 只取本表值使用rawget(table,index)，不会受元表的影响

## `__newindex` 元方法

类似 js 的 setter

使用`setmetatable(table,meta)`设置了__newindex

- 如果__newindex是表，则在本表里面设一个没有的key的时候，会写到__newindex对应的表中,如果是本表中有的key时，则会修改本表中的值

- 如果__newindex是function，则直接调用，且本表,key,value都可以作为参数

- __newindex为表时

```lua
local t1 = {name = "yong"};
local t2 = {};

local meta = {
    __newindex = t2;
}

setmetatable(t1,meta);

print(t1.name);

t1.id = "12";  --id这个key为新创建的，因为meta有__newindex,所以写到t2中的
print(t1.id); --nil
print(t2.id); --"12"
```

- __newindex为function时
  
  ```lua
  local t1 = {name = "yong"};
  local t2 = {};
  
  local meta = {
      __newindex = function (t,k,v)
          print("newindex");
          rawset(t,k,v); -- 直接修改原表值，不使用元表中的__newindex
          --[[这个地方会导致死递归，因为在写入新的key和value时会调用__newindex,
          而__newindex中又会进行调用，然后一直还没写进去就一直调用]]--
          -- t[k] = v; 
      end
  }
  setmetatable(t1,meta);
  
  t1.id = "12"; --会调用function
  --[[因为function中使用rawset进行元表数据进行修改,
  因为在function中可以拿到原表，key和value]]--
  print(t1.id); --"12"
  print(t2.id); --nil
  ```
  
  > 设置本表值使用rawset(table,index,value)，不会受设置了__newindex元方法的影响，即使是设置了新值，也是设置到本表中 

## `__tostring`方法

- __tostring用于改变table的输出，平时使用print只会输出表的地址，但实现了__tostring元方法后，输出表会调用meta中的__tostring函数，并在函数的返回值作为输出，可以用于格式化输出表。
  
  > 使用__tostring的function返回值作为本表的输出值，只要输出这个表就会调用function
  
  ```lua
  local t1 = {name = "yong",id = 12};
  local meta = {
      __tostring = function (t)
          local str = "";
          for k, v in pairs(t) do
              str = str..k..":"..v..";";
          end
          return str;
      end
  }
  setmetatable(t1,meta);
  print(t1.id); --12
  print(t1); -- name:yong;id:12;
  ```

## `__call`

- 把table当成函数来用，使用t()会去调用元方法__call的function。
  
  类似于类的构造函数，可以传值进行初始化
  
  function中的第一个参数为本表
  
  ```lua
  local t1 = {name = "yong",id = 12};
  local meta = {
      __tostring = function (t)
          local str = "";
          for k, v in pairs(t) do
              str = str..k..":"..v..";";
          end
          return str;
      end;
      __call = function (t,...)
          print(t);
          local tab = {...};
          for key, value in pairs(tab) do
              print(key..":"..value);
          end
      end
  }
  setmetatable(t1,meta);
  print(t1.id); --12
  print(t1); -- name:yong;id:12;
  print();
  ------------
  t1("abc",123);--在__call的函数中使用print也会调用__tostring方法
  ```

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
   -- print(index, value);
   -- end
   for k, v in ipairs(t3) do
   print(k, v);
   end
  
  print(t11 == t22);
  print(t11 == t11);
  ```

## 设置元表

- `setmetatable(table, metatable)`返回值为table
  
  
  
  
