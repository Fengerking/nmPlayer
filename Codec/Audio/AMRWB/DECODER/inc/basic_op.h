/*___________________________________________________________________________
|                                                                           |
|   Constants and Globals                                                   |
|                                                                           |
| $Id $
|___________________________________________________________________________|
*/
#ifndef __BASIC_OP_H__
#define __BASIC_OP_H__

#include "mem_align.h"
#if MSVC_AMRWBPLUS
#include <stdio.h>
#include <stdlib.h>
#endif

//C Level Macros
#define EVC_AMRWBPLUS   1
//*************

#ifdef LINUX
#define  __voinline  static __inline__
#else
#define  __voinline  static __inline
#endif 
#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L
#define MAX_16 (Word16)+32767    /* 0x7fff */
#define MIN_16 (Word16)-32768    /* 0x8000 */

/*___________________________________________________________________________
|                                                                           |
|   Prototypes for basic arithmetic operators                               |
|___________________________________________________________________________|
*/
#define  saturate(L_var1)   (((L_var1) > 0X00007fffL) ? (MAX_16): (((L_var1) < (Word32) 0xffff8000L) ? (MIN_16): ((L_var1) & 0xffff)))
#define  L_mult(var1, var2) ((((Word32) var1 *(Word32) var2) != (Word32)0x40000000L) ? (((Word32) var1 *(Word32) var2) << 1) : (MAX_32))
#define  abs_s(x)           ((Word16)(((x) != MIN_16) ? (((x) >= 0) ? (x) : (-(x))) : MAX_16))
#define  L_deposit_h(x)     (((Word32)(x)) << 16)
#define  L_deposit_l(x)     ((Word32)(x))
#define  L_abs(x)           (((x) != MIN_32) ? (((x) >= 0) ? (x) : (-(x))) : MAX_32)
#define  negate(var1)       ((Word16)(((var1) == MIN_16) ? MAX_16 : (-(var1)))) 
#define  L_negate(L_var1)   (((L_var1) == (MIN_32)) ? (MAX_32) : (-(L_var1)))
#define  extract_h(x)  (((x))>>16)
#define  extract_l(x) (Word16)((x))

__voinline Word16 vo_round(Word32 L_var1);                    /* Round, 1 */
__voinline Word32 L_sub(Word32 L_var1, Word32 L_var2);     /* Long sub,  2  */
__voinline Word16 shl(Word16 var1, Word16 var2);           /* Short shift left,    1   */
__voinline Word16 shr(Word16 var1, Word16 var2);           /* Short shift right,   1   */
__voinline Word32 L_msu(Word32 L_var3, Word16 var1, Word16 var2); /* Msu,  1  */
__voinline Word32 L_shl(Word32 L_var1, Word16 var2);              /* Long shift left, 2 */
__voinline Word32 L_shl2(Word32 L_var1, Word16 var2);              /* Long shift left, 2 */
__voinline Word32 L_shr(Word32 L_var1, Word16 var2);              /* Long shift right, 2*/
__voinline Word16 add(Word16 var1, Word16 var2);                  /* Short add,  1 */
__voinline Word16 sub(Word16 var1, Word16 var2);                  /* Short sub, 1 */
__voinline Word16 div_s(Word16 var1, Word16 var2);                /* Short division, 18*/
__voinline Word16 mult(Word16 var1, Word16 var2);                 /* Short mult, 1 */
__voinline Word16 norm_s(Word16 var1);                            /* Short norm, 15  */
__voinline Word16 norm_l(Word32 L_var1);                          /* Long norm,  30  */
__voinline Word32 L_mac(Word32 L_var3, Word16 var1, Word16 var2); /* Mac,  1  */
__voinline Word32 L_add(Word32 L_var1, Word32 L_var2);
__voinline Word16 mult_r(Word16 var1, Word16 var2);               /* Mult with round, 2 */
__voinline Word16 shr_r(Word16 var1, Word16 var2);                /* Shift right with round, 2 */
__voinline Word16 msu_r(Word32 L_var3, Word16 var1, Word16 var2); /* Msu with rounding,2 */
__voinline Word32 L_shr_r(Word32 L_var1, Word16 var2);            /* Long shift right with round,  3  */

/*___________________________________________________________________________
|                                                                           |
|   definitions for inline basic arithmetic operators                       |
|___________________________________________________________________________|
*/

/* Short shift left,    1   */
__voinline Word16 vo_round(Word32 L_var1)
{
#ifdef ARMGCC_OPT
	register  Word32 temp = 0;
	Word16    result = 0;	
	asm volatile (
			"MOV     r1, #0x00008000 \n"
			"QADD    %[temp], %[L_var1], r1 \n"
			"MOV     %[result], %[temp], ASR #16 \n"
			:[result] "+r" (result), [temp] "+r" (temp):[L_var1] "r" (L_var1) : "r1"
		     );
	return    result;
#else   
	Word16 var_out;
	Word32 L_rounded;
	L_rounded = L_add (L_var1, (Word32) 0x00008000L);
	var_out = extract_h (L_rounded);
	return (var_out);
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

__voinline Word16 shl (Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 result;
	if (var2 < 0)
	{
		var_out = (var1 >> (-var2));
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
			var_out = extract_l(result);
		}
	}
	return (var_out);
}

/* Short shift right,   1   */
__voinline Word16 shr (Word16 var1, Word16 var2)
{
	Word16 var_out;
	if (var2 < 0)
	{
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
	L_product = (Word32) var1 *(Word32) var2;
	L_product<<= 1;
	L_var_out = L_var3 - L_product;
	if (((L_var3 ^ L_product) & MIN_32) != 0)
	{
		if ((L_var_out ^ L_var3) & MIN_32)
		{
			L_var_out = (L_var3 < 0L) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
#endif
}

/* Short add,           1   */
__voinline Word16 add (Word16 var1, Word16 var2)
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

/* Short sub,           1   */
__voinline Word16 sub(Word16 var1, Word16 var2)
{
#ifdef ARMGCC_OPT
	register  Word16 temp = 0;
	asm volatile(
                    "QSUB  %[temp], %[var1], %[var2]\n"
		    :[temp] "=r" (temp): [var1] "r" (var1), [var2] "r" (var2)
		    );
	return  temp;
#else
	Word16 var_out;
	Word32 L_diff;
	L_diff = (Word32) var1 - var2;
	var_out = saturate(L_diff); 
	return (var_out);
#endif
}

/* Short division,       18  */
__voinline Word16 div_s (Word16 var1, Word16 var2)
{
	Word16 var_out = 0;
	Word16 iteration;
	Word32 L_num;
	Word32 L_denom;
	var_out = MAX_16;
	if (var1!= var2)
	{
		var_out = 0;
		L_num = (Word32) var1;   
		L_denom = (Word32) var2;  	
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
	}
	return (var_out);
}

/* Short mult,          1   */
__voinline Word16 mult (Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 L_product;
	L_product = (Word32) var1 *(Word32) var2;
	L_product = (L_product & (Word32) 0xffff8000L) >> 15;
	if (L_product & (Word32) 0x00010000L)
		L_product = L_product | (Word32) 0xffff0000L;
	var_out = saturate(L_product);
	return (var_out);
}

/* Short norm,           15  */
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
/* Long norm,            30  */
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

/* Mac,  1  */
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
	L_product = (Word32) var1 *(Word32) var2;
	L_product <<= 1;
	L_var_out = L_var3 + L_product ;
	if (((L_var3 ^ L_product ) & MIN_32) == 0)  
	{
		if ((L_var_out ^ L_var3 ) & MIN_32)
		{
			L_var_out = (L_var3 < 0) ? MIN_32 : MAX_32; 
		}
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

__voinline Word16 mult_r (Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 L_product_arr;
	L_product_arr = (Word32)var1 *(Word32)var2;       /* product */
	L_product_arr += (Word32)0x00004000L;      /* round */
	L_product_arr >>= 15;       /* shift */
	var_out = saturate(L_product_arr);
	return (var_out);
}

__voinline Word16 shr_r (Word16 var1, Word16 var2)
{
	Word16 var_out;
	if (var2 > 15)
	{
		var_out = 0;
	}
	else
	{
		var_out = shr(var1, var2);
		if (var2 > 0)
		{
			if ((var1 & ((Word16) 1 << (var2 - 1))) != 0)
			{
				var_out++;
			}
		}
	}
	return (var_out);
}

__voinline Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2)
{
	Word16 var_out;
	L_var3 = L_msu (L_var3, var1, var2);
	var_out = (Word16)((L_var3 + 0x8000L) >> 16);  
	return (var_out);
}

__voinline Word32 L_shr_r (Word32 L_var1, Word16 var2)
{
	Word32 L_var_out;
	if (var2 > 31)
	{
		L_var_out = 0;
	}
	else
	{
		L_var_out = L_shr(L_var1, var2);
		if (var2 > 0)
		{
			if ((L_var1 & ((Word32) 1 << (var2 - 1))) != 0)
			{
				L_var_out++;
			}
		}
	}
	return (L_var_out);
}

__voinline Word32 L_shl (Word32 L_var1, Word16 var2)
{
#ifdef  ARMGCC_OPT    
	if(var2>=0)
	{
		return  L_shl2(L_var1, var2);
	}
	else
	{
		return  (L_var1>>(-var2));
	}
#else
	if (var2 <= 0)
	{
		L_var1 = L_shr(L_var1, (Word16)-var2);
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
/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_shr                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
|   sign extension. If var2 is negative, arithmetically shift L_var1 left   |
|   by -var2 and zero fill the -var2 LSB of the result. Saturate the result |
|   in case of underflows or overflows.                                     |
|                                                                           |
|   Complexity weight : 2                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (Word16) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (Word32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
__voinline Word32 L_shr (Word32 L_var1, Word16 var2)
{
	Word32 L_var_out;

	if (var2 < 0)
	{
		L_var_out = L_shl2 (L_var1, (Word16)-var2);
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
}

#endif // __BASIC_OP_H__
