#ifndef __SERVER_H__
#define __SERVER_H__

#include "base.h"

typedef struct server_s
{
	char *name;
}server_t;

void start_server(int argc, char *argv[]);

#endif