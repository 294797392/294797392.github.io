cmake_minimum_required(VERSION 3.4)
project(demo VERSION 1.1 LANGUAGES C)

# 指定RUNTIME输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

# 设置要引用的头文件目录
include_directories(inc)

add_executable(socket src/main.c src/socket.c)

target_link_libraries(socket PRIVATE ncurses)