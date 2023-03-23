#! /usr/bin/lua5.3


local file = io.open("filetest.txt");

if file == nil then
    print("file not exist");
    os.exit();
end
print(file); --file

--读取文件内容

io.input(file);
while true do
    local str = io.read();
    if str == nil then
        break;
    end
    print(str);
end