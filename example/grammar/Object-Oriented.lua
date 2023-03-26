#! /usr/bin/lua5.3
----------------------------------------------
--self的使用，实现面向对象
--实例化类
----------------------------------------------

local t1 = {name = "yong",id = 12};

function t1:getName ()
    return self.name;
end

print(t1:getName());

-- local t2 = {name = "yong",id = 12};

-- t2.getName = function ()
--     return t2.name;--对一个nil进行索引会报错
-- end

-- local t3 = t2;

-- t2 = nil;
-- print(t3.getName());
----------------------------------------------
--实例化，没提供初始化即继承父类属性
----------------------------------------------
function t1:new(obj)
    obj = obj or {};
    setmetatable(obj,self);--继承
    self.__index = self;   --继承
    return obj;            --返回实例
end

local t4 = t1:new();
print(t4:getName());
print("--------------");
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
