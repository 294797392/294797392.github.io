#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "libnet.h"
#include "net_driver_ops.h"

extern net_driver_ops net_driver_ndis_ops;

int main(int agrc, char *argv[])
{
	libnet_init();

	interfase iflist[64] = { 0 };
	void *handle = net_driver_ndis_ops.init();
	net_driver_ndis_ops.scan(handle, iflist, 64);

	for (size_t i = 0; i < 64; i++)
	{
		interfase iface = iflist[i];
		if (strlen(iface.name) == 0)
		{
			break;
		}

		printf("name:%s, desc:%s\n", iface.name, iface.description);
	}

	/*const char *str = setlocale(LC_ALL, "");
	unsigned char buff[1024] = { '\0' };
	int i = wcstombs(buff, L"º×Óì", sizeof(buff));*/

	Sleep(999999);

	return 1;
}
