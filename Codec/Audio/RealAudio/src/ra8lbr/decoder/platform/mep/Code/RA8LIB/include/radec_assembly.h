
/**************************************************************************** */
/*    Socrates Software Ltd : Toshiba Group Company */
/*    DESCRIPTION OF CHANGES: */
/*		1. Assembly functions support for MeP platform added
/*
 *    CONTRIBUTORS : Vinayak Bhat,Deepak,Sudeendra,Naveen
 *   */
/**************************************************************************** */


#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#include "radec_defines.h"

#if (defined _WIN32 && !defined _WIN32_WCE) || (defined __WINS__ && defined _SYMBIAN) || (defined WINCE_EMULATOR) || (defined _OPENWAVE_SIMULATOR) 

#pragma warning( disable : 4035 )	/* complains about inline asm not returning a value */

static __inline int MULSHIFT32(int x, int y)	
{
    __asm {
		mov		eax, x
	    imul	y
	    mov		eax, edx
	    }
}

static __inline short CLIPTOSHORT(int x)
{
	int sign;

	/* clip to [-32768, 32767] */
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

static __inline int FASTABS(int x) 
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros;
}

/* toolchain:           MSFT Embedded Visual C++
 * target architecture: ARM v.4 and above (require 'M' type processor for 32x32->64 multiplier)
 */
#elif defined _WIN32 && defined _WIN32_WCE && defined ARM

static __inline short CLIPTOSHORT(int x)
{
	int sign;

	/* clip to [-32768, 32767] */
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

static __inline int FASTABS(int x) 
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros;
}

/* implemented in asmfunc.s */
#ifdef __cplusplus
extern "C" {
#endif
int MULSHIFT32(int x, int y);
#ifdef __cplusplus
}
#endif

/* toolchain:           ARM ADS or RealView
 * target architecture: ARM v.4 and above (requires 'M' type processor for 32x32->64 multiplier)
 */
#elif defined ARM_ADS

static __inline int MULSHIFT32(int x, int y)
{
    /* rules for smull RdLo, RdHi, Rm, Rs:
     *   RdHi != Rm 
     *   RdLo != Rm 
     *   RdHi != RdLo
     */
    int zlow;
    __asm {
    	smull zlow,y,x,y
   	}

    return y;
}

static __inline short CLIPTOSHORT(int x)
{
	int sign;

	/* clip to [-32768, 32767] */
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

static __inline int FASTABS(int x) 
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros;
}

/* toolchain:           ARM gcc
 * target architecture: ARM v.4 and above (requires 'M' type processor for 32x32->64 multiplier)
 */
#elif defined(__GNUC__) && defined(ARM)

static __inline__ int MULSHIFT32(int x, int y)
{
    int zlow;
    __asm__ volatile ("smull %0,%1,%2,%3" : "=&r" (zlow), "=r" (y) : "r" (x), "1" (y));
    return y;
}

static __inline short CLIPTOSHORT(int x)
{
	int sign;

	/* clip to [-32768, 32767] */
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

static __inline int FASTABS(int x) 
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros;
}

/* toolchain:           x86 gcc
 * target architecture: x86
 */
#elif defined(__GNUC__) && defined(__i386__)

static __inline__ int MULSHIFT32(int x, int y)
{
    int z;
	__asm__ volatile ("imull %3" : "=d" (z), "=a" (x): "1" (x), "r" (y));
    return z;
}

static __inline short CLIPTOSHORT(int x)
{
	int sign;

	/* clip to [-32768, 32767] */
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

static __inline int FASTABS(int x) 
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

static __inline int CLZ(int x)
{
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros;
}

#elif defined (MEP)
#include "asp.h"
typedef short INT16;
typedef unsigned short UINT16;

int __inline MULSHIFT32(int iVal1, int iVal2)
{
		int temp;	
		mul(iVal1, iVal2);
		temp = __HI;
		return(temp);
#if 0
		return((((INT16)(iVal1 >> 16) * (INT16)(iVal2 >> 16)))\
		+ (((INT16)(iVal1 >> 16) * (UINT16)(iVal2 &0xffff)) >> 16)\
		+ (((INT16)(iVal2 >> 16) * (UINT16)(iVal1 &0xffff)) >> 16));
#endif

}			


short __inline CLIPTOSHORT(int x)
{
	clip(x,16);
	return(x);

#if 0
    int sign;
   /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
    x = sign ^ ((1 << 15) - 1);
    return (short)x;
#endif
}

int __inline FASTABS(int x)
{
    int result;
    abs(result, x);
    return(result);

#if 0
	int sign;
	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;
    return x;
#endif
}

int __inline CLZ(int x)
{
    int result;
    ldz(result,x);
    return(result);

#if 0
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000)) 
    {
        numZeros++;
        x <<= 1;
    }
    return numZeros;
#endif
}

#else

#error Unsupported platform in assembly.h

#endif	/* platforms */

#endif /* _ASSEMBLY_H */
