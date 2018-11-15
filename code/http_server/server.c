#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base.h"
#include "server.h"

#define PORT 7777

void start_server(int argc, char *argv[])
{
	socket_handle sockfd = 0;
	struct sockaddr_in listen_addr;
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		log_error(MODULE, "create socket failed, %d", SOCKET_ERR);
		return;
	}

	listen_addr.sin_family = PF_INET;
	listen_addr.sin_port = htons(PORT);
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	memset(listen_addr.sin_zero, 0, sizeof(listen_addr.sin_zero));

	if (bind(sockfd, (struct sockaddr *)&listen_addr, sizeof(struct sockaddr)) == -1)
	{
		log_error(MODULE, "bind socket error, %d", SOCKET_ERR);
		return;
	}

	if (listen(sockfd, 5) == -1)
	{
		log_error(MODULE, "socket listen error, %d", SOCKET_ERR);
		return;
	}

error:
	return;
}