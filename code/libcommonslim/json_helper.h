/**************************************************************************
* @file     : json_helper.h
* @author   : yang yang
* @version  : 0.9
* @date     : 2016.08.12
* @brief    : 提供json操作的工具函数以及宏定义
* -------------------------------------------------------------------------
* History:
* 2018/02/08 qinhj: code review, ...
* 2018/02/09 qinhj: add JSON_SERIALIZE_ptr_struct, JSON_DESERIALIZE_PTR_STRUCT
* 2018/02/11 qinhj:
*   1. replace all 'calloc' by 'imi_calloc';
*   2. test (de)serialize for type void *, void **;
*   3. bug fix: endless loop when __jelem or __jchild is NULL;
* -------------------------------------------------------------------------
* Note:
* 1. __value, __attribute, ... is keep works for gcc, avoid to use them;
* 2. When use macro define, pay attention to operation '++' and '--';
* 3. Check if NULL == json_obj, before use macro define. Otherwise, memory leak. <-- remain: opt with code review
***************************************************************************/

#ifndef __JSON_HELPER_H__
#define __JSON_HELPER_H__

#include "defs.h"   // need for: imi_calloc, ...
#include "cJSON.h"

#define SIZE_OF_int sizeof(int)
#define SIZE_OF_double sizeof(double)
#define SIZE_OF_string sizeof(char *)

#define JSON_SERIALIZE_int(json_obj, json_attr, struct_ptr, attribute) cJSON_AddNumberToObject(json_obj, #json_attr, (struct_ptr)->attribute);
#define JSON_SERIALIZE_double(json_obj, json_attr, struct_ptr, attribute) cJSON_AddNumberToObject(json_obj, #json_attr, (struct_ptr)->attribute);
#define JSON_SERIALIZE_string(json_obj, json_attr, struct_ptr, attribute) cJSON_AddStringToObject(json_obj, #json_attr, (struct_ptr)->attribute);
#define JSON_SERIALIZE_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr) \
    { cJSON *__jchild = cJSON_CreateObject(); if (struct_ptr && func_ptr) func_ptr(__jchild, &((struct_ptr)->attribute)); cJSON_AddItemToObject(json_obj, #json_attr, __jchild); }
#define JSON_SERIALIZE_ptr_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr) \
    { cJSON *__jchild = cJSON_CreateObject(); if (struct_ptr && func_ptr) func_ptr(__jchild, (struct_ptr)->attribute); cJSON_AddItemToObject(json_obj, #json_attr, __jchild); }

#define JSON_SERIALIZE_ARRAY_int(json_obj, struct_ptr, attribute, index) cJSON_AddItemToArray(json_obj, cJSON_CreateNumber((struct_ptr)->attribute[index]));
#define JSON_SERIALIZE_ARRAY_double(json_obj, struct_ptr, attribute, index) cJSON_AddItemToArray(json_obj, cJSON_CreateNumber((struct_ptr)->attribute[index]));
#define JSON_SERIALIZE_ARRAY_string(json_obj, struct_ptr, attribute, index) cJSON_AddItemToArray(json_obj, cJSON_CreateString((struct_ptr)->attribute[index]));
#define JSON_SERIALIZE_ARRAY(json_obj, json_attr, struct_ptr, attribute, type, size) \
    { \
        int __index = 0; \
        cJSON *__jarray = cJSON_CreateArray(); \
        if (struct_ptr && __jarray) { \
            while (__index < size) { \
                JSON_SERIALIZE_ARRAY_##type(__jarray, struct_ptr, attribute, __index++); \
            } \
            cJSON_AddItemToObject(json_obj, #json_attr, __jarray); \
        } \
    }
#define JSON_SERIALIZE_PTR_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type) \
    { \
        int __index = 0; \
        cJSON *__jarray = cJSON_CreateArray(); \
        if (struct_ptr && __jarray) { \
            while (__index < (struct_ptr)->cnt_attribute) { \
            JSON_SERIALIZE_ARRAY_##type(__jarray, struct_ptr, attribute, __index++); \
            } \
            cJSON_AddItemToObject(json_obj, #json_attr, __jarray); \
        } \
    }

#define JSON_SERIALIZE_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, func_ptr, size) \
    { \
        cJSON *__jchildarray, *__jchild; int __index = 0; \
        __jchildarray = cJSON_CreateArray(); cJSON_AddItemToObject(json_obj, #json_attr, __jchildarray); \
        if (struct_ptr && func_ptr) { \
            while (__index < size) { \
                __jchild = cJSON_CreateObject(); \
                if (func_ptr(__jchild, &((struct_ptr)->attribute[__index++])) == 0) { cJSON_AddItemToArray(__jchildarray, __jchild); } \
                else { cJSON_Delete(__jchild); } \
            } \
        } \
    }
#define JSON_SERIALIZE_PTR_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, func_ptr) \
    { \
        cJSON *__jchildarray, *__jchild; int __index = 0; \
        __jchildarray = cJSON_CreateArray(); cJSON_AddItemToObject(json_obj, #json_attr, __jchildarray); \
        if (struct_ptr && func_ptr) { \
            while (__index < (struct_ptr)->cnt_attribute) { \
                __jchild = cJSON_CreateObject(); \
                if (func_ptr(__jchild, &((struct_ptr)->attribute[__index++])) == 0) { cJSON_AddItemToArray(__jchildarray, __jchild); } \
                else { cJSON_Delete(__jchild); } \
            } \
        } \
    }

#define JSON_DESERIALIZE_int(json_obj, json_attr, struct_ptr, attribute) \
    { cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); if (struct_ptr && __jchild) (struct_ptr)->attribute = __jchild->valueint; }
#define JSON_DESERIALIZE_double(json_obj, json_attr, struct_ptr, attribute) \
    { cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); if (struct_ptr && __jchild) (struct_ptr)->attribute = __jchild->valuedouble; }
#define JSON_DESERIALIZE_string(json_obj, json_attr, struct_ptr, attribute) \
    { cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); if (struct_ptr && __jchild) (struct_ptr)->attribute = imi_strdup(__jchild->valuestring); }
#define JSON_DESERIALIZE_char_array(json_obj, json_attr, struct_ptr, attribute, max) \
    { cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); if (struct_ptr && __jchild) strncpy((struct_ptr)->attribute, __jchild->valuestring, max); }
#define JSON_DESERIALIZE_STRUCT(json_obj, json_attr, struct_ptr, attribute, func_ptr) \
    { cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); if (struct_ptr && __jchild && func_ptr) func_ptr(__jchild, &((struct_ptr)->attribute)); }
#define JSON_DESERIALIZE_PTR_STRUCT(json_obj, json_attr, struct_ptr, attribute, struct_type, func_ptr) \
    { \
        cJSON* __jchild = cJSON_GetObjectItem(json_obj, #json_attr); \
        (struct_ptr)->attribute = (struct_type *)imi_calloc(1, sizeof(struct_type)); \
        if (struct_ptr && __jchild && func_ptr) func_ptr(__jchild, (struct_ptr)->attribute); \
    }

#define JSON_DESERIALIZE_ARRAY_int(json_elem, struct_ptr, attribute, index) (struct_ptr)->attribute[index] = json_elem->valueint;
#define JSON_DESERIALIZE_ARRAY_double(json_elem, struct_ptr, attribute, index) (struct_ptr)->attribute[index] = json_elem->valuedouble;
#define JSON_DESERIALIZE_ARRAY_string(json_elem, struct_ptr, attribute, index) (struct_ptr)->attribute[index] = imi_strdup(json_elem->valuestring);
#define JSON_DESERIALIZE_ARRAY(json_obj, json_attr, struct_ptr, attribute, type, max) \
    { \
        cJSON *__jarray, *__jelem; int __index = 0, __size = 0; \
        __jarray = cJSON_GetObjectItem(json_obj, #json_attr); \
        if (struct_ptr && __jarray) { \
            __size = cJSON_GetArraySize(__jarray); __size = max <= __size ? max : __size;  \
            while (__index < __size) { \
                __jelem = cJSON_GetArrayItem(__jarray, __index); \
                if (__jelem) JSON_DESERIALIZE_ARRAY_##type(__jelem, struct_ptr, attribute, __index); \
                __index++; \
            } \
        } \
    }
#define JSON_DESERIALIZE_PTR_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type) \
    { \
        cJSON *__jarray, *__jelem; int __index = 0, __size = 0; \
        __jarray = cJSON_GetObjectItem(json_obj, #json_attr); \
        if (struct_ptr && __jarray) { \
            __size = cJSON_GetArraySize(__jarray); (struct_ptr)->cnt_attribute = __size; \
            if (__size > 0) { \
                (struct_ptr)->attribute = imi_calloc(__size, SIZE_OF_##type); \
                if ((struct_ptr)->attribute) { \
                    while (__index < __size) { \
                        __jelem = cJSON_GetArrayItem(__jarray, __index); \
                        if (__jelem) JSON_DESERIALIZE_ARRAY_##type(__jelem, struct_ptr, attribute, __index); \
                        __index++; \
                    } \
                } \
            } \
        } \
    }
#define JSON_DESERIALIZE_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, func_ptr, max) \
    { \
        cJSON *__jarray, *__jelem; int __index = 0, __size = 0; \
        __jarray = cJSON_GetObjectItem(json_obj, #json_attr); \
        if (struct_ptr && __jarray) { \
            __size = cJSON_GetArraySize(__jarray); __size = max <= __size ? max : __size;  \
            while (__index < __size) { \
                __jelem = cJSON_GetArrayItem(__jarray, __index); \
                if (__jelem && func_ptr) func_ptr(__jelem, &((struct_ptr)->attribute[__index])); \
                __index++; \
            } \
        } \
    }
#define JSON_DESERIALIZE_PTR_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, struct_type, func_ptr) \
    { \
        cJSON *__jarray, *__jelem; int __index = 0, __size = 0; \
        __jarray = cJSON_GetObjectItem(json_obj, #json_attr); \
        if (struct_ptr && __jarray) { \
            __size = cJSON_GetArraySize(__jarray); (struct_ptr)->cnt_attribute = __size; \
            if (__size > 0) { \
                struct_type *___attribute = (struct_type *)imi_calloc(__size, sizeof(struct_type)); \
                if (___attribute) { \
                    while (__index < __size) { \
                        __jelem = cJSON_GetArrayItem(__jarray, __index); \
                        if (__jelem && func_ptr) func_ptr(__jelem, &(___attribute[__index])); \
                        __index++; \
                    } \
                } \
                (struct_ptr)->attribute = ___attribute; \
            } \
        } \
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// interface
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// 反序列化子结构体的函数
typedef int(*deserialize_func)(cJSON *json_obj, void *sub_struct);

// 序列化子结构的函数
typedef int(*serialize_func)(cJSON *json_obj, void *sub_struct);

// 反序列化json_obj的基本数据类型属性到struct_ptr的对应属性中. 例如 struct A { char* value; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @type:       需要反序列化属性的数据类型(目前支持int, double, string(char*))
#define json_deserialize(json_obj, json_attr, struct_ptr, attribute, type) JSON_DESERIALIZE_##type(json_obj, json_attr, struct_ptr, attribute)

// 反序列化json_obj的结构体属性到struct_ptr的对应属性中. 例如 struct A { struct B value; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @func_ptr:   反序列化子结构体的函数. 函数签名为: int(*deserialize_func)(cJSON *, void *)
#define json_deserialize_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr) JSON_DESERIALIZE_STRUCT(json_obj, json_attr, struct_ptr, attribute, func_ptr)

// 反序列化json_obj的结构体属性到struct_ptr的对应属性(结构体指针)中. 例如 struct A { struct B *value; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @struct_type:指针指向的结构体类型
// @func_ptr:   反序列化子结构体的函数. 函数签名为: int(*deserialize_func)(cJSON *, void *)
#define json_deserialize_ptr_struct(json_obj, json_attr, struct_ptr, attribute, struct_type, func_ptr) JSON_DESERIALIZE_PTR_STRUCT(json_obj, json_attr, struct_ptr, attribute, struct_type, func_ptr)

// 反序列化json_obj的属性到struct_ptr的对应字符数组属性中. 例如 struct A { char str[MAX_SIZE]; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @max:        结构体对应字符数组属性的最大长度. 如果json中对应string属性长度超过max, 则会自动截断.
#define json_deserialize_char_array(json_obj, json_attr, struct_ptr, attribute, max) JSON_DESERIALIZE_char_array(json_obj, json_attr, struct_ptr, attribute, max)

// 反序列化json_obj的基本数据类型属性数组到struct_ptr的对应数组属性中. 例如 struct A { char* str_array[MAX_SIZE]; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @type:       需要反序列化属性的数据类型(目前支持int, double, string(char*))
// @max:        结构体对应字符数组属性的最大长度. 如果json中对应数组属性长度超过max, 则会自动截断.
#define json_deserialize_array(json_obj, json_attr, struct_ptr, attribute, type, max) JSON_DESERIALIZE_ARRAY(json_obj, json_attr, struct_ptr, attribute, type, max)

// 反序列化json_obj的基本数据类型属性数组到struct_ptr的对应指针数组属性. 接口会根据数组的大小自动分配空间. 例如 struct A { int count; char** str_array; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @cnt_attribute: 记录指针数组长度的属性名称
// @type:       需要反序列化属性的数据类型(目前支持int, double, string(char*))
#define json_deserialize_ptr_array(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type) JSON_DESERIALIZE_PTR_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type)

// 反序列化json_obj的对象数组到struct_ptr的对应结构体数组属性中. 例如 struct A { struct B b_array[MAX_SIZE]; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称 
// @func_ptr:   反序列化子结构体的函数 int(*deserialize_func)(cJSON *, void *)
// @max:        结构体对应数组属性的最大长度. 如果json中对象数组长度超过max, 则会自动截断.
#define json_deserialize_struct_array(json_obj, json_attr, struct_ptr, attribute, func_ptr, max) JSON_DESERIALIZE_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, func_ptr, max)

// 反序列化json_obj的对象数组到struct_ptr的对应结构体指针数组属性中. 例如 struct A { int count; struct B* b_array; }
// @json_obj:   需要反序列化的json对象
// @json_attr:  需要反序列化的json属性
// @struct_ptr: 需要反序列化的结构体指针
// @attribute:  需要反序列化的属性名称
// @cnt_attribute:  记录指针数组长度的属性名称
// @struct_type:指针数组指向的结构体类型
// @func_ptr:   反序列化子结构体的函数 int(*deserialize_func)(cJSON *, void *)
#define json_deserialize_ptr_struct_array(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, struct_type, func_ptr) JSON_DESERIALIZE_PTR_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, struct_type, func_ptr)


// 序列化结构体对象的基本属性到json对象对应的属性当中. 例如 struct A { char* value; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @type:       属性的数据类型(目前支持int, double, string(char*))
#define json_serialize(json_obj, json_attr, struct_ptr, attribute, type) JSON_SERIALIZE_##type(json_obj, json_attr, struct_ptr, attribute)

// 序列化结构体对象的结构体属性到json对象对应的属性当中. 例如 struct A { struct B value; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @func_ptr:   序列化子结构的函数. 函数签名为: int(*serialize_func)(cJSON *, void *). 如果返回值为0, 表示序列化成功; 其他: 失败.
#define json_serialize_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr) JSON_SERIALIZE_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr)
#define json_serialize_ptr_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr) JSON_SERIALIZE_ptr_struct(json_obj, json_attr, struct_ptr, attribute, func_ptr)

// 序列化结构体对象的数组属性到json对象对应的数组属性当中. 例如 struct A { char* str_array[MAX_SIZE]; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @type:       属性数组的数据类型(目前支持int, double, string(char*))
// @size:       结构体属性数组的长度
#define json_serialize_array(json_obj, json_attr, struct_ptr, attribute, type, size) JSON_SERIALIZE_ARRAY(json_obj, json_attr, struct_ptr, attribute, type, size)

// 序列化结构体对象的数组属性到json对象对应的指针数组属性当中. 例如 struct A { int count; char** str_array; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @cnt_attribute:  记录指针数组长度的属性名称
// @type:       属性数组的数据类型(目前支持int, double, string(char*))
#define json_serialize_ptr_array(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type) JSON_SERIALIZE_PTR_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, type)

// 序列化结构体对象的结构体数组属性到json对象对应的数组属性当中. 例如 struct A { struct B b_array[MAX_SIZE]; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @func_ptr:   序列化子结构的函数. 函数签名为: int(*serialize_func)(cJSON *, void *).
//              如果返回值为0, 表示序列化成功; 其他: 失败. 如果数组中有结构体需要跳过, 则让子函数返回非0即可.
// @size:       结构体属性数组的长度
#define json_serialize_struct_array(json_obj, json_attr, struct_ptr, attribute, func_ptr, size) JSON_SERIALIZE_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, func_ptr, size)

// 序列化结构体对象的数组属性到json对象对应的结构体指针数组属性当中. 例如 struct A { int count; struct B* b_array; }
// @json_obj:   需要序列化的json对象
// @json_attr:  需要序列化的json属性
// @struct_ptr: 需要序列化的结构体指针
// @attribute:  需要序列化的结构体属性名称
// @cnt_attribute:  记录指针数组长度的属性名称
// @func_ptr:   序列化子结构的函数. 函数签名为: int(*serialize_func)(cJSON *, void *).
//              如果返回值为0, 表示序列化成功; 其他: 失败. 如果数组中有结构体需要跳过, 则让子函数返回非0即可.
#define json_serialize_ptr_struct_array(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, func_ptr) JSON_SERIALIZE_PTR_STRUCT_ARRAY(json_obj, json_attr, struct_ptr, attribute, cnt_attribute, func_ptr)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // __JSON_HELPER_H__