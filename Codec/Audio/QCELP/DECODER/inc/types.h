/*****************************************************************************
* File:           types.h
*
* Version:        DSPSE Release    2.0
*
* Description:    
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

/***************************************************************************/
/* Type definitions                                                        */
/***************************************************************************/
#ifndef __TYPES_H__
#define __TYPES_H__

typedef char            Char;       /* 8  bit character                    */
typedef short int       Int;        /* 16 bit integer                      */
typedef long            Long;       /* 32 bit integer                      */
typedef float           Float;      /* Floating point number               */
typedef unsigned char   Uchar;      /* 8  bit unsigned character           */
typedef unsigned short  Uint;       /* 16 bit unsigned integer             */
typedef unsigned long   Ulong;      /* 32 bit unsigned integer             */
typedef signed char     Short;      /* 8  bit unsigned integer             */
typedef Short           Int8;       /* 8  bit signed integer               */
typedef Int             Int16;      /* 16 bit signed integer               */
typedef Long            Int32;      /* 32 bit signed integer               */
typedef Uchar           Uint8;      /* 8  bit unsigned integer             */
typedef Uint            Uint16;     /* 16 bit unsigned integer             */
typedef Ulong           Uint32;     /* 32 bit unsigned integer             */
typedef char            Byte;       /* Smallest addressable word size      */
typedef Int             Type;       /* General parameter type              */
typedef Byte            Arg;        /* General argument                    */
typedef Uint            Bool;       /* Boolean logical                     */
typedef Byte           *Ptr;        /* Pointer to arbitrary type           */
typedef Char           *String;     /* Pointer to character                */
typedef Int           (*Fnptr)();   /* Function pointer                    */
typedef void            Void;

#endif //__TYPES_H__
