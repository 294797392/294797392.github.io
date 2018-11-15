#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#ifdef WINDOWS
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include "base.h"
#include "server.h"

typedef struct server_socket_s
{
	socket_handle socket;
}server_socket;

#endif