/***********************************************************************
 *
 *   FILE : basic_op.h (essentially identical to mathevrc.h)
 *
 *   PURPOSE:
 *     
 *     Modified TIA basic operations.  Bit-exact simulation of a
 *     generic 32 bit accumulator DSP chip (fractional math).  This
 *     version has a latching overflow bit (giOverflow) and
 *     non-compound MAC's (One where initial mult does not saturate)
 *     
 ***********************************************************************/

#ifndef __MATHTIA
#define __MATHTIA

#include "typedef_fx.h"
extern short gArray0[200];

#ifdef LINUX
#define  __voinline  static __inline__
#else
#define  __voinline  static __inline
#endif 

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L
#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/
#define abs_s(x)         ((Word16)(((x) != MIN_16) ? (((x) >= 0) ? (x) : (-(x))) : MAX_16))
#define L_deposit_h(x)   (((Word32)(x)) << 16)
#define L_deposit_l(x)   ((Word32)(x))
#define L_negate(L_var1) (((L_var1) == (MIN_32)) ? (MAX_32) : (-(L_var1)))
#define negate(var1)     ((Word16)(((var1) == SW_MIN) ? SW_MAX : (-(var1))))
#define extract_h(x)     (((x))>>16)
#define extract_l(x)     (Word16)((x))
/* arithmetic shifts */
__voinline Word16 saturate(Word32 L_var1);
__voinline Word16 add(Word16 var1, Word16 var2);
__voinline Word32 L_abs (Word32 L_var1);
__voinline Word32 L_mult(Word16 var1, Word16 var2);
__voinline Word32 L_add (Word32 L_var1, Word32 L_var2);
__voinline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);
__voinline Word32 L_sub(Word32 L_var1, Word32 L_var2);
__voinline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);
__voinline Word32 L_shl(Word32 L_var1, Word16 var2);
__voinline Word32 L_shr(Word32 L_var1, Word16 var2);
__voinline Word16 shl(Word16 var1, Word16 var2);
__voinline Word16 shr(Word16 var1, Word16 var2);
__voinline Word32 L_shl2(Word32 L_var1, Word16 var2);
__voinline Word32 L_shr2(Word32 L_var1, Word16 var2);
__voinline Word16 mult(Word16 var1, Word16 var2);
__voinline Word16 mult_r(Word16 var1, Word16 var2);
__voinline Word16 norm_l (Word32 L_var1);
__voinline Word16 norm_s (Word16 var1);
__voinline Word16 round32(Word32 L_var1);
__voinline Word16 sub(Word16 var1, Word16 var2);
__voinline Word16 divide_s(Word16 var1, Word16 var2);
__voinline Word32 L_mpy_ll(Word32 L_var1, Word32 L_var2);
__voinline Word32 L_mpy_ls(Word32 L_var2, Word16 var1);
__voinline Word32 L_divide(Word32 L_num, Word32 L_denom);
__voinline Word16 sqroot(Word32 L_SqrtIn);
__voinline Word32 fnLog10(Word32 L_Input);
__voinline Word32 fnLog2(Word32 L_Input);
__voinline Word32 fnLog(Word32 L_Input);

Word16 divide_s_cb(Word16 var1, Word16 var2);

__voinline Word16 saturate(Word32 L_var1)
{
#ifdef ARMGCC_OPT
    Word32   result = 0;
    asm volatile(
		"SSAT     %[result], #15,  %[L_var1] \n\t"
		:[result] "+r" (result) : [L_var1] "r" (L_var1)
		);
    return   (Word16)result;
#else
    Word16 var_out;
    if (L_var1 > SW_MAX)
    {
        var_out = SW_MAX;
    }
    else if (L_var1 < SW_MIN)
    {
        var_out = SW_MIN;
    }
    else
    {
        var_out = (Word16) L_var1;
    }
    return (var_out);
#endif
}

__voinline Word16 add(Word16 var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	register Word16 temp = 0;
	asm volatile(
			"QADD  %[temp], %[var1], %[var2]\n"
			:[temp] "=r" (temp): [var1] "r" (var1), [var2] "r" (var2)
		    );
	return  temp;
#else
	Word16 var_out;
	Word32 L_sum;
	L_sum = (Word32) var1 + var2;
	var_out = saturate(L_sum);
	return (var_out);
#endif
}

__voinline Word32 L_abs (Word32 L_var1)
{
    Word32 L_var_out;
    if (L_var1 == LW_MIN)
    {
        L_var_out = LW_MAX;
    }
    else
    {
        if (L_var1 < 0)
        {
            L_var_out = -L_var1;
        }
        else
        {
            L_var_out = L_var1;
        }
    }
    return (L_var_out);
}


__voinline Word32 L_mult(Word16 var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	Word32    result = 0;
	asm volatile(
			"SMULBB   %[result], %[var1], %[var2] \n"
			"QADD     %[result], %[result], %[result] \n"
			:[result] "+r" (result): [var1] "r" (var1), [var2] "r" (var2)
		    );
	return  result;
#else
	Word32 L_var_out;
	L_var_out = (Word32) var1 * (Word32) var2;
	if (L_var_out != (Word32) 0x40000000L)
	{
		L_var_out <<= 1;
	}
	else
	{
		L_var_out = MAX_32;
	}  
	return (L_var_out);
#endif
}

__voinline Word32 L_add (Word32 L_var1, Word32 L_var2)
{
#ifdef ARMGCC_OPT
	register  Word32 ra = L_var1;
	register  Word32 rb = L_var2;
	Word32    result;
	asm volatile(
			"QADD   %[result], %[ra], %[rb]\n"
			:[result] "=r" (result): [ra] "r" (ra), [rb] "r" (rb) 
		    );
	return    result;
#else
	Word32 L_var_out;
	L_var_out = L_var1 + L_var2;
	if (((L_var1 ^ L_var2) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
#endif
}

__voinline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	Word32   result = 0;
	asm volatile (
			"SMULBB  %[result], %[var1], %[var2]\n"
			"QADD    %[result], %[result], %[result]\n"
			"QADD    %[result], %[result], %[L_var3]\n"
			:[result] "+r" (result):[L_var3] "r" (L_var3), [var1] "r" (var1), [var2] "r" (var2)
		     );
	return   result;	
#else
	Word32 L_var_out;
	Word32 L_product;
	L_product = L_mult(var1, var2);
	L_var_out = L_add (L_var3, L_product);
	return (L_var_out);
#endif
}

__voinline Word32 L_sub(Word32 L_var1, Word32 L_var2)
{
#ifdef ARMGCC_OPT
	register  Word32 ra = L_var1;
	register  Word32 rb = L_var2;
	Word32    result;
	asm volatile(
			"QSUB   %[result], %[ra], %[rb]\n"
			:[result] "=r" (result): [ra] "r" (ra), [rb] "r" (rb) 
		    );
	return    result;	
#else
	Word32 L_var_out;
	L_var_out = L_var1 - L_var2;
	if (((L_var1 ^ L_var2) & MIN_32) != 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
#endif
}

__voinline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	Word32   result = 0;
	Word32   temp = 0;
	asm volatile (
			"SMULBB  %[temp], %[var1], %[var2]\n"
			"QADD    %[result], %[temp], %[temp]\n"
			"QSUB    %[result], %[L_var3], %[result]\n"
			:[result] "+r" (result), [temp] "+r" (temp):[L_var3] "r" (L_var3), [var1] "r" (var1), [var2] "r" (var2)
		     );
	return   result;
#else  
	Word32 L_var_out;
	Word32 L_product;
	L_product = ((long)var1 * var2) << 1;
	L_var_out = L_var3 - L_product;
	return (L_var_out);
#endif
}

__voinline Word32 L_shl2(Word32 L_var1, Word16 var2)
{
#ifdef ARMGCC_OPT
        register  Word32 result = 0; 
	asm volatile(
	            "MOV    r2, #0x7FFFFFFF \n\t"
	            "MOV    %[result], %[L_var1], LSL %[var2] \n\t"
	            "TEQ    %[L_var1], %[result], ASR %[var2] \n\t"
	            "EORNE  %[result], r2, %[L_var1], ASR #31 \n\t"	    
	            :[result] "+r" (result), [L_var1] "+r" (L_var1) : [var2] "r" (var2): "r2"
		    );
	return result;
#else
	for (; var2 > 0; var2--)
	{
		if (L_var1 > (Word32) 0X3fffffffL)
		{
			return MAX_32;
		}
		else
		{
			if (L_var1 < (Word32) 0xc0000000L)
			{
				return MIN_32;
			}
		}
		L_var1 <<= 1;
	}
	return (L_var1);
#endif
}

__voinline Word32 L_shl(Word32 L_var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	if(var2>=0)
	{
		return  L_shl2(L_var1, var2);
	}
	else
	{
		return  L_shr2(L_var1, -var2);
	}
#else
	if (var2 <= 0)
	{
		L_var1 = L_shr2(L_var1, (Word16)-var2);
	}
	else
	{
		for (; var2 > 0; var2--)
		{
			if (L_var1 > (Word32) 0X3fffffffL)
			{
				return MAX_32;
			}
			else
			{
				if (L_var1 < (Word32) 0xc0000000L)
				{
					return MIN_32;
				}
			}
			L_var1 <<= 1;
		}
	}
	return (L_var1);
#endif 
}

__voinline Word32 L_shr2(Word32 L_var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	register Word32 result = 0;
	asm volatile(
			"MOV  %[result], %[L_var1], ASR %[var2] \n\t"
			:[result] "+r" (result) : [L_var1] "r" (L_var1), [var2] "r" (var2)
		    );
	return result;
#else
	Word32 L_var_out;
	if (var2 >= 31)
	{
		L_var_out = (L_var1 < 0L) ? -1 : 0;
	}
	else
	{
		if (L_var1 < 0)
		{
			L_var_out = ~((~L_var1) >> var2);
		}
		else
		{
			L_var_out = L_var1 >> var2;
		}
	}
	return (L_var_out);
#endif
}

__voinline Word32 L_shr(Word32 L_var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	if(var2>=0)
	{
		return L_shr2(L_var1, var2);		
	}
	else
	{
		return L_shl2(L_var1, -var2);
	}
#else
	Word32 L_var_out;
	if (var2 < 0)
	{
		L_var_out = L_shl2(L_var1, (Word16)-var2);
	}
	else
	{
		if (var2 >= 31)
		{
			L_var_out = (L_var1 < 0L) ? -1 : 0;
		}
		else
		{
			if (L_var1 < 0)
			{
				L_var_out = ~((~L_var1) >> var2);
			}
			else
			{
				L_var_out = L_var1 >> var2;
			}
		}
	}
	return (L_var_out);
#endif
}

__voinline Word16 mult(Word16 var1, Word16 var2)
{
    Word16 var_out;
    Word32 L_product;
    L_product = (Word32)var1 * var2;
    if (L_product > (long)0x3fff8000)
    {
        var_out = MAX_16;
    }
    else
    {
        var_out = (short)(L_product >> 15);
    }
    return (var_out);
}

__voinline Word16 mult_r(Word16 var1, Word16 var2)
{
    Word16 var_out;
    Word32 L_product_arr;
    L_product_arr = (Word32) var1 *(Word32) var2;       /* product */
    if (L_product_arr > (long)0x3fffc000)
    {
        var_out = MAX_16;
    }
    else
    {
        var_out = (short)((L_product_arr + 0x4000) >> 15);
    }
    return (var_out);
}

__voinline Word16 norm_l (Word32 L_var1)
{
    Word16 var_out;
    if (L_var1 == 0)
    {
        var_out = 0;
    }
    else
    {
        if (L_var1 == (Word32) 0xffffffffL)
        {
            var_out = 31;
        }
        else
        {
            if (L_var1 < 0)
            {
                L_var1 = ~L_var1;
            }
            for (var_out = 0; L_var1 < (Word32) 0x40000000L; var_out++)
            {
                L_var1 <<= 1;
            }
        }
    }
    return (var_out);
}

//**********************************************************
__voinline Word16 norm_s (Word16 var1)
{
    Word16 var_out;
    if (var1 == 0)
    {
        var_out = 0;
    }
    else
    {
        if (var1 == -1)
        {
            var_out = 15;
        }
        else
        {
            if (var1 < 0)
            {
                var1 = (Word16)~var1;
            }
            for (var_out = 0; var1 < 0x4000; var_out++)
            {
                var1 <<= 1;
            }
        }
    }
    return (var_out);
}

__voinline Word16 round32(Word32 L_var1)
{ 
    Word16 var_out;
    if (L_var1 < 0x7fff8000)
    {
        var_out = (Word16)((L_var1 + 0x8000) >> 16);
    }
    else
    {
        var_out = MAX_16;
    }
    return (var_out);
}

__voinline Word16 shl (Word16 var1, Word16 var2)
{
    Word16 var_out;
    Word32 result;
    if (var2 < 0)
    {
        if (var2 < -16)
            var2 = -16;
        var_out = shr (var1, (Word16)-var2);
    }
    else
    {
        result = (Word32) var1 *((Word32) 1 << var2);
        if ((var2 > 15 && var1 != 0) || (result != (Word32) ((Word16) result)))
        {
            var_out = (Word16)((var1 > 0) ? MAX_16 : MIN_16);
        }
        else
        {
            var_out = (Word16)result;
        }
    }
    return (var_out);
}

__voinline Word16 shr (Word16 var1, Word16 var2)
{
    Word16 var_out;
    if (var2 < 0)
    {
        if (var2 < -16)
            var2 = -16;
        var_out = shl (var1, (Word16)-var2);
    }
    else
    {
        if (var2 >= 15)
        {
            var_out = (Word16)((var1 < 0) ? -1 : 0);
        }
        else
        {
            if (var1 < 0)
            {
                var_out = (Word16)(~((~var1) >> var2));
            }
            else
            {
                var_out = (Word16)(var1 >> var2);
            }
        }
    }
    return (var_out);
}

__voinline Word16 sub(Word16 var1, Word16 var2)
{
    Word16 var_out;
    Word32 L_diff;

    L_diff = (Word32) var1 - var2;
    var_out = saturate(L_diff);
    if (L_diff > MAX_16) var_out = MAX_16;
    else if (L_diff < MIN_16) var_out = MIN_16;
    else var_out = (short)L_diff;
    return (var_out);
}

__voinline Word16 divide_s(Word16 var1, Word16 var2)
{
#if 1 
	Word32 L_div;
	Word16 swOut;	
	if (var1 < 0 || var2 < 0 || var1 > var2)
	{
		return (0);
	}
	if (var1 == var2)
	{
		return (0x7fff);
	}
	L_div = ((0x00008000L * (Word32) var1) / (Word32) var2);
	swOut = saturate(L_div);
	return (swOut);
#else
	Word16 var_out = 0;
	Word16 iteration;
	Word32 L_num;
	Word32 L_denom;	
	if (var1 < 0 || var2 < 0 || var1 > var2)
	{
		return (0);
	}

	if (var1 == var2)
	{
		var_out = 0x7fff;
	}
	L_num = var1;
	L_denom = var2;
	for (iteration = 0; iteration < 15; iteration++)
	{
		var_out <<= 1;
		L_num <<= 1;
		if (L_num >= L_denom)
		{
			L_num -= L_denom;
			var_out++;
		}
	}	
	return (var_out);
#endif
}

__voinline Word32 L_mpy_ll(Word32 L_var1, Word32 L_var2)
{
    unsigned short swLow1;
    unsigned short swLow2;
    Word16 swHigh1;
    Word16 swHigh2;  
    Word32 l_var_temp;
    Word32 l_var_out;

    swLow1 = (unsigned short)(0xffff & L_var1);
    swLow2 = (unsigned short)(0xffff & L_var2);
    swHigh1 = (Word16)(L_var1 >> 16);
    swHigh2 = (Word16)(L_var2 >> 16);

    /* (unsigned)low1 * (unsigned)low1 */
    l_var_out = (unsigned long)(swLow1) * (unsigned long)(swLow2) >> 16;

    /* (unsigned)low1 * (signed)high2 */
    l_var_out += (signed long)(swLow1) * (signed long)swHigh2;
    l_var_temp = ((signed long)swLow2) * ((signed long)swHigh1);
    if ((l_var_out & 0x01) && (l_var_temp & 0x01))
    {
        l_var_out = (l_var_out >> 1) + 1;
    }
    else
    {
        l_var_out >>= 1;
    } 
    /* (unsigned)low2 * (signed)high1 */
    l_var_out += l_var_temp >> 1;
    l_var_out >>= 14;  
    //l_var_out = L_mac(l_var_out, swHigh1, swHigh2);
    l_var_temp = ((long)swHigh1 * swHigh2) << 1;
    l_var_out += l_var_temp;
    return(l_var_out);
}

__voinline Word32 L_mpy_ls(Word32 L_var2, Word16 var1)
{
    unsigned short swLow1;
    Word16 swHigh1;
    Word32 l_var_out;
    //swLow1 = (unsigned short)(0xffff & L_var2);
    swLow1 = (unsigned short)(L_var2);
    swHigh1 = (Word16)(L_var2 >> 16);
    /* (unsigned)low2 * (signed)low1 */
    l_var_out = (long)swLow1 * (long)var1 >> 15; 
    //l_var_out = L_mac(l_var_out, swHigh1, var1);
    l_var_out += swHigh1 * var1 << 1;
    return(l_var_out);
}

__voinline Word32 L_divide(Word32 L_num, Word32 L_denom)
{
	Word16 approx;
	Word32 L_div;
	if (L_num < 0 || L_denom < 0 || L_num > L_denom)
	{
		return (0);
	}

	/* First approximation: 1 / L_denom = 1/extract_h(L_denom) */
	approx = divide_s((Word16) 0x3fff, (short)(L_denom >> 16));
	/* 1/L_denom = approx * (2.0 - L_denom * approx) */
	L_div = L_mpy_ls(L_denom, approx);
	L_div = (Word32)0x7fffffffL - L_div;
	L_div = L_mpy_ls(L_div, approx);
	/* L_num * (1/L_denom) */
	L_div = L_mpy_ll(L_num, L_div);
	L_div <<= 2;
	return (L_div);

}

__voinline Word16 sqroot(Word32 L_SqrtIn)
{
          #define    PLUS_HALF          0x40000000L	/* 0.5 */
          #define    MINUS_ONE          0x80000000L	/* -1 */
          #define    TERM5_MULTIPLER    0x5000	/* 0.625 */
          #define    TERM6_MULTIPLER    0x7000	/* 0.875 */

	/*_________________________________________________________________________
	  |                                                                         |
	  |                            Automatic Variables                          |
	  |_________________________________________________________________________|
	  */
	Word32 L_Temp0, L_Temp1;
	Word16 swTemp, swTemp2, swTemp3, swTemp4, swSqrtOut;

	/* determine 2nd term x/2 = (y-1)/2 */
	L_Temp1 = L_SqrtIn >> 1;			/* L_Temp1 = y/2 */
	L_Temp1 -= (long)PLUS_HALF;			/* L_Temp1 = (y-1)/2 */
	swTemp = (short)(L_Temp1 >> 16);		/* swTemp = x/2 */

	/* add contribution of 2nd term */
	L_Temp1 -= (long)MINUS_ONE;			/* L_Temp1 = 1 + x/2 */

	/* determine 3rd term */
	L_Temp0 = -((long)swTemp * swTemp);		/* L_Temp0 = -(x/2)^2 */
	swTemp2 = (short)(L_Temp0 >> 15);		/* swTemp2 = -(x/2)^2 */

	/* add contribution of 3rd term */
	L_Temp0 += L_Temp1;				/* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */

	/* determine 4rd term */
	L_Temp1 = -((long)swTemp * swTemp2);		/* L_Temp1 = (x/2)^3 */
	swTemp3 = (short)(L_Temp1 >> 15);		/* swTemp3 = (x/2)^3 */
	/* add contribution of 4rd term */
	L_Temp1 += L_Temp0;
	/* determine partial 5th term */
	L_Temp0 = (long)swTemp * swTemp3;		/* L_Temp0 = (x/2)^4 */
	swTemp4 = (short)((L_Temp0 + 0x4000) >> 15);	/* swTemp4 = (x/2)^4 */

	/* determine partial 6th term */
	L_Temp0 = -((long)swTemp2 * swTemp3);		/* L_Temp0 = (x/2)^5 */
	swTemp2 = (short)((L_Temp0 + 0x4000) >> 15);	/* swTemp2 = (x/2)^5 */

	/* L_Temp0 = -0.625*(x/2)^4 */
	L_Temp0 = -(((long)TERM5_MULTIPLER * swTemp4) << 1);
	/* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 */
	L_Temp1 += L_Temp0;
	/* swSqrtOut = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
	/* - 0.625*(x/2)^4 + 0.875*(x/2)^5     */
	L_Temp0 = L_Temp1 + ((long)TERM6_MULTIPLER * swTemp2 << 1);
	if (L_Temp0 < 0x7fff8000)
	{
		swSqrtOut = (short)((L_Temp0 + 0x8000) >> 16);
	}
	else
	{
		swSqrtOut = MAX_16;
	}
	return (swSqrtOut);
}

__voinline Word32 fnLog10(Word32 L_Input)
{
	static Word16 Scale = 9864;			/* 0.30103 = log10(2) */
	Word32 LwIn;
	/* 0.30103*log2(x) */
	LwIn = fnLog2(L_Input);
	LwIn = L_mpy_ls(LwIn, Scale);

	return (LwIn);
}

__voinline Word32 fnLog2(Word32 L_Input)
{
	static Word16 swC0 = -0x2b2a, swC1 = 0x7fc5, swC2 = -0x54d0;
	Word16 siShiftCnt, swInSqrd, swIn;
	Word32 LwIn;
	/* normalize input and store shifts required */
	siShiftCnt = norm_l(L_Input);
	LwIn = L_Input << siShiftCnt;
	siShiftCnt++;
	siShiftCnt = -siShiftCnt;

	/* calculate x*x*c0 */
	swIn = (short)(LwIn >> 16);
	swInSqrd = (short)(((long)swIn * swIn + 0x4000) >> 15);
	LwIn = ((long)swInSqrd * swC0) << 1;

	/* add x*c1 */
	/* --------- */

	LwIn += ((long)swIn * swC1) << 1;

	/* add c2 */
	/* ------ */

	LwIn += (long)swC2 << 16;

	/* apply *(4/32) */
	/* ------------- */

	LwIn >>= 3;
	LwIn = LwIn & 0x03ffffff;
	siShiftCnt <<= 10;
	LwIn += (long)siShiftCnt << 16;

	return (LwIn);
}

__voinline Word32 fnLog(Word32 L_Input)
{
	static Word16 Scale = 22713;			/* 0.693147 = log(2) */
	Word32 LwIn;
	/* 0.693147*log2(x) */
	/* ---------------- */
	LwIn = fnLog2(L_Input);
	LwIn = L_mpy_ls(LwIn, Scale);
	return (LwIn);
}

#endif
