/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/


#ifndef __JCONFIG_H__
#define __JCONFIG_H__
#include "voMJPEGEncID.h"

#ifdef WINCE
#pragma warning(disable:4142)
#endif


#define		MIN(A, B)       ((A) < (B) ? (A) : (B))
#define		MAX(A, B)       ((A) > (B) ? (A) : (B))
#define		FAST_ABS(A)		((A) < 0) ? (-(A)) : (A)

//#define		IPP_SUPPORT

#define		DEFAULT_QUALITY		75
#define		DEFAULT_THUMWIDTH	160
#define		DEFAULT_THUMHEIGHT	120
#define		DEFAULT_RESOLUTION	72
#define		DEFAULT_COMCONFIG	0x00030201      /* 01 02 03 */	
#define		EXIF_VERSION		0x30323230		/*   0220	*/
#define		FlashPIX_VERSION	0x30303130		/*	 0100  	*/
#define		GPS_VERSION			0x00000202		/*	 2200	*/

#define		JPEG_MAX_BLOCKS		6
#define		JPEG_MAX_COLOR		3
#define		JPEG_BLOCK_LINE		8
#define		JPEG_BLOCK_SIZE		64

#define		JPEG_BREAK_LENTH	384  /* JPEG_BLOCK_SIZE * JPEG_MAX_BLOCKS  */

#define		NUM_BLOCK_TBLS		2

#define		CONST_BITS			14

#define		DEFAULT_THUMBPIC_QUALITY	50
#define		DATATIMESTRINGLENGTH		20
#define		MACHINEMAKESTRLENGTH		32
#define		MACHINEMADELSTRLENGTH		40

/*
 * set alignment for XSCALE
 */
#define		ALIGN_NUM_1			1
#define		ALIGN_NUM_4			4
#define		ALIGN_NUM_8			8
#define		ALIGN_NUM_16		16
#define		ALIGN_NUM_32		32	 
#define		DCACHE_ALIGN_NUM	ALIGN_NUM_32

#define		TIFFDIRENALBE		0x000003F8
#define		EXIFDIRENABLE		0x00F00024	 	
#define		THUMDIRENABLE		0x0000007F

#define		FIX_0_382683433  98			/* 0.382683433 * (1 << 8) */
#define		FIX_0_541196100  139		/* 0.541196100 * (1 << 8) */
#define		FIX_0_707106781  181		/* 0.707106781 * (1 << 8) */ 
#define		FIX_1_306562965  334		/* 1.306562965 * (1 << 8) */


typedef	char	INT8;
typedef	short	INT16;
typedef	long	INT32;

typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned long	UINT32;

#endif