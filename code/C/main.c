#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "mplayer.h"

int event_handler(mplayer_event_enum evt, void *evt_data, void *userdata)
{
	fprintf(stdout, "�յ�evt:%d\n", evt);
	return 0;
}

void test_mplayer()
{
	const char *source = "http://file.suibiwu.com/typecho/2017/03/3787831629.mp3";

	mplayer_t *mp = mplayer_create_instance();
	mplayer_open(mp, source, strlen(source));
	mplayer_play(mp);

	mplayer_listener_t listener;
	listener.userdata = mp;
	listener.handler = event_handler;
	mplayer_listen_event(mp, listener);
	SLEEP(2000);
	int duration = mplayer_get_duration(mp);
	fprintf(stdout, "duration:%d\n", duration);
	while (1)
	{
		int position = mplayer_get_position(mp);
		fprintf(stdout, "pos:%d\n", position);
		SLEEP(1000);
	}
}

int main(int argc, char *argv[])
{
	test_mplayer();
	return 0;
}
