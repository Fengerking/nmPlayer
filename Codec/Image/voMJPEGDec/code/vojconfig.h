/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JCONFIG_H__
#define __JCONFIG_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "voMJPEGDecID.h"
#ifndef MIN
#define		MIN(A, B)       ((A) < (B) ? (A) : (B))
#endif

#ifndef MAX
#define		MAX(A, B)       ((A) > (B) ? (A) : (B))
#endif

#ifndef SAT
#define		SAT(A)			(A) = (A) < 0 ? 0: ((A) > 255 ? 255: (A))
#endif

#ifndef DIV
#define		DIV(A, B)		((A) + (B) - 1) / (B)  
#endif

#define SAMPLE_ALIGN8(addr)  (((unsigned int)(addr)+7)&(~7))


//#define		MJPEG_SUPPORT

#define		JPEG_BUFFER_LENGTH	0x10000 
#define		MAX_BUUFER_SIZE		0x10000000

#define		JPEG_MAX_BLOCKS		6 //10 //zou
#define		JPEG_BLOCK_LINE		8
#define		JPEG_BLOCK_SIZE		64

#define		NUM_BLOCK_TBLS		4
#define		NUM_HUFF_TBLS		4

#define		VLC_BITS			9

#define		JPEG_MAX_COLOR		3

#define		SUPPORT_BREAK
#define		BREAK_BLOCK_NUM		2

/*
 * set alignment for XSCALE
 */
#define		ALIGN_NUM_1			1
#define		ALIGN_NUM_4			4
#define		ALIGN_NUM_8			8
#define		ALIGN_NUM_16		16
#define		ALIGN_NUM_32		32	 
#define		DCACHE_ALIGN_NUM	ALIGN_NUM_32

typedef	char	INT8;
typedef	short	INT16;
typedef	long	INT32;
typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned long	UINT32;



#ifdef __cplusplus
}
#endif

#endif

