#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wsdd.h"

int uricmp(const char *uri1, const char *uri2)
{
	/* 
	 * 这里假设uri只包含ascii码，没考虑中文情况 
	 * 如果uri包含中文，先把uri转成字节数组，然后逐字节比较
	 */
	int len1 = strlen(uri1);
	int len2 = strlen(uri2);

	if (len1 == len2 && strncmp(uri1, uri2, len1) == 0)
	{
		return 1;
	}

	return 0;
}

int wsdd_match(char **scopes, int scope_cnt, const char *target_scope)
{
	int scope_matches = 0;
	if (target_scope == NULL || strlen(target_scope) == 0)
	{
		scope_matches = 1;
	}
	else
	{
		for (int i = 0; i < scope_cnt; i++)
		{
			char *scope = scopes[i];
			if (uricmp(scope, target_scope))
			{
				scope_matches = 1;
				break;
			}
		}
	}
	if (!scope_matches)
	{
		return 0;
	}

	return 1;
}
