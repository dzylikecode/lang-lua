#! /usr/bin/lua5.3
----------------------------------------------
--表的常用api
----------------------------------------------
local t1 = { "abc", "efg", "aaa" };
print(table.concat(t1));

table = { 1, 2, { "abc", "aaa" }, name = "yong", sex = "male" };

for k1, v1 in pairs(table) do
    if type(v1) == "table" then
        for k2, v2 in pairs(v1) do
            print(k2, v2);
        end
    else
        print(k1, v1);
    end
end
