#ifndef __MPLAYER_H__
#define __MPLAYER_H__

/*
 * 正常情况下，播放器进程有以下几种退出方式：
 * 1.歌曲播放结束，自动退出
 *		是否通知：是
 * 2.手动调用stop退出
 *		是否通知：否
 * 3.向进程发送stop命令退出
 *		是否通知：否
 */

#ifdef WINDOWS
#define HAVE_STRUCT_TIMESPEC		/* 解决引用pthread头文件编译不过的问题（“timespec” : “struct”类型重定义） */
#pragma comment(lib, "pthreadVC2.lib") /* 连接pthread库 */
#endif

#include <pthread.h>

#ifdef WINDOWS
#define MPLAYER_PATH "D:/MPlayer/mplayer/mplayer.exe"
#define SLEEP(dwMilliseconds) Sleep(dwMilliseconds)
#else
#define MPLAYER_PATH "mplayer"
typedef int BOOL;
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define SLEEP(dwMilliseconds) sleep(dwMilliseconds / 1000)
#endif
#define DEFAULT_SOURCE_SIZE 1024
#define MP_SAFE_FREE(FUNC, PTR) if(PTR) { FUNC(*PTR); *PTR = NULL; }

/* 音量相关常量 */
#define VOLUME_DEFAULT 70
#define VOLUME_MAX 100
#define VOLUME_MIN 0
#define VOLUME_DELTA 10

/* 返回值 */
#define MP_FAILED -1
#define MP_SUCCESS 0
#define MP_CREATE_PROCESS_FAILED 1
#define MP_SEND_COMMAND_FAILED 2
#define MP_READ_DATA_FAILED 3

/* 播放器事件类型定义 */
typedef enum tagMPEVENT
{
	MPEVT_STATUS_CHANGED
}mplayer_event_enum;

/* 播放器状态 */
typedef enum tagMPSTATUS
{
    MPSTAT_PLAYING,
    MPSTAT_PAUSED,
	MPSTAT_STOPPED
}mplayer_status_enum;

/* mplayer事件回调 */
typedef int(*mp_event_handler)(mplayer_event_enum evt, void *evt_data, void *userdata);

/* 事件监听器 */
typedef struct tagMPLISTENER
{
	mp_event_handler handler;
	void *userdata;
} mplayer_listener_t;

/* 不同平台下的播放器内部对象指针 */
typedef struct tagMPLAYER_PRIV mplayer_priv_t;

/* 播放器句柄 */
typedef struct tagMPLAYER mplayer_t;

/* 不同平台下相同的播放器操作 */
typedef struct tagMPLAYER_OPS mplayer_ops_t;

/* 播放器实例 */
struct tagMPLAYER{
	char source[DEFAULT_SOURCE_SIZE];
	int volume;
	mplayer_status_enum status;
	mplayer_listener_t *listener;
	pthread_t monitor_thread;
	mplayer_priv_t *priv;
	mplayer_ops_t *ops;
};


/* 封装不同平台下，对播放器所执行的相同的操作 */
struct tagMPLAYER_OPS
{
	/* 打开mplayer播放进程并开始播放 */
	int(*mpops_open_player_process)(mplayer_t *mp);

	/* 等待mplayer播放进程结束 */
	int(*mpops_wait_process_exit)(mplayer_t *mp);

	/* 关闭进程 */
	void(*mpops_close_player_process)(mplayer_t *mp);

	/* 判断mplayer进程是否已经退出 */
	int(*mpops_process_is_exit)(mplayer_t *mp);

	/* 释放mplayer进程所占用的资源 */
	void(*mpops_release_process_resource)(mplayer_t *mp);

	/* 向mplayer播放进程发送消息 */
	int(*mpops_send_command)(mplayer_t *mp, const char *cmd, int size);

	/* 从mplayer进程读取数据 */
	int(*mpops_read_data)(mplayer_t *mp, char *buff, int size);
};

mplayer_t* mplayer_create_instance();
void mplayer_free_instance(mplayer_t *mp);
void mplayer_open(mplayer_t *mp, const char *source, int source_size);
void mplayer_close(mplayer_t *mp);
int mplayer_play(mplayer_t *mp);
void mplayer_stop(mplayer_t *mp);
int mplayer_pause(mplayer_t *mp);
int mplayer_resume(mplayer_t *mp);
void mplayer_increase_volume(mplayer_t *mp);
void mplayer_decrease_volume(mplayer_t *mp);
/* 返回总时长，以秒为单位 */
int mplayer_get_duration(mplayer_t *mp);
/* 返回当前播放进度，以秒为单位 */
int mplayer_get_position(mplayer_t *mp);
void mplayer_listen_event(mplayer_t *mp, mplayer_listener_t listener);
mplayer_status_enum mplayer_get_status(mplayer_t *mp);

#endif
