#! /usr/bin/lua5.3

----------------------------------------------
--自己实现ipairs和pairs
----------------------------------------------

local function my_ipairs(tab)
    local count = #tab;
    local index = 0;
    return function()
        index = index + 1;
        if index <= count then
            if tab[index] ~= nil then
                return index, tab[index];
            end
        end
    end
end


local t = { 1, 2, 3, nil, 5, 6, nil, 8 };
for k, v in my_ipairs(t) do
    print(k, v);
end
for k, v in ipairs(t) do
    print(k, v);
end

local function my_pairs(tab)
    local count = #tab;
    local index = 0;
    return function()
        index = index + 1;
        if index <= count then
            if tab[index] ~= nil then
                return index, tab[index];
            else
                while not tab[index] do
                    if index <= count then
                        index = index + 1;
                    else
                        break;
                    end
                end
                if index <= count then
                    return index, tab[index];
                end
            end
        end
    end
end

for k, v in my_pairs(t) do
    print(k, v);
end

for k, v in pairs(t) do
    print(k, v);
end
