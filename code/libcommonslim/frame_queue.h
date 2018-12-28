/**************************************************************************
* @ file    : frame_queue.h
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.08.15
* @ brief   : 帧数据缓冲队列，用于管理流媒体缓冲。 该队列为一个循环缓冲队列
*             用户只需要不停的往里push数据, 队列会自动丢弃覆盖最老的帧数据。
*
*             同时，队列提供一系列帧数据的检索功能，可以从队列中取出缓冲的帧
*             
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#include "media_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// 帧队列数据结构
typedef struct _frame_queue_s *frame_queue_t;

// 创建帧缓冲队列
// @maxsize:    帧缓冲区缓冲最大的数据大小. 单位(bytes)
// @dupFunc:    帧信息复制函数
// @releaseFunc 帧信息释放函数
frame_queue_t create_frame_queue(int maxsize,
    duplicate_frame_info_func dupFunc, release_frame_info_func releaseFunc);

// 释放帧缓冲队列
void release_frame_queue(frame_queue_t queue);

// @brief:     帧缓冲队列管理。该队列为一个循环队列，用户只需要不停
// @queue:     缓冲队列实例
// @timestamp: 当前帧的时间戳
// @keyframe:  TRUE 表示送入的帧是否为I帧, 否则不是.
// @frameData: 数据指针
// @frameSize: 数据大小
// @frameInfo
int frame_queue_enqueue(frame_queue_t queue, timestamp_t timestamp,
    int keyframe, const char *frameData, int frameSize, void* frameInfo);

// @brief:      从帧缓冲队列中获取最接近timestamp的且不晚于timestamp的key帧. 如果没有不晚于timestamp的key帧
//              则返回最接近的key帧
// @queue:      缓冲队列实例
// @timestamp:  指定的时间戳
// @enforceKey: 如果该参数设为TRUE, 则会找到早于timestamp的第一个I帧。
//              否则会直接返回早于timestamp的第一帧
// @return:     大于等于0: 和当前队尾的偏移量（单位：帧数）, 小于0: 表示没有
int frame_queue_search(frame_queue_t queue, timestamp_t timestamp, int enforceKey);

// @brief:      从队列中读取帧数据。需要注意的是该函数读取是不锁定队列，将交给外部逻辑来决定是否需要锁定
// @offset:     距离队尾的offset
// @timestamp:  [out] 输出的对应帧的时间戳
// @keyframe:   [out] 输出帧是否为key帧
// @data        [out] 输出帧的数据指针
// @size        [out] 输出帧的大小
// @frameInfo   [out] 输出帧的frameInfo
// @return      如果队列中还有满足需求的数据，则返回0，否则返回错误编码
int frame_queue_get_frame(frame_queue_t queue, int offset, timestamp_t *timestamp,
    int* keyframe, char **data, int* size, void** frameInfo);

// @brief:      从队列中指定帧信息
// @offset:     距离队尾的offset
// @timestamp:  [out] 输出的对应帧的时间戳
// @return      如果队列中还有满足需求的数据，则返回0，否则返回错误编码
int frame_queue_peek_frame(frame_queue_t queue, int offset, timestamp_t *timestamp);

// @brief: 锁定帧缓冲队列。通常用于从缓冲区中读取大块数据时使用
void frame_queue_lock(frame_queue_t queue);

// @brief: 解锁帧缓冲队列。通常用于从缓冲区中读取大块数据时使用
void frame_queue_unlock(frame_queue_t queue);

// @brief: 获取帧缓冲队列已经缓冲的帧数
int frame_queue_get_size(frame_queue_t queue);

#ifdef __cplusplus
}
#endif

#endif // __FRAME_QUEUE_H__
