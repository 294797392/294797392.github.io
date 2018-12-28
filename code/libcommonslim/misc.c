/**************************************************************************
* @ file    : misc.c
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.05.15
* @ brief   : 常用工具函数
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h> // need for: rand
#include <string.h>
#include <sys/stat.h>

#include "defs.h"
#include "string_utils.h"
#include "misc.h"
#include "win32.h"

#define READ_BUF_SIZE 8192
//
//// 生成随机数
//int random_range(int min, int max) {
//    return min + (rand() % (max - min));
//}
//
//// 随机休眠ms
//void random_sleep(int minSleepMs, int maxSleepMs) {
//    long ms = random_range(minSleepMs, maxSleepMs) * 1000;
//	SLEEP(ms / 1000);
//}

const char* get_args_string(int argc, const char* argv[], const char* key, const char* defaultValue) {
    if (NULL != argv) {
        int i;
        for (i = 0; i < argc - 1; i++) {
            if (strcmp(key, argv[i]) == 0) {
                return argv[i + 1];
            }
        }
    }

    return defaultValue;
}

int get_args_int(int argc, const char* argv[], const char* key, const int defaultValue) {
    if (NULL != argv) {
        int i;
        for (i = 0; i < argc - 1; i++) {
            if (strcmp(key, argv[i]) == 0) {
                return atoi(argv[i + 1]);
            }
        }
    }

    return defaultValue;
}

long get_args_long(int argc, const char* argv[], const char* key, const long defaultValue) {
    if (NULL != argv) {
        int i;
        for (i = 0; i < argc - 1; i++) {
            if (strcmp(key, argv[i]) == 0) {
                return atol(argv[i + 1]);
            }
        }
    }

    return defaultValue;
}

double get_args_double(int argc, const char* argv[], const char* key, const double defaultValue) {
    if (NULL != argv) {
        int i;
        for (i = 0; i < argc - 1; i++) {
            if (strcmp(key, argv[i]) == 0) {
                return atof(argv[i + 1]);
            }
        }
    }

    return defaultValue;
}

BOOL is_args_exist(int argc, const char* argv[], const char* key) {
    if (NULL != argv) {
        int i;
        for (i = 0; i < argc; i++) {
            if (strcmp(key, argv[i]) == 0) {
                return TRUE;
            }
        }
    }

    return FALSE;
}


//----------------------------------------------------------------------------
// 其他接口函数
//----------------------------------------------------------------------------
void throw_not_implement_exception() {
    int *p = NULL;
    *p = 0;
}

void imi_dispath(const char *filepath, char *path, char *file) {
    struct stat st;

    path[0] = 0;
    file[0] = 0;

    if (!filepath)
        return;

    stat(filepath, &st);
    if (st.st_mode & S_IFDIR) {
        // 目录
        strcpy(path, filepath);
    }
    else {
        // 文件
        int i;

        for (i = strlen(filepath) - 1; i >= 0; i--)
            if (filepath[i] == '\\' || filepath[i] == '/')
                break;

        if (i < 0) {
            // 只有文件名
            strcpy(file, filepath);
        }
        else {
            strncpy(path, filepath, i);
            path[i] = 0;
            strcpy(file, &filepath[i + 1]);
        }
    }

    if (!path[0])
        strcpy(path, ".");

    return;
}

char *imi_realpath(const char *path, char *resolved_path) {
    char work_path[256];
    char new_path[256];

    char path_dir[256];
    char path_file[256];

    int len;

    getcwd(work_path, 256);

    imi_dispath(path, path_dir, path_file);

    chdir(path_dir);
    memset(new_path, 0, 256);
    getcwd(new_path, 256);

    len = strlen(new_path);
    if (new_path[len - 1] == '\\' || new_path[len - 1] == '/')
        sprintf(&new_path[len], "%s", path_file);
    else
        sprintf(&new_path[len], "\\%s", path_file);

    strcpy(resolved_path, new_path);

    chdir(work_path);

    if (new_path[0])
        return resolved_path;
    else
        return NULL;
}

// @brief: 动态加载library
void *imi_load_library(const char* path) {
#ifdef WIN32
    return LoadLibrary(path);
#else
    return dlopen(path, RTLD_LAZY);
#endif
}

void *imi_load_function(void* libhandle, const char* func_name) {
#ifdef WIN32
    return GetProcAddress(libhandle, func_name);
#else
    dlerror(); // clear last error
    void* funcPtr = dlsym(libhandle, func_name);
    if (dlerror() != NULL) {
        return NULL;
    }

    return funcPtr;
#endif
}

int imi_close_library(void* libhandle) {
#ifdef WIN32
    return FreeLibrary(libhandle);
#else
    return dlclose(libhandle);
#endif
}

void* imi_create_sharemem(const char *sharefile, size_t size, int mode) {
    //int fd;
#ifdef WIN32
    //fd = sh
#else
    //fd = shm_open(sharefile, O_RDWR | O_CREAT | O_EXCL, 0777);
#endif
    return NULL;
}


// 从文件中读取所有内容至缓冲区当中
// @return: 小于0: 错误代码, 大于0: 读出的实际大小
int imi_read_all(const char *path, char* buffer, int bufferSize) {
    if (!buffer || bufferSize <= 0) {
        return CODE_INVALID_PARAM;
    }

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return CODE_OPEN_FAILED;
    }

    memset(buffer, 0, bufferSize);
    int max = bufferSize - 1;

    int offset = 0;
    while (!feof(fp) && offset < max) {
        int count = fread(buffer + offset, 1, MIN(max - offset, READ_BUF_SIZE), fp);
        offset += count;
    }

    fclose(fp);
    return offset;
}


// 从文件中读取所有内容
// 调用者需要自己释放内存
char* imi_read_all_new(const char *path, int* out_size) {
    FILE *fp = fopen(path, "rb");
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
        int count = MIN(READ_BUF_SIZE, fsize - offset);
        count = fread(buffer + offset, 1, count, fp);

        offset += count;
    }

    fclose(fp);
    *out_size = offset;
    return buffer;
}
