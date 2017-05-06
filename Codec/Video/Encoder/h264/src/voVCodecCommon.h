/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOVCODEC_COMMON_H_
#define _VOVCODEC_COMMON_H_


#include <stdio.h>


#include "voType.h"

typedef VO_U32             VOINTPTR;

#ifndef HAVE_LOG2F
//#define log2f(x) (logf((x))/0.693147180559945f)

#define log2f(x)     ((float)log((double)(x)))/(log((double)2))
#endif


//#if defined(LINUX) && !defined(RVDS)
//#   define VOINLINE static __inline__
//#else
#   define VOINLINE __inline
//#endif


#ifdef _MSC_VER
#define VO_DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
#else
#define VO_DECLARE_ALIGNED( var, n ) var __attribute__((aligned(n)))
#endif

#define VO_ALIGNED_16( var ) VO_DECLARE_ALIGNED( var, 16 )
#define VO_ALIGNED_4( var )  VO_DECLARE_ALIGNED( var, 4 )



#ifdef ARCH_ARM
#define VO_ALIGNED_ARRAY_16( type, name, sub1, ... )\
    VO_U8 name##_u [sizeof(type sub1 __VA_ARGS__) + 15];\
    type (*name) __VA_ARGS__ = (void*)((VOINTPTR)(name##_u+15) & ~15)
#else
#define VO_ALIGNED_ARRAY_16( type, name, sub1, ... )\
    VO_ALIGNED_16( type name sub1 __VA_ARGS__ )
#endif

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define VO_ALWAYS_INLINE __attribute__((always_inline)) inline
#define VO_NOINLINE __attribute__((noinline))
#define VO_MAY_ALIAS __attribute__((may_alias))
#else
#define VO_ALWAYS_INLINE __inline
#define VO_NOINLINE
#define VO_MAY_ALIAS
#endif



#define WORD_SIZE sizeof(void*)

#define asm __asm__



static VO_ALWAYS_INLINE VO_U32 EndianFix32( VO_U32 x )
{
    return (x<<24) + ((x<<8)&0xff0000) + ((x>>8)&0xff00) + (x>>24);
}



#endif //_VOVCODEC_COMMON_H_
