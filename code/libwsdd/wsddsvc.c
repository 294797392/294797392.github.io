#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stdsoap2.h"
#include "wsddapi.h"

#include "wsdd.h"
#include "wsddsvc.h"
#include "wsaapi.h"
#include "gSOAP_generated/wsdd.nsmap"

struct wsddsvc_s
{
	/* 
	 * 服务的唯一标识符，形如uuid:98190dc2-0890-4ef8-ac9a-5940995e6119的常量
	 */
	char identifier[128];
	char type[256];

	/**/
	char scope[256];

	/*
	 * 可以访问到服务的地址
	 * onvif使用http://{ipaddr}:{port}/onvif/device_service固定的地址格式
	 */
	char xaddr[256];
};

wsddsvc_config* parse_wsddsvc_config(const char *path)
{
	wsddsvc_config *config = (wsddsvc_config*)calloc(1, sizeof(wsddsvc_config));
	if (!config)
	{
		printf("malloc wsddsvc_config failed\n");
		return NULL;
	}

	return config;
}

wsddsvc* start_wsddsvc()
{
	wsddsvc *svc = (wsddsvc*)calloc(1, sizeof(wsddsvc));
	if (!svc)
	{
		printf("malloc svc instance failed\n");
		goto error;
	}

	struct soap *soap = NULL;
	// init gsoap server for WS-Discovery service
	if (!(soap = soap_new1(SOAP_IO_UDP)))
	{
		printf("create soap instance failed");
		goto error;
	}

	soap->connect_flags = SO_BROADCAST;	// for UDP multicast
	soap->bind_flags = SO_REUSEADDR;
	soap->ipv4_multicast_ttl = 1;

	if (!soap_valid_socket(soap_bind(soap, NULL, WSDD_SERVER_PORT, 100)))
	{
		soap_print_fault(soap, stderr);
		goto error;
	}

	soap->user = svc;	// set our data for handlers

	struct ip_mreq mcast;
	mcast.imr_multiaddr.s_addr = inet_addr(WSDD_IPV4_MULTICAST_IP);
	mcast.imr_interface.s_addr = inet_addr(LOCAL_IPADDR);

	int ttl = 1;
	setsockopt(soap->master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast));

	/* 发送Hello消息 */
	const char *ep = soap_wsa_rand_uuid(soap);
	strncpy(svc->identifier, ep, strlen(ep));
	int ret = soap_wsdd_Hello(soap, SOAP_WSDD_ADHOC, WSDD_SOAP_TS_ADDRESS, soap_wsa_rand_uuid(soap), NULL, svc->identifier, svc->type, svc->scope, NULL, svc->xaddr, WSDD_METADATA_VERSION);
	if (ret == SOAP_OK)
	{
		soap_wsdd_listen(soap, 1);
	}
	else
	{
		soap_print_fault(soap, stderr);
	}

	while (1)
	{
		if (soap_wsdd_listen(soap, 20))
		{
			soap_print_fault(soap, stderr); // report the problem
		}
	}

	return svc;

error:
	return NULL;
}