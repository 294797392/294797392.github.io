/**************************************************************************
* @ file    : elog.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2016.08.12
* @ brief   : 日志输出
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#include <pthread.h>
#include <windows.h>
#include "win32.h"
#else
#include <unistd.h>
#include <pthread.h>
#endif // WIN32

#include "defs.h"
#include "array.h"
#include "string_utils.h"
#include "trash_stack.h"
#include "inihandler.h"
#include "elog.h"
#include "util_internal.h"

// max logger name
#define ELOG_MAX_LOGGER_NAME    64
#define ELOG_MAX_LOGGERS        64

#define ELOG_MAX_APPENDERS    8 // 最多支持的appender数量
#define WORD_DELIMETER ','      // 设置filter的module配置时, 多个module之间的分隔符
#define FILTER_DELIMETER ':'    // 定义某个filter时， 分隔moduleName:level之间的分隔符

#define STREAM_BUFFER_SIZE 8196

// filter支持module的最大长度
#define ELOG_FILTER_MAX_FILTERS 32
// 缺省Level
#define ELOG_DEFAULT_LVL  ELOG_LVL_INFO
// 缺省appender level
#define DEFAULT_APPENDER_LVL  ELOG_LVL_ALL

// 日志打印的时间格式, 目前永远是输出UTC时间
#define GTIME_FORMAT "%Y-%m-%dT%H:%M:%S"
#define TIME_FORMAT "%s.%03dZ"
#define TIME_FORMAT_LEN 20

#define THREAD_MAX_LEN 20   // 线程号,进程号最大长度
#define PROCESS_MAX_LEN 12  // 进程信息的最大长度

#define LINE_MAX_LEN 8      // 行号最大长度

#define LOG_RED "\033[0;32;31m"

#define LOGGER_SECTION_NAME     "logger"

#define KEY_ENABLE              "enable"        // 配置是否起用
#define KEY_LEVEL               "level"         // 配置日志level
#define KEY_APPENDER_COLOR      "color"         // 配置日志颜色是否启用
#define KEY_APPENDER_FILTERS    "filter"        // 配置appender的filter
#define KEY_APPENDER_TYPE       "type"          // 配置appender的类型

#define KEY_APPENDER            "appender"      // 配置logger所对应的appender

#define KEY_APPENDER_CONFIG     "config"
#define KEY_APPENDER_FILE_LIMIT "limit"
#define KEY_APPENDER_FILE_SIZE  "size"

#define ALL_APPENDERS           "all"           // 用于指定使用所有appender

// 日志文件名最大长度
#define ELOG_FILE_MAX_LEN 256
// msg垃圾回收的最大长度
#define ELOG_MAX_TRASH_LEN 32

#define FORMAT_CONFIG_NUM 8     // 可配置格式选项的个数
const char* s_format_mask_key[] = { "level", "tag", "module", "process", "thread", "file", "func", "line" };
const int s_format_mask[] = { ELOG_FMT_LVL, ELOG_FMT_TAG, ELOG_FMT_MODULE, ELOG_FMT_P_INFO, ELOG_FMT_T_INFO, ELOG_FMT_FILE , ELOG_FMT_FUNC, ELOG_FMT_LINE };

const char* s_logger_level_str[] = { "assert", "error", "warn", "info", "debug", "verbose", "all" };
const char* s_appender_typestr[] = { "nil", "stdout", "stderr", "file", "syslog", "udp", "tcp" };

#pragma region 内部结构定义

#ifndef ELOG_OUTPUT_ENABLE
void __fake_log__(const char *fmt, ...) { }
#endif

// log level output
#define LEVEL_FORMAT_LEN 2
static const char *s_level_output_info[] = {
	"A/",   // assert
	"E/",   // error
	"W/",   // warn
	"I/",   // info
	"D/",   // debug
	"V/",   // verbose
};

// level输出对应的颜色
static const char *s_level_output_color[] = {
	"\033[0;35m",     // assert: PURPLE
	"\033[0;32;31m",  // error:  RED
	"\033[1;33m",     // warn:   YELLOW
	"\033[0;32;32m",  // info:   GREEN
	"\033[m",         // debug:  NONE
	"\033[m",         // VERBOSE:NONE
};

typedef enum {
	STOP = 0,
	RUNNING = 1,
} logger_system_status_t;

typedef unsigned long thead_id_t;    // 线程ID

typedef struct _logger_message_s {
	uint8_t level;          // level

	logger_t logger;        // 所属的logger

	time_t tm;          // 输出日志消息时的时间

	const char* module;     // 日志消息所属的module
	unsigned module_hash;   // 日志消息module的hash

	const char* tag;        // 日志的tag
	const char* file;       // 打印日志消息的代码所在的文件
	const char* func;       // 打印日志消息的代码所属函数
	long  line;             // 打印日志消息的代码的行号

	thead_id_t thread_id;   // 输出日志的thread id

	char  message[ELOG_BUF_SIZE];   // 消息buffer
} *logger_message_t;

// rolling file appender的配置
typedef struct _rolling_file_s {
	int limit;    // rolling的文件个数
	long currentSize; // 当前文件大小
	long max_size; // 每个文件的最大长度
} *rolling_file_t;


typedef struct _console_config_s {
	int color;
} *console_config_t;

typedef struct _filter_s {
	unsigned hash;  // module的hash值
	char* module;
	int level;
} *filter_t;

typedef struct _logger_appender_s {
	int  enabled;  // 是否输出
	int level;
	char* name;
	char* configString;

	array_t filters;    // filters

	appender_type_t type;
	void *fp_config;    // 具体fp的配置信息. 需要和fp_type配合, 确定该字段的类型
	FILE *fp;
} *logger_appender_t;

struct _logger_s {
	int enabled;                   // 是否输出
	int level;
	uint8_t format[ELOG_LVL_ALL];       // logger输出的格式, 可以为每个level定义不同的输出

	int appender_count;
	logger_appender_t appenders[ELOG_MAX_APPENDERS];   // 绑定的appender

	char name[ELOG_MAX_LOGGER_NAME];    // logger name
	char buffer[ELOG_LINE_BUF_SIZE];    // logger使用的行缓冲
};

#pragma endregion

#pragma region 静态变量

// 记录当前进程信息
static char s_process_info[PROCESS_MAX_LEN] = { 0 };

// 缺省logger
logger_t s_default_logger;
logger_t s_logger_libcommon;

// logger
static int s_logger_count;
static logger_t s_logger_list[ELOG_MAX_LOGGERS];

// 日志系统维护的appenders
static pthread_mutex_t s_appenders_lock; // appender队列锁
static int s_appender_count;    // 当前已加载的appender数量
static logger_appender_t s_appenders[ELOG_MAX_APPENDERS];  // 系统中所有appender

// 日志线程以及缓冲队列
static pthread_mutex_t s_log_msg_queue_lock;       // 队列锁
static pthread_cond_t  s_log_msg_queue_cond;       // 队列信号量
static array_t s_log_msg_queue;           // 异步写日志时，消息缓冲队列

static trash_stack_t s_log_msg_trash; // 消息回收队列

static pthread_t* s_logger_worker_thread;  // 异步写日志的工作线程
static logger_system_status_t s_log_worker_status; // 线程状态

// 记录当前平面上正在输出的颜色index
static uint8_t s_console_current_color_idx = ELOG_LVL_VERBOSE;

#pragma endregion

//---------------------------------------------------------------------------
//  内部函数
//---------------------------------------------------------------------------
extern int g_unlink(const char *filename);

static size_t elog_min(size_t x, size_t y) {
	return x > y ? y : x;
}

static logger_appender_t find_appender(const char* name) {
	int i;
	for (i = 0; i < s_appender_count; i++) {
		logger_appender_t appender = s_appenders[i];
		if (strcmp(name, appender->name) == 0) {
			return appender;
		}
	}

	return NULL;
}

// 根据名字查找logger
static logger_t find_logger(const char* name) {
	int i;
	for (i = 0; i < s_logger_count; i++) {
		logger_t logger = s_logger_list[i];
		if (strcmp(name, logger->name) == 0) {
			return logger;
		}
	}

	return NULL;
}

static int parse_level(const char* levelText, int defaultValue) {
	if (levelText != NULL) {
		// 首先尝试用名称去匹配
		int i;
		for (i = 0; i <= ELOG_LVL_ALL; i++) {
			if (strstr(levelText, s_logger_level_str[i])) {
				return i;
			}
		}

		// 查看是否为数字
		return atoi(levelText);
	}
	else {
		return defaultValue;
	}
}

static int get_level(ini_file_t file, const char* section, int defaultValue) {
	char* levelText = get_ini_string(file, section, KEY_LEVEL, NULL);
	int level = parse_level(levelText, defaultValue);
	if (levelText) {
		imi_free(levelText);
	}
	return level;
}

static thead_id_t get_current_thread_id() {
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();;
#endif // WIN32
}

static int get_current_process_id() {
#ifdef WIN32
	return GetCurrentProcessId();
#else
	return getpid();
#endif // WIN32
}

// 判断configString是否相等
static int config_strcmp(const char* str1, const char* str2) {
	return (str1 != NULL && str2 != NULL) ? strcmp(str1, str2) :
		(str1 == str2 ? 0 : 1);
}

// 从文本中解析appender type
static appender_type_t parse_appender_type(char* typeString) {
	appender_type_t type = APPENDER_TYPE_NULL;
	int i;
	for (i = 0; i <= APPENDER_TYPE_TCP; i++) {
		if (strcmp(typeString, s_appender_typestr[i]) == 0) {
			type = i;
			break;
		}
	}

	return type;
}

static logger_t create_logger(const char* name) {
	logger_t logger = imi_new0(struct _logger_s, 1);

	strncpy(logger->name, name, strlen(name));

	int i;
	for (i = 0; i < ELOG_LVL_ALL; i++) {
		logger->format[i] = ELOG_FMT_DEFAULT;
	}

	return logger;
}

static void release_logger(logger_t logger) {
	if (logger) {
		imi_free(logger);
	}
}

// 释放appender
static void release_log_appender(logger_appender_t appender) {
	if (appender != NULL) {
		if (appender->name != NULL) {
			imi_free(appender->name);
		}

		if (appender->configString != NULL) {
			imi_free(appender->configString);
		}

		if (appender->filters != NULL) {
			filter_t filter;
			while ((filter = array_dequeue(appender->filters)) != NULL) {
				imi_free(filter->module);
				imi_free(filter);
			}

			release_array(appender->filters);
		}

		if (appender->fp_config != NULL) {
			imi_free(appender->fp_config);
		}

		switch (appender->type) {
			case APPENDER_TYPE_FILE:
				if (appender->fp != NULL) {
					fclose(appender->fp);
				}
				break;
		}

		imi_free(appender);
	}
}

// 加载logger
static void load_log_logger(logger_t logger, ini_file_t log_config, const char* name) {

	logger->enabled = get_ini_bool(log_config, name, KEY_ENABLE, TRUE); // is enable
	logger->level = get_level(log_config, name, ELOG_DEFAULT_LVL);      // level

	// logger可以为每个level设置不同的输出格式，针对每一个level去检查配置文件中是否有相应的配置
	// 如果没有，则采用缺省格式
	int i, j;
	for (i = 0; i < ELOG_LVL_ALL; i++) {
		int format = 0;
		char* formatCfg = get_ini_string(log_config, name, s_logger_level_str[i], NULL);
		if (formatCfg != NULL) {
			for (j = 0; j < FORMAT_CONFIG_NUM; j++) {
				if (strstr(formatCfg, s_format_mask_key[j])) {
					format |= s_format_mask[j];
				}
			}

			imi_free(formatCfg);
		}
		else {
			format = ELOG_FMT_DEFAULT;
		}

		logger->format[i] = format;
	}

	// 设置appenders, 此时已经appenders已经全部加载
	memset(logger->appenders, 0, sizeof(appender_type_t) * ELOG_MAX_APPENDERS);
	char* appenderText = get_ini_string(log_config, name, KEY_APPENDER, NULL);
	if (appenderText != NULL) {
		if (strcmp(appenderText, ALL_APPENDERS) == 0) {
			memcpy(logger->appenders, s_appenders, sizeof(logger_appender_t) * s_appender_count);
		}
		else {
			char* nameList[ELOG_MAX_APPENDERS];
			int count = split(appenderText, WORD_DELIMETER, nameList, ELOG_MAX_APPENDERS);
			int appenderCount = 0;
			for (i = 0; i < count; i++) {
				logger_appender_t appender = find_appender(nameList[i]);
				if (appender) {
					logger->appenders[appenderCount++] = appender;
				}

				imi_free(nameList[i]);
			}

			logger->appender_count = appenderCount;
		}

		imi_free(appenderText);
	}
	else if (strcmp(name, LOGGER_SECTION_NAME) == 0) {
		// default logger如果没有配置，默认使用全部appender. 否则默认取空
		memcpy(logger->appenders, s_appenders, sizeof(logger_appender_t) * s_appender_count);
		logger->appender_count = s_appender_count;
	}
}

// 加载appender
static logger_appender_t load_log_appender(ini_file_t log_config, appender_type_t type, const char* name) {
	char* configString = get_ini_string(log_config, name, KEY_APPENDER_CONFIG, NULL);

	// 查看是否有同名的logger。 如果有同名appender，查看配置是否一致
	// 如果完全一致，说明无需要重新加载，使用原先的即可。如果不一致，则释放原先的
	int i;
	logger_appender_t appender = NULL;
	for (i = 0; i < s_appender_count; i++) {
		if (s_appenders[i] && strcmp(s_appenders[i]->name, name) == 0) {
			appender = s_appenders[i];
			s_appenders[i] = NULL;  // 先从appenders中移除

			if (type != appender->type || config_strcmp(configString, appender->configString) != 0) {
				release_log_appender(appender);
				appender = NULL;
			}

			break;
		}
	}

	BOOL enabled = get_ini_bool(log_config, name, KEY_ENABLE, TRUE); // is enable

	// 创建appender
	if (appender == NULL) {
		appender = imi_new0(struct _logger_appender_s, 1);
		appender->level = ELOG_OUTPUT_LVL;
		appender->type = type;
		appender->name = imi_strdup(name);
		appender->configString = imi_strdup(configString);
		appender->filters = create_array(1, ELOG_FILTER_MAX_FILTERS);

		FILE *fp;
		switch (type) {
			case APPENDER_TYPE_STDOUT:
				appender->fp = stdout;
				appender->fp_config = imi_new0(struct _console_config_s, 1);
				break;
			case APPENDER_TYPE_STDERR:
				appender->fp = stderr;
				appender->fp_config = imi_new0(struct _console_config_s, 1);
				break;
			case APPENDER_TYPE_FILE:
				appender->fp_config = imi_new0(struct _rolling_file_s, 1);

				// open文件
				if (configString != NULL) {
					fp = fopen(configString, "a");
					if (fp == NULL) {
						ERR("open appender file failed: name=%s, file=%s\n", name, configString);
						enabled = FALSE;
						appender->fp = NULL;
					}
					else {
						appender->fp = fp;
						fseek(fp, 0L, SEEK_END);

						long currentSize = ftell(fp);
						((rolling_file_t)appender->fp_config)->currentSize = currentSize; // 读取当前文件大小
					}
				}
				else {
					ERR("file appender %s: filepath is NULL\n", name);
					enabled = FALSE;
					appender->fp = NULL;
				}
				break;

			case APPENDER_TYPE_TCP:
			case APPENDER_TYPE_UDP:
			case APPENDER_TYPE_SYSLOG:
			case APPENDER_TYPE_NULL:
			default:
				enabled = FALSE;
				appender->fp = NULL;
				break;
		}
	}

	imi_free(configString); // 释放configString

	appender->enabled = enabled;
	appender->level = get_level(log_config, name, DEFAULT_APPENDER_LVL); // level

	// color
	if (type == APPENDER_TYPE_STDOUT || type == APPENDER_TYPE_STDERR) {
		int color = get_ini_bool(log_config, name, KEY_APPENDER_COLOR, FALSE);
		((console_config_t)appender->fp_config)->color = color;
	}

	// rolling config
	if (type == APPENDER_TYPE_FILE) {
		int limit = MAX(get_ini_int(log_config, name, KEY_APPENDER_FILE_LIMIT, ELOG_DEFAULT_FILE_LIMIT), ELOG_MIN_FILE_LIMIT);
		long max_size = MAX(get_ini_long(log_config, name, KEY_APPENDER_FILE_SIZE, ELOG_DEFAULT_FILE_MAX_SIZE), ELOG_MIN_FILE_MAX_SIZE);

		((rolling_file_t)appender->fp_config)->limit = limit;
		((rolling_file_t)appender->fp_config)->max_size = max_size;
	}

	// 清除原有filter
	filter_t filter;
	while ((filter = array_dequeue(appender->filters)) != NULL) {
		imi_free(filter->module);
		imi_free(filter);
	}

	// 重新加载new filters。 filter定义为模块+输出的日志级别
	// filterStr格式如下: "m1:2, m2:3, m3:1"  每个模块可以指定输出的level, filter之间用,隔开
	// 如果不指定module的level，该module将使用缺省level
	// 每个appender最多可以指定ELOG_FILTER_MAX_FILTERS个filter
	char* filterStr = get_ini_string(log_config, name, KEY_APPENDER_FILTERS, NULL);
	if (filterStr != NULL) {
		char* filterList[ELOG_FILTER_MAX_FILTERS];
		int filterCount = split(filterStr, WORD_DELIMETER, filterList, ELOG_FILTER_MAX_FILTERS);
		if (filterCount > 0) {
			int i, j;
			for (i = 0; i < filterCount; i++) {
				// 解析每个filter, 把module和level分开，读入到parts中
				char* parts[2];
				int partCount = split(filterList[i], FILTER_DELIMETER, parts, 2);
				if (partCount > 0) {
					filter = imi_new(struct _filter_s, 1);
					filter->module = imi_strdup(parts[0]);
					filter->hash = string_hash(filter->module);
					filter->level = partCount > 1 ? parse_level(parts[1], ELOG_OUTPUT_LVL) : ELOG_OUTPUT_LVL;
					array_enqueue(appender->filters, filter);
				}

				// 释放split出来的资源
				for (j = 0; j < partCount; j++) {
					imi_free(parts[j]);  // split是malloc出来的，使用free
				}

				imi_free(filterList[i]);
			}
		}

		imi_free(filterStr);
	}

	return appender;
}

static inline logger_message_t new_log_message() {
	return imi_new(struct _logger_message_s, 1);
}

static inline void clean_log_message(logger_message_t msg) {
	// 其余字段都应该是常量, 无需处理
}

static inline void release_log_message(logger_message_t msg) {
	clean_log_message(msg);
	imi_free(msg);
}

static bool get_fmt_enabled(logger_t logger, uint8_t level, size_t item) {
	return logger->format[level] & item ? true : false;
}

// 填充缓冲区
// @offset:  buffer可用空间开始的偏移量
// @return:  返回buffer下一个可用空间的offset
static int fill_log_item(char* buffer, int offset, const char* item, int bufferSize) {
	if (item) {
		// the last one should be '\0' and cannot be used
		int availableSize = bufferSize - 1;
		if (offset >= availableSize) {
			return availableSize;
		}

		int available = availableSize - offset;
		int itemSize = strlen(item);
		itemSize = itemSize < available ? itemSize : available;
		memcpy(buffer + offset, item, itemSize);
		return offset + itemSize;
	}
	else {
		return offset;  // item为空直接返回当前位置
	}
}

// 根据logger的format, 将msg写入buffer中,
static int fill_log_buffer(logger_message_t msg, char* buffer, int bufferSize) {
	int offset = 0;

	// fill time info
	struct tm *now = gmtime(&msg->tm);
	snprintf(buffer, TIME_FORMAT_LEN, "%d-%d-%dT%02d:%02d:%02d", 1900 + now->tm_year, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	buffer[TIME_FORMAT_LEN - 1] = ' ';
	offset = TIME_FORMAT_LEN;
	
	// level info
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_LVL)) {
		memcpy(buffer + offset, s_level_output_info[msg->level], LEVEL_FORMAT_LEN);
		offset += LEVEL_FORMAT_LEN;
	}

	// tag info
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_TAG)) {
		offset = fill_log_item(buffer, offset, "[", bufferSize);
		offset = fill_log_item(buffer, offset, msg->tag, bufferSize);
		offset = fill_log_item(buffer, offset, "]", bufferSize);
	}

	// module info
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_MODULE)) {
		offset = fill_log_item(buffer, offset, "[", bufferSize);
		offset = fill_log_item(buffer, offset, msg->module, bufferSize);
		offset = fill_log_item(buffer, offset, "]", bufferSize);
	}

	//  process info
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_P_INFO)) {
		offset = fill_log_item(buffer, offset, s_process_info, bufferSize);
	}

	//  thread info
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_T_INFO)) {
		char cur_thread_info[THREAD_MAX_LEN] = { 0 };
		snprintf(cur_thread_info, THREAD_MAX_LEN, "[%08ld]", msg->thread_id);
		offset = fill_log_item(buffer, offset, cur_thread_info, bufferSize);
	}

	// file, function and line
	if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_FILE | ELOG_FMT_FUNC | ELOG_FMT_LINE)) {
		offset = fill_log_item(buffer, offset, "[", bufferSize);

		bool delimeter = false;

		// file
		if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_FILE)) {
			offset = fill_log_item(buffer, offset, msg->file, bufferSize);
			delimeter = true;
		}

		// function
		if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_FUNC)) {
			if (delimeter) {
				offset = fill_log_item(buffer, offset, " ", bufferSize);
			}

			offset = fill_log_item(buffer, offset, msg->func, bufferSize);
			delimeter = true;
		}

		// line
		if (get_fmt_enabled(msg->logger, msg->level, ELOG_FMT_LINE)) {
			if (delimeter) {
				offset = fill_log_item(buffer, offset, ":", bufferSize);
			}

			char lineBuffer[LINE_MAX_LEN] = { 0 };
			snprintf(lineBuffer, LINE_MAX_LEN, "%ld", msg->line);
			offset = fill_log_item(buffer, offset, lineBuffer, bufferSize);
		}

		offset = fill_log_item(buffer, offset, "]", bufferSize);
	}

	// message
	offset = fill_log_item(buffer, offset, " ", bufferSize);
	offset = fill_log_item(buffer, offset, msg->message, bufferSize);
	buffer[offset] = '\0';
	return offset;
}

// 调用者须保证appender必须为file appender
static void rollover_file(logger_appender_t appender) {
	// 关闭当前文件
	fclose(appender->fp);

	rolling_file_t config = (rolling_file_t)appender->fp_config;
	int lastIndex = config->limit - 1;

	char buffer[ELOG_FILE_MAX_LEN * 2];
	snprintf(buffer, 128, "%s.%d", appender->configString, lastIndex);
	unlink(buffer);

	// 全部往后改名
	char *last = buffer;
	char *current = buffer + ELOG_FILE_MAX_LEN;
	int i;
	for (i = lastIndex; i > 0; i--) {
		snprintf(current, 128, "%s.%d", appender->configString, i);
		rename(current, last);

		char* tmp = last;
		last = current;
		current = tmp;
	}

	rename(appender->configString, last);

	// 重新打开文件
	appender->fp = fopen(appender->configString, "w");
	config->currentSize = 0;
}

// 将日志输出到appender上
static void log_output(logger_message_t msg) {
	int i, j;

	logger_t logger = msg->logger;

	pthread_mutex_lock(&s_appenders_lock);

	char* buffer = logger->buffer;
	buffer[0] = '\0'; // 重置buffer

	for (i = 0; i < logger->appender_count; i++) {
		logger_appender_t appender = logger->appenders[i];

		if (!appender->enabled) {
			continue;
		}

		// module filter
		int numFilters = array_get_size(appender->filters);
		if (msg->module != NULL && numFilters > 0) {
			if (msg->module_hash == 0) {
				msg->module_hash = string_hash(msg->module);
			}

			int matched = 0;
			for (j = 0; j < numFilters; j++) {
				filter_t filter = array_get_value(appender->filters, j);
				if (filter->hash == msg->module_hash && strcmp(filter->module, msg->module) == 0) {
					matched = 1;
					if (filter->level >= msg->level) {
						continue; // 没有满足module定义中
					}
				}
			}

			if (!matched && appender->level < msg->level) {
				continue; // 不满足filter的message按照appender level定义
			}
		}
		else if (appender->level < msg->level) {
			continue;
		}

		// 为了提高效率, logger的行缓冲buffer的填充采用delay load的方式,  即遇到第一个需要输出的appender时,
		// 才去format并生成最终的输出. 第二个需要输出的appender则可以直接输出buffer
		if (msg->logger->buffer[0] == '\0') {
			fill_log_buffer(msg, msg->logger->buffer, ELOG_LINE_BUF_SIZE);
		}

		int writeSize = 0;

		// set appender color
		if ((appender->type == APPENDER_TYPE_STDOUT || appender->type == APPENDER_TYPE_STDERR) &&
			(appender->fp_config != NULL && ((console_config_t)appender->fp_config)->color) &&
			s_console_current_color_idx != msg->level) { // 如果当前颜色标签 和msg需要的颜色标签不一致，才需要更新
			writeSize += fwrite(s_level_output_color[msg->level], 1, strlen(s_level_output_color[msg->level]), appender->fp);
			s_console_current_color_idx = msg->level;
		}

		// write to appender
		writeSize += fwrite(buffer, 1, strlen(buffer), appender->fp);
		writeSize += fwrite("\n", 1, 1, appender->fp);

		// 检查是否需要rollup
		if (appender->type == APPENDER_TYPE_FILE) {
			rolling_file_t config = (rolling_file_t)appender->fp_config;
			config->currentSize += writeSize;

			if (config->currentSize > config->max_size) {
				//DBG("file size=%d (max=%d), start rollover\n", config->currentSize, config->max_size);
				rollover_file(appender);
			}
		}
	}

	pthread_mutex_unlock(&s_appenders_lock);
}

// flush日志输出队列
static void flush_log_output(int flushSize) {
	logger_message_t msg;

	int count = 0;
	while (count < flushSize && (msg = array_dequeue(s_log_msg_queue))) {
		log_output(msg);
		recycle_object(s_log_msg_trash, msg);
		count++;
	}
}

// logger工作线程：负责处理日志事件, 发给appender打印
static void *log_worker_thread_proc(void *data) {
	while (s_log_worker_status) {
		// dequeue an event
		logger_message_t msg = NULL;
		pthread_mutex_lock(&s_log_msg_queue_lock);

		while (s_log_worker_status && array_get_size(s_log_msg_queue) == 0) {
			pthread_cond_wait(&s_log_msg_queue_cond, &s_log_msg_queue_lock);
		}

		if (s_log_worker_status) {
			msg = array_dequeue(s_log_msg_queue);
		}

		pthread_mutex_unlock(&s_log_msg_queue_lock);

		if (s_log_worker_status && msg != NULL) {
			// 目前采用单线程负责所有的appender
			// 如果某个appender输出很慢的话, 后续可以考虑每个appender一个独立线程
			log_output(msg);
			recycle_object(s_log_msg_trash, msg);
		}
	}

	//DBG("logger worker thread quitted\n");
	return NULL;
}

//---------------------------------------------------------------------------
//  接口实现
//---------------------------------------------------------------------------

void load_log_config(const char *iniCfgFile) {
	ini_file_t log_config = open_ini_file(iniCfgFile);
	if (!log_config) {
		ERR("load log cfg file (%s) failed\n", iniCfgFile);
		return;
	}

	int index = 0;
	logger_appender_t newAppenderList[ELOG_MAX_APPENDERS];
	memset(newAppenderList, 0, sizeof(logger_appender_t) * ELOG_MAX_APPENDERS);

	pthread_mutex_lock(&s_appenders_lock);

	// 首先遍历配置文件，更新或加载所有appender
	char* sectionName;
	iterator_ini_section_t iter = get_iterator_ini_section(log_config);
	while (index < ELOG_MAX_APPENDERS && get_next_ini_section(iter, &sectionName)) {
		// 如果有type字段，则是appender
		char* typeString = get_ini_string(log_config, sectionName, KEY_APPENDER_TYPE, NULL);
		if (typeString != NULL) {
			appender_type_t type = parse_appender_type(typeString);
			newAppenderList[index++] = load_log_appender(log_config, type, sectionName);
			imi_free(typeString);
		}
	}

	release_iterator_ini_section(iter);

	// 清除配置文件中已经不存在的appender. 已重用的appender已经在原来的数组里清空了
	int i;
	for (i = 0; i < ELOG_MAX_APPENDERS; i++) {
		if (s_appenders[i] != NULL) {
			release_log_appender(s_appenders[i]);
			s_appenders[i] = NULL;
		}
	}

	// copy 新appenders到appender数组中
	memcpy(s_appenders, newAppenderList, sizeof(logger_appender_t) * index);
	s_appender_count = index;

	// 再遍历一次配置文件，更新或加载所有logger
	// 如果运行代码中已经缓存了logger实例，如果重新加载配置时释放的话，可能会导致悬空引用
	// 因此在重新加载时，并不释放logger。而是先把所有的logger置为false.
	// 后续如果logger继续使用，则会在读取配置时变为TRUE
	for (i = 0; i < s_logger_count; i++) {
		s_logger_list[i]->enabled = FALSE;
	}

	iter = get_iterator_ini_section(log_config);
	while (index < ELOG_MAX_APPENDERS && get_next_ini_section(iter, &sectionName)) {
		if (strcmp(LOGGER_SECTION_NAME, sectionName) == 0) {
			// 缺省logger的名字永远是logger
			load_log_logger(s_default_logger, log_config, sectionName);
		}
		else if (ini_file_is_key_exist(log_config, sectionName, KEY_APPENDER)) {
			logger_t logger = find_logger(sectionName);
			if (logger == NULL) {
				logger = create_logger(sectionName);
				s_logger_list[s_logger_count++] = logger;
			}

			load_log_logger(logger, log_config, sectionName);
		}
	}

	release_iterator_ini_section(iter);

	pthread_mutex_unlock(&s_appenders_lock);

	release_ini_file(log_config);
}

void log_init(const char *iniCfgFile) {
	// logger数组
	s_logger_count = 0;
	memset(s_logger_list, 0, sizeof(logger_t) * ELOG_MAX_LOGGERS);

	pthread_mutex_init(&s_appenders_lock, NULL);

	// 初始化预定义的
	s_default_logger = get_logger(LOGGER_SECTION_NAME);
	s_default_logger->enabled = true;
	s_logger_libcommon = get_logger(LIB_COMMON_LOGGER);

	// 初始化process info
	snprintf(s_process_info, PROCESS_MAX_LEN, "[p:%04d]", get_current_process_id());

	// 初始化appenders队列
	s_appender_count = 0;
	memset(s_appenders, 0, sizeof(logger_appender_t) * ELOG_MAX_APPENDERS);

	// load from config file
	load_log_config(iniCfgFile);

	// 初始化msg回收队列
	s_log_msg_trash = create_trash(new_log_message, release_log_message, clean_log_message, ELOG_MAX_TRASH_LEN);

#ifdef ELOG_ASYNC_OUTPUT

	// 异步写时，需要增加日志线程和日志消息缓冲队列
	pthread_mutex_init(&s_log_msg_queue_lock);
	g_cond_init(&s_log_msg_queue_cond);
	s_log_msg_queue = create_array(ELOG_INIT_QUEUE_SIZE, ELOG_MAX_QUEUE_SIZE);

	// start worker thread
	s_log_worker_status = RUNNING;
	s_logger_worker_thread = g_thread_new("elog", log_worker_thread_proc, NULL);
	if (s_logger_worker_thread == NULL) {
		ERR(LOG_RED "start logger thread failed");
	}
#endif
}

void log_init_from_pwd(const char *psz_name) {
	char sz_startup_directory[256];
	char sz_log_config_path[256];
	char sz_executeable[32];
	int bytes;

	sprintf(sz_executeable, "/proc/%d/exe", getpid());
	bytes = elog_min(readlink(sz_executeable, sz_startup_directory, sizeof(sz_startup_directory)), sizeof(sz_startup_directory) - 1);
	if (bytes >= 0) {
		sz_startup_directory[bytes] = '\0';
	}
	char *p_path_char_pos = strrchr(sz_startup_directory, '/');
	if (NULL != p_path_char_pos) {
		*p_path_char_pos = '\0';
	}
	snprintf(sz_log_config_path, sizeof(sz_log_config_path), "%s/%s", sz_startup_directory, psz_name);
	log_init(sz_log_config_path);
}

void log_uninit() {
	// first set logger to disable
	int i;
	for (i = 0; i < s_logger_count; i++) {
		s_logger_list[i]->enabled = false;
	}

#ifdef ELOG_ASYNC_OUTPUT
	// stop threads by setting status
	s_log_worker_status = STOP;

	// send cond signal (twice to ensure) to thread if it is waiting
	pthread_mutex_lock(&s_log_msg_queue_lock);
	g_cond_signal(&s_log_msg_queue_cond);
	g_cond_signal(&s_log_msg_queue_cond);
	pthread_mutex_unlock(&s_log_msg_queue_lock);

	g_thread_join(s_logger_worker_thread);

	// flush and release existing events
	if (get_size_array(s_log_msg_queue) > 0) {
		flush_log_output(ELOG_MAX_QUEUE_SIZE);
	}

	release_array(s_log_msg_queue);

	pthread_mutex_destroy(&s_log_msg_queue_lock);
	g_cond_clear(&s_log_msg_queue_cond);

#endif

	// release recycle queue
	release_trash(s_log_msg_trash);

	// release appenders
	for (i = 0; i < s_appender_count; i++) {
		release_log_appender(s_appenders[i]);
	}

	for (i = 0; i < s_logger_count; i++) {
		release_logger(s_logger_list[i]);
	}

	s_appender_count = 0;
	pthread_mutex_destroy(&s_appenders_lock);
}

void log_queue_output(logger_t logger, uint8_t level, const char *tag, const char *module, const char *file, const char *func,
	long line, const char *format, ...) {
	if (!logger || !logger->enabled || logger->level < level) {
		return;
	}

	logger_message_t msg = request_object(s_log_msg_trash);
	if (msg != NULL) {
		memset(msg, 0, sizeof(struct _logger_message_s));

		msg->level = level;
		msg->logger = logger;

		msg->thread_id = get_current_thread_id();
		msg->module = module;
		msg->tag = tag;
		msg->file = file;
		msg->func = func;
		msg->line = line;
		msg->tm = time(NULL);// g_date_time_new_now_utc(); //g_date_time_new_now_local();

		va_list args;
		va_start(args, format);
		vsnprintf(msg->message, ELOG_BUF_SIZE - 1, format, args);
		va_end(args);

#ifdef ELOG_ASYNC_OUTPUT
		// 如果是异步日志输出，则将日志消息入队
		pthread_mutex_lock(&s_log_msg_queue_lock);

		if (get_size_array(s_log_msg_queue) >= ELOG_MAX_QUEUE_SIZE) {
			// 如果当前队列长度已经到达最大队列长度，则在本线程中flush一半
			flush_log_output(ELOG_MAX_QUEUE_SIZE / 2);
		}

		enqueue_array(s_log_msg_queue, msg);
		g_cond_signal(&s_log_msg_queue_cond);
		pthread_mutex_unlock(&s_log_msg_queue_lock);
#else
		log_output(msg);
		recycle_object(s_log_msg_trash, msg);
#endif
	}
}

void log_set_output_enabled(logger_t logger, bool enabled) {
	if (logger != NULL) {
		logger->enabled = enabled;
	}
}

int log_get_output_enabled(logger_t logger) {
	return logger != NULL ? logger->enabled : FALSE;
}

void log_set_fmt(logger_t logger, uint8_t level, uint8_t set) {
	if (logger != NULL && level < ELOG_LVL_ALL) {
		logger->format[level] = set;
	}
}

logger_t get_logger(const char* name) {
	logger_t logger = NULL;

	pthread_mutex_lock(&s_appenders_lock);

	if (s_logger_count < ELOG_MAX_LOGGERS) {
		logger = find_logger(name);
		if (logger == NULL) {
			logger = create_logger(name);
			s_logger_list[s_logger_count++] = logger;
		}
	}

	pthread_mutex_unlock(&s_appenders_lock);

	return logger;
}

