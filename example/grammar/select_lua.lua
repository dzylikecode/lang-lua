#!/usr/bin/lua5.3

----------------------------------------------
--select的使用
----------------------------------------------

function f(...)
    a = select(3, ...)
    print(a)
    print(select(3, ...))
end

f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

----------
a = 1;
-- print('a'+1) 无法将一个非数字字符进行运算
print("5" + 6) --会将5转换成number进行运算
----------
function square(iteratorMaxCount, currentNumber)
    if currentNumber < iteratorMaxCount
    then
        currentNumber = currentNumber + 1
        return currentNumber, currentNumber * currentNumber
    end
end

for i, n in square, 3, 0
do
    print(i, n)
end
-------------
