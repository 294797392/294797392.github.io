#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>

#include "libnet.h"


void libnet_init()
{
	/* ���õ�ǰ������������Ϣ�����뷽ʽ�ȣ���WindowsĬ��ʹ��utf16���� */
	setlocale(LC_ALL, "");
}