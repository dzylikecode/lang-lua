#! /usr/bin/lua5.3
----------------------------------------------
--__newindex使用
----------------------------------------------
local t1 = {name = "yong"};
local t2 = {};

-- local meta = {
--     __newindex = t2;
-- }

local meta = {
    
    __newindex = function (t,k,v)
        print("newindex");
        -- rawset(t,k,v); -- 直接修改原表值，不使用元表中的__newindex
        -- t[k] = v; --这个地方会导致死递归，因为在写入新的key和value时会调用__newindex,而__newindex中又会进行调用，然后一直还没写进去就一直调用
    end
}

setmetatable(t1,meta);

-- print(t1.name);

t1.id = "12";  --id这个key为新创建的，因为meta有__newindex,所以写到t2中的
print(t1.id); --nil
print(t2.id); --"12"