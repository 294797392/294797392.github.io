#ifndef __BASE_H__
#define __BASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef WINDOWS
#include <WinSock2.h>
#else
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef WINDOWS
typedef SOCKET socket_handle;
#define SOCKET_ERR WSAGetLastError()
#define SYSTEM_ERROR GetLastError()
#else
typedef int socket_handle;
#define SOCKET_ERR errno
#define SYSTEM_ERROR errno
#endif


/* ErrorCode */
#define ERR_FAIL 0
#define ERR_OK 1

#define log_error(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")
#define log_info(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")
#define log_warn(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")


typedef struct server_config_s
{
	char *place_holder;
}server_config;

#endif
