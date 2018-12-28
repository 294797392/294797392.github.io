#include <stdlib.h>
#include <string.h> // need for: memset

#include "defs.h"
#include "util_internal.h"

//void* mwMalloc(size_t size, const char* file, int line);

void *__imi_malloc0(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

//// 分配内存并置0 (mem watch模式)
//void *__imi_mw_malloc0(size_t size, const char* file, int line) {
//    void* ptr = mwMalloc(size, file, line);
//    if (ptr) {
//        memset(ptr, 0, size);
//    }
//
//    return ptr;
//}

void __imi_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}
