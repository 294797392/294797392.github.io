#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>

#include "libnet.h"


void libnet_init()
{
	/* 设置当前环境的区域信息（编码方式等），Windows默认使用utf16编码 */
	setlocale(LC_ALL, "");
}