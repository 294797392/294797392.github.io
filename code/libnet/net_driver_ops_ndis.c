#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <Windows.h>
#include <WinBase.h>
#include <ntddndis.h>

#include "libnet.h"
#include "net_driver_ops.h"

#define NDISUIO_DEVICE_NAME TEXT("\\\\.\\\\Ndisuio")

#define FSCTL_NDISUIO_BASE FILE_DEVICE_NETWORK
#define _NDISUIO_CTL_CODE(_Function, _Method, _Access) \
	CTL_CODE(FSCTL_NDISUIO_BASE, _Function, _Method, _Access)

#define IOCTL_NDISUIO_OPEN_DEVICE \
	_NDISUIO_CTL_CODE(0x200, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISUIO_QUERY_OID_VALUE \
	_NDISUIO_CTL_CODE(0x201, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISUIO_SET_OID_VALUE \
	_NDISUIO_CTL_CODE(0x205, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISUIO_SET_ETHER_TYPE \
	_NDISUIO_CTL_CODE(0x202, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISUIO_QUERY_BINDING \
	_NDISUIO_CTL_CODE(0x203, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISUIO_BIND_WAIT \
	_NDISUIO_CTL_CODE(0x204, METHOD_BUFFERED, \
			  FILE_READ_ACCESS | FILE_WRITE_ACCESS)

typedef struct net_driver_ndis_data_s
{
	/* userspace io */
	HANDLE ndisuio;

	/* 当前指定的网络接口名字 */
	char ifname[128];
}net_driver_ndis_data;

typedef struct _NDISUIO_QUERY_BINDING
{
	ULONG BindingIndex;
	ULONG DeviceNameOffset;
	ULONG DeviceNameLength;
	ULONG DeviceDescrOffset;
	ULONG DeviceDescrLength;
} NDISUIO_QUERY_BINDING, *PNDISUIO_QUERY_BINDING;

static void *net_driver_ndis_init()
{
	net_driver_ndis_data *drv = (net_driver_ndis_data*)calloc(1, sizeof(net_driver_ndis_data));
	if (!drv)
	{
		goto error;
	}

	if ((drv->ndisuio = CreateFile(NDISUIO_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, INVALID_HANDLE_VALUE)) == INVALID_HANDLE_VALUE)
	{
		goto error;
	}

	DWORD written;
	if (!DeviceIoControl(drv->ndisuio, IOCTL_NDISUIO_BIND_WAIT, NULL, 0, NULL, 0, &written, NULL))
	{
		goto error;
	}

	return drv;

error:
	if (drv)
	{
		if (drv->ndisuio)
		{
			CloseHandle(drv->ndisuio);
		}
		free(drv);
	}
	return NULL;
}

static int net_driver_ndis_scan(void *handle, interfase *iflist, size_t list_len)
{
	net_driver_ndis_data *drv = (net_driver_ndis_data*)handle;

	size_t bufsize = sizeof(NDISUIO_QUERY_BINDING) + 1024;
	NDISUIO_QUERY_BINDING *binding = (NDISUIO_QUERY_BINDING*)malloc(bufsize);
	DWORD written = 0;

	for (size_t i = 0; i < list_len; i++)
	{
		memset(binding, 0, bufsize);
		binding->BindingIndex = i;
		if (DeviceIoControl(drv->ndisuio, IOCTL_NDISUIO_QUERY_BINDING, binding, sizeof(NDISUIO_QUERY_BINDING), binding, (DWORD)bufsize, &written, NULL) == 0)
		{
			DWORD error = GetLastError();
			if (error == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			else
			{
				printf("IOCTL_NDISUIO_QUERY_BINDING failed: %d", error);
				goto error;
			}
		}

		wcstombs(iflist[i].name, (wchar_t*)((char*)binding) + binding->DeviceNameOffset, binding->DeviceNameLength);
		wcstombs(iflist[i].description, (wchar_t*)(char*)binding + binding->DeviceDescrOffset, binding->DeviceDescrLength);
	}

	return NERR_OK;

error:
	if (binding)
	{
		free(binding);
	}
	return NERR_FAIL;
}

net_driver_ops net_driver_ndis_ops =
{
	.name = "ndis",
	.description = "management network using windows ndis",
	.init = net_driver_ndis_init,
	.scan = net_driver_ndis_scan
};
