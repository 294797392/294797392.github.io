#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include "libnet.h"
#include "libwl.h"

typedef struct libwl_s
{
	HANDLE client_handle;
	libwl_event_callback callback;
}libwl;

LIBNET_SYMBOL_EXPORT int libwl_init(void **handle)
{
	DWORD ret = ERROR_SUCCESS;

	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	libwl *hwnd = (libwl*)calloc(1, sizeof(libwl));
	if (!hwnd)
	{
		return NERR_FAIL;
	}

	if ((ret = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hwnd->client_handle)) != ERROR_SUCCESS)
	{
		return NERR_FAIL;
	}

	*handle = hwnd;

	return NERR_OK;
}

LIBNET_SYMBOL_EXPORT int libwl_callback(void *handle, libwl_event_callback callback)
{
	libwl *wl = (libwl*)handle;
	wl->callback = callback;
	return NERR_OK;
}

LIBNET_SYMBOL_EXPORT int libwl_scan_iflist(void *handle, wliface **ppiflist, int *piflen)
{
	PWLAN_INTERFACE_INFO_LIST iflist = NULL;
	DWORD ret = ERROR_SUCCESS;

	libwl *wl = (libwl*)handle;
	if ((ret = WlanEnumInterfaces(wl->client_handle, NULL, &iflist)) != ERROR_SUCCESS)
	{
		return NERR_FAIL;
	}

	*ppiflist = (interfase*)calloc(iflist->dwNumberOfItems, sizeof(interfase));
	*piflen = iflist->dwNumberOfItems;

	DWORD iflen = iflist->dwNumberOfItems;
	for (DWORD i = 0; i < iflen; i++)
	{
		WLAN_INTERFACE_INFO iface = iflist[i].InterfaceInfo[0];
		memcpy((*ppiflist)[i].id, &iface.InterfaceGuid, sizeof(GUID));
		wcstombs((*ppiflist)[i].name, iface.strInterfaceDescription, MAX_IF_NAME_SIZE);
		switch (iface.isState)
		{
			case wlan_interface_state_connected: (*ppiflist)[i].state = WIFSTAT_CONNECTED; break;
			case wlan_interface_state_disconnecting:(*ppiflist)[i].state = WIFSTAT_DISCONNECTING; break;
			case wlan_interface_state_disconnected:(*ppiflist)[i].state = WIFSTAT_DISCONNECTED; break;
			case wlan_interface_state_associating:(*ppiflist)[i].state = WIFSTAT_ASSOCIATING; break;
			case wlan_interface_state_discovering:(*ppiflist)[i].state = WIFSTAT_DISCOVERING; break;
			case wlan_interface_state_authenticating:(*ppiflist)[i].state = WIFSTAT_AUTHENTICATING; break;
			default:
				break;
		}
	}
	WlanFreeMemory(iflist);
	return NERR_OK;
}

LIBNET_SYMBOL_EXPORT int libwl_scan_ifdetail(void *handle, void *ifid)
{
	return NERR_OK;
}

LIBNET_SYMBOL_EXPORT int libwl_scan_ap()
{
	return 0;
}

