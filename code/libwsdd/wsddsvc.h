#ifndef __WSDDSVC_H__
#define __WSDDSVC_H__

typedef struct wsddsvc_s wsddsvc;

typedef struct wsddsvc_config_s
{
	/* ����probe�ಥ��Ϣʹ�õ�����ӿ� */
	char *iface;

	/* �������� */
	char **types;
	int type_cnt;

	/* ����Χ��Ϣ */
	char **scopes;
	int scope_cnt;

	/* �����ַ */
	char **addresses;
	int address_cnt;

}wsddsvc_config;

#endif