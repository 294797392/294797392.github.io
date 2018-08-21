#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <Windows.h>
#else
#endif

#include "mplayer.h"

int main(int argc, char *argv[])
{
	const char *source = "http://file.suibiwu.com/typecho/2017/03/3021894958.mp3";

	mplayer_t *mp = mplayer_create_instance();
	mplayer_open(mp, source, strlen(source));
	mplayer_play(mp);

	char c;
	scanf(&c);

	return 0;
}