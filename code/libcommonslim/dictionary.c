/**************************************************************************
* @ file    :   dictionary.c
* @ author  :   yang yang
* @ version :   0.9
* @ date    :   2017.07.05
* @ brief   :   字典类的实现
* @Copyright (c)2016  chuangmi inc.
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "string_utils.h"
#include "dictionary.h"

//---------------------------------------------------------------------------
//  参数定义
//---------------------------------------------------------------------------
#define DICT_LOAD_FACTOR    0.75

//---------------------------------------------------------------------------
//  内部数据结构
//---------------------------------------------------------------------------

typedef struct _dict_entry_s *_dict_entry_t;
struct _dict_entry_s {
    void *key;
    void *value;

    unsigned hash;
    _dict_entry_t next;
};

typedef struct _dictionary_s {
    int size;       // 当前entry的条数
    int capacity;   // 字典容量
    int init_capacity;

    imi_hash_func       hash_func;      // key的hash函数
    element_comparer    key_equal_func; // key值比较函数
    key_duplicate_func  key_dup_func;
    key_release_func    key_release_func;

    _dict_entry_t*      entries;    // 采用拉链法构造散列表
} _dictionary_s;

struct _iterator_dictionary_s {
    int pos;
    _dict_entry_t entry;
    dictionary_t dict;
};

//---------------------------------------------------------------------------
//  内部函数
//---------------------------------------------------------------------------

// @prev    [out] 查找到的节点的上一个节点, 为空表示查找到节点为首节点
// @entry   [out] 查找到的节点
// @return  查找到的节点在entries中的index
static unsigned dictionary_lookup_node_internal(dictionary_t dict, const void* key,
    _dict_entry_t* prev, _dict_entry_t* entry) {
    unsigned hash = dict->hash_func(key);
    unsigned index = hash % dict->capacity;

    _dict_entry_t p = NULL;
    _dict_entry_t current = dict->entries[index];
    while (current != NULL) {
        if (current->hash == hash && dict->key_equal_func(current->key, key) == 0) {
            break; // hash一致也需要判断key是否相等, 防止hash碰撞
        }

        *prev = current;
        current = current->next;
    }

    *prev = p;
    *entry = current;
    return index;
}

static void dictionary_insert_node_internal(dictionary_t dict, const void* key, const void* value, void** oldValue) {
    unsigned hash = dict->hash_func(key);
    unsigned index = hash % dict->capacity;

    _dict_entry_t entry = dict->entries[index];
    while (entry != NULL) {
        if (entry->hash == hash && dict->key_equal_func(entry->key, key) == 0) {
            break;
        }
        else {
            entry = entry->next;
        }
    }

    if (entry == NULL) {
        entry = imi_malloc(sizeof(struct _dict_entry_s));
        entry->hash = hash;
        entry->key = dict->key_dup_func ? dict->key_dup_func(key) : (void*)key;
        entry->next = dict->entries[index];
        dict->entries[index] = entry;

        dict->size++;
    }
    else if (oldValue != NULL) {
        *oldValue = entry->value;
    }

    entry->value = (void*)value;
}

static void* remove_key_dictionary_internal(dictionary_t dict, const void* key) {
    void* value = NULL;
    _dict_entry_t entry, prev;
    unsigned index = dictionary_lookup_node_internal(dict, key, &prev, &entry);
    if (entry) {
        if (prev) {
            prev->next = entry->next;
        }
        else {
            dict->entries[index] = entry->next;
        }

        value = entry->value;
        if (dict->key_release_func) {
            dict->key_release_func(entry->key);
        }

        imi_free(entry);

        dict->size--;
    }

    return value;
}

// @enlarge:  1: 扩大容量; 0: 缩减容量
static int dictionary_resize(dictionary_t dict, int capacity) {
    size_t size = sizeof(_dict_entry_t)* capacity;
    _dict_entry_t *entries = imi_malloc(size);
    if (!entries) {
        return CODE_MALLOC_FAILED;
    }

    memset(entries, 0, size);

    // 将dictionary里原有的每个entry重新hash定位
    int i;
    _dict_entry_t entry;
    for (i = 0; i < dict->capacity; i++) {
        while ((entry = dict->entries[i]) != NULL) {
            dict->entries[i] = entry->next;

            unsigned index = entry->hash % capacity;
            entry->next = entries[index];
            entries[index] = entry;
        }
    }

    imi_free(dict->entries);

    dict->capacity = capacity;
    dict->entries = entries;
    return CODE_SUCCESS;
}

//---------------------------------------------------------------------------
//  接口实现
//---------------------------------------------------------------------------

dictionary_t create_dictionary(
    int init_capacity,
    imi_hash_func hash_func,
    element_comparer key_equal_func,
    key_duplicate_func key_dup_func,
    key_release_func key_release_func) {

    dictionary_t dict = (dictionary_t)imi_malloc(sizeof(_dictionary_s));
    dict->init_capacity = dict->capacity = MAX(DICT_MIN_CAPACITY, init_capacity);
    dict->size = 0;

    dict->hash_func = hash_func;
    dict->key_equal_func = key_equal_func;
    dict->key_dup_func = key_dup_func;
    dict->key_release_func = key_release_func;

    size_t size = sizeof(_dict_entry_t)* dict->capacity;
    dict->entries = imi_malloc(size);
    if (!dict->entries) {
        imi_free(dict);
        return NULL;
    }

    memset(dict->entries, 0, size);
    return dict;
}

void release_dictionary(dictionary_t dict) {
    if (dict) {
        int i;
        for (i = 0; i < dict->capacity; i++) {
            _dict_entry_t entry = dict->entries[i];
            while (entry) {
                _dict_entry_t next = entry->next;

                if (dict->key_release_func) {
                    dict->key_release_func(entry->key);
                }

                imi_free(entry);
                entry = next;
            }
        }

        imi_free(dict->entries);
        imi_free(dict);
    }
}

int get_size_dictionary(dictionary_t dict) {
    return dict ? dict->size : CODE_INVALID_PARAM;
}

int get_capacity_dictionary(dictionary_t dict) {
    return dict ? dict->capacity : CODE_INVALID_PARAM;
}

void* get_value_dictionary(dictionary_t dict, const void *key, void* defaultValue) {
    _dict_entry_t entry, prev;
    dictionary_lookup_node_internal(dict, key, &prev, &entry);
    return entry ? entry->value : defaultValue;
}

int set_value_dictionary(dictionary_t dict, const void *key, const void *val, void** oldValue) {
    if (dict == NULL || key == NULL || val == NULL) {
        return CODE_INVALID_PARAM;
    }

    // ensure size
    int threshold = (int)(dict->capacity * DICT_LOAD_FACTOR);
    if (threshold < dict->size + 1) {
        int new_capacity = dict->capacity * 2;
        int ret = dictionary_resize(dict, new_capacity);
        if (ret != CODE_SUCCESS) {
            return ret;
        }
    }

    dictionary_insert_node_internal(dict, key, val, oldValue);
    return CODE_SUCCESS;
}

void* remove_key_dictionary(dictionary_t dict, const void *key) {
    if (!key || !dict) {
        return NULL;
    }

    int threshold = (int)(dict->capacity / 2 * DICT_LOAD_FACTOR);
    if (dict->size - 1 < threshold) {
        int new_capacity = MAX(dict->capacity / 2, DICT_MIN_CAPACITY);
        if (new_capacity < dict->capacity) {
            dictionary_resize(dict, new_capacity);
        }
    }

    return remove_key_dictionary_internal(dict, key);
}

int is_key_exist_dictionary(dictionary_t dict, const void *key) {
    if (!key || !dict) {
        return 0;
    }

    _dict_entry_t prev, entry;
    dictionary_lookup_node_internal(dict, key, &prev, &entry);
    return entry ? 1 : 0;
}

iterator_dictionary_t get_iterator_dictionary(dictionary_t dict) {
    iterator_dictionary_t iter = imi_malloc(sizeof(struct _iterator_dictionary_s));
    iter->pos = 0;
    iter->dict = dict;
    iter->entry = NULL;
    return iter;
}

int get_next_dictionary(iterator_dictionary_t iter, void **key, void** value) {
    if (!iter || !iter->dict || !key || !value) {
        return 0;
    }

    do {
        if (!iter->entry) {
            iter->entry = iter->dict->entries[iter->pos];
        }
        else {
            iter->entry = iter->entry->next;
        }

        if (iter->entry) {
            *key = iter->entry->key;
            *value = iter->entry->value;
            return 1;
        }

        iter->pos++;
    } while (iter->pos < iter->dict->capacity);

    return 0;
}

void release_iterator_dictionary(iterator_dictionary_t iter) {
    if (iter) {
        imi_free(iter);
    }
}
