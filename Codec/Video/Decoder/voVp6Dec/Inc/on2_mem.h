/*
//==========================================================================
//
//  Copyright (c) On2 Technologies Inc. All Rights Reserved.
//
//--------------------------------------------------------------------------
//
//  File:        $Workfile$
//               $Revision$
//
//  Last Update: $DateUTC$
//
//--------------------------------------------------------------------------
*/
#ifndef __ON2_MEM_H__
#define __ON2_MEM_H__

#include <stdlib.h>
#include <stddef.h>
#include "voVP6DecID.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ADDRESS_STORAGE_SIZE      sizeof(size_t)
#define DEFAULT_ALIGNMENT        1

#if DEFAULT_ALIGNMENT < 1
# error "DEFAULT_ALIGNMENT must be >= 1!"
#endif

# define TRY_BOUNDS_CHECK         0
# define BOUNDS_CHECK_VALUE       0
# define BOUNDS_CHECK_PAD_SIZE    0
#undef _P
# define _P(x) x

/*returns an addr aligned to the byte boundary specified by align*/
#define align_addr(addr,align) (void*)(((size_t)(addr) + ((align) - 1)) & (size_t)-(align))
/*
    on2_mem_set_heap_size(size_t size)
      size - size in bytes for the memory manager to allocate for its heap
    Sets the memory manager's initial heap size
    Return:
      0: on success
      -1: if memory manager calls have not been included in the on2_mem lib
      -2: if the memory manager has been compiled to use static memory
      -3: if the memory manager has already allocated its heap
*/
int on2_mem_set_heap_size(size_t size);

void* on2_memalign(size_t align, size_t size);
void* on2_malloc(size_t size);
void* on2_calloc(size_t num, size_t size);
void* on2_realloc(void* memblk, size_t size);
void on2_free(void* memblk);

void* on2_memcpy(void* dest, const void* src, size_t length);
void* on2_memset(void* dest, int val, size_t length);
void* on2_memmove(void* dest, const void* src, size_t count);

// special memory functions
void* on2_mem_alloc(int id, size_t size, size_t align);
void on2_mem_free(int id, void* mem, size_t size);

#define duck_memalign(X,Y) on2_memalign(X,Y)
//#define duck_malloc(X,Y) on2_malloc(X)
//#define duck_calloc(X,Y) on2_calloc(X,Y)
//#define duck_realloc  on2_realloc
#define duck_free     on2_free
#define duck_memcpy   on2_memcpy
#define duck_memmove  on2_memmove
#define duck_memset   on2_memset

#if defined(__cplusplus)
}
#endif

#endif /* __ON2_MEM_H__ */
