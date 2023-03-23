# coroutine

协同程序

Lua 协同程序(coroutine)与线程比较类似：拥有独立的堆栈，独立的局部变量，独立的指令指针，同时又与其它协同程序共享全局变量和其它大部分东西，其执行过程更类似于子例程，或者说不带返回值的函数调用 

线程与协同程序的主要区别在于，一个具有多个线程的程序可以同时运行几个线程，且线程的切换受系统控制，而协同程序却需要**彼此协作**的运行.**在任一指定时刻只有一个协同程序在运行，并且这个正在运行的协同程序只有在明确的被要求挂起的时候才会被挂起**，当前协程切换由自己控制

可以把协程看成是一个可自行决定调度切换的线程

协程的类型为thread，在lua中表示协程，其他语言中表示线程

| 方法                  | 描述                                                                             |
| ------------------- | ------------------------------------------------------------------------------ |
| coroutine.create()  | 创建 coroutine，返回 coroutine， 参数是一个函数，当和 resume 配合使用的时候就唤醒函数调用                    |
| coroutine.resume()  | 重启 coroutine，和 create 配合使用                                                     |
| coroutine.yield()   | 挂起 coroutine，将 coroutine 设置为挂起状态，这个和 resume 配合使用能有很多有用的效果                      |
| coroutine.status()  | 查看 coroutine 的状态注：coroutine 的状态有三种：dead，suspended，running，具体什么时候有这样的状态请参考下面的程序 |
| coroutine.wrap（）    | 创建 coroutine，返回一个函数，一旦你调用这个函数，就进入 coroutine，和 create 功能重复                      |
| coroutine.running() | 返回正在跑的 coroutine，一个 coroutine 就是一个线程，当使用 running 的时候，就是返回一个 coroutine 的线程号     |

## 一般使用

- 使用`cor_handle = coroutine.create(function)`创建协程，返回协程句柄

- 使用`coroutine.status(cor_handle)`查看协程的状态，返回值为协程的状态(string)

- 使用`coroutine.resume(cor_handle,arg1,arg2,...)`启动挂起的协程，第一次启动会将参数传给`function`，成功调用第一个返回值为true，在又被挂起时，会返回true并且返回`coroutine.yield(...)`中的参数`...`

- 调用`coroutine.yield(arg1,...)`将协程挂起，右边的返回值的位置是用于接受下次`coroutine.resume(cor_handle,...)`传进来的参数的`...`可用于后面的程序。

> 关键是理解`coroutine.resume`和`coroutine.yield`的传参和返回值的作用

```lua
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
```

- 使用协程实现双循环
  
  ```lua
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
  ```

- 简单的生产者消费者模型实现
  
  ```lua
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
  ```
