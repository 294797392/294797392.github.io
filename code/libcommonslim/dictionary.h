/**************************************************************************************
* @ file    :   dictionary.h
* @ author  :   yang yang
* @ version :   0.9
* @ date    :   2017.07.05
* @ brief   :   字典类的实现
* @ todo    :   目前为了简单，采用线性表管理hash. 后续如果处理大规模字典时
*               可以考虑使用红黑树管理hash
* @Copyright (c)2016  chuangmi inc.
***************************************************************************************/

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DICT_MIN_CAPACITY   32

//-------------------------------------------------------------------------------------
// 释放dictionary数据结构宏
// @dict:                   需要释放的array
// @ptr_func_release_value  释放value的函数指针: void (*ptr_func_release_value)(void*) 
//                          如果不需要释放value, 则设为NULL
//-------------------------------------------------------------------------------------
#define RELEASE_DICTIONARY(dict, ptr_func_release_value) \
{ \
    if(ptr_func_release_value) { \
        void (*ptr_func)(void*); ptr_func = ptr_func_release_value; \
        iterator_dictionary_t __iter = get_iterator_dictionary(dict); \
        if(__iter) { \
            void *__key, *__value; \
            while (get_next_dictionary(__iter, &__key, &__value)) ptr_func(__value); \
            release_iterator_dictionary(__iter); \
        } \
    } \
    release_dictionary(dict); \
}

//-------------------------------------------------------------------------------------
//  key复制函数定义
//-------------------------------------------------------------------------------------
typedef void* (*key_duplicate_func)(const void* key);

//-------------------------------------------------------------------------------------
//  key释放函数
//-------------------------------------------------------------------------------------
typedef void (*key_release_func)(const void* key);

//--------------------------------------------------------------------------------------
// dictionary的数据结构定义
//--------------------------------------------------------------------------------------
typedef struct _dictionary_s* dictionary_t;

//--------------------------------------------------------------------------------------
// 遍历dictionary的iterator定义
//--------------------------------------------------------------------------------------
typedef struct _iterator_dictionary_s *iterator_dictionary_t;

//--------------------------------------------------------------------------------------
// 创建字典对象.
// @init_capacity:      初始容量. 如果小于DICT_MIN_CAPACITY，则等于该值
// @hash_func           key的哈希函数（不可空）
// @key_equal_func      key的比较函数，用于判断2个key值是否相等 (不可空)
// @key_dup_func        key的复制函数，可设为空. 如果该函数为空，则字典会使用
//                      输入key作为记录。此时外部调用者需保证key在整个生命周期
//                      中不被释放，
// @key_release_func    key的释放函数。在释放字典或删除key时会调用该函数
// @return:             创建的字典对象. NULL 失败
//--------------------------------------------------------------------------------------
dictionary_t create_dictionary(
    int init_capacity,
    imi_hash_func key_hash_func,
    element_comparer key_equal_func,
    key_duplicate_func key_dup_func,
    key_release_func key_release_func
);

//--------------------------------------------------------------------------------------
// 释放字典对象以及内部分配的数据结构。
// 需要注意的是dictionary只管理key和value的指针, 并不复制他们.
// 因此在释放dictionary时，如果key和value需要释放的话，需要调用者需要自己释放
// 调用遍历函数将key，value的内存释放掉。
//
// @dict:   需要释放的字典。
//--------------------------------------------------------------------------------------
void release_dictionary(dictionary_t dict);

//--------------------------------------------------------------------------------------
//  读取字典中的键值对的数量
//  @dict:  需要操作的字典。
//  @return:大于等于0，字典内键值对的数量; 小于0，错误代码
//--------------------------------------------------------------------------------------
int get_size_dictionary(dictionary_t dict);

//--------------------------------------------------------------------------------------
// 读取字典当前分配的空间大小
// @return: 大于0, 数组的容量。小于0: 错误编码
//--------------------------------------------------------------------------------------
int get_capacity_dictionary(dictionary_t dict);

//--------------------------------------------------------------------------------------
// 读取字典中的值
// @dict:        字典实例
// @key:         需要释放的字典
// @defaultValue:如果没有找到值的话，返回的缺省值
//--------------------------------------------------------------------------------------
void* get_value_dictionary(dictionary_t dict, const void *key, void* defaultValue);

//--------------------------------------------------------------------------------------
// 设置字典Key-Value. 如果原有key上已经有值，返回
// @dict:   字典实例
// @key:    需要设置的key值
// @val:    [out],如果val不为空，且set的key上已经有值，则返回该值
// @return: 0 成功，其他错误编码
//--------------------------------------------------------------------------------------
int set_value_dictionary(dictionary_t dict, const void *key, const void *val, void** oldVal);

//--------------------------------------------------------------------------------------
// 删除字典Key的key
// @dict:   字典实例
// @key:    需要释放的键值
// @return  如果要删除的key存在，则返回对应的value值，否则返回NULL
//--------------------------------------------------------------------------------------
void* remove_key_dictionary(dictionary_t dict, const void *key);

//--------------------------------------------------------------------------------------
// 检测Key是否存在
// @dict:   字典实例
// @key:    需要释放的键值
// @return: 1：存在，0 不存在
//--------------------------------------------------------------------------------------
int is_key_exist_dictionary(dictionary_t dict, const void *key);

//--------------------------------------------------------------------------------------
// 获取遍历字典类的iterator
//--------------------------------------------------------------------------------------
iterator_dictionary_t get_iterator_dictionary(dictionary_t dict);

//--------------------------------------------------------------------------------------
// 遍历字典
// @iter:   iterator
// @key:    [out] 输出的key
// @value:  [out] 输出的value
// @return: 0: 遍历已经结束，其他: 字典中还有值没有遍历
//--------------------------------------------------------------------------------------
int get_next_dictionary(iterator_dictionary_t iter, void**key, void** value);

//--------------------------------------------------------------------------------------
// 释放字典iterator
// @iter:   iterator
//--------------------------------------------------------------------------------------
void release_iterator_dictionary(iterator_dictionary_t iter);

#ifdef __cplusplus
}
#endif

#endif  // __DICTIONARY_H__
