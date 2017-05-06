#ifndef _BASIC_OP_H_
#define _BASIC_OP_H_
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

#define  DISABLE_DTX	0
#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L
#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

//C Level optimization Macros
#define  C_OPT
#define  MMS_IO

extern Flag Overflow;

//*************************************

#ifdef LINUX
#define  __voinline  static __inline__
#else
#define  __voinline  static __inline
#endif 

__voinline Word16   saturate(Word32 L_var1);
__voinline Word16   add(Word16 var1, Word16 var2);                    /* Short add,           1   */
__voinline Word16   sub(Word16 var1, Word16 var2);                    /* Short sub,           1   */
__voinline Word16   abs_s(Word16 var1);                               /* Short abs,           1   */
__voinline Word16   shl(Word16 var1, Word16 var2);                    /* Short shift left,    1   */
__voinline Word16   shr(Word16 var1, Word16 var2);                    /* Short shift right,   1   */
__voinline Word16   shl2(Word16 var1, Word16 var2);                   /* Short shift left,    var2>=0   */
__voinline Word16   shr2(Word16 var1, Word16 var2);                   /* Short shift right,   var2>=0    */
__voinline Word16   mult(Word16 var1, Word16 var2);                   /* Short mult,          1   */
__voinline Word32   L_mult(Word16 var1, Word16 var2);				  /* Long mult,           1   */
__voinline Word16   vo_round(Word32 L_var1);                          /* Round, 1   */
__voinline Word32   L_mac(Word32 L_var3, Word16 var1, Word16 var2);   /* Mac,  1  */
__voinline Word32   L_msu(Word32 L_var3, Word16 var1, Word16 var2);   /* Msu,  1  */
__voinline Word32   L_add(Word32 L_var1, Word32 L_var2);              /* Long add, 2 */
__voinline Word32   L_sub(Word32 L_var1, Word32 L_var2);              /* Long sub,        2 */
__voinline Word32   L_shl(Word32 L_var1, Word16 var2);                /* Long shift left, 2 */
__voinline Word32   L_shr(Word32 L_var1, Word16 var2);                /* Long shift right, 2*/
__voinline Word32   L_shl2(Word32 L_var1, Word16 var2);               /* Long shift left, var2>=0 */
__voinline Word32   L_shr2(Word32 L_var1, Word16 var2);               /* Long shift right, var2>=0*/
__voinline Word16   shr_r(Word16 var1, Word16 var2);                  /* Shift right with round, 2 */
__voinline Word32   L_abs(Word32 L_var1);                             /* Long abs, 3  */
__voinline Word16   norm_s(Word16 var1);                              /* Short norm, 15  */
__voinline Word16   div_s(Word16 var1, Word16 var2);                  /* Short division, 18  */
__voinline Word16   norm_l(Word32 L_var1);                            /* Long norm, 30  */   
__voinline Word16   negate(Word16 var1);                              /* Short negate, 1   */
__voinline Word32   L_negate(Word32 L_var1);                          /* Long negate,2 */

//#ifdef D_LOG
//#define LOG_TAG "voAMNBDec"
//#include <utils/Log.h>
//#define __D(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
//#else
//#define __D(fmt, args)
//#endif

#define extract_h(x)     (((x))>>16)
#define extract_l(x)     (Word16)((x))
#define L_deposit_h(x)   ((Word32) ((x)) << 16)
#define L_deposit_l(x)   ((Word32) ((x)))

#define shr3(a,b) ((a)>>(b))
#define shl3(a,b) ((a)<<(b))
#define add3(a,b) ((a)+(b))
#define sub3(a,b) ((a)-(b))
#define L_shr3(a,b) ((a)>>(b))
#define L_shl3(a,b) ((a)<<(b))
#define L_add3(a,b) ((a)+(b))
#define L_sub3(a,b) ((a)-(b))
#define L_mac3(s,a,b) ((s)+(a)*(b)*2)
#define L_msu3(s,a,b) ((s)-(a)*(b)*2)
#define L_mult3(a,b) ((a)*(b)*2)
#define round3(s) (((s)+0x00008000)>>16)
#define mult3(a,b) ((a)*(b)>>15)
/*___________________________________________________________________________
 |                                                                           |
 |   Functions                                                               |
 |___________________________________________________________________________|
*/
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : saturate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Limit the 32 bit input to the range of a 16 bit word.                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 saturate (Word32 L_var1)
{
    if (L_var1 > 0X00007fffL)
    {
        return MAX_16;
    }
    else if (L_var1 < (Word32) 0xffff8000L)
    {
        return MIN_16;
    }    
    return (Word16)L_var1;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : add                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the addition (var1+var2) with overflow control and saturation;|
 |    the 16 bit result is set at +32767 when overflow occurs or at -32768   |
 |    when underflow occurs.                                                 |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 add(Word16 var1, Word16 var2)
{
	return  saturate (var1 + var2);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : sub                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the subtraction (var1+var2) with overflow control and satu-   |
 |    ration; the 16 bit result is set at +32767 when overflow occurs or at  |
 |    -32768 when underflow occurs.                                          |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 sub (Word16 var1, Word16 var2)
{
    return saturate (var1 - var2);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : abs_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of var1; abs_s(-32768) = 32767.                         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

__voinline Word16 abs_s(Word16 var1)
{
    if (var1 == (Word16) 0x8000)
    {
        return  MAX_16;
    }
    else
    {
        if (var1 < 0)
        {
             return  -var1;
        }
        else
        {
             return  var1;
        }
    }
}

__voinline Word32 L_shl2 (Word32 L_var1, Word16 var2)
{
	Word32 L_var_out =L_var1 ;
	for (; var2 > 0; var2--)
	{
		if (L_var1 > (Word32) 0X3fffffffL)
		{
			Overflow = 1;
			L_var_out = MAX_32;
			break;
		}
		else
		{
			if (L_var1 < (Word32) 0xc0000000L)
			{
				Overflow = 1;
				L_var_out = MIN_32;
				break;
			}
		}
		L_var1 *= 2;
		L_var_out = L_var1;
	}
	return (L_var_out);
}

__voinline Word32 L_shr2 (Word32 L_var1, Word16 var2)
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

__voinline Word16 shr2 (Word16 var1, Word16 var2)
{
	if (var2 < 15)
	{
		if (var1 >= 0)
		{
			return var1 >> var2;
		}
		else
		{
			return  ~((~var1) >> var2);
		}
	}
	else
	{
		return -(var1 < 0);
	}
}

__voinline Word16 shl2 (Word16 var1, Word16 var2)
{
	Word32 result = (Word32) var1 *((Word32) 1 << var2);
	if ((var2 > 15 && var1 != 0) || (result != (Word32) ((Word16) result)))
	{
		return (var1 > 0) ? MAX_16 : MIN_16;
	}
	return result;
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shl                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 left var2 positions.Zero fill|
 |   the var2 LSB of the result. If var2 is negative, arithmetically shift   |
 |   var1 right by -var2 with sign extension. Saturate the result in case of |
 |   underflows or overflows.                                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 shl (Word16 var1, Word16 var2)
{
	Word32 result;
	if (var2 >= 0)
	{
		result = (Word32) var1 *((Word32) 1 << var2);
		if ((var2 > 15 && var1 != 0) || (result != (Word32) ((Word16) result)))
		{
			return (var1 > 0) ? MAX_16 : MIN_16;
		}
		return result;
	}
	else
	{
		if (var2 < -16)
			var2 = -16;
		return shr(var1, -var2);
	}
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 right var2 positions with    |
 |   sign extension. If var2 is negative, arithmetically shift var1 left by  |
 |   -var2 with sign extension. Saturate the result in case of underflows or |
 |   overflows.                                                              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 shr (Word16 var1, Word16 var2)
{
	if (var2 > 0)
	{
		if (var2 < 15)
		{
			if (var1 >= 0)
			{
				return var1 >> var2;
			}
			else
			{
				return  ~((~var1) >> var2);
			}
		}
		else
		{
			return -(var1 < 0);
		}
	}
	else
	{
		if (var2 < -16)
			var2 = -16;
		return shl (var1, -var2);
	}
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult                                                    |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the multiplication of var1 by var2 and gives a 16 bit result  |
 |    which is scaled i.e.:                                                  |
 |             mult(var1,var2) = extract_l(L_shr((var1 times var2),15)) and  |
 |             mult(-32768,-32768) = 32767.                                  |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 mult (Word16 var1, Word16 var2)
{
    return saturate(((Word32) var1 * (Word32) var2)>>15);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mult                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   L_mult is the 32 bit result of the multiplication of var1 times var2    |
 |   with one shift left i.e.:                                               |
 |        L_mult(var1,var2) = L_shl((var1 times var2),1) and                 |
 |        L_mult(-32768,-32768) = 2147483647.                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
__voinline Word32 L_mult (Word16 var1, Word16 var2)
{
	Word32 L_var_out = (Word32) var1 *(Word32) var2;
	if (L_var_out <  0x40000000L)
		return L_var_out << 1;
	else
		return MAX_32;
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : negate                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate var1 with saturation, saturate in the case where input is -32768:|
 |                negate(var1) = sub(0,var1).                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 negate (Word16 var1)
{
	return (var1 != MIN_16)?-var1:MAX_16;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : extract_l                                               |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Return the 16 LSB of L_var1.                                            |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
/*
__voinline Word16 extract_l (Word32 L_var1)
{
    return (Word16) L_var1;
}
*/
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : round                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Round the lower 16 bits of the 32 bit input number into the MS 16 bits  |
 |   with saturation. Shift the resulting bits right by 16 and return the 16 |
 |   bit number:                                                             |
 |               round(L_var1) = extract_h(L_add(L_var1,32768))              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 vo_round (Word32 L_var1)
{
	Word32 L_rounded = L_add (L_var1, (Word32) 0x00008000L);
	return extract_h (L_rounded);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mac                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation, return a 32 bit result:               |
 |        L_mac(L_var3,var1,var2) = L_add(L_var3,L_mult(var1,var2)).         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
__voinline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2)
{
	Word32 L_var_out;
	Word32 L_product;
	L_product = (Word32) var1 *(Word32) var2;
	if (L_product <  0x40000000L)
		L_product<<= 1;
	else
		L_product  = MAX_32;
	L_var_out = L_var3 + L_product ;
	if (((L_var3 ^ L_product ) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var3 ) & MIN_32)
		{
			L_var_out = (L_var3 < 0) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_msu                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result to L_var3 with saturation, return a 32 bit result:           |
 |        L_msu(L_var3,var1,var2) = L_sub(L_var3,L_mult(var1,var2)).         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
__voinline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2)
{
	Word32 L_var_out;
	Word32 L_product;
	L_product = (Word32) var1 *(Word32) var2;
	if (L_product <  0x40000000L)
		L_product<<= 1;
	else
		L_product  = MAX_32;
	L_var_out = L_var3 - L_product;

	if (((L_var3 ^ L_product) & MIN_32) != 0)
	{
		if ((L_var_out ^ L_var3) & MIN_32)
		{
			L_var_out = (L_var3 < 0L) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      |
 |   overflow control and saturation; the result is set at +2147483647 when  |
 |   overflow occurs or at -2147483648 when underflow occurs.                |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
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

__voinline Word32 L_add (Word32 L_var1, Word32 L_var2)
{
	Word32 L_var_out = L_var1 + L_var2;
	if (((L_var1 ^ L_var2) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   |
 |   overflow control and saturation; the result is set at +2147483647 when  |
 |   overflow occurs or at -2147483648 when underflow occurs.                |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
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

__voinline Word32 L_sub (Word32 L_var1, Word32 L_var2)
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
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_negate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate the 32 bit variable L_var1 with saturation; saturate in the case |
 |   where input is -2147483648 (0x8000 0000).                               |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
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
__voinline Word32 L_negate (Word32 L_var1)
{
	return (L_var1 != MIN_32) ?  -L_var1:MAX_32;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shl                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  |
 |   fill the var2 LSB of the result. If var2 is negative, arithmetically    |
 |   shift L_var1 right by -var2 with sign extension. Saturate the result in |
 |   case of underflows or overflows.                                        |
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
__voinline Word32 L_shl (Word32 L_var1, Word16 var2)
{
	Word32 L_var_out;
	if (var2 <= 0)
	{
		if (var2 < -32)
			var2 = -32;
		L_var_out = L_shr2(L_var1, -var2);
	}
	else
	{
		for (; var2 > 0; var2--)
		{
			if (L_var1 > (Word32) 0X3fffffffL)
			{
				L_var_out = MAX_32;
				break;
			}
			else
			{
				if (L_var1 < (Word32) 0xc0000000L)
				{
					L_var_out = MIN_32;
					break;
				}
			}
			L_var1 *= 2;
			L_var_out = L_var1;
		}
	}
	return (L_var_out);
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
		if (var2 < -32)
			var2 = -32;
		L_var_out = L_shl2(L_var1, -var2);
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
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as shr(var1,var2) but with rounding. Saturate the result in case of|
 |   underflows or overflows :                                               |
 |    - If var2 is greater than zero :                                       |
 |          if (sub(shl(shr(var1,var2),1),shr(var1,sub(var2,1))))            |
 |          is equal to zero                                                 |
 |                     then                                                  |
 |                     shr_r(var1,var2) = shr(var1,var2)                     |
 |                     else                                                  |
 |                     shr_r(var1,var2) = add(shr(var1,var2),1)              |
 |    - If var2 is less than or equal to zero :                              |
 |                     shr_r(var1,var2) = shr(var1,var2).                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
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
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
__voinline Word16 shr_r (Word16 var1, Word16 var2)
{
	int  var_out = 0;
	if (var2 <= 15)
	{
		var_out = shr (var1, var2);
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

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_h                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 MS bits of the 32 bit output. The   |
 |   16 LS bits of the output are zeroed.                                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
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
 |             range : 0x8000 0000 <= var_out <= 0x7fff 0000.                |
 |___________________________________________________________________________|
*/
/* __voinline Word32 L_deposit_h (Word16 var1)
{

    return (Word32) var1 << 16;

}*/
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_l                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 LS bits of the 32 bit output. The   |
 |   16 MS bits of the output are sign extended.                             |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
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
 |             range : 0xFFFF 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
/* __voinline Word32 L_deposit_l (Word16 var1)
{

    return  (Word32) var1;

}*/

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shr_r                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as L_shr(L_var1,var2) but with rounding. Saturate the result in    |
 |   case of underflows or overflows :                                       |
 |    - If var2 is greater than zero :                                       |
 |          if (L_sub(L_shl(L_shr(L_var1,var2),1),L_shr(L_var1,sub(var2,1))))|
 |          is equal to zero                                                 |
 |                     then                                                  |
 |                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2)             |
 |                     else                                                  |
 |                     L_shr_r(L_var1,var2) = L_add(L_shr(L_var1,var2),1)    |
 |    - If var2 is less than or equal to zero :                              |
 |                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2).            |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
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
 |             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/
__voinline Word32 L_shr_r (Word32 L_var1, Word16 var2)
{
    Word32 L_var_out = 0;
    if (var2 <= 31)
    {
         L_var_out = L_shr (L_var1, var2);
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
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_abs                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of L_var1; Saturate in case where the input is          |
 |                                                               -214783648  |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x0000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/
__voinline Word32 L_abs (Word32 L_var1)
{
	Word32  L_var_out = MAX_32;
	if (L_var1 != MIN_32)
	{
		return  L_var1>0?L_var1:-L_var1;
	}   
	return (L_var_out);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_s                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shift needed to normalize the 16 bit varia- |
 |   ble var1 for positive values on the interval with minimum of 16384 and  |
 |   maximum of 32767, and for negative values on the interval with minimum  |
 |   of -32768 and maximum of -16384; in order to normalize the result, the  |
 |   following operation must be done :                                      |
 |                    norm_var1 = shl(var1,norm_s(var1)).                    |
 |                                                                           |
 |   Complexity weight : 15                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 000f.                |
 |___________________________________________________________________________|
*/
__voinline Word16 norm_s (Word16 var1)
{
	Word16 var_out = 0;
	if (var1 != 0)
	{
		var_out = 15;
		if (var1 != (Word16) 0xffff)
		{
			if (var1 < 0)
			{
				var1 = ~var1;
			}
			for (var_out = 0; var1 < 0x4000; var_out++)
			{
				var1 <<= 1;
			}
		}
	}
	return (var_out);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : div_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces a result which is the fractional integer division of var1  by  |
 |   var2; var1 and var2 must be positive and var2 must be greater or equal  |
 |   to var1; the result is positive (leading bit equal to 0) and truncated  |
 |   to 16 bits.                                                             |
 |   If var1 = var2 then div(var1,var2) = 32767.                             |
 |                                                                           |
 |   Complexity weight : 18                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var1 <= var2 and var2 != 0.            |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : var1 <= var2 <= 0x0000 7fff and var2 != 0.            |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
 |             It's a Q15 value (point between b15 and b14).                 |
 |___________________________________________________________________________|
*/
__voinline Word16 div_s (Word16 var1, Word16 var2)
{
#if 1	
	Word32 quot;
	if ((var1 > var2) || (var1 < 0) || (var2 < 0))
	{
		return 0;
	}
	quot = var1<<15;
	quot /= var2;
	if (quot > MAX_16)
		return MAX_16;
	else
		return (Word16)quot;
#else
	Word16 var_out = 0;
	Word16 iteration;
	Word32 L_num;
	Word32 L_denom;
	if ((var1 > var2) || (var1 < 0) || (var2 < 0))
	{
		printf ("Division Error var1=%d  var2=%d\n", var1, var2);
		return 0;
	}
	if (var2 == 0)
	{
		printf ("Division by 0, Fatal error \n");
		return 0;
	}
	if (var1 != 0)//if var1==0 var_out=0
	{
		var_out = MAX_16;//var1 == var2
		if (var1 != var2)
		{
			L_num = L_deposit_l (var1);
			L_denom = L_deposit_l (var2);
			for (iteration = 0,var_out=0; iteration < 15; iteration++)
			{
				var_out <<= 1;
				L_num <<= 1;
				if (L_num >= L_denom)
				{
					L_num = L_sub (L_num, L_denom);
					var_out = add (var_out, 1);
				}
			}
		}
	}
	return (var_out);
#endif
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_l                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shifts needed to normalize the 32 bit varia-|
 |   ble L_var1 for positive values on the interval with minimum of          |
 |   1073741824 and maximum of 2147483647, and for negative values on the in-|
 |   terval with minimum of -2147483648 and maximum of -1073741824; in order |
 |   to normalize the result, the following operation must be done :         |
 |                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             |
 |                                                                           |
 |   Complexity weight : 30                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 001f.                |
 |___________________________________________________________________________|
*/
__voinline Word16 norm_l (Word32 L_var1)
{
	Word16 var_out = 0;
	if (L_var1 != 0)
	{
		var_out = 31;//L_var1 == (Word32) 0xffffffffL
		if (L_var1 != (Word32) 0xffffffffL)
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
#endif //_BASIC_OP_H_
