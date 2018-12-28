/**************************************************************************
* @ file    : trash_stack.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.08.01
* @ brief   : 可回收对象的管理, 减少对象的内存分配
*
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/


#ifndef __RECYCLE_BIN_H__
#define __RECYCLE_BIN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _trash_stack_s *trash_stack_t;

// 创建对象函数指针
typedef void* (*new_object_func)();

// 指定一个对象在进入回收栈时，需要做某些资源的释放或清理
typedef void(*clean_object_func)(void*);

// release对象
typedef void (*release_object_func)(void*);

//----------------------------------------------------------------------------
// 创建一个trash_stact实例
//
// @new_func:       trash_stack管理对象的构造函数，用于创建对象
// @release_func:   trash_stack管理对象的释放函数。用于释放资源
// @clean_func      对象在进入trash_stack,需要执行资源释放函数。
//                  如果不需要，可以设为NULL
// @maxSize:        trash_stack的最大长度。当超过该值时，如果继续往trash中插入
//                  对象, trash将会调用release_func释放对象而不是继续入栈
// @return:         trash_stack_t的实例.
//----------------------------------------------------------------------------
trash_stack_t create_trash(new_object_func new_func,
    release_object_func release_func,
    clean_object_func clean_func,
    int maxSize);

//----------------------------------------------------------------------------
// 释放一个trash_stact实例。该函数将会把栈内所管理的回收对象一起释放，
//
// @trash:  trash_stack实例
//----------------------------------------------------------------------------
void release_trash(trash_stack_t trash);

//----------------------------------------------------------------------------
// 从回收栈获取一个之前回收的实例。如果栈已空，则调用new_func创建一个新对象返回
// 需要注意的是，得到request_object返回的对象后，结构体的前4个字节会和送入trash
// 时的不一致。不要假设从回收站中拿回的值和送入回收站的值是一致的
//
// @trash:  trash_stack实例
// @return: 申请的对象
//----------------------------------------------------------------------------
void *request_object(trash_stack_t trash);

//----------------------------------------------------------------------------
// 向回收栈压入一个无用对象供后续重用。如果栈高已达到最大值，则调用release_func
// 释放资源
//
// @trash:  trash_stack实例
// @object: 需要回收的对象
//----------------------------------------------------------------------------
void recycle_object(trash_stack_t trash, void* object);

//----------------------------------------------------------------------------
// 获取当前栈高
//
// @trash:  trash_stack实例
// @return: 当前栈高
//----------------------------------------------------------------------------
int get_trash_size(trash_stack_t trash);

#ifdef __cplusplus
}
#endif

#endif // __RECYCLE_BIN_H__
