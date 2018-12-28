/**************************************************************************
* @ file    : string_utils.c
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2016.08.15
* @ brief   :
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "defs.h"
#include "string_utils.h"

#define READ_SIZE 8192
#define NUM_SIZE 64

static int is_space(char c) {
    return (c == '\t' || c == '\n' || c == ' ' || c == '\r') ? 1 : 0;
}

// 返回字符串中delimeter所在的位置. 如果不存在则返回NULL
// @n   需要跳过的delimeter的个数
static char* str_get_delimiter(char* str, char delimeter, int skipN) {
    char* p = str;
    int count = 0;
    while (p && *p) {
        if (*p == delimeter) {
            count++;
            if (count > skipN) {
                return p;
            }
        }

        p++;
    }

    return NULL;
}

char* str_trim(char *str) {
    if (str) {
        char *r = str;
        char *p = r;

        char *q = str + strlen(str) - 1;
        while (*p && is_space(*p)) p++;
        while (q > p && is_space(*q)) *q-- = '\0';
        while (p <= q) *r++ = *p++;

        *r = '\0';
    }

    return str;
}

char* str_trim_start(char *str) {
    if (str) {
        char *r = str;
        char *p = r;

        char *q = str + strlen(str) - 1;
        while (*p && is_space(*p)) p++;
        while (p <= q) *r++ = *p++;

        *r = '\0';
    }

    return str;
}

char* str_trim_end(char *str) {
    if (str) {
        char *q = str + strlen(str) - 1;
        while (q > str && is_space(*q)) *q-- = '\0';
    }

    return str;
}

char *str_read_token_new(char* buffer, char** outToken, char delimeter) {
    char *p = buffer;
    while (*p == ' ' || *p == '\t')  p++;   // skip blanks

    char *start = p;
    int token_size = 0;
    int lastBlankSize = 0;
    while (*p != delimeter && *p != '\r' && *p != '\n' && *p != '\0') {
        if (*p == ' ' || *p == '\t') {
            lastBlankSize++;
        }
        else if (lastBlankSize > 0) {
            token_size = token_size + lastBlankSize + 1;
            lastBlankSize = 0;
        }
        else {
            token_size++;
        }

        p++;
    }

    if (token_size > 0) {
        if (*p == delimeter) {
            p++;
        }

        char *token = imi_malloc(token_size + 1);
        strncpy(token, start, token_size);
        token[token_size] = '\0';

        *outToken = token;
    }
    else {
        *outToken = NULL;
    }

    return p;
}

char *str_read_token(char* str, char* outBuffer, int bufferSize, char delimeter) {
    char *p = str;
    while (*p == ' ' || *p == '\t')  p++;      // skip blanks

    char *start = p;
    int tokenSize = 0;
    int lastBlankSize = 0;
    while (*p != delimeter && *p != '\r' && *p != '\n' && *p != '\0') {
        if (*p == ' ' || *p == '\t') {
            lastBlankSize++;
        }
        else if (lastBlankSize > 0) {
            // 说明之前的空格是token内部的空格, 重置lastBlankSize
            tokenSize = tokenSize + lastBlankSize + 1;
            lastBlankSize = 0;
        }
        else {
            tokenSize++;
        }

        p++;
    }

    if (tokenSize > 0) {
        if (*p == delimeter) {
            p++;
        }

        int copySize = bufferSize <= tokenSize ? bufferSize - 1 : tokenSize;
        strncpy(outBuffer, start, copySize);
        outBuffer[copySize] = '\0';
    }
    else {
        *outBuffer = '\0';
    }

    return p;
}

bool str_read_int(char** ptr_str, int *out, char delimeter) {
    if (!ptr_str || !(*ptr_str)) {
        return false;
    }

    char* token;
    char* ptr = str_read_token_new(*ptr_str, &token, delimeter);
    if (token != NULL) {
        *out = atoi(token);
        *ptr_str = ptr;
        imi_free(token);
        return true;
    }
    else {
        return false;  // no token available
    }
}

bool str_read_double(char** ptr_str, double *out, char delimeter) {
    if (!ptr_str || !(*ptr_str)) {
        return false;
    }

    char* token;
    char* ptr = str_read_token_new(*ptr_str, &token, delimeter);
    if (token != NULL) {
        *out = atof(token);
        *ptr_str = ptr;
        imi_free(token);
        return true;
    }
    else {
        return false;  // no token available
    }
}

char *str_read_token_between(char* str, char start, char end, char* outBuffer, int bufferSize, int skipN) {
    char *p = str_get_delimiter(str, start, skipN);
    if (p == NULL || *p == '\0') {
        *outBuffer = '\0';
        return NULL;
    }

    return str_read_token(p + 1, outBuffer, bufferSize, end);
}

int split(char *src, char delimeter, char** tokenArray, int maxSize) {
    int count = 0;

    char *p = src;
    while (*p != '\0') {
        char *token;
        p = str_read_token_new(p, &token, delimeter);
        if (token != NULL) {
            tokenArray[count] = token;
            count++;

            if (count >= maxSize) {
                break;
            }
        }
        else if (*p == delimeter) {
            p++; // meet empty token
        }
    }

    return count;
}

bool str_start_with(char *src, char* prefix) {
    char *pSrc = src;
    char *pPrefix = prefix;

    while (*pSrc && *pPrefix) {
        if (*pSrc == *pPrefix) {
            pSrc++;
            pPrefix++;
        }
        else {
            return false;
        }
    }

    return *pPrefix == '\0' ? true : false;
}

// 哈希函数. 暂时不采用外部实现. 
unsigned string_hash(const char * key) {
    size_t      len;
    unsigned    hash;
    size_t      i;

    len = strlen(key);
    for (hash = 0, i = 0; i<len; i++) {
        hash += (unsigned)key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

char *clone_string(const char* value) {
    if (value != NULL) {
        int size = (strlen(value) + 1) * sizeof(char);
        char *buffer = imi_malloc(size);
        strncpy(buffer, value, size);
        return buffer;
    }
    else {
        return NULL;
    }
}

void release_string(char* value) {
    if (value) {
        imi_free(value);
    }
}

int imi_read_all_text(const char *path, char* buffer, int size) {
    if (!buffer || size <= 0) {
        return CODE_INVALID_PARAM;
    }

    FILE *fp = fopen(path, "rt");
    if (!fp) {
        return CODE_OPEN_FAILED;
    }

    memset(buffer, 0, size);
    int max = size - 1;

    int offset = 0;
    while (!feof(fp) && offset < max) {
        int count = fread(buffer + offset, 1, MIN(max - offset, READ_SIZE), fp);
        offset += count;
    }

    fclose(fp);
    return offset + 1;
}

char* imi_read_all_text_new(const char *path, int* out_size) {
    FILE *fp = fopen(path, "rt");
    if (!fp) {
        *out_size = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);

    char* buffer = imi_malloc(fsize + 1);
    if (buffer == NULL) {
        *out_size = 0;
        fclose(fp);
        return NULL;
    }

    memset(buffer, 0, fsize + 1);
    fseek(fp, 0, SEEK_SET);

    int offset = 0;
    while (!feof(fp) && offset < fsize) {
        int count = MIN(READ_SIZE, fsize - offset);
        count = fread(buffer + offset, 1, count, fp);

        offset += count;
    }

    fclose(fp);
    *out_size = offset + 1;
    return buffer;
}

int imi_str_printf(char **ppsz_buffer, const char *fmt, ...) {
    int n;
    int size = 256;
    char *psz_buffer, *psz_buffer_temp;
    va_list ap;

    if (ppsz_buffer == NULL || fmt == NULL) {
        return -EINVAL;
    }
    if ((psz_buffer = imi_malloc(size)) == NULL) {
        return -ENOMEM;
    }

    while (1) {
        va_start(ap, fmt);
        n = vsnprintf(psz_buffer, size, fmt, ap);
        va_end(ap);

        if (n > -1 && n < size) {
            if (n + 1 != size) {
                psz_buffer = imi_realloc(psz_buffer, n + 1);
            }
            *ppsz_buffer = psz_buffer;
            return n;
        }

        if (n > -1) {
            size = n + 1;
        }
        else {
            size *= 2;
        }

        if ((psz_buffer_temp = imi_realloc(psz_buffer, size)) == NULL) {
            imi_free(psz_buffer);
            return -ENOMEM;
        }
        else {
            psz_buffer = psz_buffer_temp;
        }
    }
}

int imi_str_repeat(char **ppsz_buffer, const char *string, int repeat) {
    int length;
    int i;
    char *psz_buffer = NULL;

    if (ppsz_buffer == NULL || string == NULL || repeat <= 0) {
        return -EINVAL;
    }

    length = strlen(string);
    psz_buffer = imi_malloc(length * repeat + 1);
    if (psz_buffer == NULL) {
        return -ENOMEM;
    }

    for (i = 0; i < repeat; i++) {
        strcpy(psz_buffer + length * i, string);
    }

    *ppsz_buffer = psz_buffer;
    return length * repeat;
}

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
