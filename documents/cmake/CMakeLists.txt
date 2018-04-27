cmake_minimum_required(VERSION 3.4)
project(cmake VERSION 1.0 LANGUAGES C)

# 调用project命令之后，cmake会在内部初始化一些变量的值，这里打印出一些变量的值做测试用
message(STATUS 
    "PROJECT_NAME:" ${PROJECT_NAME} "\n"
    "   PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR} "\n"
    "   cmake_SOURCE_DIR:" ${cmake_SOURCE_DIR} "\n"
    "   PROJECT_BINARY_DIR:" ${PROJECT_BINARY_DIR} "\n"
    "   cmake_BINARY_DIR:" ${cmake_BINARY_DIR}
)

# 引用头文件目录
include_directories(source/inc)

# 生成cmake动态库
add_library(cmake SHARED source/src/cmake.c)

# 生成cmake可执行程序
add_executable(main source/src/main.c)

# 注意，add_library和add_executable两个命令不能出现重复的name