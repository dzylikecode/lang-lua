#! /usr/local/lua5.3/bin/lua
--[[ 这里"require"的参数对应C库中"luaopen_mylib()"中的"mylib"。
     C库就放在"a.lua"的同级目录，"require"可以找到。]]
local libtestlib = require "libtestlib"

-- 结果与上面的例子中相同，但是这里是通过调用C库中的函数实现。
print(libtestlib.add(1,2));    --> 0.99999968293183
