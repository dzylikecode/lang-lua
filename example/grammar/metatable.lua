----------------------------------------------
--常用重载
----------------------------------------------

local t11 = { 1, 2, 3, nil };
local t22 = { 1, 2, 3, 6 };
local metat = {};

setmetatable(t11, metat);
setmetatable(t22, metat);

metat.__add = function(t1, t2)
    local len1 = #t1;
    local len2 = #t2;
    local t = {};
    if len1 <= len2 then
        len1 = len2;
    end
    for i = 1, len1 do
        local a = t1[i] or 0;
        local b = t2[i] or 0;
        t[i] = a + b;
    end
    return t;
end

metat.__eq = function(t1, t2)
    local len1 = #t1;
    local len2 = #t2;
    if len1 ~= len2 then
        return false;
    end
    for i = 1, len1 do
        if t1[i] ~= t2[i] then
            return false;
        end
    end
    return true;
end


local t3 = t11 + t22;
-- for index, value in pairs(t3) do
--     print(index, value);
-- end
for k, v in ipairs(t3) do
    print(k, v);
end

print(t11 == t22);
print(t11 == t11);
print(math.random(0,100));


