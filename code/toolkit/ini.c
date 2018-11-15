#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "toolkit_ini.h"
#include "toolkit_string.h"

#ifdef WINDOWS
#define strdup(str) _strdup(str)
#endif // WINDOWS

#define KEY_SIZE 32
#define VALUE_SIZE 64
#define PAIR_SIZE 16
#define SECTION_NAME_LEN 64
#define SECTION_COUNT 32

typedef struct ini_item_s
{
	char key[KEY_SIZE];
	char value[VALUE_SIZE];
}ini_item;

typedef struct ini_section_s
{
	char name[SECTION_NAME_LEN];
	ini_item items[PAIR_SIZE];
	size_t item_count;
}ini_section;

struct ini_s
{
	ini_section sections[SECTION_COUNT];
	size_t section_count;
};

static int get_section(ini *handle, const char *s_name, ini_section *s_out)
{
	for (size_t i = 0; handle->section_count; i++)
	{
		ini_section section = handle->sections[i];
		if (strncmp(section.name, s_name, strlen(section.name)) == 0)
		{
			*s_out = section;
			return 1;
		}
	}
	return 0;
}

static int get_item(ini_section section, const char *k_name, ini_item *i_out)
{
	for (size_t i = 0; i < section.item_count; i++)
	{
		ini_item item = section.items[i];
		if (strncmp(item.key, k_name, strlen(item.key)) == 0)
		{
			*i_out = item;
			return 1;
		}
	}
	return 0;
}

ini *ini_open(char *path)
{
	ini *handle = (ini*)calloc(1, sizeof(ini));
	if (!handle)
	{
		goto error;
	}
	FILE *file = fopen(path, "r");
	if (!file)
	{
		goto error;
	}

	char line[256] = { '\0' };
	while (fgets(line, sizeof(line), file))
	{
		string_make_printable(line, strlen(line));

		if (strlen(line) == 0)
		{
			continue;
		}

		if (line[0] == '[')
		{
			/* 如果遇到了左大括号，则认为是section */
			ini_section *section = handle->sections + handle->section_count;
			strncpy(section->name, line + 1, strlen(line) - 2);
			handle->section_count += 1;
		}
		else
		{
			/* 解析key和value */
			ini_section *section = handle->sections + (handle->section_count - 1);
			ini_item *current_item = section->items + section->item_count;
			string_splite(line, '=', current_item->key, sizeof(current_item->key), current_item->value, sizeof(current_item->value));
			section->item_count += 1;
		}
	}

	fclose(file);
	return handle;

error:
	if (handle)
	{
		free(handle);
	}
	if (file)
	{
		fclose(file);
	}
	return NULL;
}

void ini_close(ini *handle)
{
	if (handle)
	{
		free(handle);
	}
}

int ini_is_section_exist(ini *handle, const char* s_name)
{
	for (size_t i = 0; i < handle->section_count; i++)
	{
		ini_section section = handle->sections[i];
		if (strncmp(section.name, s_name, strlen(section.name)) == 0)
		{
			return 1;
		}
	}

	return 0;
}

int ini_get_section_name_new(ini *handle, size_t n_section, char **name)
{
	if (n_section > handle->section_count)
	{
		return 0;
	}

	ini_section section = handle->sections[n_section];
	size_t size = strlen(section.name) + 1;
	*name = calloc(1, size);
	strncpy(*name, section.name, size - 1);
	return 1;
}

int ini_is_key_exist(ini *handle, const char* s_name, const char* key)
{
	ini_section section;
	if (get_section(handle, s_name, &section) == 0)
	{
		return 0;
	}

	for (size_t i = 0; i < section.item_count; i++)
	{
		ini_item item = section.items[i];
		if (strncmp(item.key, key, strlen(item.key)) == 0)
		{
			return 1;
		}
	}
	return 0;
}

char* ini_get_string_new(ini *handle, const char* s_name, const char* key, const char* defaultValue)
{
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return strdup(defaultValue);
	}

	size_t size = strlen(item.value) + 1;
	char *ret = (char*)calloc(1, size);
	strncpy(ret, item.value, size - 1);
	return ret;
}

int ini_get_bool(ini *handle, const char* s_name, const char* key, int defaultValue)
{
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return defaultValue;
	}

	return atoi(item.value);
}

int ini_get_int(ini *handle, const char* s_name, const char* key, const int defaultValue)
{
	return ini_get_bool(handle, s_name, key, defaultValue);
}

long ini_get_long(ini *handle, const char* s_name, const char* key, const long defaultValue)
{
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return defaultValue;
	}

	return atol(item.value);
}

