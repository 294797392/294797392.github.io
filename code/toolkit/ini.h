#ifndef __TOOLKIT_INI_H__
#define __TOOLKIT_INI_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ini_s ini;

ini *ini_open(char *path);

void ini_close(ini *handle);

int ini_is_section_exist(ini *handle, const char* s_name);

int ini_get_section_name_new(ini *handle, size_t n_section, char **s_name);

int ini_is_key_exist(ini *handle, const char* s_name, const char* key);

char* ini_get_string_new(ini *handle, const char* s_name, const char* key, const char* defaultValue);

int ini_get_bool(ini *handle, const char* s_name, const char* key, int defaultValue);

int ini_get_int(ini *handle, const char* s_name, const char* key, const int defaultValue);

long ini_get_long(ini *handle, const char* s_name, const char* key, const long defaultValue);


#endif
