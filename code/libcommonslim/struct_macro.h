/**************************************************************************
* @ file    : struct_macro.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.05.15
* @ brief   : 定义一些常用的宏
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __STRUCT_MACRO_H__
#define __STRUCT_MACRO_H__

// 复制数组中的char**属性到结构体对象的char**属性. 例如 struct A { int count; char** str_array; }
#define DUPLICATE_PSTR_ARRAY(src_arr, src_cnt, tgt_arr, tgt_cnt) \
{ \
    if(src_cnt > 0) { \
        tgt_cnt = src_cnt; \
        src_arr = imi_calloc(tgt_cnt, sizeof(char*)); \
        if(tgt_arr) { \
            int __i;  \
            for (__i = 0; __i < src_cnt; __i++) \
                if(src_arr[__i]) tgt_arr[__i] = imi_strdup(src_arr[__i]); \
        } \
    } \
}

// 复制结构体数组中的char**属性到另一个结构体对象的char**属性. 例如 struct A { int count; char** str_array; }
#define DUPLICATE_STRUCT_PSTR_ARRAY(src_obj, src_attr, src_cnt, tgt_obj, tgt_attr, tgt_cnt) \
{ \
    if((src_obj)->src_cnt > 0) { \
        (tgt_obj)->tgt_cnt = (src_obj)->src_cnt; \
        (tgt_obj)->tgt_attr = imi_calloc((tgt_obj)->tgt_cnt, sizeof(char*)); \
        if((tgt_obj)->tgt_attr) { \
            int __i;  \
            for (__i = 0; __i < (src_obj)->src_cnt; __i++) \
                if((src_obj)->src_attr[__i]) (tgt_obj)->tgt_attr[__i] = imi_strdup((src_obj)->src_attr[__i]); \
        } \
    } \
}

// 释放结构体数组中的char**属性. 例如 struct A { int count; char** str_array; }
#define RELEASE_STRUCT_PSTR_ARRAY(struct_obj, attribute, cnt_attribute) \
{ \
    if((struct_obj)->cnt_attribute > 0 && (struct_obj)->attribute) { \
        int __i; \
        for (__i = 0; __i < (struct_obj)->cnt_attribute; __i++) if((struct_obj)->attribute[__i]) imi_free((struct_obj)->attribute[__i]); \
        imi_free((struct_obj)->attribute); \
    } \
}

// 释放结构体数组中的char* []属性. 例如 struct A { char* str_array[max]; }
#define RELEASE_STRUCT_STR_ARRAY(struct_obj, attribute, max) \
{ \
    int __i; \
    for (__i = 0; __i < max; __i++) if((struct_obj)->attribute[__i]) imi_free((struct_obj)->attribute[__i]); \
}

// 释放结构体数组中的struct B*属性. 例如 struct A { int count; struct B *str_array; }
#define RELEASE_STRUCT_PTR_ARRAY(struct_obj, attribute, cnt_attribute, func_release) \
{ \
    int __i; \
    for (__i = 0; __i < (struct_obj)->cnt_attribute; __i++) func_release(&((struct_obj)->attribute[__i])); \
    imi_free((struct_obj)->attribute); \
}





#endif // __STRUCT_MACRO_H__