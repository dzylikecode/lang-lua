#! /usr/bin/lua5.3
----------------------------------------------
--使用自索引实现继承、多继承
----------------------------------------------


local grandfather = {age = 123};
grandfather.__index = grandfather;
function grandfather:grandfathersay()
    print("i'm "..self.age.." years old,grandfather!");
end
function grandfather:rewirte()
    print("grandfather-rewrite");
end

local father = {name = "Father",id = 0};
father.__index = father;
setmetatable(father,grandfather);
function father:sayfather()
    print("father",self.name,self.id);
end


local son = {}
setmetatable(son,father);
function son:sonsay()
    print("sayson");
end


setmetatable(son, father);
son:sonsay();
son:sayfather();
son:grandfathersay();
print(son.age); --123



