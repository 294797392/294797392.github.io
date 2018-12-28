/**************************************************************************
* @ file    : inihandler.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.08.20
* @ brief   : ini文件操作的封装
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __INI_HANDLER_H__
#define __INI_HANDLER_H__

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// 记录ini key file的数据结构
//----------------------------------------------------------------------------
typedef void* ini_file_t;

//----------------------------------------------------------------------------
// 遍历ini文件sections的iterator
//----------------------------------------------------------------------------
typedef struct _iterator_ini_section_s *iterator_ini_section_t;

//----------------------------------------------------------------------------
// 遍历ini文件kvpair的iterator
//----------------------------------------------------------------------------
typedef struct _iterator_ini_kvpair_s *iterator_ini_kvpair_t;

//----------------------------------------------------------------------------
// 打开一个ini文件
//
// @filePath       ini文件路径
// @return         成功，已打开的ini文件结构，错误NULL,
//----------------------------------------------------------------------------
ini_file_t open_ini_file(const char *filePath);

//----------------------------------------------------------------------------
// 创建一个全新的ini文件结构
//
// @return         成功，已打开的ini文件结构，错误NULL,
//----------------------------------------------------------------------------
ini_file_t create_ini_file();

//----------------------------------------------------------------------------
// 将一个ini文件结构保持到文件中
// @iniFile        需要保存的ini文件结构
// @filePath       ini文件路径
// @return         0成功，其他错误编码
//----------------------------------------------------------------------------
int save_ini_file(const char *filePath, ini_file_t iniFile);

//----------------------------------------------------------------------------
// 释放ini文件结构
//
// @iniFile        需要释放的ini文件结构
//----------------------------------------------------------------------------
void release_ini_file(ini_file_t iniFile);

//----------------------------------------------------------------------------
// 检查ini文件结构中指定section是否存在
// @iniFile        需要保存的ini文件结构
// @section        section名称
// @return         TRUE 存在; FALSE 不存在
//----------------------------------------------------------------------------
BOOL ini_file_is_section_exist(ini_file_t iniFile, const char* section);

//----------------------------------------------------------------------------
// 检查ini文件结构中指定key是否存在
// @iniFile        需要保存的ini文件结构
// @section        section名称
// @key            指定的key
// @return         TRUE 存在; FALSE 不存在
//----------------------------------------------------------------------------
BOOL ini_file_is_key_exist(ini_file_t iniFile, const char* section, const char* key);

//----------------------------------------------------------------------------
// 读取ini文件中的字符串。返回值会经过trim. 需要注意的是，返回值是新分配的字符
// 调用者需要自己调用free释放返回的字符串
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
char* get_ini_string(ini_file_t iniFile, const char* section, const char* key, const char* defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_string(ini_file_t iniFile, const char* section, const char *key, const char* value);

//----------------------------------------------------------------------------
// 读取ini文件中的BOOL值
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
BOOL get_ini_bool(ini_file_t iniFile, const char* section, const char* key, BOOL defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_bool(ini_file_t iniFile, const char* sectionName, const char *key, BOOL value);

//----------------------------------------------------------------------------
// 读取ini文件中的int值
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
int get_ini_int(ini_file_t iniFile, const char* section, const char* key, const int defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_int(ini_file_t iniFile, const char* sectionName, const char *key, int value);

//----------------------------------------------------------------------------
// 读取ini文件中的long值
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
long get_ini_long(ini_file_t iniFile, const char* section, const char* key, const long defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_long(ini_file_t iniFile, const char* sectionName, const char *key, long value);

//----------------------------------------------------------------------------
// 读取ini文件中的long long值
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
long long get_ini_longlong(ini_file_t iniFile, const char* section,
    const char* key, const long long defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_longlong(ini_file_t iniFile, const char* sectionName,
    const char *key, long long value);


//----------------------------------------------------------------------------
// 读取ini文件中的double值
//
// @iniFile        需要读取的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         key所对应的value值，或者default value
//----------------------------------------------------------------------------
double get_ini_double(ini_file_t iniFile, const char* section,
    const char* key, const double defaultValue);

//----------------------------------------------------------------------------
// 将指定的key-value值写入对应的iniFile中
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @defaultValue   指定key值不存在或无效时，需要返回的default value
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int set_ini_double(ini_file_t iniFile, const char* sectionName,
    const char *key, double value);


//----------------------------------------------------------------------------
// 删除指定的key
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @key            指定的key
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int ini_remove_key(ini_file_t iniFile, const char* sectionName, const char *key);

//----------------------------------------------------------------------------
// 删除指定的section
//
// @iniFile        需要写入的ini文件结构
// @section        指定的section
// @return         0 成功，其他错误编码
//----------------------------------------------------------------------------
int ini_remove_section(ini_file_t iniFile, const char* section);

//----------------------------------------------------------------------------
// 获取ini文件section遍历器
//
// @iniFile        ini文件结构
// @return         ini文件section iterator, NULL 失败
//----------------------------------------------------------------------------
iterator_ini_section_t get_iterator_ini_section(ini_file_t iniFile);

//----------------------------------------------------------------------------
// 遍历ini文件sections
//
// @iniFile        ini文件结构
// @iter           ini section遍历器
// @section[out]   如果下一个还有值, 则返回下一个section的name
// @return         1: 后续还有值; 0:后续已经没有值了
//----------------------------------------------------------------------------
int get_next_ini_section(iterator_ini_section_t iter, char** section);

//----------------------------------------------------------------------------
// 释放ini文件group遍历器
// @iter：  iterator
//----------------------------------------------------------------------------
void release_iterator_ini_section(iterator_ini_section_t iter);

//----------------------------------------------------------------------------
// 获取ini文件kvpair遍历器
//
// @iniFile        ini文件结构
// @section        需要遍历的section
// @return         ini文件iterator, NULL 失败
//----------------------------------------------------------------------------
iterator_ini_kvpair_t get_iterator_ini_kvpair(ini_file_t iniFile, const char* section);

//----------------------------------------------------------------------------
// 遍历ini文件kvpairs. 需要注意的是, 返回的key和value值都是新分配的值，
// 调用者需要自行free
//
// @iniFile        ini文件结构
// @iter           ini kvpair遍历器
// @key[out]       如果后续还有值, 则返回下一个key
// @value[out]     如果后续还有值, 则返回下一个key对应的value
// @return         1: 后续还有值; 0:后续已经没有值了
//----------------------------------------------------------------------------
int get_next_ini_kvpair(iterator_ini_kvpair_t iter, char** key, char** value);

//----------------------------------------------------------------------------
// 释放ini文件kvpair遍历器
// @iter:   iterator
//----------------------------------------------------------------------------
void release_iterator_ini_kvpair(iterator_ini_kvpair_t iter);

#ifdef __cplusplus
}
#endif

#endif // __INI_HANDLER_H__

