#ifndef __MPLAYER_H__
#define __MPLAYER_H__

/*
 * ��������£����������������¼����˳���ʽ��
 * 1.�������Ž������Զ��˳�
 *		�Ƿ�֪ͨ����
 * 2.�ֶ�����stop�˳�
 *		�Ƿ�֪ͨ����
 * 3.����̷���stop�����˳�
 *		�Ƿ�֪ͨ����
 */

#ifdef WINDOWS
#define HAVE_STRUCT_TIMESPEC		/* �������pthreadͷ�ļ����벻�������⣨��timespec�� : ��struct�������ض��壩 */
#pragma comment(lib, "pthreadVC2.lib") /* ����pthread�� */
#endif

#include <pthread.h>

#ifdef WINDOWS
#define MPLAYER_PATH "D:/MPlayer/mplayer/mplayer.exe"
#else
#define MPLAYER_PATH "mplayer"
#endif
#define DEFAULT_SOURCE_SIZE 1024
#define MP_SAFE_FREE(FUNC, PTR) if(PTR) { FUNC(*PTR); *PTR = NULL; }

/* ������س��� */
#define VOLUME_DEFAULT 70
#define VOLUME_MAX 100
#define VOLUME_MIN 0
#define VOLUME_DELTA 10

/* ����ֵ */
#define MP_SUCCESS 0
#define MP_CREATE_PROCESS_FAILED 1
#define MP_SEND_COMMAND_FAILED 2

/* �������¼����Ͷ��� */
typedef enum tagMPEVENT
{
	MPEVT_STATUS_CHANGED
}mplayer_event_enum;
typedef int(*mp_event_listener)(mplayer_event_enum evt, void *data);

/* ������״̬ */
typedef enum tagMPSTATUS
{
    MPSTAT_PLAYING,
    MPSTAT_PAUSED,
	MPSTAT_STOPPED
}mplayer_status_enum;

/* ��ͬƽ̨�µĲ������ڲ�����ָ�� */
typedef struct tagMPLAYER_PRIV mplayer_priv_t;

typedef struct tagMPLAYER mplayer_t;
typedef struct tagMPLAYER_OPS mplayer_ops_t;

/* ������ʵ�� */
struct tagMPLAYER{
	char source[DEFAULT_SOURCE_SIZE];
	int volume;
	mp_event_listener event_listener;
	pthread_t monitor_thread;
	mplayer_priv_t *priv;
	mplayer_ops_t *ops;
};

/* ��װ��ͬƽ̨�£��Բ�������ִ�е���ͬ�Ĳ��� */
struct tagMPLAYER_OPS
{
	/* ��mplayer���Ž��̲���ʼ���� */
	int(*mpops_open_player_process)(mplayer_t *mp);

	/* �ȴ�mplayer���Ž��̽��� */
	int(*mpops_wait_process_exit)(mplayer_t *mp);

	/* �رս��� */
	void(*mpops_close_player_process)(mplayer_t *mp);

	/* �ͷ�mplayer������ռ�õ���Դ */
	void(*mpops_release_process_resource)(mplayer_t *mp);

	/* ��mplayer���Ž��̷�����Ϣ */
	int(*mpops_send_command)(mplayer_t *mp, const char *cmd, int size);
};

mplayer_t* mplayer_create_instance();
void mplayer_open(mplayer_t *mp, const char *source, int source_size);
void mplayer_close(mplayer_t *mp);
int mplayer_play(mplayer_t *mp);
void mplayer_stop(mplayer_t *mp);
int mplayer_pause(mplayer_t *mp);
int mplayer_resume(mplayer_t *mp);
void mplayer_increase_volume(mplayer_t *mp);
void mplayer_decrease_volume(mplayer_t *mp);
void mplayer_listen_event(mplayer_t *mp, mp_event_listener listener);

#endif