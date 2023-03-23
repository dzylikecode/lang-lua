# 文件操作

## 基本IO操作api

- `io.open(file_path,mode)`，打开文件
  
  - **"`r`":** read mode (the default);
  
  - **"`w`":** write mode;
  
  - **"`a`":** append mode;
  
  - **"`r+`":** update mode, all previous data is preserved;
  
  - **"`w+`":** update mode, all previous data is erased;
  
  - **"`a+`":** append update mode, previous data is preserved, writing is only allowed at the end of file.

- `io.input(file)`，读取文件到io终端

- `io.read()`，读出文件中的一行，需要先执行`io.input(file)`输入重定向到文件，默认是标准输入，即键盘。
  
  - `io.read()`参数
    
    - **"`n`":** 读取有效的数字表示
    
    - **"`a`":** 读取当前位置到文件结束
    
    - **"`l`":** 读取一行跳过换行符
    
    - **"`L`":** 读取一行包括换行符
    
    - ***number*:** 读取number个字符

```lua
--print文件中的每一行
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
```

- `io.output(file)`把文件句柄输出到当前io，可以理解问重定向到file中，默认是显示器

- `io.write(str)`写内容到文件中，前提是open时不是只读

## 基本的文件操作API

- 使用文件句柄对文件进行操作，不用每次读不同的文件都`io.input(file)`在进行读取

- `file = io.open(file_path,mode)`

- `file:read()`
  
  - **"`n`":** 读取有效的数字表示
  
  - **"`a`":** 读取当前位置到文件结束
  
  - **"`l`":** 读取一行跳过换行符
  
  - **"`L`":** 读取一行包括换行符
  
  - ***number*:** 读取number个字符

- `file:seek(whence,offset)`
  
  - 无参数，默认返回当前位置
  
  - whence
    
    - **"`set`":** base is position 0 (beginning of the file);
    
    - **"`cur`":** base is current position;
    
    - **"`end`":** base is end of file;
    
    > `file:seek("cur")`return the current position,if offset is not zero,then set position `cur+offset`；`file:seek("set")` sets the position to the beginning of the file (and returns 0); and the call `file:seek("end")` sets the position to the end of the file, and returns its size.
  
  - offset：表示从whence偏移多少设置光标，正的offset表示向后偏移，负的offset表示向前偏移。

- `file:lines`返回文件行迭代器，参数和`file:read()`一致。
