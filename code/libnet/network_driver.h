#ifndef __NETWORK_DRIVER_H__
#define __NETWORK_DRIVER_H__

typedef struct network_driver_s
{
	const char name[256];
	const char description[256];

	void*(*init)(const char *ifname);

}network_driver;

#endif // !__NETWORK_DRIVER_H__
