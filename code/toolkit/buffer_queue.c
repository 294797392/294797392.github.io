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
		/* �ȴ��ź��� */
#ifdef WINDOWS
		WaitForSingleObject(queue->sem, INFINITE);
#endif

		pthread_mutex_lock(&queue->queue_mutex);

		/* �����ǰҪ���ӵ�������MAX_QUEUE_SIZE����ô����Ϊ0����ͷ��ʼ�������� */
		int index = queue->dequeue_index;
		if (index == MAX_QUEUE_SIZE)
		{
			index = 0;
			queue->dequeue_index = 0;
		}

		void *element = queue->elements[index];

		/* ���������ΪNULL */
		queue->elements[index] = NULL;

		/* ������һ��Ҫ���ӵ����� */
		queue->dequeue_index += 1;

		pthread_mutex_unlock(&queue->queue_mutex);

		/* ���ź����ˣ��ص� */
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
		/* �������ˣ���ͷ��ʼ */
		queue->enqueue_index = 0;
		index = 0;
	}

	/*
		����������Ԫ��Ϊ�գ�˵�����������˻���û�б�ʹ�ù�����ô���ź�����1
		����������Ԫ�ز�Ϊ�գ�˵����û�б����ѵ�, ���������Ԫ��
	*/
	if (queue->elements[index] == NULL)
	{
		increament = 1;
	}
	queue->elements[index] = element;

	/* ������һ��Ҫ��ӵ�Ԫ������ */
	queue->enqueue_index += 1;

	pthread_mutex_unlock(&queue->queue_mutex);

	if (increament == 1)
	{
#ifdef WINDOWS
		ReleaseSemaphore(queue->sem, 1, NULL);
#endif
	}
}