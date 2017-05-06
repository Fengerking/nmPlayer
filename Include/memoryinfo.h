/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifdef _LINUX
#include <string.h>
#include <stdio.h>

extern "C" void get_malloc_leak_info(uint8_t** info, size_t* overallSize,
        size_t* infoSize, size_t* totalMemory, size_t* backtraceSize);
extern "C" void free_malloc_leak_info(uint8_t* info);

void memStatus(char* args)
{
    typedef struct {
        size_t size;
        size_t dups;
        intptr_t * backtrace;
    } AllocEntry;

    uint8_t *info = NULL;
    size_t overallSize = 0;
    size_t infoSize = 0;
    size_t totalMemory = 0;
    size_t backtraceSize = 0;

    get_malloc_leak_info(&info, &overallSize, &infoSize, &totalMemory, &backtraceSize);
    if (info) 
	{
		uint8_t *ptr = info;

     	FILE* dumpmemory = fopen(args, "wb");
		if(dumpmemory)
		{
			fwrite(&overallSize, 1, 4, dumpmemory);
			fwrite(&infoSize, 1, 4, dumpmemory);
			fwrite(&totalMemory, 1, 4, dumpmemory);
			fwrite(&backtraceSize, 1, 4, dumpmemory);
			fwrite(ptr, 1, overallSize, dumpmemory);
			fclose(dumpmemory);
		} 

		free((void *)ptr);
	}
}
#endif