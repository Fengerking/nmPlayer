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
#define __ON2_MEM_C__

#include "on2_mem.h"
//#include "on2_mem_intrnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define ON2_MALLOC_L  malloc
# define ON2_REALLOC_L realloc
# define ON2_FREE_L    free
# define ON2_MEMCPY_L  memcpy
# define ON2_MEMSET_L  memset
# define ON2_MEMMOVE_L memmove


//#ifdef _DEBUG
//#define OUT_mem_FILE_NAME    "E:\\vector\\vp6\\clips\\mem.txt"
//	FILE *memFile;
//	char outmemName[256];
//	int i_mem = 0;
//	int mac = 0;
//	int fre = 0;
//#endif


void* on2_memalign(size_t align, size_t size)
{
    void* addr,
        * x = NULL;

    addr = ON2_MALLOC_L(size + align-1 + ADDRESS_STORAGE_SIZE);

    if(addr) {
        x = align_addr((unsigned char*)addr+ADDRESS_STORAGE_SIZE,(int)align);
        /* save the actual malloc address */
        ((size_t*)x)[-1] = (size_t)addr;
    }

    return x;
}

void* on2_malloc(size_t size)
{   
    return on2_memalign(DEFAULT_ALIGNMENT, size);
}

void* on2_calloc(size_t num, size_t size)
{   
    void *x; 

    x = on2_memalign(DEFAULT_ALIGNMENT, num*size);

    if(x)
        ON2_MEMSET_L(x, 0, num*size);

    return x;
}

void* on2_realloc(void* memblk, size_t size)
{
    void* addr,
        * new_addr = NULL;
    int align = DEFAULT_ALIGNMENT;
    /*
    The realloc() function changes the size of the object pointed to by 
    ptr to the size specified by size, and returns a pointer to the 
    possibly moved block. The contents are unchanged up to the lesser 
    of the new and old sizes. If ptr is null, realloc() behaves like 
    malloc() for the specified size. If size is zero (0) and ptr is 
    not a null pointer, the object pointed to is freed. 
    */
    if(!memblk)
        new_addr = on2_malloc(size);
    else if (!size)
        on2_free(memblk);
    else
    {
        addr   = (void*)(((size_t*)memblk)[-1]);
        memblk = NULL;

        new_addr = ON2_REALLOC_L(addr, size + align + ADDRESS_STORAGE_SIZE); 
        if(new_addr) {
            addr = new_addr;
            new_addr = (void*)(((size_t)
                ((unsigned char*)new_addr + ADDRESS_STORAGE_SIZE) + (align - 1)) &
                (size_t)-align);
            /* save the actual malloc address */
            ((size_t*)new_addr)[-1] = (size_t)addr;
        }
    }

    return new_addr;
}

void on2_free(void* memblk)
{
    if(memblk) {
        void* addr = (void*)(((size_t*)memblk)[-1]);
        ON2_FREE_L(addr);
    }
}


void * on2_memcpy(void *dest, const void *source, size_t length)
{
    return ON2_MEMCPY_L(dest, source, length);
}

void * on2_memset(void *dest, int val, size_t length)
{
    return ON2_MEMSET_L(dest, val, length);
}

void * on2_memmove(void *dest, const void *src, size_t count)
{
    return ON2_MEMMOVE_L(dest, src, count);
}