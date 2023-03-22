#! /usr/bin/lua5.3
----------------------------------------------
--常用的元方法
----------------------------------------------
local t1 = {name = "yong",id = 12};
local meta = {
    __tostring = function (t)
        local str = "";
        for k, v in pairs(t) do
            str = str..k..":"..v..";";
        end
        return str;
    end,
    __call = function (t,...)
        print(t);
        local tab = {...};
        for key, value in pairs(tab) do
            print(key..":"..value);
        end
    end
}
setmetatable(t1,meta);
-- print(t1.id); --12
-- print(t1); -- name:yong;id:12;

------------
t1("abc",123);--在__call的函数中使用print也会调用__tostring方法
