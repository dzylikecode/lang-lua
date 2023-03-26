#! /usr/bin/lua5.3

----------------------------------------------
--ipairs和pairs的使用
----------------------------------------------

--[[t1 = {1, 2, 3, 4};
for k,v in ipairs(t1) do
	print(k,v);
end

t1[6] = 6;
for k,v in ipairs(t1) do
	print(k,v);
end

--使用pairs遍历table
for k,v in pairs(t1) do
	print(k,v);
end

t={
    {11,12,13},
    {21,22,23}
};
for row,row_table in ipairs(t) do
    for col,ele in ipairs(row_table) do
        print(row,col,ele);
    end
end

--创建二维数组
t3 = {};
for i = 1,3 do
	t3[i] = {};
	for j = 1,3 do
		t3[i][j] = (i - 1)*3 + j;
	end
end

--遍历二维数组
for i = 1,3 do
	for j = 1,3 do
		print(t3[i][j]);
	end
end
]]--


--创建二维数组
local t4 = {};
for i = 1,3 do
--	t4[i] = {};
	for j = 1,3 do
		t4[i][j] = (i -1)*3 + j;
	end
end

--遍历二维数组
for i = 1,3 do
	for j = 1,3 do
		print(t4[i][j]);
	end
end


