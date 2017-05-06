/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VO_MPEG4DEC_GLOBAL_H__
#define __VO_MPEG4DEC_GLOBAL_H__
#include "voMpegPort.h"
#include "voMpeg4DID.h"

#define MSMPEG4_ID		0X10
#define MPEG4_ID		0X20
#define H263_ID			0X30
#define S263_ID			0X40

#define CODECID_MASK	0XF0
#define VERSION_MASK	0X0F
#define VERSION_0		0X0
#define VERSION_1		0X1

/*Global switch flag*/
// #ifdef _IOS
// #define ENABLE_LICENSE_CHECK		0
// #else
#define ENABLE_LICENSE_CHECK		0
// #endif
#define ENABLE_SIMPLE_LC			0

#define ENABLE_ASP					1
#define ENABLE_DATA_PARTITION		1	
#define ENABLE_ERROR_CONCEAL		1
#ifdef	VOPC
#define ENABLE_HEADINFO				1
#else
#define ENABLE_HEADINFO				0
#endif
//RIM
#define ENABLE_DEBLOCK				0	
//RIM hardware deblock
#define ENABLE_HWIMX31_DEBLOCK		0 
#define ENABLE_MBINFO				0
#define ENABLE_VT					0

//---------------yh
// #define ENABLE_MULTI_THREAD			1
// #if ENABLE_MULTI_THREAD
// #define MIN_MB_TASK_NUM		20
// #define MAX_MB_TASK_NUM		80
// #define SUB_THREAD_NUM	(1)
// #define THREAD_NUM		(SUB_THREAD_NUM+1)
// #else
// #define MAX_MB_TASK_NUM		4
// #define THREAD_NUM		(1)
// #endif
#define USE_WHILE_FORVER	0

#define ENABLE_TIME_ST		0
#if ENABLE_TIME_ST

extern double start0,start1,start2,start3,end0,end1,end2,end3;
extern double tST0, tST1, tST2, tST3;

#ifndef MARKTIME

#ifdef WIN32

#include <windows.h>
#include <winbase.h>
#include <time.h>

static LARGE_INTEGER tmp1,tmp2;
#define MARKTIME(t)	\
	(QueryPerformanceCounter(&tmp2),t = tmp2.QuadPart/(double)tmp1.QuadPart)

#define MARKTIME_INIT(t)	\
	(QueryPerformanceFrequency(&tmp1),MARKTIME(t))

#else

#include		<time.h>

static struct timeval tval;
#define MARKTIME(t)	\
	(gettimeofday(&tval, NULL), t=tval.tv_sec + tval.tv_usec/1000000.0)

#define MARKTIME_INIT(t)	MARKTIME(t)

#endif

#endif

#endif

/* ASP Profile feature*/
#if ENABLE_ASP 
#define ASP_QPEL
#define ASP_BVOP
#define ASP_GMC
#endif

/* Error concealment*/
#define ERR_CBPY			0X1
#define ERR_QUANT			0X2
#define ERR_FCODE_FOR		0x4
#define ERR_FCODE_BAK		0X8
#define ERR_LDC_SIZE		0x10
#define ERR_CDC_SIZE		0X20
#define ERR_MCBPC			0X40
#define	ERR_MV				0X80

#define CONCEAL_ERROR		((ERR_CBPY | ERR_QUANT | ERR_MCBPC))

#endif//__VO_MPEG4DEC_GLOBAL_H__
