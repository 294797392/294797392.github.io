#ifndef __LIBWL_H__
#define __LIBWL_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libnet.h"

typedef enum
{
	WIFSTAT_CONNECTED = 0,
	WIFSTAT_DISCONNECTING,
	WIFSTAT_DISCONNECTED,
	WIFSTAT_ASSOCIATING,
	WIFSTAT_DISCOVERING,
	WIFSTAT_AUTHENTICATING
}wlifstate;

typedef struct wliface_s
{
	char id[MAX_IF_ID_SIZE];
	char name[MAX_IF_NAME_SIZE];
	char description[MAX_IF_DESC_SIZE];
	wlifstate state;
}wliface;

typedef enum
{
	WEVT_AP_SCANNED
}wl_event;

typedef void(*libwl_event_callback)(void *handle, wl_event evt_type, void *data1, void *data2, size_t d1size, size_t d2size);

LIBNET_SYMBOL_EXPORT int libwl_init(void **handle);
LIBNET_SYMBOL_EXPORT int libwl_callback(void *handle, libwl_event_callback callback);
LIBNET_SYMBOL_EXPORT int libwl_scan_iflist(void *handle, wliface **ppiflist, int *piflen);
LIBNET_SYMBOL_EXPORT int libwl_scan_ifdetail(void *handle);
LIBNET_SYMBOL_EXPORT int libwl_scan_ap();

#endif