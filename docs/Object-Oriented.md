# Object-Oriented

## 类

- lua中没有实现类，但可以使用table实现类的操作

- 在定义function时使用下面方式则可以在function中使用self
  
  ```lua
  local t1 = {name = "yong",id = 12};
  
  function t1:getName ()--此函数不能写在table里面，否则找不到t1，因为t1还未创建
      return self.name;
  end
  
  print(t1:getName());
  ```

- 在lua中的赋值默认是引用，或者说是shared_ptr
  
  ```lua
  local t2 = {name = "yong",id = 12};
  
  t2.getName = function () 
      return t2.name;--对一个nil进行索引会报错
  end
  
  local t3 = t2;
  
  t2 = nil;
  print(t3.getName()); --报错，因为在调用t3.getName会找t2中的name，但t2已经无效
  ```

## 自索引

- table.__index = table
  
  > 必须在表创建完后再进行自索引，否则在表中进行自索引表还未存在，会给索引赋值为nil

- 使用自索引可以避免多创建一个metatable用来设置index，直接使用本表作为__index,别人把本表设置为他的metatable时，可以使用到本表里面的东西。

### 自索引实现继承

- 将父类表作为本表的__index，并将父类自索引

- 方法实现在表的外部进行实现，使用`function father:method(){}`，在方法名前面加上`father:`在方法中可以使用self访问father本表

```lua
----------------------------------------------
--使用自索引实现继承
----------------------------------------------

local father = {name = "Father",id = 0};
function father:sayfather()
    print("father",self.name,self.id);
end

local son = {}
function son:sonsay()
    print("sayson");
end

father.__index = father;
setmetatable(son, father);
son:sonsay();
son:sayfather();
print(son.name);
```

### 类的实例化

-  实现new一个对象
  
  ```lua
  function obj:new(newobj)
      newobj = newobj or {};
      setmetatable(newobj,self);--继承
      self.__index = self;   --自索引继承
      return obj;            --返回实例
  end
  ```

## 多重继承

- grandfather和father设置为自索引，并且都设置自己的父类为元表
  
  ```lua
  local grandfather = {age = 123};
  grandfather.__index = grandfather;
  function grandfather:grandfathersay()
      print("i'm "..self.age.." years old,grandfather!");
  end
  
  local father = {name = "Father",id = 0};
  father.__index = father;
  setmetatable(father,grandfather);
  function father:sayfather()
      print("father",self.name,self.id);
  end
  
  
  local son = {}
  setmetatable(son,father);
  function son:sonsay()
      print("sayson");
  end
  
  
  setmetatable(son, father);
  son:sonsay();
  son:sayfather();
  son:grandfathersay();
  print(son.age); --123
  ```

## 重写

- 如果子类没有初始化（特化），则默认使用父类的值，且值和方法受父类的影响
  
  ```lua
  ----------------------------------------------
  --重写成员变量和方法
  local obj0 = {age = 123};
  --示例化对象，并且支持多继承
  function obj0:new(obj)
      obj = obj or {};
      setmetatable(obj,self);--继承
      self.__index = self;   --继承
      return obj;            --返回实例
  end
  function obj0:rewrite()
      print("obj0 rewrite");
  end
  
  local obj2 = obj0:new();--此时继承了age = 123
  --local obj1 = obj0:new({age = 123}); --重写了age成员变量，不会去obj0的__index方法找，因为自索引，也就是不会去obj0中找
  function obj2:rewrite()
      print("obj2 rewrite");
  end
  
  local obj1 = obj0:new({age = 50});
  
  print(obj0.age);
  print(obj1.age);
  print("");
  obj0:rewrite();
  obj2:rewrite();
  ```

## 私有化

- 使用function作为一个类的封闭环境，在函数中定义局部变量，并将对这个类的操作接口作为这个函数的返回值，放在一个table中，后面别人使用只要拿到这个table后使用table中的方法进行操作即可。 
  
  注意需要使用全局变量进行返回，局部变量无法return出去，需要定义变量进行return
  
  ```lua
  local function private_obj()
      local member ={
          name = "yong",
          id   = 12,
          age  = 23,
          sex  = "male"
      }
      local function getName()
          return member.name;
      end
      local function getId()
          return member.id;
      end
      local function setId(id)
          member.id = id;
      end
  
      return {
          getId = getId,
          getName = getName,
          setId = setId,
      }
  end
  
  local obj = private_obj();
  print(obj.getId());
  obj.setId(13);
  print(obj.getId());
  print(obj.getName());
  ```
