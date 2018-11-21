#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"

#ifdef LINUX
#define API_EXPORT
#define MFG_ACK_FILE_PATH "/tmp/mfg_ack"
#else
#define API_EXPORT __declspec(dllexport)
#define MFG_ACK_FILE_PATH "mfg_ack"
#endif
#define MFG_COMMAND_ACK_PREFIX "mfgack"

#define XIEHAOWANCHENG 0
#define XIEHAOSHIBAI 1
#define KAISHIXIEHAO 2

typedef struct efuse_data
{
	char address[64];
	char data[1024];
} efuse_data_t;

typedef struct wifi_cal_value
{
	int wlan_802dot11b_11mbps[1024];
	int wlan_802dot11g_54mbps[1024];
	int wlan_802dot11n_mcs7_b20[1024];
	int wlan_802dot11n_mcs7_b40[1024];
}wifi_cal_value_t;

typedef struct bt_cal_value
{
	int ble_gfsk_dm1[1024];
	int ble_gfsk_dm3[1024];
	int ble_gfsk_dm5[1024];

	int bdr_gfsk_1mbps_dh1[1024];
	int bdr_gfsk_1mbps_dh3[1024];
	int bdr_gfsk_1mbps_dh5[1024];

	int edr_dqpsk_2mbps_dh1[1024];
	int edr_dqpsk_2mbps_dh3[1024];
	int edr_dqpsk_2mbps_dh5[1024];

	int edr_8dpsk_3mbps_dh1[1024];
	int edr_8dpsk_3mbps_dh3[1024];
	int edr_8dpsk_3mbps_dh5[1024];

}bt_cal_value_t;

typedef struct ipc_fields_s
{
	char sn[128];
	char mac[128];
	char mac_nocolon[128];
	char did[128];
	char key[128];
	char uid[128]; /* p2pid */
	char fid[64];
	int has_uid;
} ipc_fields;

typedef struct wifi_chip_iface wifi_chip_iface_t;
typedef struct bt_chip_iface bt_chip_iface_t;
typedef struct device_iface device_iface_t;
typedef struct context context_t;


struct device_iface
{
	const char name[128];
	const char *supported[10];
	void*(*create_instance)(int arc, char *argv[]);
	int(*play_sound)(context_t *context, play_sound ps);
	int(*write)(context_t *context, device_write dw);
	int(*check)(context_t *context, device_check dc);
	int(*info)(context_t *context, device_info *di);
	int(*read)(context_t *context, device_read_in dri, device_read_out *dro);
	int(*audio_record_start)(context_t *context, record_audio_in rai);
	int(*audio_record_stop)(context_t *context, record_audio_in rai);
	int(*audio_record_get)(context_t *context, record_audio_in rai, record_audio_out *rao);
};

/* 芯片接口 */
struct wifi_chip_iface
{
	/* 接口名称 */
	const char name[128];

	/* 作者 */
	const char author[128];

	/* 支持的芯片名称列表 */
	const char* supported[10];
	void*(*create_instance)(context_t *context, int argc, char *argv[]);
	int(*init)(context_t *context);
	int(*release)(context_t *context);
	int(*tx_init)(context_t *context, wifi_tx_init wti);
	int(*tx_release)(context_t *context, wifi_tx_release wtr);
	int(*tx_start)(context_t *context, wifi_tx_start wts);
	int(*tx_stop)(context_t *context, wifi_tx_stop wts);
	int(*rx_init)(context_t *context, wifi_rx_init wri);
	int(*rx_release)(context_t *context, wifi_rx_release wrr);
	int(*rx_prepare)(context_t *context, wifi_rx_prepare wrp);
	int(*rx_clear)(context_t *context, wifi_rx_clear wrc);
	int(*rx_counters)(context_t *context, wifi_rx_counters_in wrci, wifi_rx_counters_out *wrco);
	int(*write_rom)(context_t *context, write_rom wr);
	int(*read_rom)(context_t *context, read_rom_in rri, read_rom_out *rro);
	int(*write_mac)(context_t *context, write_mac wm);
	int(*read_mac)(context_t *context, read_mac_in rmi, read_mac_out *rmo);

	/* 把当前校准的数据写入芯片 */
	int(*cal_write)(context_t *context, int freq_err, wifi_cal_value_t *wcvs); /* 写功率校准值，频偏校准值，温补值 */

	/* 根据rf_param读取缓存在内存里的校准值 */
	int(*cal_read_from_mem)(context_t *context, wifi_cal_read_in wcri, wifi_cal_read_out *wcro);

	/* 根据wifi_cal_set设置内存里的校准值 */
	int(*cal_set)(context_t *context, wifi_cal_value_t *wcvs, wifi_cal_set wcs);

	/* 校准完成后调用 */
	int(*cal_end)(context_t *context, wifi_cal_value_t *wcvs);

	/* 计算下一次校准值 */
	int(*find_next_calvalue)(context_t *context, find_next_calvalue_in fnci, find_next_calvalue_out *fnco);
};

struct bt_chip_iface
{
	/* 接口名称 */
	const char name[128];

	/* 作者 */
	const char author[128];

	/* 支持的芯片名称列表 */
	const char* supported[10];
	void*(*create_instance)(context_t *context, int argc, char *argv[]);
	int(*init)(context_t *context);
	int(*release)(context_t *context);
	int(*tx_init)(context_t *context, bt_tx_init bti);
	int(*tx_release)(context_t *context, bt_tx_release btr);
	int(*tx_start)(context_t *context, bt_tx_start bts);
	int(*tx_stop)(context_t *context, bt_tx_stop bts);
	int(*rx_init)(context_t *context, bt_rx_init bri);
	int(*rx_release)(context_t *context, bt_rx_release brr);
	int(*rx_prepare)(context_t *context, bt_rx_prepare brp);
	int(*rx_clear)(context_t *context, bt_rx_clear brc);
	int(*rx_counters)(context_t *context, bt_rx_counters_in brci, bt_rx_counters_out *brco);
	int(*write_rom)(context_t *context, write_rom wr);
	int(*read_rom)(context_t *context, read_rom_in rri, read_rom_out *rro);
	int(*write_mac)(context_t *context, write_mac wm);
	int(*read_mac)(context_t *context, read_mac_in rmi, read_mac_out *rmo);

	/* 把当前校准的数据写入芯片 */
	int(*cal_write)(context_t *context, int freq_err, bt_cal_value_t *bcvs); /* 写功率校准值，频偏校准值，温补值 */

	/* 根据rf_param读取缓存在内存里的校准值 */
	int(*cal_read_from_mem)(context_t *context, bt_cal_read_in wcri, bt_cal_read_out *wcro);

	/* 根据bt_cal_set设置校准值 */
	int(*cal_set)(context_t *context, bt_cal_value_t *bcvs, bt_cal_set bcs);

	/* 校准完成后调用 */
	int(*cal_end)(context_t *context, bt_cal_value_t *wcvs);

	/* 计算下一次校准值 */
	int(*find_next_calvalue)(context_t *context, find_next_calvalue_in fnci, find_next_calvalue_out *fnco);
};

struct chip_cal_info
{
	int standard;
	int cur_calvalue;
	int cur_power;
	int target_power;
};

struct context
{
	/* 上位机传递过来的参数 */
	char *args;

	/* 输出 */
	FILE *output;

	const device_iface_t *device_iface;
	const wifi_chip_iface_t *wifi_chip_iface; /* 对WIIF芯片的操作接口封装 */
	const bt_chip_iface_t *bt_chip_iface; /* 对蓝牙芯片的操作接口封装 */

	wireless_params wlp;	/* 射频参数 */

	void *wifi_priv;	/* wifi内部对象 */
	wifi_cal_value_t *wifi_cal_value; /* wifi校准值 */
	int wifi_frequencyoffset; /* 频偏校准值, WIFI和蓝牙共用, 十进制 */

	void *bt_priv; /* bt内部对象 */
	bt_cal_value_t *bt_cal_value; /* bt校准值 */
	int bt_frequencyoffset; /* 频偏校准值, WIFI和蓝牙共用, 十进制 */

	char ifname[128]; /* 要测试的网络接口名称 */
	char prefix[128]; /* 传导测试程序的前缀 */
};

#define WRITE_MFG_ACK(ack)	fprintf(context->output, "\n%s:%s\n", MFG_COMMAND_ACK_PREFIX, ack)
#define WRITE_MFG_ACK_INT(ack)	fprintf(context->output, "\n%s:%d\n", MFG_COMMAND_ACK_PREFIX, ack)

typedef enum
{
	WIFI_BAND_WIDTH_20M = 0,
	WIFI_BAND_WIDTH_40M = 1,
	/// <summary>
	/// 802.11ac only
	/// </summary>
	WIFI_BAND_WIDTH_80M = 2
} WIFIBandWidthEnum;

typedef enum
{
	// 802.11b
	WLAN_80211B_DSSS_DPSK_PN_1M = 0,
	WLAN_80211B_DSSS_DQPSK_PN_2M,
	WLAN_80211B_HRDSSS_DQPSK_CCK_5P5M,
	WLAN_80211B_HRDSSS_DQPSK_CCK_11M,

	// 802.11g
	WLAN_80211G_OFDM_BPSK_6M = 4,
	WLAN_80211G_OFDM_BPSK_9M,
	WLAN_80211G_OFDM_QPSK_12M,
	WLAN_80211G_OFDM_QPSK_18M,
	WLAN_80211G_OFDM_16QAM_24M,
	WLAN_80211G_OFDM_16QAM_36M,
	WLAN_80211G_OFDM_64QAM_48M,
	WLAN_80211G_OFDM_64QAM_54M,

	// 802.11n
	WLAN_80211N_MCS0 = 12,
	WLAN_80211N_MCS1,
	WLAN_80211N_MCS2,
	WLAN_80211N_MCS3,
	WLAN_80211N_MCS4,
	WLAN_80211N_MCS5,
	WLAN_80211N_MCS6,
	WLAN_80211N_MCS7,
	WLAN_80211N_MCS8,
	WLAN_80211N_MCS9,
	WLAN_80211N_MCS10,
	WLAN_80211N_MCS11,
	WLAN_80211N_MCS12,
	WLAN_80211N_MCS13,
	WLAN_80211N_MCS14,
	WLAN_80211N_MCS15

} WLANStandardEnum;

typedef enum
{
	WIFI_FREQUENCY_BAND_24G_HZ = 0,
	WIFI_FREQUENCY_BAND_4G_HZ,
	WIFI_FREQUENCY_BAND_5G_HZ
} WIFIFrequencyBandEnum;

typedef enum
{
	BT_BLE_GFSK_DM1 = 0,
	BT_BLE_GFSK_DM3,
	BT_BLE_GFSK_DM5,

	BT_BDR_GFSK_1Mbps_DH1 = 3,
	BT_BDR_GFSK_1Mbps_DH3,
	BT_BDR_GFSK_1Mbps_DH5,

	BT_EDR_DQPSK_2Mbps_DH1 = 6,
	BT_EDR_DQPSK_2Mbps_DH3,
	BT_EDR_DQPSK_2Mbps_DH5,

	BT_EDR_8DPSK_3Mbps_DH1 = 9,
	BT_EDR_8DPSK_3Mbps_DH3,
	BT_EDR_8DPSK_3Mbps_DH5,
} BTStandardEnum;

typedef enum
{
	BT_PAYLOAD_All0,
	BT_PAYLOAD_All1,
	BT_PAYLOAD_PN9,
	BT_PAYLOAD_AA,
	BT_PAYLOAD_F0,
	BT_PAYLOAD_PN15,
	BT_PAYLOAD_0x0F,
	BT_PAYLOAD_0x55
} BTPayloadPatternEnum;

#endif
