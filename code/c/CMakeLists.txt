# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(cutils VERSION 1.1 LANGUAGES C)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")

# 设置要引用的头文件目录
include_directories(.)

# 全局宏定义
add_definitions(-D LINUX)

# 设置要编译的源文件列表
list(APPEND CUTILS_SRC_FILES process.c)

# 生成可执行程序
add_library(cutils SHARED CUTILS_SRC_FILES)
