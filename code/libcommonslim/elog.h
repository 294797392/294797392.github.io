 /**************************************************************************
  * @ file    : elog.h
  * @ author  : yang yang
  * @ version : 0.9
  * @ date    : 2016.08.12
  * @ brief   : 日志输出
***************************************************************************/

#ifndef __ELOG_H__
#define __ELOG_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 是否使用异步输出
// #define ELOG_ASYNC_OUTPUT

// 日志事件队列最大长度
#define ELOG_MAX_QUEUE_SIZE   128
// 日志队列初始长度
#define ELOG_INIT_QUEUE_SIZE  64
// 回收事件队列最大长度
#define ELOG_MAX_RECYCLE_QUEUE  128


#define ELOG_DEFAULT_FILE_LIMIT     10  // 缺省rolling file 个数
#define ELOG_DEFAULT_FILE_MAX_SIZE  1048576  // 缺省rolling file size (1M)
#define ELOG_MIN_FILE_LIMIT         1
#define ELOG_MIN_FILE_MAX_SIZE      1024


// 日志事件中消息缓冲大小, 即用户消息的最大长度
#define ELOG_BUF_SIZE    128
// 日志输出行缓冲大小, 即日志文件中每行的最大长度
#define ELOG_LINE_BUF_SIZE 1024

// log level
#define ELOG_LVL_ASSERT    0
#define ELOG_LVL_ERROR     1
#define ELOG_LVL_WARN      2
#define ELOG_LVL_INFO      3
#define ELOG_LVL_DEBUG     4
#define ELOG_LVL_VERBOSE   5
#define ELOG_LVL_ALL       6

// 编译期 日志开关
#define ELOG_OUTPUT_ENABLE
// 编译期 日志输出级别
#define ELOG_OUTPUT_LVL  ELOG_LVL_ALL

// format definition
typedef enum {
    ELOG_FMT_LVL    = 1 << 0, // level
    ELOG_FMT_TAG    = 1 << 1, // tag info
    ELOG_FMT_MODULE = 1 << 2, // module
    ELOG_FMT_P_INFO = 1 << 3, // process info
    ELOG_FMT_T_INFO = 1 << 4, // thread info
    ELOG_FMT_FILE   = 1 << 5, // file directory and name
    ELOG_FMT_FUNC   = 1 << 6, // function name
    ELOG_FMT_LINE   = 1 << 7, // line number
} ElogFmtIndex;

#define ELOG_FMT_DEFAULT (ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_MODULE|ELOG_FMT_T_INFO)
#define ELOG_FMT_ALL  (ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_MODULE|ELOG_FMT_P_INFO|ELOG_FMT_T_INFO|\
    ELOG_FMT_FILE|ELOG_FMT_FUNC|ELOG_FMT_LINE)

#ifndef ELOG_OUTPUT_ENABLE

#define log_tag_assert(tag, module, ...)
#define log_tag_error(tag, module, ...)
#define log_tag_warn(tag, module, ...)
#define log_tag_info(tag, module, ...)
#define log_tag_debug(tag, module, ...)
#define log_tag_verbose(tag, module, ...)

#define log_assert(module, ...)
#define log_error(module, ...)
#define log_warn(module, ...)
#define log_info(module, ...)
#define log_debug(module, ...)
#define log_verbose(module, ...)

#else // ELOG_OUTPUT_ENABLE

#if ELOG_OUTPUT_LVL >= ELOG_LVL_ASSERT
#define log_assert(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_ASSERT, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_tag_assert(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_ASSERT, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#else
#define log_assert(module, ...)
#define log_tag_assert(tag, module, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_ERROR
#define log_error(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_ERROR, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define log_tag_error(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_ERROR, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define log_error(module, ...)
#define log_tag_error(tag, module, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_WARN
#define log_warn(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_WARN, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_tag_warn(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_WARN, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define log_warn(module, ...)
#define log_tag_warn(tag, module, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_INFO
#define log_info(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_INFO, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_tag_info(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_INFO, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define log_info(module, ...)
#define log_tag_info(tag, module, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_DEBUG
#define log_debug(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_DEBUG, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_tag_debug(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_DEBUG, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define log_debug(module, ...)
#define log_tag_debug(tag, module, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_VERBOSE
#define log_verbose(module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_VERBOSE, NULL, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_tag_verbose(tag, module, ...) \
        log_queue_output(s_default_logger, ELOG_LVL_VERBOSE, tag, module, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define log_verbose(module, ...)
#define log_tag_verbose(tag, module, ...)
#endif

#endif // ELOG_OUTPUT_ENABLE

typedef enum
{
    APPENDER_TYPE_NULL = 0,     // 输出到NIL
    APPENDER_TYPE_STDOUT = 1,   // 输出到stdout
    APPENDER_TYPE_STDERR = 2,   // 输出到stderr
    APPENDER_TYPE_FILE = 3,     // 输出到文件
    APPENDER_TYPE_SYSLOG = 4,   // 输出到syslog中（暂未实现）
    APPENDER_TYPE_UDP = 5,      // 输出到udp流中（暂未实现）
    APPENDER_TYPE_TCP = 6       // 输出到tcp流中（暂未实现）
} appender_type_t;

typedef struct _logger_s *logger_t;

extern logger_t s_default_logger;

void log_init(const char *iniCfgFile);
void log_init_from_pwd(const char *psz_name);
void log_uninit(void);
void load_log_config(const char *iniCfgFile);

// 输出日志
void log_queue_output(logger_t logger, uint8_t level, const char *tag, const char *module, const char *file,const char *func, long line, const char *format, ...);

// 设置logger参数
void log_set_output_enabled(logger_t logger, bool enabled);

// 查看是否输出日志：0：不输出，1输出
int log_get_output_enabled(logger_t logger);

// 设置logger的格式
void log_set_fmt(logger_t logger, uint8_t level, uint8_t set);

// 获取一个logger, 如果系统中没有则创建一个实例
logger_t get_logger(const char* name);

#ifdef __cplusplus
}
#endif

#endif // __ELOG_H__
