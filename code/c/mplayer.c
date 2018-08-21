#define WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#else
#endif

#include "mplayer.h"

#ifdef WINDOWS
struct tagMPRIV
{

};
#else
#endif
