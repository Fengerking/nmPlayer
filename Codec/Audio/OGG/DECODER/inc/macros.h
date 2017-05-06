//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    macros.h

Abstract:

    Vorbis macros define header file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/
#ifndef __MACROS_H_	//"macros.h"
#define __MACROS_H_

#include <stdlib.h>

#include "voOGG.h"
#include "voOGGDecID.h"

typedef long long		OGG_S64;
typedef int				OGG_S32;
typedef unsigned int	OGG_U32;
typedef unsigned char	OGG_U8;
//#ifdef WIN32
#define STIN static __inline
//#else
//#define STIN static inline
//#endif
#if defined(_WIN32) || defined(LINUX)
#define LITTLE_ENDIAN  1
#define BYTE_ORDER     LITTLE_ENDIAN
#endif	//defined(_WIN32) || defined(LINUX)

/*
 * This should be used as a memory barrier, forcing all cached values in
 * registers to wr writen back to memory.  Might or might not be beneficial
 * depending on the architecture and compiler.
 */
#define MB()

#ifndef _LOW_ACCURACY_
#  define X(n) (n)
#  define LOOKUP_T const OGG_S32
#else
#  define X0(n) (((n)>>22)+1)
#  define X(n) ((X0(n)>>1) - (X0(n)>>9)) 
#  define LOOKUP_T const OGG_U8
#endif

#ifndef  _LOW_ACCURACY_
/* 64 bit multiply */

#if !(defined WIN32 && defined WINCE)
//#include <sys/types.h>
#endif

#if BYTE_ORDER==LITTLE_ENDIAN
union magicNumber {
  struct {
    OGG_U32 loPart;
    OGG_S32 hiPart;
  } halves;
  OGG_S64 whole;
};
#endif 

#if BYTE_ORDER==BIG_ENDIAN
union magicNumber {
  struct {
    OGG_S32 hiPart;
    OGG_U32 loPart;
  } halves;
  OGG_S64 whole;
};
#endif

STIN OGG_S32 MULT31_SHIFT15(OGG_S32 x, OGG_S32 y) {
  union magicNumber magic;
  magic.whole  = (OGG_S64)x * y;
  return ((magic.halves.hiPart)<<17) | ((OGG_U32)(magic.halves.loPart)>>15);
}

STIN OGG_S32 MULT32(OGG_S32 x, OGG_S32 y) {
  union magicNumber magic;
  magic.whole = (OGG_S64)x * y;
  return  magic.whole>>32;
}

STIN OGG_S32 MULT31(OGG_S32 x, OGG_S32 y) { 
  union magicNumber magic;
  magic.whole = (OGG_S64)x * y;
  return magic.halves.hiPart<<1;
}

#else	//_LOW_ACCURACY_
/* 32 bit multiply, more portable although less accurate */

STIN OGG_S32 MULT31_SHIFT15(OGG_S32 x, OGG_S32 y) 
{
  return (x >> 6) * y;  /* y preshifted >>9 */
}

STIN OGG_S32 MULT32(OGG_S32 x, OGG_S32 y) 
{
  return (x >> 9) * y;  /* y preshifted >>23 */
}

STIN OGG_S32 MULT31(OGG_S32 x, OGG_S32 y) 
{
  return (x >> 8) * y;  /* y preshifted >>23 */
}

#endif	//_LOW_ACCURACY_


#define X_ADD_SUB(a,b,c,d) { (c) = (a) - (b); (d) = (a) + (b);} 


STIN void XPROD32(OGG_S32  a, OGG_S32  b,
			   OGG_S32  t, OGG_S32  v,
			   OGG_S32 *x, OGG_S32 *y)
{
  *x = MULT32(a, t) + MULT32(b, v);
  *y = MULT32(b, t) - MULT32(a, v);
}

STIN void XPROD31(OGG_S32  a, OGG_S32  b,
			   OGG_S32  t, OGG_S32  v,
			   OGG_S32 *x, OGG_S32 *y)
{
  *x = MULT31(a, t) + MULT31(b, v);
  *y = MULT31(b, t) - MULT31(a, v);
}

STIN void XNPROD31(OGG_S32  a, OGG_S32  b,
			    OGG_S32  t, OGG_S32  v,
			    OGG_S32 *x, OGG_S32 *y)
{
  *x = MULT31(a, t) - MULT31(b, v);
  *y = MULT31(b, t) + MULT31(a, v);
}

STIN OGG_S32 VFLOAT_MULT(OGG_S32 a,OGG_S32 ap,
				      OGG_S32 b,OGG_S32 bp,
				      OGG_S32 *p)
{
	if(a && b)
	{
#ifndef _LOW_ACCURACY_
		*p = ap + bp + 32;
		return MULT32(a,b);
#else
		*p = ap + bp + 31;
		return (a>>15) * (b>>16); 
#endif
	}else
		return 0;
}

#include "codebook.h"

STIN OGG_S32 VFLOAT_MULTI(OGG_S32 a,OGG_S32 ap,
				      OGG_S32 i,
				      OGG_S32 *p)
{
	int ip = ILOG(abs(i)) - 31;
	return VFLOAT_MULT(a, ap, i<<-ip, ip, p);
}

STIN OGG_S32 VFLOAT_ADD(OGG_S32 a,OGG_S32 ap,
				      OGG_S32 b,OGG_S32 bp,
				      OGG_S32 *p)
{

	if(!a)
	{
		*p = bp;
		return b;
	}else if(!b)
	{
		*p = ap;
		return a;
	}

	/* yes, this can leak a bit. */
	if (ap>bp)
	{
		int shift = ap - bp + 1;
		*p = ap + 1;
		a >>= 1;
		if (shift<32)
		{
			b = (b+(1<<(shift-1))) >> shift;
		}else{
			b = 0;
		}
	}else{
		int shift = bp - ap + 1;
		*p = bp + 1;
		b >>= 1;
		if (shift<32){
			a = (a+(1<<(shift-1)))>>shift;
		}else{
			a = 0;
		}
	}

	a += b;
	if ((a&0xc0000000)==0xc0000000 || 
		(a&0xc0000000)==0)
	{
			a <<= 1;
			(*p)--;
	}
	return(a);
}

#ifndef _V_CLIP_MATH
#define _V_CLIP_MATH

STIN OGG_S32 CLIP_TO_15(OGG_S32 x) 
{
	int ret = x;
	ret -= ((x<=32767)-1)&(x-32767);
	ret -= ((x>=-32768)-1)&(x+32768);
	return(ret);
}

#endif

#endif	//__MACROS_H_


