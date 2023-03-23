#! /usr/bin/lua5.3

local function cor(a,b)
    print("cor start");
    local x,y,z = coroutine.yield(a,b,"first_yield");--返回true aa bb first_yield
    print(x,y,z); --打印出"xx","yy","zz"
end

local cor_handle = coroutine.create(cor);

print(coroutine.status(cor_handle));

print(coroutine.resume(cor_handle,"aa","bb"));

print(coroutine.status(cor_handle));--suspended

print(coroutine.resume(cor_handle,"xx","yy","zz"));--调用会将参数传给x，y，z，结束时打印true

print(coroutine.status(cor_handle));


----------------------------------------------
--使用协程实现双循环
local function sleep(s)
    local now = os.clock();
    while now + s >= os.clock() do
    end 
end

local function fun1()
    while true do
        local rs = coroutine.resume(Co_handle2);
        print("fun1");
        sleep(1);
    end
end


local function fun2()
    while true do
        coroutine.yield();
        print("fun2");
        sleep(1);
    end
end


Co_handle1 = coroutine.create(fun1);
Co_handle2 = coroutine.create(fun2);

coroutine.resume(Co_handle1);


