#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tk_string.h"

int string_splite(char *str, char splitter, char *lvalue, size_t lsize, char *rvalue, size_t rsize)
{
	int lvalue_start = 0;
	int lvalue_len = 0;
	int rvalue_start = 0;
	int rvalue_len = 0;
	int find = 0;

	size_t str_len = strlen(str);
	for (size_t i = 0; i < str_len; i++)
	{
		if (str[i] == splitter)
		{
			lvalue_len = i;
			rvalue_start = i + 1;
			rvalue_len = str_len - i - 1;
			find = 1;
			break;
		}
	}

	if (find == 0) 
	{
		return 0;
	}
	else
	{
		strncpy(lvalue, str, lvalue_len);
		strncpy(rvalue, str + rvalue_start, rvalue_len);
		return 1;
	}
}

void string_make_printable(char *str, size_t s_size)
{
	char buf[256] = { '\0' };
	strncpy(buf, str, sizeof(buf));
	memset(str, 0, s_size);
	int index = 0;

	for (size_t i = 0; i < s_size; i++)
	{
		char c = buf[i];
		if (c >= 0x20 && c <= 0x7E && c != ' ')
		{
			str[index++] = c;
		}
	}
}