	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/
#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "portab.h"

#define ERR_NONE			0
#define ERR_BUFFER_FULL		-1
#define ERR_OUT_OF_MEMORY	-2
#define ERR_INVALID_DATA	-3
#define ERR_INVALID_PARAM	-4
#define ERR_NOT_SUPPORTED	-5
#define ERR_NEED_MORE_DATA	-6
#define ERR_FILE_NOT_FOUND	-8
#define ERR_END_OF_FILE		-9
#define ERR_DEVICE_ERROR	-10
#define ERR_SYNCED			-11
#define ERR_DATA_NOT_FOUND	-12
#define ERR_MIME_NOT_FOUND	-13
#define ERR_NOT_DIRECTORY	-14
#define ERR_NOT_COMPATIBLE	-15
#define ERR_CONNECT_FAILED	-16
#define ERR_DROPPING		-17
#define ERR_STOPPED			-18
#define ERR_UNAUTHORIZED	-19
#define ERR_LOADING_HEADER	-20
#define ERR_NO_FILTER	    -21
#define ERR_INVALID_CHANNELNUM	    -22
#define ERR_INVALID_SAMPLERATE	    -23
#define ERR_INVALID_PCMFORMAT	    -24

#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ ((1 << 15) - 1))
#define MAX(x,y)	((x)>(y)?(x):(y))
#define MIN(x,y)	((x)<(y)?(x):(y))
#endif//_GLOBAL_H