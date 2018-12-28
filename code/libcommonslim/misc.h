/**************************************************************************
* @ file    : misc.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.05.15
* @ brief   : 常用工具函数
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __MISC_H__
#define __MISC_H__

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
#define SLEEP(sec) Sleep(sec * 1000)
#else
#define SLEEP(sec) sleep(sec)
#endif

////----------------------------------------------------------------------------
//// 取随机数
////----------------------------------------------------------------------------
//int random_range(int min, int max);
//
////----------------------------------------------------------------------------
//// 随机休眠一段时间（单位ms)
////----------------------------------------------------------------------------
//void random_sleep(int minSleepMs, int maxSleepMs);

void imi_dispath(const char *filepath, char *path, char *file);
char * imi_realpath(const char *path, char *resolved_path);


// @brief: 动态加载library
// @path: 需要加载的库的path
// @return:  所加载的lib的handle. NULL: 错误
void* imi_load_library(const char* path);

// @brief: 从动态加载的library中加载函数
// @libhandle: load_library返回的handle
// @func_name: 需要加载的函数名
// @return:    加载的函数指针。 NULL: 错误
void* imi_load_function(void* libhandle, const char* func_name);

// @brief: 释放动态加载的library
// @libhandle: load_library返回的handle
// @return: 0: 成功，其他错误编码
int imi_close_library(void* libhandle);


void* imi_create_sharemem(const char *sharefile, size_t size, int mode);

// 从arguments读取对应key的value. 如果指定key不存在，则使用default value
// 例如参数列表为 -c abc时:  调用get_args_string(argc, argv, "-c", NULL)获得abc
// @argc            main函数输入的参数数组长度
// @argv            main函数输入的参数数组
// @key             需要读取的key值
// @defaultValue    指定key值不存在或无效时，需要返回的default value
// @return          key所对应的value值，或者default value. (注意，返回值为直接引用，并不分配内存)
const char* get_args_string(int argc, const char* argv[], const char* key, const char* defaultValue);
int get_args_int(int argc, const char* argv[], const char* key, const int defaultValue);
long get_args_long(int argc, const char* argv[], const char* key, const long defaultValue);
double get_args_double(int argc, const char* argv[], const char* key, const double defaultValue);

// 检测arguments列表中是否存在指定的key
BOOL is_args_exist(int argc, const char* argv[], const char* key);


// 从文件中读取所有内容至缓冲区当中
// @return: 小于0: 错误代码, 大于0: 读出的实际大小
int imi_read_all(const char *path, char* buffer, int bufferSize);

// 从文件中读取所有文本
// 调用者需要自己释放内存
char* imi_read_all_new(const char *path, int* out_size);



void throw_not_implement_exception();

#ifdef __cplusplus
}
#endif

#endif // __MISC_H__