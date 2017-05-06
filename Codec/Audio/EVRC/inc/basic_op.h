

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
#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L
#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

#ifdef LINUX
#define  __voinline  static __inline__
#else
#define  __voinline  static __inline
#endif 
/*_________________________________________________________________________
|                                                                         |
|                            Function Prototypes                          |
|_________________________________________________________________________|
*/
__voinline Word16 shr(Word16 var1, Word16 var2);  /* 1 ops */
__voinline Word16 shl(Word16 var1, Word16 var2);  /* 1 ops */
__voinline Word16 shift_r(Word16 var, Word16 var2);       /* 2 ops */
__voinline Word32 L_shift_r(Word32 L_var, Word16 var2);     /* 3 ops */
__voinline Word16 mac_r(Word32 L_var3, Word16 var1, Word16 var2); /* 2 op */
__voinline Word16 msu_r(Word32 L_var3, Word16 var1, Word16 var2); /* 2 op */
__voinline Word16 divide_s(Word16 var1, Word16 var2);     /* 18 ops */
__voinline Word32 L_msu(Word32 L_var3, Word16 var1, Word16 var2);
__voinline Word32 L_shl (Word32 L_var1, Word16 var2);
__voinline Word32 L_shl2 (Word32 L_var1, Word16 var2);
__voinline Word32 L_shr (Word32 L_var1, Word16 var2);
__voinline Word32 L_shr2 (Word32 L_var1, Word16 var2);
__voinline Word16 mult(Word16 var1, Word16 var2);
__voinline Word16 mult_r(Word16 var1, Word16 var2);
__voinline Word32 norm_l (Word32 L_var1);
__voinline Word16 norm_s (Word16 var1);
__voinline Word16 sub(Word16 var1, Word16 var2);
__voinline Word32 L_abs (Word32 L_var1);
__voinline Word32 L_mult(Word16 var1, Word16 var2);
__voinline Word32 L_add (Word32 L_var1, Word32 L_var2);
__voinline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);
__voinline Word32 L_sub(Word32 L_var1, Word32 L_var2);

#define abs_s(x)       ((Word16)(((x) != MIN_16) ? (((x) >= 0) ? (x) : (-(x))) : MAX_16))
#define extract_h(x)   ((Word16)((x) >> 16))
#define extract_l(x)    ((Word16)(x))

#define L_deposit_h(x) (((Word32)(x)) << 16)
#define L_deposit_l(x) ((Word32)(x))
#define L_negate(L_var1) (((L_var1) == (MIN_32)) ? (MAX_32) : (-(L_var1)))
#define negate(var1) ((Word16)(((var1) == SW_MIN) ? SW_MAX : (-(var1))))
#define round32(L_var1) ((short)(((long)(L_var1) < 0x7fff8000L) ? (((long)(L_var1) + 0x00008000L) >> 16) : 0x7fff))

__voinline Word16 saturate(Word32 L_var1)
{
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
}

__voinline Word16 add(Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 L_sum;
	L_sum = (Word32) var1 + var2;
	var_out = saturate(L_sum);
	return (var_out);
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
}

__voinline Word32 L_add (Word32 L_var1, Word32 L_var2)
{
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
}

__voinline Word32 L_sub(Word32 L_var1, Word32 L_var2)
{
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
}


__voinline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2)
{
	Word32 L_var_out;
	Word32 L_product;
	L_product = ((long)var1 * var2) << 1;
	L_var_out = L_var3 + L_product;
	return (L_var_out);
}


__voinline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2)
{
	Word32 L_var_out;
	Word32 L_product;

	L_product = ((long)var1 * var2) << 1;
	L_var_out = L_var3 - L_product;
	return (L_var_out);
}

__voinline Word32 L_shl (Word32 L_var1, Word16 var2)
{
	int i = var2;
	Word32 L_var_out = 0L;
	if (var2 <= 0)
	{
		L_var1 = L_shr2(L_var1, (Word16)-var2);
	}
	else
	{
		for (; i > 0; i--)
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
			L_var_out = L_var1;
		}
	}
	return (L_var1);
}

__voinline Word32 L_shl2(Word32 L_var1, Word16 var2)
{
	Word32 L_var_out = 0L;
	int i = var2;
	for (; i > 0; i--)
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
		L_var_out = L_var1;
	}
	return (L_var1);
}

__voinline Word32 L_shr(Word32 L_var1, Word16 var2)
{
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
}

__voinline Word32 L_shr2(Word32 L_var1, Word16 var2)
{
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

__voinline Word32 norm_l (Word32 L_var1)
{
	int var_out;

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

__voinline Word16 norm_s (Word16 var1)
{
	int var_out;
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

__voinline Word16 shl (Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 result;
	if (var2 < 0)
	{
		if (var2 < -16)
			var2 = -16;
		var_out = var1 >> ((Word16)-var2);
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

__voinline Word16 shl2(Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 result;
	result = (Word32) var1 *((Word32) 1 << var2);

	if ((var2 > 15 && var1 != 0) || (result != (Word32) ((Word16) result)))
		{
			var_out = (Word16)((var1 > 0) ? MAX_16 : MIN_16);
		}
		else
		{
			var_out = (Word16)result;
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
		var_out = shl2(var1, (Word16)-var2);
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

__voinline Word16 shr2(Word16 var1, Word16 var2)
{
	Word16 var_out;
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
	return (var_out);
}

__voinline Word16 sub(Word16 var1, Word16 var2)
{
	Word16 var_out;
	Word32 L_diff;

	L_diff = (Word32) var1 - var2;
	var_out = saturate(L_diff);
	//if (L_diff > MAX_16) var_out = MAX_16;
	//else if (L_diff < MIN_16) var_out = MIN_16;
	//else var_out = (short)L_diff;

	return (var_out);
}

__voinline Word16 divide_s(Word16 var1, Word16 var2)
{
	Word16 var_out = 0;
	int iteration;
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
}

/***************************************************************************
*
*   FUNCTION NAME: L_shift_r
*
*   PURPOSE:
*
*     Shift and round.  Perform a shift right. After shifting, use
*     the last bit shifted out of the LSB to round the result up
*     or down.
*
*   INPUTS:
*
*     L_var1
*                     32 bit long signed integer (Word32) whose value
*                     falls in the range
*                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
*     var2
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
*
*   OUTPUTS:
*
*     none
*
*   RETURN VALUE:
*
*     L_var1
*                     32 bit long signed integer (Word32) whose value
*                     falls in the range
*                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
*
*
*   IMPLEMENTATION:
*
*     Shift and round.  Perform a shift right. After shifting, use
*     the last bit shifted out of the LSB to round the result up
*     or down.  This is just like shift_r above except that the
*     input/output is 32 bits as opposed to 16.
*
*     if var2 is positve perform a arithmetic left shift
*     with saturation (see L_shl() above).
*
*     If var2 is zero simply return L_var1.
*
*     If var2 is negative perform a arithmetic right shift (L_shr)
*     of L_var1 by (-var2)+1.  Add the LS bit of the result to
*     L_var1 shifted right (L_shr) by -var2.
*
*     Note that there is no constraint on var2, so if var2 is
*     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
*     This is the reason the L_shl function is used.
*
*
*   KEYWORDS:
*
*************************************************************************/
__voinline Word32 L_shift_r(Word32 L_var1, Word16 var2)
{
	Word32 L_Out, L_rnd;	
	if (var2 < -31)
	{
		L_Out = 0;
	}
	else if (var2 < 0)
	{
		/* right shift */
		L_rnd = L_shl(L_var1, (Word16)(var2 + 1)) & 0x1; 
		L_Out = L_add((L_var1 >> (-var2)), L_rnd);  
	}
	else
	{
		L_Out = L_shl2(L_var1, var2);         
	}	
	return (L_Out);
}

/***************************************************************************
*
*   FUNCTION NAME:mac_r
*
*   PURPOSE:
*
*     Multiply accumulate and round.  Fractionally multiply two 16
*     bit numbers together with saturation.  Add that result to
*     the 32 bit input with saturation.  Finally round the result
*     into a 16 bit number.
*
*
*   INPUTS:
*
*     var1
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
*     var2
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
*     L_var3
*                     32 bit long signed integer (Word32) whose value
*                     falls in the range
*                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
*
*   OUTPUTS:
*
*     none
*
*   RETURN VALUE:
*
*     swOut
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range
*                     0xffff 8000 <= swOut <= 0x0000 7fff.
*
*   IMPLEMENTATION:
*
*     Fractionally multiply two 16 bit numbers together with
*     saturation.  The only numbers which will cause saturation on
*     the multiply are 0x8000 * 0x8000.
*
*     Add that result to the 32 bit input with saturation.
*     Round the 32 bit result by adding 0x0000 8000 to the input.
*     The result may overflow due to the add.  If so, the result
*     is saturated.  The 32 bit rounded number is then shifted
*     down 16 bits and returned as a Word16.
*
*     Please note that this is not a true multiply accumulate as
*     most processors would implement it.  The 0x8000*0x8000
*     causes and overflow for this instruction.  On most
*     processors this would cause an overflow only if the 32 bit
*     input added to it were positive or zero.
*
*   KEYWORDS: mac, multiply accumulate, macr
*
*************************************************************************/

__voinline Word16 mac_r(Word32 L_var3, Word16 var1, Word16 var2)
{
	return (round32(L_mac(L_var3, var1, var2)));
}

/***************************************************************************
*
*   FUNCTION NAME:  msu_r
*
*   PURPOSE:
*
*     Multiply subtract and round.  Fractionally multiply two 16
*     bit numbers together with saturation.  Subtract that result from
*     the 32 bit input with saturation.  Finally round the result
*     into a 16 bit number.
*
*
*   INPUTS:
*
*     var1
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
*     var2
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
*     L_var3
*                     32 bit long signed integer (Word32) whose value
*                     falls in the range
*                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
*
*   OUTPUTS:
*
*     none
*
*   RETURN VALUE:
*
*     swOut
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range
*                     0xffff 8000 <= swOut <= 0x0000 7fff.
*
*   IMPLEMENTATION:
*
*     Fractionally multiply two 16 bit numbers together with
*     saturation.  The only numbers which will cause saturation on
*     the multiply are 0x8000 * 0x8000.
*
*     Subtract that result from the 32 bit input with saturation.
*     Round the 32 bit result by adding 0x0000 8000 to the input.
*     The result may overflow due to the add.  If so, the result
*     is saturated.  The 32 bit rounded number is then shifted
*     down 16 bits and returned as a Word16.
*
*     Please note that this is not a true multiply accumulate as
*     most processors would implement it.  The 0x8000*0x8000
*     causes and overflow for this instruction.  On most
*     processors this would cause an overflow only if the 32 bit
*     input added to it were positive or zero.
*
*   KEYWORDS: mac, multiply accumulate, macr
*
*************************************************************************/

__voinline Word16 msu_r(Word32 L_var3, Word16 var1, Word16 var2)
{
	return (round32(L_msu(L_var3, var1, var2)));
}

/***************************************************************************
*
*   FUNCTION NAME: shift_r
*
*   PURPOSE:
*
*     Shift and round.  Perform a shift right. After shifting, use
*     the last bit shifted out of the LSB to round the result up
*     or down.
*
*   INPUTS:
*
*     var1
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
*     var2
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
*
*   OUTPUTS:
*
*     none
*
*   RETURN VALUE:
*
*     swOut
*                     16 bit short signed integer (Word16) whose value
*                     falls in the range
*                     0xffff 8000 <= swOut <= 0x0000 7fff.
*
*
*   IMPLEMENTATION:
*
*     Shift and round.  Perform a shift right. After shifting, use
*     the last bit shifted out of the LSB to round the result up
*     or down.
*
*     If var2 is positive perform a arithmetic left shift
*     with saturation (see shl() above).
*
*     If var2 is zero simply return var1.
*
*     If var2 is negative perform a arithmetic right shift (shr)
*     of var1 by (-var2)+1.  Add the LS bit of the result to var1
*     shifted right (shr) by -var2.
*
*     Note that there is no constraint on var2, so if var2 is
*     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
*     This is the reason the shl function is used.
*
*
*   KEYWORDS:
*
*************************************************************************/
__voinline Word16 shift_r(Word16 var1, Word16 var2)
{
	Word16 swOut, swRnd;
	if (var2 >= 0)
	{
		swOut = shl2(var1, var2);
	}
	else
	{
		/* right shift */
		if (var2 < -15)
		{
			swOut = 0;
		}
		else
		{
			swRnd = shl(var1, (Word16)(var2 + 1)) & 0x1;
			swOut = (var1 >> (-var2)) + swRnd;
		}
	}
	return (swOut);
}
#endif

