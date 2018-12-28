/**************************************************************************
* @ file    : win32.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.09.11
* @ brief   : windows下编译头文件, 仅用于windows下模拟调试使用
*
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __WIN32_H__
#define __WIN32_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#include <direct.h>     // need for: getcwd, chdir
#include <process.h>    // need for: getpid
#include <Windows.h>    // need for: InterlockedExchangeAdd

// 模拟linux下access函数
#define R_OK 0
#define F_OK 0
#define snprintf _snprintf
#define getcwd   _getcwd
#define chdir    _chdir
#define __sync_add_and_fetch(a, b) InterlockedExchangeAdd(a, b)
#define __sync_sub_and_fetch(a, b) InterlockedExchangeAdd(a, -b)

#define unlink(filename) _unlink(filename)
#define getpid() _getpid()

// 模拟linux下access函数
int access(const char* pathname, int mode);

// 模拟linux下readlink函数
int readlink(const char *path, char *buf, size_t bufsiz);

// 模拟pthread
//typedef void* pthread_rwlock_t; 
//typedef void* pthread_mutex_t;
//typedef void* pthread_cond_t;


#else  // suppose GNU

#include <dlfcn.h>
#include <unistd.h>

#endif // WIN32

#ifdef __cplusplus
}
#endif

#endif // __WIN32_H__
