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
