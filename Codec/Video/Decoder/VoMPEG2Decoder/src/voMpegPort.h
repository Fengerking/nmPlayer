/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef __PORTAB_H
#define __PORTAB_H
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
typedef int jmp_buf[64];
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
#undef voCDECL


#if defined(ARM) || defined(MIPS) || defined(SH3) || defined(_M_IX86)||defined(RVDS)
#define LITTLE_ENDIAN
#else
#define BIG_ENDIAN
#endif

#define TICKSPERSEC			16384

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#define MAX_INT	0x7FFFFFFF

#ifdef _MSC_VER

#if _MSC_VER >= 1400
#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")
#pragma comment(linker, "/nodefaultlib:oldnames.lib")
#endif

#define NOINLINE 
#define INLINE __forceinline
#define STDCALL __stdcall
#define voCDECL __cdecl
#else
#include <stdint.h>


#define INLINE __inline
#define NOINLINE __attribute__((noinline))
#if defined(TARGET_WINCE) || defined(TARGET_PALMOS)
#define __stdcall
#define voCDECL 
#define STDCALL 
#else
#define STDCALL 
#endif
#endif

#ifdef __MMX__
#define MMX
#endif

#include "voType.h"

#if defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLEXPORT
#define DLLIMPORT
#endif

#if !defined(SH3) && !defined(MIPS)
#define _INLINE INLINE
#define VOCONST const
#else
#define _INLINE
#define VOCONST
#endif

#ifndef min
#  define min(x,y)  ((x)>(y)?(y):(x))
#endif
 
#ifndef max
#  define max(x,y)  ((x)<(y)?(y):(x))
#endif

#endif
