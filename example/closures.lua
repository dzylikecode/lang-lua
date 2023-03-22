#! /usr/bin/lua5.3
----------------------------------------------
--闭包理解
----------------------------------------------
local function fun(a, b)
    local x = 0;
    return function()
        print(a, b, x);
        x = x + 1;
    end
end

fun(1, 2);

local env1 = fun(1, 2); -- 此处是传参，没有进行函数调用
env1();                 --x = 0
env1();                 --x = 1
env1();                 --x = 2

local env2 = fun(1, 2);
env2(); --x = 0
env2(); --x = 1

local env2_copy = env2;
-- env2_copy 保留了env2的环境
env2_copy(); --x = 2
