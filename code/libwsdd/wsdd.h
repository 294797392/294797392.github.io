#ifndef __WSDD_H__
#define __WSDD_H__

/* base header file */

/* ws-discovery��׼ʹ�õ�IPV4�Ķಥ��ַ */
#define WSDD_IPV4_MULTICAST_IP "239.255.255.250"
/* ws-discovery��׼ʹ�õ�IPV6�Ķಥ��ַ */
#define WSDD_IPV6_MULTICAST_IP "FF02::C"
/* ws-discovery��׼ʹ�õĶಥ�˿� */
#define WSDD_SERVER_PORT 3702
/* libwsddʹ�õ�SOAP��Ϣ�ṹ�İ汾��Ϣ��ÿ���汾�ĸ�ʽ�п����ǲ�ͬ�� */
#define WSDD_METADATA_VERSION 75965

/* As designated below, before sending some message types defined herein, a Target 
Service MUST wait for a timer to elapse before sending the message. This timer 
MUST be set to a random value between 0 and APP_MAX_DE */
#define APP_MAX_DELAY 500

#define WSDD_SOAP_TS_ADDRESS "soap.udp://239.255.255.250:3702"

#define LOCAL_IPADDR "192.168.1.190"
#define IPADDR_LEN 32

int uricmp(const char *uri1, const char *uri2);

int wsdd_match(char **scopes, int scope_cnt, const char *target_scope);
void wsdd_concat_types(char *buff, char **datas, int len);
void wsdd_concat_xaddrs(char *buff, char **addrs, int len, char *ip);

#endif
