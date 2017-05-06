/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __PORTAB_H
#define __PORTAB_H

#define DISABLE_C_LIB 0


#if DISABLE_C_LIB
#define malloc  voMalloc                                           
#define free 	voFree                          
#define memcpy	voMemcpy                                 
#define memset	voMemset
#define NULL 0
#else
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#endif//DISABLE_C_LIB

#if 0
#undef INLINE
#undef VO_S32
#undef VO_U32  
#undef VO_S16
#undef VO_U16 
#undef VO_S8
#undef VO_U8
#undef VO_S64
#undef VO_U64

typedef signed long VO_S32;
typedef unsigned long VO_U32;  
typedef signed short VO_S16; 
typedef unsigned short VO_U16; 
typedef signed char VO_S8;
typedef unsigned char VO_U8;
#ifdef _MSC_VER
typedef signed __int64 VO_S64;
typedef unsigned __int64 VO_U64;
#else
typedef signed long long VO_S64;
typedef unsigned long long VO_U64;
#endif

#else
#include "voType.h"
#endif

#ifdef _MSC_VER
#define INLINE __forceinline
#define STDCALL __stdcall
#define VOCONST const
#else
#define INLINE __inline
#define STDCALL 
#define VOCONST
#endif


typedef VO_S16 idct_t;


#ifndef min
#  define min(x,y)  ((x)>(y)?(y):(x))
#endif

#ifndef max
#  define max(x,y)  ((x)<(y)?(y):(x))
#endif

#ifndef sign
#  define sign(x) ((x)<0?-1:1)
#endif

#endif
