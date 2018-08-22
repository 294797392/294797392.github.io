# 一些常用选项
-I：指定gcc搜索头文件的目录，如果有多个头文件目录，则写多个-I
-g：编译时带上符号信息，使用-g选项编译出来的文件可用于gdb调试用
-L：指定gcc搜索库的目录，先搜索指定的目录，再搜索/lib，/usr/lib，/usr/local/lib
-l：
-Wl,-rpath：指定程序运行时，链接库的搜索目录

# 在CMakeLists.txt里设置编译时带上符号信息，用于调试使用
cmake . -DCMAKE_BUILD_TYPE=Debug