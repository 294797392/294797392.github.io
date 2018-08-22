#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "mplayer.h"

void test_mplayer()
{
	const char *source = "http://file.suibiwu.com/typecho/2017/03/3021894958.mp3";

	mplayer_t *mp = mplayer_create_instance();
	mplayer_open(mp, source, strlen(source));
	mplayer_play(mp);
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