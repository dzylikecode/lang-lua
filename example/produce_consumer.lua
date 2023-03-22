#! /usr/bin/lua5.3
---@diagnostic disable: lowercase-global

--只能一生产一消费
local function sleep(s)
    local now = os.clock();
    while now + s >= os.clock() do
    end 
end

local function produce()
    local i = 0;
    while true do
        i = i + 1;
        print("product")
        print(coroutine.status(consumerHandle));
        coroutine.yield(i); --当consumer唤醒produceHandle时就会收到i进行消费，可以借此传输产品,并将cpu让出
    end
end

local function consumer()
    while true do
        --在一个协程启动另一个协程会将本协程挂起,本协程释放cup给produceHandle运行，处于normal状态，即活跃但是不运行
        local flag,i = coroutine.resume(produceHandle);--请求生产者生产，同时返回yield中的参数i
        print(flag,i);
        sleep(2);
    end
end

produceHandle = coroutine.create(produce);
consumerHandle = coroutine.create(consumer);

coroutine.resume(consumerHandle);