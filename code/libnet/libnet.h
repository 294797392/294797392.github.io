#ifndef __LIBNET_H__
#define __LIBNET_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#endif

#define MAX_IF_NAME_SIZE 256
#define MAX_IF_DESC_SIZE 512

/* ������ */
#define NERR_FAIL 0
#define NERR_OK 1

/* �洢����ӿ���Ϣ */
typedef struct interface_s
{
	char name[MAX_IF_NAME_SIZE];
	char description[MAX_IF_DESC_SIZE];
}interfase;


void libnet_init();

#endif // !__LIBNET_H__
