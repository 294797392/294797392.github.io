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
#define LOG_CFGF_PATH "../log.ini"
#else
#define CONFIG_PATH "/etc/wsddsvc/wsddsvc.config.json"
#define LOG_CFGF_PATH /etc/wsddsvc/log.ini
#endif

typedef struct service_info_s
{
	/* 服务名字 */
	char *name;

	/* 唯一标识符 */
	char endpoint[4096];

	/*
	 * 所支持的服务类型
	 * Target Service：直接写实现了的服务类型，onvif规定要实现tds:device类型的服务。如果为空没有默认值
	 * DiscoveryProxy：必须写d:DiscoveryProxy和d:TargetService
	 */
	char **supported_types;
	int supported_type_cnt;
	char types[4096];

	/*
	 * 服务范围信息
	 * 可以有多个值，如果为空，默认是http://schemas.xmlsoap.org/ws/2005/04/discovery/adhoc
	 * 不能有空格出现
	 */
	char **supported_scopes;
	int supported_scope_cnt;
	char scopes[4096];

	/* 可以直接访问的服务地址（http/https/udp/tcp） */
	char **addresses;
	int address_cnt;
	char xaddrs[4096];

	/* soap元数据版本信息 */
	int metadata_version;

}service_info;

typedef struct wsddsvc_config_s
{
	/*
	 * 发送probe多播消息使用的网络接口
	 * 监听客户端请求的网络接口
	*/
	char *ifname;

	/* 支持的所有服务类型 */
	char *supported_types;
	char *supported_scopes;
	char *xaddrs;

	/* 存储服务信息 */
	service_info *svclist;
	int svc_cnt;

}wsddsvc_config;

struct wsddsvc_s
{
	wsddsvc_config *config;

	/*
	 * 配置文件里的网络接口对应的IP地址
	 * 取第一个地址
	 */
	char ipaddr[64];
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

	/* 从配置文件里找对应的服务信息 */
	int matched = 0;
	service_info *service = NULL;
	for (int i = 0; i < svc->config->svc_cnt; i++)
	{
		service = &svc->config->svclist[i];
		/* 匹配Scope和Type */
		if (wsdd_match(service->supported_scopes, service->supported_scope_cnt, Scopes) == 1 &&
			wsdd_match(service->supported_types, service->supported_type_cnt, Types) == 1)
		{
			matched = 1;
			/* 如果服务信息的Endpoint是空的，则默认填写uuid */
			if (!service->endpoint || strlen(service->endpoint) == 0)
			{
				/* 在配置文件里没指定EndpointReference，自动生成 */
				const char *uuid = soap_wsa_rand_uuid(soap);
				strncpy(svc->config->svclist[i].endpoint, uuid, strlen(uuid));
			}

			if (!service->xaddrs || strlen(service->xaddrs) == 0)
			{
				wsdd_concat_xaddrs(svc->config->svclist[i].xaddrs, service->addresses, service->address_cnt, svc->ipaddr);
			}
			break;
		}
	}

	/* 没找到服务信息，什么都不做 */
	if (matched == 0)
	{
		log_error(MODULE, "no matched service, scopes:%s, types:%s", Scopes, Types);
		return SOAP_WSDD_ADHOC;
	}

	log_info(MODULE, "find service, name:%s, endpoint:%s, address:%s", service->name, service->endpoint, service->xaddrs);

	/* 初始化结构体 */
	soap_wsdd_init_ProbeMatches(soap, matches);
	const char *endpoint_reference = service->endpoint;
	const char *matchBy = NULL;  /* scope的匹配规则 */
	char *xaddr = service->xaddrs;
	int metadata_version = service->metadata_version;
	const char *types = service->types;
	const char *scopes = service->scopes;

	/* 填充结构体 */
	if ((ret = soap_wsdd_add_ProbeMatch(soap, matches, endpoint_reference, types, scopes, MatchBy, xaddr, metadata_version)) != SOAP_OK)
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




static void parse_service_info(cJSON* json, struct service_info_s *svcinfo)
{
	json_deserialize_ptr_array(json, supported_types, svcinfo, supported_types, supported_type_cnt, string);
	json_deserialize_ptr_array(json, supported_scopes, svcinfo, supported_scopes, supported_scope_cnt, string);
	json_deserialize_ptr_array(json, addresses, svcinfo, addresses, address_cnt, string);
	json_deserialize(json, metadata_version, svcinfo, metadata_version, int);
	json_deserialize(json, name, svcinfo, name, string);
	wsdd_concat_types(svcinfo->types, svcinfo->supported_types, svcinfo->supported_type_cnt);
	wsdd_concat_types(svcinfo->scopes, svcinfo->supported_scopes, svcinfo->supported_scope_cnt);
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
	json_deserialize(json, ifname, config, ifname, string);
	json_deserialize(json, supported_types, config, supported_types, string);
	json_deserialize(json, supported_scopes, config, supported_scopes, string);
	json_deserialize(json, xaddrs, config, xaddrs, string);
	json_deserialize_ptr_struct_array(json, services, config, svclist, svc_cnt, service_info, parse_service_info);
	cJSON_Delete(json);

	return config;
}

static void getip(char *ipbuf, const char *ifname)
{
#ifdef WINDOWS
	strncpy(ipbuf, LOCAL_IPADDR, IPADDR_LEN);
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
	log_init(LOG_CFGF_PATH);

	wsddsvc *svc = (wsddsvc*)calloc(1, sizeof(wsddsvc));
	if (!svc)
	{
		log_error(MODULE, "malloc svc instance failed\n");
		goto error;
	}

	if (!(svc->config = parse_wsddsvc_config(CONFIG_PATH)))
	{
		log_error(MODULE, "parse wsddsvc config failed");
		goto error;
	}

	getip(svc->ipaddr, svc->config->ifname);

	struct soap *soap = init_soap_udp(svc);

	/* 发送Hello消息 */
	const char *messageID = soap_wsa_rand_uuid(soap);
	const char *relatesTo = NULL;
	const char *endpoint_reference = soap_wsa_rand_uuid(soap);
	const char *matchBy = NULL;  /* scope的匹配规则 */
	const char *scopes = svc->config->supported_scopes;
	const char *types = svc->config->supported_types;
	char xaddr[1024] = { '\0' };
	snprintf(xaddr, sizeof(xaddr), svc->config->xaddrs, svc->ipaddr);

	int ret = soap_wsdd_Hello(soap, SOAP_WSDD_ADHOC, WSDD_SOAP_TS_ADDRESS, messageID, relatesTo, endpoint_reference, types, scopes, matchBy, xaddr, WSDD_METADATA_VERSION);
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