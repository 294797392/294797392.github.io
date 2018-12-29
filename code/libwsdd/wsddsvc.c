#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* libcommonslim */
#include <defs.h>
#include <cJSON.h>
#include <cJSON_Utils.h>
#include <json_helper.h>
#include <misc.h>
#include <elog.h>

/* gSOAP ws-discovery,ws-addressing plugin */
#include <wsddapi.h>
#include <wsaapi.h>

/* libwsdd */
#include "wsdd.h"
#include "stdsoap2.h"
#include "wsddsvc.h"
#include "gSOAP_generated/wsdd.nsmap"

#define MODULE "wsddsvc"

#ifdef WIN32
#define CONFIG_PATH "wsddsvc.config.json"
#else
#define CONFIG_PATH "/etc/wsddsvc/wsddsvc.config.json"
#endif

typedef struct wsddsvc_config_s
{
	/*
	 * 发送probe多播消息使用的网络接口
	 * 监听客户端请求的网络接口
	*/
	char *iface;

	/*
	 * 所支持的服务类型
	 * Target Service：直接写实现了的服务类型，onvif规定要实现tds:device类型的服务。如果为空没有默认值
	 * DiscoveryProxy：必须写d:DiscoveryProxy和d:TargetService
	 */
	char **supported_types;
	int supported_type_cnt;

	/*
	 * 服务范围信息
	 * 可以有多个值，如果为空，默认是http://schemas.xmlsoap.org/ws/2005/04/discovery/adhoc
	 * 不能有空格出现
	 */
	char **supported_scopes;
	int supported_scope_cnt;

	/* 可以直接访问的服务地址（http/https/udp/tcp） */
	char **addresses;
	int address_cnt;

	/* soap元数据版本信息 */
	int metadata_version;

}wsddsvc_config;

typedef struct endpoint_reference_s
{
	/*
		服务的逻辑地址或者网络地址，必须是唯一的
		如果是一个逻辑地址，那么网络地址存放在XAddrs元素里

		参考：
			https://www.w3.org/Submission/ws-addressing/#Toc77464317 2. Endpoint References
			ws-discovery.pdf 2.6 Endpoint References
	*/
	const char *address;

}endpoint_reference;

struct wsddsvc_s
{
	endpoint_reference *epref;

	wsddsvc_config *config;

	char ipaddrs[32][IPADDR_LEN];

	/* 与服务通信的真正地址 */
	char xaddress[1024];

	char types[4096];
	char scopes[4096];
};



void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{

}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{

}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{

}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{

}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
	return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	wsddsvc *svc = (wsddsvc*)soap->user;
	int ret = SOAP_OK;

	/* 匹配Scope和Type */
	if (wsdd_match(svc->config->supported_scopes, svc->config->supported_scope_cnt, Scopes) == 0 ||
		wsdd_match(svc->config->supported_types, svc->config->supported_type_cnt, Types) == 0)
	{
		return SOAP_WSDD_ADHOC;
	}

	/* 初始化结构体 */
	soap_wsdd_init_ProbeMatches(soap, matches);
	const char *endpoint_reference = soap_wsa_rand_uuid(soap);
	const char *matchBy = NULL;  /* scope的匹配规则 */
	char *xaddr = svc->xaddress;
	int metadata_version = svc->config->metadata_version;

	/* 填充结构体 */
	if ((ret = soap_wsdd_add_ProbeMatch(soap, matches, endpoint_reference, Types, svc->scopes, MatchBy, xaddr, metadata_version)) != SOAP_OK)
	{
		log_error(MODULE, "soap_wsdd_add_ProbeMatch failed, %d", ret);
		return SOAP_WSDD_ADHOC;
	}

	/* 发送ProbeMatch */
	const char *messageID = soap_wsa_rand_uuid(soap);
	const char *relatesTo = MessageID;
	if ((ret = soap_wsdd_ProbeMatches(soap, "http://", messageID, relatesTo, NULL, matches)) != SOAP_OK)
	{
		log_error(MODULE, "soap_wsdd_ProbeMatches failed, %s", ret);
		return SOAP_WSDD_ADHOC;
	}

	return SOAP_WSDD_ADHOC;
}










static wsddsvc_config* parse_wsddsvc_config(const char *path)
{
	wsddsvc_config *config = (wsddsvc_config*)calloc(1, sizeof(wsddsvc_config));
	if (!config)
	{
		log_error(MODULE, "malloc wsddsvc_config failed\n");
		return NULL;
	}

	cJSON* json = NULL;
	int size = 0;
	// parse json config file
	char* buffer = imi_read_all_text_new(path, &size);
	if (buffer == NULL) {
		log_warn(MODULE, "read config (%s) failed", path);
		return NULL;
	}

	json = cJSON_Parse(buffer);
	imi_free(buffer);
	if (json == NULL) {
		log_warn(MODULE, "parse config (%s) failed", path);
		return NULL;
	}

	// deserialize config
	json_deserialize(json, iface, config, iface, string);
	json_deserialize_ptr_array(json, supported_types, config, supported_types, supported_type_cnt, string);
	json_deserialize_ptr_array(json, supported_scopes, config, supported_scopes, supported_scope_cnt, string);
	json_deserialize_ptr_array(json, addresses, config, addresses, address_cnt, string);
	json_deserialize(json, metadata_version, config, metadata_version, int);
	cJSON_Delete(json);

	return config;
}

static void retrive_ipaddr(wsddsvc *svc, const char *ifname)
{
#ifdef WINDOWS
	strncpy(svc->ipaddrs[0], LOCAL_IPADDR, IPADDR_LEN);
#else

#endif
}

static struct soap *init_soap_udp(void *user_data)
{
	struct soap *soap = NULL;
	// init gsoap server for WS-Discovery service
	if (!(soap = soap_new1(SOAP_IO_UDP)))
	{
		log_error(MODULE, "create soap instance failed");
		return NULL;
	}

	soap_register_plugin(soap, soap_wsa);

	soap->connect_flags = SO_BROADCAST;	// for UDP multicast
	soap->bind_flags = SO_REUSEADDR;
	soap->ipv4_multicast_ttl = 1; /* soap-over-udp.pdf 4.3 Multicast */

	if (!soap_valid_socket(soap_bind(soap, NULL, WSDD_SERVER_PORT, 100)))
	{
		soap_print_fault(soap, stderr);
		return NULL;
	}

	struct ip_mreq mcast;
	mcast.imr_multiaddr.s_addr = inet_addr(WSDD_IPV4_MULTICAST_IP);
	mcast.imr_interface.s_addr = inet_addr(LOCAL_IPADDR);

	int ttl = 1;
	setsockopt(soap->master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast));

	soap->user = user_data; // set our data for handlers

	return soap;
}

wsddsvc* start_wsddsvc()
{
	wsddsvc *svc = (wsddsvc*)calloc(1, sizeof(wsddsvc));
	if (!svc)
	{
		log_error(MODULE, "malloc svc instance failed\n");
		goto error;
	}
	svc->epref = (endpoint_reference*)calloc(1, sizeof(endpoint_reference));
	
	if (!(svc->config = parse_wsddsvc_config(CONFIG_PATH)))
	{
		log_error(MODULE, "parse wsddsvc config failed");
		goto error;
	}

	wsdd_fill_buffer(svc->types, svc->config->supported_types, svc->config->supported_type_cnt);
	wsdd_fill_buffer(svc->scopes, svc->config->supported_scopes, svc->config->supported_scope_cnt);
	retrive_ipaddr(svc, svc->config->iface);

	snprintf(svc->xaddress, sizeof(svc->xaddress), svc->config->addresses[0], svc->ipaddrs[0]);

	struct soap *soap = init_soap_udp(svc);

	/* 发送Hello消息 */
	const char *messageID = soap_wsa_rand_uuid(soap);
	const char *relatesTo = NULL;
	const char *ep = soap_wsa_rand_uuid(soap);
	const char *endpoint_reference = soap_wsa_rand_uuid(soap);
	char *types = svc->config->supported_types[0];
	char *scopes = svc->config->supported_scopes[0];
	const char *matchBy = NULL;  /* scope的匹配规则 */
	char *xaddr = svc->xaddress;
	svc->epref->address = ep;
	int metadata_version = svc->config->metadata_version;

	int ret = soap_wsdd_Hello(soap, SOAP_WSDD_ADHOC, WSDD_SOAP_TS_ADDRESS, messageID, relatesTo, endpoint_reference, types, scopes, matchBy, xaddr, metadata_version);
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