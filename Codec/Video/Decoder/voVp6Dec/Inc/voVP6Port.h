/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __voVP6Port_H
#define __voVP6Port_H

#include "voVP6DecID.h"

#ifdef RVDS
#define BUILD_WITHOUT_C_LIB 0
#endif//

#if BUILD_WITHOUT_C_LIB
#define USE_DIVIDE_FUNC 1
#define malloc  voMalloc                       
#define calloc  voCalloc  
#define realloc voRealloc                     
#define free 	voFree                          
#define memcpy	voMemcpy                    
#define memmove	voMemmove                 
#define memset	voMemset
#define assert(...)
#define longjmp voLongjmp
#define setjmp  voSetjmp 
#define printf  voPrintf
#define fprintf voFprintf
#define fopen   voFopen
#define fflush  voFflush
#define fclose  voFclose
#define NULL 0
#define size_t int
#endif//BUILD_WITHOUT_C_LIB
#if !BUILD_WITHOUT_C_LIB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#endif//BUILD_WITHOUT_C_LIB


#undef INLINE
#undef NOINLINE
#undef LITTLE_ENDIAN
#undef BIG_ENDIAN


#if defined(ARM) || defined(MIPS) || defined(SH3) || defined(_M_IX86)||defined(RVDS)
#define LITTLE_ENDIAN
#else
#define BIG_ENDIAN
#endif


typedef signed long voint32_t;
typedef unsigned long vouint32_t;  
typedef signed short voint16_t; 
typedef unsigned short vouint16_t; 
typedef signed char voint8_t;
typedef unsigned char vouint8_t;
#    define ptr_t vouint32_t

#ifdef _MSC_VER

typedef signed __int64 voint64_t;
typedef unsigned __int64 vouint64_t;
#define NOINLINE 
#define INLINE __forceinline
#define STDCALL __stdcall
#else
//#include <stdint.h>
typedef signed long long voint64_t;
typedef unsigned long long vouint64_t;

#define INLINE __inline
#define NOINLINE __attribute__((noinline))
#if defined(TARGET_WINCE) || defined(TARGET_PALMOS)
#define __stdcall
#define STDCALL 
#else
#define STDCALL 
#endif
#endif

#endif
