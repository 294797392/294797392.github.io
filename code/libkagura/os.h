#ifndef __OS_H__
#define __OS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#endif


#ifdef WINDOWS
typedef WCHAR OSCHAR;
#else
typedef char OSCHAR;
#endif

void osstrlen(OSCHAR *str);

#endif