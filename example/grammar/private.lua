#! /usr/bin/lua5.3

local function private_obj()
    local member ={
        name = "yong",
        id   = 12,
        age  = 23,
        sex  = "male"
    }
    local function getName()
        return member.name;
    end
    local function getId()
        return member.id;
    end
    local function setId(id)
        member.id = id;
    end

    return {
        getId = getId,
        getName = getName,
        setId = setId,
    }
end

local obj = private_obj();
print(obj.getId());
obj.setId(13);
print(obj.getId());
print(obj.getName());
