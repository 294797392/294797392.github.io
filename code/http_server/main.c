#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base.h"
#include "server.h"

int main(int argc, char *argv[])
{
	start_server(argc, argv);
	return ERR_OK;
}