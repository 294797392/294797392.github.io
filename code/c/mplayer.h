#ifndef __MPLAYER_H__
#define __MPLAYER_H__

typedef enum tagMPLAYER_STATUS
{
    MSTATUS_IDLE = 0,
    MSTATUS_PLAYING,
    MSTATUS_PAUSED
}mplayer_status_enum;

#define MP_SUCCESS 0

typedef struct tagMPRIV mpriv_t;
typedef struct tagMPLAYER 
{
    mpriv_t *priv;
    mplayer_status_enum status;
    int progress;
    void(*open)(mplayer_t *mplayer, const char *source);
    void(*close)(mplayer_t *mplayer);
    int(*play)(mplayer_t *mplayer);
    int(*stop)(mplayer_t *mplayer);
    int(*pause)(mplayer_t *mplayer);
    int(*resume)(mplayer_t *mplayer);
} mplayer_t;

#endif