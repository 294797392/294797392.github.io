/**************************************************************************
* @ file    : win32.c
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.09.11
* @ brief   : windows下编译头文件, 仅用于windows下模拟调试使用
*
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#include <stdio.h>

#include "win32.h"

#ifdef WIN32

int access(const char* pathname, int mode) {
    return R_OK;
}

int readlink(const char *path, char *buf, size_t bufsiz) {
    return 0;
}

#endif // WIN32