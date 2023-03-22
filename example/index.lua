#! /usr/bin/lua5.3
----------------------------------------------
--__index使用
----------------------------------------------

local t1={};
print(t1.phone); --nil

local meta = {};

setmetatable(t1,meta);

meta.__index = function ()
    return "apple";
end;

print(t1.phone);  --apple
-----------------

local t2 = {};
print(t2.phone);

local meta = {};

meta.__index = function (t,k)
    t[k] = "apple"; --使用t,k参数进行操作
    return "index-phone";
end

setmetatable(t2,meta);
print(t2.phone);
print(t2.phone);