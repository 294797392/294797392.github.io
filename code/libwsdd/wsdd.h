#ifndef __WSDD_H__
#define __WSDD_H__

/* base header file */

/* ws-discovery标准使用的IPV4的多播地址 */
#define WSDD_IPV4_MULTICAST_IP "239.255.255.250"
/* ws-discovery标准使用的IPV6的多播地址 */
#define WSDD_IPV6_MULTICAST_IP "FF02::C"
/* ws-discovery标准使用的多播端口 */
#define WSDD_SERVER_PORT 3702
/* libwsdd使用的SOAP消息结构的版本信息，每个版本的格式有可能是不同的 */
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
