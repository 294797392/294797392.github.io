# 使用CMake编译生成安卓原生（C/C++）程序

## 使用NDK自带的make-standalone-toolchain.sh脚本定制编译工具链接
/ndkroot/build/tools/make-standalone-toolchain.sh –-toolchain=arm-linux-androideabi-4.9 --arch=arm --platform=23
–-toolchain：指定要使用的平台的工具链，所有的工具链可以在/ndkroot/toolchains目录里找到
--arch：指定目标平台类型
--platform：指定要使用的安卓SDK的版本

## 配置CMakeLists.txt，使其使用安卓工具链编译程序
`SET(NATIVE_ANDROID_TOOLCHAIN /tmp/ndk-zyf/arm-linux-androideabi)`
    定义编译工具链所在目录
`SET(CMAKE_SYSTEM_NAME Android)`
    告诉CMake启用安卓跨平台编译，CMake会初始化安卓交叉编译环境
`SET(CMAKE_SYSTEM_VERSION 23)`
    配置要使用的AndroidSDK版本号，和生成工具链时配置的版本号（--platform参数）要一致
`SET(CMAKE_ANDROID_STANDALONE_TOOLCHAIN ${NATIVE_ANDROID_TOOLCHAIN})`
    告诉CMake安卓编译工具链所在目录，CMake会使用${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}/sysroot目录作为sysroot。如果没有指定，CMake会使用一个默认的目录
`SET(CMAKE_C_COMPILER ${NATIVE_ANDROID_TOOLCHAIN}/bin/arm-linux-androideabi-gcc)`
    告诉CMake GCC的位置
`SET(CMAKE_CXX_COMPILER ${NATIVE_ANDROID_TOOLCHAIN}/bin/arm-linux-androideabi-g++)`
    告诉CMake G++的位置
`set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")`
    安卓5.0以上要加上-fPIE选项
`set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")`
    安卓5.0以上要加上-fPIE选项

## 在Android原生程序里使用pthread的问题
pthread相关的函数都在android bionic库里，安卓工具链会默认链接这个库，所以在编译的时候，不用在cmake里加target_link_libraries(android_program PRIVATE pthread)选项，不然会编译不过

## 参考
https://developer.android.google.cn/ndk/guides/standalone_toolchain
https://blog.csdn.net/MingHuang2017/article/details/79000345
