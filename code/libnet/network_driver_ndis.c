#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <WinBase.h>
#include <ntddndis.h>

#include "network_driver.h"

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

typedef struct _NDISUIO_QUERY_BINDING
{
	ULONG BindingIndex;
	ULONG DeviceNameOffset;
	ULONG DeviceNameLength;
	ULONG DeviceDescrOffset;
	ULONG DeviceDescrLength;
} NDISUIO_QUERY_BINDING, *PNDISUIO_QUERY_BINDING;

typedef struct ndis_driver_s
{
	/* userspace io */
	HANDLE ndisuio;
}ndis_driver;

static void *ndis_drv_init(const char *ifname)
{
	ndis_driver *drv = (ndis_driver*)calloc(1, sizeof(ndis_driver));
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

struct network_driver_s network_driver_ndis =
{
	.name = "ndis",
	.description = "Windows NDIS driver",
	.init = ndis_drv_init
};
