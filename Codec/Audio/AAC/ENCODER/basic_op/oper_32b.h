/* Double precision operations */
/* $Id: oper_32b.h,v 1.7 2004-11-26 12:45:13 schug Exp $ */

#ifndef __OPER_32b_H
#define __OPER_32b_H

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POW2_TABLE_BITS 8
#define POW2_TABLE_SIZE (1<<POW2_TABLE_BITS)

void L_Extract (Word32 L_32, Word16 *hi, Word16 *lo);
Word32 L_Comp (Word16 hi, Word16 lo);
Word32 Mpy_32 (Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n);
Word32 Div_32 (Word32 L_num, Word32 denom);
Word16 iLog4(Word32 value);
Word32 rsqrt(Word32 value,  Word32 accuracy);
Word32 pow2_xy(Word32 x, Word32 y);

#if (!L_MPY_LS_IS_INLINE)
Word32 L_mpy_ls(Word32 L_var2, Word16 var1);
#else
__inline Word32 L_mpy_ls(Word32 L_var2, Word16 var1)
{
#if (FUNC_L_MPY_LS_OPT)
#if ARMV5TE_L_MPY_LS
#ifdef LINUX
	Word32 result = 0; 
	asm volatile( 
		"SMULWB  r2, %[L_var2], %[var1] \n" 
		"QADD	 %[result], r2, r2 \n"
		:[result]"+r"(result)
		:[L_var2]"r"(L_var2), [var1]"r"(var1)
		:"r2" 
		); 
	return result;
#else //LINUX
	__asm
	{
		SMULWB	r3, r0, r1
		qadd	r0, r3, r3
	}
#endif //LINUX
#else
    unsigned short swLow1;
    Word16 swHigh1;
    Word32 l_var_out;

//    swLow1 = (unsigned short)(0xffff & L_var2);
    swLow1 = (unsigned short)(L_var2);
    swHigh1 = (Word16)(L_var2 >> 16);

    /* (unsigned)low2 * (signed)low1 */
    l_var_out = (long)swLow1 * (long)var1 >> 15;
    
 //   l_var_out = L_mac(l_var_out, swHigh1, var1);
    l_var_out += swHigh1 * var1 << 1;
    
    return(l_var_out);
#endif
#else
    unsigned short swLow1;
    Word16 swHigh1;
    Word32 l_var_out;

    swLow1 = (unsigned short)(0xffff & L_var2);
    swHigh1 = (Word16)(L_var2 >> 16);

    /* (unsigned)low2 * (signed)low1 */
    l_var_out = (long)swLow1 * (long)var1;
    l_var_out >>= 15;
    
    l_var_out = L_mac(l_var_out, swHigh1, var1);
    
    return(l_var_out);
#endif
}
#endif

#ifdef __cplusplus
}
#endif

#endif
