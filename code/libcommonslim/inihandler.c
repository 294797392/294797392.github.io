/**************************************************************************
* @ file    : inihandler.c
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.08.20
* @ brief   : ini文件操作的封装
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "win32.h"
#include "string_utils.h"
#include "inihandler.h"

//---------------------------------------------------------------------------
//  ini接口函数
//---------------------------------------------------------------------------

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

typedef struct ini_s
{
	ini_section sections[SECTION_COUNT];
	size_t section_count;
}ini;

//----------------------------------------------------------------------------
// ini文件section遍历器
//----------------------------------------------------------------------------
typedef struct _iterator_ini_section_s {
	ini *handle;
	size_t       pos;
} _iterator_ini_section_s;

typedef struct _iterator_ini_kvpair_s {
	ini *handle;
	int       pos;
} _iterator_ini_kvpair_s;


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


ini_file_t open_ini_file(const char *filePath) {
	ini *handle = (ini*)calloc(1, sizeof(ini));
	if (!handle)
	{
		goto error;
	}
	FILE *file = fopen(filePath, "r");
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

ini_file_t create_ini_file() {
	return NULL;
}

int save_ini_file(const char *filePath, ini_file_t iniFile) {
	return 1;
}

void release_ini_file(ini_file_t iniFile) {
	if (iniFile)
	{
		free(iniFile);
	}
}

BOOL ini_file_is_section_exist(ini_file_t iniFile, const char* s_name) {
	ini *handle = (ini*)iniFile;
	for (size_t i = 0; i < handle->section_count; i++)
	{
		ini_section section = handle->sections[i];
		if (strncmp(section.name, s_name, strlen(section.name)) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;

}

BOOL ini_file_is_key_exist(ini_file_t iniFile, const char* s_name, const char* key) {
	ini *handle = (ini*)iniFile;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0)
	{
		return FALSE;
	}

	for (size_t i = 0; i < section.item_count; i++)
	{
		ini_item item = section.items[i];
		if (strncmp(item.key, key, strlen(item.key)) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

char* get_ini_string(ini_file_t iniFile, const char* s_name, const char* key, const char* defaultValue) {
	ini *handle = (ini*)iniFile;
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return clone_string(defaultValue);
	}

	size_t size = strlen(item.value) + 1;
	char *ret = (char*)calloc(1, size);
	strncpy(ret, item.value, size - 1);
	return ret;
}

int set_ini_string(ini_file_t iniFile, const char* section, const char *key, const char* value) {
	return 0;
}

BOOL get_ini_bool(ini_file_t iniFile, const char* s_name, const char* key, BOOL defaultValue) {
	ini *handle = (ini*)iniFile;
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return defaultValue;
	}

	if (strncmp(item.value, "true", 4) == 0)
	{
		return TRUE;
	}

	if (strncmp(item.value, "false", 5) == 0)
	{
		return FALSE;
	}

	return atoi(item.value) > 0 ? TRUE : FALSE;
}

int set_ini_bool(ini_file_t iniFile, const char* sectionName, const char *key, BOOL value) {
	return 0;
}

int get_ini_int(ini_file_t iniFile, const char* section, const char* key, const int defaultValue) {
	return get_ini_bool(iniFile, section, key, defaultValue);
}

int set_ini_int(ini_file_t iniFile, const char* sectionName, const char *key, int value) {
	return 0;
}

long get_ini_long(ini_file_t iniFile, const char* s_name, const char* key, const long defaultValue) {
	ini *handle = (ini*)iniFile;
	ini_item item;
	ini_section section;
	if (get_section(handle, s_name, &section) == 0 || get_item(section, key, &item) == 0)
	{
		return defaultValue;
	}

	return atol(item.value);
}

int set_ini_long(ini_file_t iniFile, const char* sectionName, const char *key, long value) {
	return 0;
}

long long get_ini_longlong(ini_file_t iniFile, const char* section,
	const char* key, const long long defaultValue) {
	return 0;
}

int set_ini_longlong(ini_file_t iniFile, const char* sectionName,
	const char *key, long long value) {
	return 0;
}

double get_ini_double(ini_file_t iniFile, const char* section,
	const char* key, const double defaultValue) {
	return 0;
}

int set_ini_double(ini_file_t iniFile, const char* sectionName,
	const char *key, double value) {
	return 0;
}

int ini_remove_key(ini_file_t iniFile, const char* sectionName, const char *key) {
	return 0;
}

int ini_remove_section(ini_file_t iniFile, const char* section) {
	return 0;
}

iterator_ini_section_t get_iterator_ini_section(ini_file_t iniFile) {
	iterator_ini_section_t iter = imi_new0(struct _iterator_ini_section_s, 1);
	iter->handle = (ini*)iniFile;
	return iter;
}

int get_next_ini_section(iterator_ini_section_t iter, char** s_name) {
	ini *handle = (ini*)iter->handle;
	if (iter->pos > handle->section_count)
	{
		return 0;
	}

	ini_section section = handle->sections[iter->pos];
	size_t size = strlen(section.name) + 1;
	*s_name = calloc(1, size);
	strncpy(*s_name, section.name, size - 1);
	iter->pos += 1;
	return 1;
}

void release_iterator_ini_section(iterator_ini_section_t iter) {
	if (iter)
	{
		imi_free(iter);
	}
}

iterator_ini_kvpair_t get_iterator_ini_kvpair(ini_file_t iniFile, const char* section) {
	return 0;
}

int get_next_ini_kvpair(iterator_ini_kvpair_t iter, char** key, char** value) {
	return 0;
}

void release_iterator_ini_kvpair(iterator_ini_kvpair_t iter) {
}
