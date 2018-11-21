#ifndef __NETWORK_OPS_H__
#define __NETWORK_OPS_H__

#include "libnet.h"

typedef struct net_driver_ops_s
{
	const char name[256];
	const char description[256];

	/* 初始化ops */
	void*(*init)();

	/* 扫描系统里的所有网络接口 */
	int(*scan)(void *handle, interfase *iflist, size_t len);
}net_driver_ops;

#endif // !__NETWORK_DRIVER_H__
