/* **************************************************** BEGIN LICENSE BLOCK ******************************************** 
* Version: RCSL 1.0 and Exhibits. 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
* Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
* All Rights Reserved. 
* 
* The contents of this file, and the files included with this file, are 
* subject to the current version of the RealNetworks Community Source 
* License Version 1.0 (the "RCSL"), including Attachments A though H, 
* all available at http://www.helixcommunity.org/content/rcsl. 
* You may also obtain the license terms directly from RealNetworks. 
* You may not use this file except in compliance with the RCSL and 
* its Attachments. There are no redistribution rights for the source 
* code of this file. Please see the applicable RCSL for the rights, 
* obligations and limitations governing use of the contents of the file. 
* 
* This file is part of the Helix DNA Technology. RealNetworks is the 
* developer of the Original Code and owns the copyrights in the portions 
* it created. 
* 
* This file, and the files included with this file, is distributed and made 
* available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
* FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
* 
* Technology Compatibility Kit Test Suite(s) Location: 
* https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
********************************************** END LICENSE BLOCK ************************************************************ 

****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc.
*    All Rights Reserved.
*    Do not redistribute. 
****************************************************************************************
****************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1997 - 2001 Intel Corporation.
*    All Rights Reserved. 
****************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/

#ifndef RVTYPES_H__
#define RVTYPES_H__
/**********************************************************************************************************************************************************************
* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/rvtypes.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ 
* This file defines the fundamental types used by the HIVE/RV interfaces. 
* These types are intended to be portable across a wide variety of 
* compilation environments. 
* The following identifiers define scalar data types having a known size 
* and known range of values, regardless of the host compiler. 
* 
* Name     Size     Comments      Range of Values 
* -------+--------+-------------+-------------------------- 
* U8        8 bits  unsigned                0 ..        255 
* I8        8 bits  signed               -128 ..        127 
* U16      16 bits  unsigned                0 ..      65535 
* I16      16 bits  signed             -32768 ..      32767 
* U32      32 bits  unsigned                0 .. 4294967295 
* I32      32 bits  signed        -2147483648 .. 2147483647 
* Bool8     8 bits  boolean                 0 ..   non-zero 
* Bool16   16 bits  boolean                 0 ..   non-zero
* Bool32   32 bits  boolean                 0 ..   non-zero
* Enum32   32 bits  enumeration   -2147483648 .. 2147483647
* F32      32 bits  floating point 
* F64      64 bits  floating point 
*          NOTE:  floating point representations are compiler specific
* The following identifiers define scalar data types whose size is 
* compiler specific.  They should only be used in contexts where size 
* is not relevant. 
* RV_Boolean       boolean                 0 ..   non-zero 
* The following pointers to the above types are also defined. 
* PU8      PI8     PBool8      PEnum32 
* PU16     PI16    PBool16 
* PU32     PI32    PBool32 
* PF32     PF64    PBoolean 
* 
* The following macros are defined to support compilers that provide 
* a variety of calling conventions.  They expand to nothing (i.e., empty) 
* for compilation environments where they are not needed. 
* 
*     RV_CDECL 
*      
*     RV_STDCALL 
* These are the _WIN32 __cdecl, __fastcall and __stdcall conventions. 
* 
*         RV_CALL 
*         This is the calling convention for HIVE/RV functions. 
*         We use an explicit calling convention so that the HIVE/RV 
*         functionality could be packaged in library, and linked to by 
*         a codec built with an arbitrary calling convention. 
* 
*         RV_FREE_STORE_CALL 
* 
*         This represents the host compiler's calling convention for 
*         the C++ new and delete operators. 
*******************************************************************************************************************************************************/
#include "voRVDecID.h"

//#define _DEBUG
#define LOG_BUF_SZ 1024
#if defined(_MSC_VER)
                  
#    include <windows.h>
#    include <stdio.h>

#    define snprintf _snprintf
#    define vsnprintf _vsnprintf

#  ifdef _DEBUG
#define voVLog printf
#if 0
static __inline void voVLog(char *str, ...)
{
		va_list args;
		char buf[LOG_BUF_SZ];
		va_start(args, str);
		vsprintf(buf, str, args);
		va_end(args);
		OutputDebugStringA(buf);
		printf("%s", buf);
		//fprintf(stderr, "%s", buf);
}
#endif
#    else
static __inline void voVLog(char *str, ...) {}
#    endif
                            
                  
#elif defined(VOANDROID) /* Compiler test */                       

#    ifdef _DEBUG

#define voVLog printf
#if 0
                            
#include <stdio.h>
#include <stdarg.h>
                            
static __inline void voVLog(char *str, ...)
{
	va_list args;
	va_start(args, str);
    vfprintf(stderr, str, args);
	va_end(args);
}
#endif                            
#    else /* _DEBUG */
static __inline void voVLog( char *str, ...) {}
#    endif /* _DEBUG */
                            
                         
                            

#else
                            
                            
#    ifdef _DEBUG
                            
/* Needed for all debuf fprintf calls */
#       include <stdio.h>
#       include <stdarg.h>
static __inline void voVLog( char *str, ...)
{
	va_list args;
	va_start(args, str);
    vfprintf(stderr, str, args);
	va_end(args);
}
                            
#    else /* _DEBUG */
static __inline void voVLog(char *str, ...) {}
#    endif /* _DEBUG */

                            
#endif /* Compiler test */
                            
                            
                            
                            
                            

#if (defined(ARM) || defined(_ARM_)) && defined(_WIN32_WCE)

#define RV_CDECL               __cdecl
#define RV_FREE_STORE_CALL
#define RV_STDCALL
#define INLINE __inline

#elif (defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64))

#define RV_CDECL               __cdecl
#define RV_STDCALL             __stdcall
#define INLINE                 __forceinline

//The Microsoft compiler uses the __cdecl convention for new  and delete.

#define RV_FREE_STORE_CALL     __cdecl

#else // (defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64)) 

#define RV_CDECL
#define RV_STDCALL
#define RV_FREE_STORE_CALL
#define INLINE                 __inline
#endif //(defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64)) 

/******************************************************************************
* GNU variants of STDCALL, CDECL and FASTCALL 
* GD 1/15/01 not supported by SA linux gnu at this point 
******************************************************************************/
#if defined __GNUC__ && !defined(ARM) && !defined(_ARM_)

#define GNUSTDCALL __attribute__ ((stdcall))
#define GNUCDECL   __attribute__ ((cdecl))
#define GNUFASTCALL __attribute__ ((regparm (2)))

#else // defined __GNUC__ 

#define GNUSTDCALL
#define GNUCDECL
#define GNUFASTCALL

#endif // defined __GNUC__ 

#define RV_CALL                RV_STDCALL

#if defined(ARM) || defined(_ARM_)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef float               F32;
typedef double              F64;

#elif defined(_M_IX86) && !defined(_WIN32)

#error "Non-_MSC_VER or 16-bit environments are not supported at this time."

#elif defined(_M_IX86) && defined(_WIN32)

/*************************************************************************************
* Win32 definitions for Windows 95 and Windows NT on Intel
* processors compatible with the x86 instruction set. 
*************************************************************************************/

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef float               F32;
typedef double              F64;

#elif defined(_M_IA64) && defined(_WIN64)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef unsigned __int64	U64;
typedef signed __int64		I64;
typedef float               F32;
typedef double              F64;

#elif defined (_OSF1) || defined(_ALPHA)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned int        U32;
typedef int                 I32;
typedef unsigned long		U64;
typedef signed long			I64;
typedef float               F32;
typedef double              F64;

#elif defined(__ia64)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned int        U32;
typedef signed int          I32;
typedef unsigned long		U64;
typedef signed long			I64;
typedef float               F32;
typedef double              F64;

#elif defined(_MACINTOSH)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
//typedef unsigned __int64	U64;
//typedef signed __int64		I64; 
typedef float               F32;
typedef double              F64;

#elif defined(_LINUX)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
//typedef unsigned __int64	U64;
//typedef signed __int64		I64; 
typedef float               F32;
typedef double              F64;

#elif defined(_HPUX)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
//typedef unsigned __int64	U64;
//typedef signed __int64		I64;
typedef float               F32;
typedef double              F64;

#else
/******************************************************************************************
* These definitions should work for most other "32-bit" environments.
* If not, an additional "#elif" section can be added above. 
******************************************************************************************/

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
//typedef unsigned __int64	U64;
//typedef signed __int64		I64;
typedef float               F32;
typedef double              F64;

#endif

/**********************************************************************************************
* Enumerations 
* The size of an object declared with an enumeration type is 
* compiler-specific.  The Enum32 type can be used to represent 
* enumeration values when the representation is significant. 
***********************************************************************************************/

typedef I32         Enum32;

/*********************************************************************************************************
* RV_Boolean values 
* The "RV_Boolean" type should be used only when an object's size is not 
* significant.  Bool8, Bool16 or Bool32 should be used elsewhere. 
* "TRUE" is defined here for assignment purposes only, for example 
* "is_valid = TRUE;".  As per the definition of C and C++, any 
* non-zero value is considered to be TRUE.  So "TRUE" should not be used 
* in tests such as "if (is_valid == TRUE)".  Use "if (is_valid)" instead. 
*********************************************************************************************************/

#undef  FALSE
#undef  TRUE
#define VO_MAX_ENUM_VALUE	0X7FFFFFFF
typedef int         RV_Boolean;
enum                { FALSE, TRUE,Boolean_MAX = VO_MAX_ENUM_VALUE};


typedef I8          Bool8;
typedef I16         Bool16;
typedef I32         Bool32;

/*Define the "P*" pointer types*/

typedef U8             *PU8;
typedef I8             *PI8;
typedef U16            *PU16;
typedef I16            *PI16;
typedef U32            *PU32;
typedef I32            *PI32;
typedef F32            *PF32;
typedef F64            *PF64;
typedef Bool8          *PBool8;
typedef Bool16         *PBool16;
typedef Bool32         *PBool32;
typedef RV_Boolean    *PBoolean;
typedef Enum32         *PEnum32;

/**********************************************************************************
* NULL is defined here so that you don't always have to 
* include <stdio.h> or some other standard include file. 
**********************************************************************************/

#undef  NULL
#define NULL 0

//Define some useful macros 

#undef  ABS
#define ABS(a)          (((a) < 0) ? (-(a)) : (a))

#undef  MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

#undef  MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))

/* Perform byte swapping on a 16 or 32-byte value. */

#define RV_SWAP16(x) (U16( (((x) & 0xff) << 8) | (((x) & 0xff00) >> 8) ))
#define RV_SWAP32(x) (U32( (((x) & 0xff)     << 24) \
	| (((x) & 0xff00)   <<  8) \
	| (((x) & 0xff0000) >>  8) \
	| (((x) >> 24) & 0xff) ))


#endif /* RVTYPES_H__ */

