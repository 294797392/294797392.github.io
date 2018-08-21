# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(C VERSION 1.1 LANGUAGES C)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")

# 设置要引用的头文件目录
include_directories(./)

# 全局宏定义
add_definitions(-D LINUX)

# 设置要编译的源文件列表
list(APPEND C_SRC_FILES mplayer.c)
list(APPEND C_SRC_FILES main.c)

# 生成可执行程序
add_executable(player ${C_SRC_FILES})
target_link_libraries(player PRIVATE pthread)