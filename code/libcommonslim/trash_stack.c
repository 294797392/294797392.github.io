/**************************************************************************
* @ file    : trash_stack.c
* @ author  : yang yang
* @ version : 0.9
* @ date    : 2017.08.01
* @ brief   : 可回收对象的管理
*
* @Copyright (c) 2016  chuangmi inc.
***************************************************************************/

#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "trash_stack.h"

typedef struct _trash_s *_trash_t;
struct _trash_s {
    _trash_t next;
};

struct _trash_stack_s {
    int max_size;
    int size;

    new_object_func new_func;
    release_object_func release_func;
    clean_object_func clean_func;

    _trash_t stack;

    pthread_mutex_t lock;
};

//---------------------------------------------------------------------------
//  内部函数
//---------------------------------------------------------------------------

void* trash_stack_pop(_trash_t *stack) {
    _trash_t data;
    data = *stack;
    if (data) {
        *stack = data->next;
        data->next = NULL;
    }

    return data;
}

//---------------------------------------------------------------------------
//  接口函数
//---------------------------------------------------------------------------

trash_stack_t create_trash(new_object_func new_func,
    release_object_func release_func,
    clean_object_func clean_func,
    int maxSize) {

    trash_stack_t trash = imi_malloc(sizeof(struct _trash_stack_s));
    if (trash) {
        trash->max_size = MAX(0, maxSize);
        trash->size = 0;
        trash->new_func = new_func;
        trash->release_func = release_func;
        trash->clean_func = clean_func;
        trash->stack = NULL;

		pthread_mutex_init(&trash->lock, NULL);
    }

    return trash;
}

void release_trash(trash_stack_t trash) {
    if (trash) {
		pthread_mutex_destroy(&trash->lock);

        while (trash->stack) {
            void* data = trash_stack_pop(&(trash->stack));
            trash->release_func(data);
        }

        imi_free(trash);
    }
}

void *request_object(trash_stack_t trash) {
    void* object = NULL;

	pthread_mutex_lock(&trash->lock);

    if (trash->size > 0) {
        object = trash_stack_pop(&(trash->stack));
        trash->size--;
    }

	pthread_mutex_unlock(&trash->lock);

    if (!object) {
        object = trash->new_func();
    }

    return object;
}

void recycle_object(trash_stack_t trash, void* object) {
    int pushed = FALSE;

	pthread_mutex_lock(&trash->lock);

    if (trash->size < trash->max_size) {
        if (trash->clean_func) {
            trash->clean_func(object);
        }

        _trash_t data = (_trash_t)object;
        data->next = trash->stack;
        trash->stack = data;
        trash->size++;

        pushed = TRUE;
    }

	pthread_mutex_unlock(&trash->lock);

    if (!pushed) {
        trash->release_func(object);
    }
}

int get_trash_size(trash_stack_t trash) {
    int result = 0;

	pthread_mutex_lock(&trash->lock);

    result = trash->size;

	pthread_mutex_unlock(&trash->lock);

    return result;
}
