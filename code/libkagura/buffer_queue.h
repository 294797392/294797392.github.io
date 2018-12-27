#ifndef __BUFFER_QUEUE_H__
#define __BUFFER_QUEUE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

#define MAX_QUEUE_SIZE 256

typedef enum
{
	QUEUE_STATE_IDLE,
	QUEUE_STATE_RUNNING
}queue_state;

typedef void(*buffer_queue_callback)(void *userdata, void *element);
typedef struct buffer_queue_s
{
	pthread_t dequeue_thread;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;

	buffer_queue_callback callback;
	queue_state state;
	void *userdata;
	void *elements[MAX_QUEUE_SIZE];
	int enqueue_index;  /* 当前要入队的索引 */
	int dequeue_index;  /* 当前要出队的索引 */

#ifdef WINDOWS
	HANDLE sem;
#endif

}buffer_queue;

buffer_queue *create_buffer_queue(void *userdata);
void set_buffer_queue_callback(buffer_queue *queue, buffer_queue_callback callback);
void start_buffer_queue(buffer_queue *queue);
void buffer_queue_enqueue(buffer_queue *queue, void *element);

#endif
