#ifndef __WSDDSVC_H__
#define __WSDDSVC_H__

typedef struct wsddsvc_s wsddsvc;

typedef struct wsddsvc_config_s
{
	/* 发送probe多播消息使用的网络接口 */
	char *iface;

	/* 服务类型 */
	char **types;
	int type_cnt;

	/* 服务范围信息 */
	char **scopes;
	int scope_cnt;

	/* 服务地址 */
	char **addresses;
	int address_cnt;

}wsddsvc_config;

#endif