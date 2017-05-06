//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifndef _CONSTANTS_WMV_H_
#define _CONSTANTS_WMV_H_
#define MB_SIZE 16
#define BLOCK_SIZE 8
#define NUMBITS_SLICE_SIZE_WMV2         3 // To indicate Processor's #
#define MAX_THREAD_NUM  4
#define MAX_MB_HEIGHT  1000
enum { THREAD_FREE=0, THREAD_BUSY,THREAD_FINISHED,THREAD_ERROR};

enum { BUFFER_PARPARED=0xFFFFFFFF};

#define voprintf 
#define tbprintf 
#define bkprintf 
#define printf 

//enum { THREAD_BUFFERREAD=0, THREAD_BUSY,THREAD_FINISHED,THREAD_ERROR};
#endif

 
