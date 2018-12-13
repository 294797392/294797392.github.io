#ifndef __LIBNET_H__
#define __LIBNET_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#endif

#define LIBNET_SYMBOL_EXPORT

#define MAX_IF_ID_SIZE 128
#define MAX_IF_NAME_SIZE 256
#define MAX_IF_DESC_SIZE 512
#define MAX_AP_NAME_SIZE 256
#define MAX_AP_DESC_SIZE 512
#define MAX_AP_ID_SIZE 32

/* 错误码 */
#define NERR_FAIL 0
#define NERR_OK 1

typedef struct access_point_s
{
	char id[MAX_AP_ID_SIZE];
	char name[MAX_AP_NAME_SIZE];
	char description[MAX_AP_DESC_SIZE];
}access_point;

/* 存储网络接口信息 */
typedef struct interface_s
{
	char id[MAX_IF_ID_SIZE];
	char name[MAX_IF_NAME_SIZE];
	char description[MAX_IF_DESC_SIZE];
}interfase;

LIBNET_SYMBOL_EXPORT void libnet_init();

#endif // !__LIBNET_H__
