# vim: ff=unix ts=4 sw=4 et ai
cmake_minimum_required(VERSION 3.4)
project(uart_daemon VERSION 1.1 LANGUAGES C)

# set(UART_DAEMON_VERSION_MAJOR 1)
# set(UART_DAEMON_VERSION_MINOR 0)

# 设置宏定义
add_definitions(-D LINUX)
add_definitions(-D SHOW_PROMPT)
add_definitions(-D CONSOLELOG_ENABLED)

# configure_file(inc/main.h.in inc/main.h)

# 设置要引用的头文件目录
include_directories(inc)

# 设置变量
set(SRC_FILES src/log.c src/utils.c src/sha256.c src/command.c)

# 生成so库
add_library(gm8136 SHARED src/gm8136.c ${SRC_FILES})
add_library(ambas2l22m SHARED src/ambas2lm.c ${SRC_FILES})
add_library(ambas3l22m SHARED src/ambas3lm.c ${SRC_FILES})
add_library(allwinner_r16 SHARED src/allwinner_r16.c ${SRC_FILES})
add_library(mstar_318 SHARED src/mstar_318.c ${SRC_FILES})
add_library(linux SHARED src/linux.c ${SRC_FILES})

# 生成可执行文件
add_executable(uart_daemon src/main.c src/log.c src/command.c src/utils.c src/sha256.c src/socket.c )

# 设置引用的外部库
target_link_libraries(uart_daemon PRIVATE dl)
target_link_libraries(uart_daemon PRIVATE pthread)

# 交叉编译配置
SET(CMAKE_SYSTEM_NAME ambas3l22m)
SET(CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_FIND_ROOT_PATH /usr/local/libra/ambarella/s3l22m/linaro-armv7ahf-2015.11-gcc5.2)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_COMPILER /usr/local/libra/ambarella/s3l22m/linaro-armv7ahf-2015.11-gcc5.2/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /usr/local/libra/ambarella/s3l22m/linaro-armv7ahf-2015.11-gcc5.2/bin/arm-linux-gnueabihf-g++)