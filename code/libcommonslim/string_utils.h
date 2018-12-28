/**************************************************************************
 * @ file    :  string_utils.h
 * @ author  :  yang yang
 * @ version :  0.9
 * @ date    :  2016.08.15
 * @ brief   :
 * @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------
//  @brief   从指定buffer中copy第一个以delimeter结尾的字符串, 放到pToken中.
//           token的内存将在函数中分配, 调用者必须自己处理相关的内存释放
//  @param   buffer   需要处理的buffer
//  @param   outToken  输出的token
//  @param   deliemter
//
//  @return:  当前处理到的位置
//-----------------------------------------------------------------------
char *str_read_token_new(char* buffer,  char** outToken, char delimeter);


//-----------------------------------------------------------------------
//
//  @brief   split指定字符串. 输出token的内存将在函数中分配,  调用者必须自己处理相关的内存释放
//  @param   src   需要split的字符串
//  @param   delimeter   分隔符
//  @param   tokenArray  保存的token
//  @param   maxSize     tokenArray的最大长度
//  @return: 找到的token个数
 //-----------------------------------------------------------------------
int split(char *src, char delimeter, char** tokenArray, int maxSize);

//-----------------------------------------------------------------------
// @brief   检测字符串是否以指定串开始
//-----------------------------------------------------------------------
bool str_start_with(char *src, char* prefix);

////-----------------------------------------------------------------------
//// @brief   复制字符串. 内存在该函数中分配, 调用者需要负责回收
////-----------------------------------------------------------------------
//char *clone_string(char* value);
//
////-----------------------------------------------------------------------
//// @brief   释放clone_string分配处理的字符串
////-----------------------------------------------------------------------
//void release_string(char* value);

//-----------------------------------------------------------------------
// @brief   去除字符串两端的空白字符。将会在字符串原地修改
// @str     需要处理的字符串
// @return  处理完成的字符（也是原串）
//-----------------------------------------------------------------------
char* str_trim(char *str);
char* str_trim_start(char *str);
char* str_trim_end(char *str);

//-----------------------------------------------------------------------
// @brief       从指定字符串的开始位置，读取指定分隔符前的字符串作为token，
//              复制到指定的缓冲区当中。例如"hello, world", delimeter=','
//              则缓冲区填入"hello", 返回指针指向" world"
// @str         需要处理的字符串
// @outBuffer   输出的缓冲区
// @bufferSize  缓冲区的长度
// @delimeter   区分token分隔符，例如"," 
// @return      处理到下一个字符位置（指向原串）
//-----------------------------------------------------------------------
char *str_read_token(char* str, char* outBuffer, int bufferSize, char delimeter);

//-----------------------------------------------------------------------
// @brief       从指定字符串的开始位置，读取一个指定类型的值，例如
//              "3, 4", delimeter=','将会返回3, ptr_str将会执行" 4"
// @ptr_str     [in/out]指向字符串的指针, 函数返回时，ptr_str会指向处理到位置
// @out         读取的值
// @delimeter   区分token分隔符，例如"," 
// @return      true: 成功读取到了值; false: 没有读取到值
//-----------------------------------------------------------------------
bool str_read_int(char** ptr_str, int *out, char delimeter);
bool str_read_double(char** ptr_str, double *out, char delimeter);

//-----------------------------------------------------------------------
// @brief 获取指定从第n个delimeter开始，2个delimiter之间的token，并复制到
//        指定的buffer中。token两端的空格会被跳过
//        如果起始delimiter之后再也没有delimiter，则截取到字结束为止。
//        如果指定buffer不够大，则到填满buffer为止.
//
// @return 第二个delimeter所在位置或是字符串结束位置. NULL表明copy失败
//-----------------------------------------------------------------------
char *str_read_token_between(char* str, char start, char end,
    char* outBuffer, int maxBufferSize, int skipN);

//-----------------------------------------------------------------------
// @brief 计算string的hash值
//-----------------------------------------------------------------------
unsigned string_hash(const char * key);

//-----------------------------------------------------------------------
// @brief   复制字符串. 内存在该函数中分配, 调用者需要负责回收
//-----------------------------------------------------------------------
char *clone_string(const char* value);

//-----------------------------------------------------------------------
// @brief   释放clone_string分配处理的字符串
//-----------------------------------------------------------------------
void release_string(char* value);


// 从文件中读取所有文本至缓冲区当中
// @return: 小于0: 错误代码, 大于0: 读出的实际大小
int imi_read_all_text(const char *path, char* buffer, int bufferSize);

// 从文件中读取所有文本
// 调用者需要自己释放内存
char* imi_read_all_text_new(const char *path, int* out_size);

int imi_str_printf(char **ppsz_buffer, const char *fmt, ...);
int imi_str_repeat(char **ppsz_buffer, const char *string, int repeat);

int string_splite(char *str, char splitter, char *lvalue, size_t lsize, char *rvalue, size_t rsize);
void string_make_printable(char *str, size_t s_size);

#ifdef __cplusplus
}
#endif

#endif // __STRING_UTILS_H__
