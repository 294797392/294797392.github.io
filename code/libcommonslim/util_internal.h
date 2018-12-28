/**************************************************************************
* @ file    : util_internal.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2016.08.12
* @ brief   : libcommon内部诊断或工具函数
***************************************************************************/

#ifndef __LIBCOMMON_INTERNAL_H__
#define __LIBCOMMON_INTERNAL_H__

#include "elog.h"

#define LIB_COMMON_LOGGER   "libcommon"     // libcommon内部使用logger

extern logger_t s_logger_libcommon;

#define log_inner_assert(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_ASSERT, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_inner_error(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_ERROR, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_inner_warn(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_WARN, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_inner_info(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_INFO, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_inner_debug(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_DEBUG, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_inner_verbose(module, ...) \
        log_queue_output(s_logger_libcommon, ELOG_LVL_VERBOSE, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

// 分配内存并置0
void* __imi_malloc0(size_t size);

// 分配内存并置0 (mem watch模式)
void* __imi_mw_malloc0(size_t size, const char* file, int line);

// free
void __imi_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif // __LIBCOMMON_INTERNAL_H__
