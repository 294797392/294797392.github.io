#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "buffer_queue.h"

#define MODULE "buffer_queue"
#define log_error(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")
#define log_info(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")
#define log_warn(m, f, ...) fprintf(stdout, f, __VA_ARGS__);fprintf(stdout, "\n")

static void *dequeue_thread_process(void *state)
{
	buffer_queue *queue = (buffer_queue*)state;
	while (queue->state == QUEUE_STATE_RUNNING)
	{
		/* 等待信号量 */
#ifdef WINDOWS
		WaitForSingleObject(queue->sem, INFINITE);
#endif

		pthread_mutex_lock(&queue->queue_mutex);

		/* 如果当前要出队的索引是MAX_QUEUE_SIZE，那么重置为0，从头开始继续出队 */
		int index = queue->dequeue_index;
		if (index == MAX_QUEUE_SIZE)
		{
			index = 0;
			queue->dequeue_index = 0;
		}

		void *element = queue->elements[index];

		/* 处理完后置为NULL */
		queue->elements[index] = NULL;

		/* 计算下一个要出队的索引 */
		queue->dequeue_index += 1;

		pthread_mutex_unlock(&queue->queue_mutex);

		/* 有信号量了，回调 */
		if (queue->callback)
		{
			queue->callback(queue->userdata, element);
		}
	}
	return NULL;
}

buffer_queue *create_buffer_queue(void *userdata)
{
	buffer_queue *queue = (buffer_queue*)calloc(1, sizeof(buffer_queue));
	if (!queue)
	{
		log_error(MODULE, "create buffer_queue instance failed");
		return NULL;
	}

#ifdef WINDOWS
	if (!(queue->sem = CreateSemaphore(NULL, 0, MAX_QUEUE_SIZE, "buffer_queue")))
	{
		log_error(MODULE, "create sem failed");
		free(queue);
		return NULL;
	}
#endif

	queue->state = QUEUE_STATE_IDLE;
	queue->userdata = userdata;
	pthread_cond_init(&queue->queue_cond, NULL);
	pthread_mutex_init(&queue->queue_mutex, NULL);
	return queue;
}

void set_buffer_queue_callback(buffer_queue *queue, buffer_queue_callback callback)
{
	queue->callback = callback;
}

void start_buffer_queue(buffer_queue *queue)
{
	queue->state = QUEUE_STATE_RUNNING;
	pthread_create(&queue->dequeue_thread, NULL, dequeue_thread_process, queue);
	pthread_detach(queue->dequeue_thread);
}

void buffer_queue_enqueue(buffer_queue *queue, void *element)
{
	if (element == NULL)
	{
		return;
	}

	int increament = 0;
	pthread_mutex_lock(&queue->queue_mutex);

	int index = queue->enqueue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		/* 队列满了，从头开始 */
		queue->enqueue_index = 0;
		index = 0;
	}

	/*
		如果队列里的元素为空，说明被消费完了或者没有被使用过，那么把信号量加1
		如果队列里的元素不为空，说明还没有被消费到, 丢弃最早的元素
	*/
	if (queue->elements[index] == NULL)
	{
		increament = 1;
	}
	queue->elements[index] = element;

	/* 计算下一个要入队的元素索引 */
	queue->enqueue_index += 1;

	pthread_mutex_unlock(&queue->queue_mutex);

	if (increament == 1)
	{
#ifdef WINDOWS
		ReleaseSemaphore(queue->sem, 1, NULL);
#endif
	}
}