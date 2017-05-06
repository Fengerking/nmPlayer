 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_porting.h
    \brief    OS related stuff
    \author   Renjie Yu
	\change
*/

#ifndef __VOH265PORTING_H__
#define __VOH265PORTING_H__
#include "voType.h"
#include "h265dec_debug.h"

#include "assert.h"

#define VOASSERT assert

#ifdef _IOS
#define  VOINLINE inline
#else
#define  VOINLINE __inline
#endif

#if defined(__GNUC__) &&  defined(VOARMV7)
#define  VONOINLINE __attribute__((noinline))
#else
#define  VONOINLINE 
#endif

typedef VO_U8  boolean;
#define TRUE      1
#define FALSE     0   
#endif //__VOH265PORTING_H__