/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: scale.c                                                  *
*                                                                      *
*       Description: Scale signal to get maximum of dynamic            *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"


void Scale_sig(
			   Word16 x[],                           /* (i/o) : signal to scale               */
			   Word16 lg,                            /* (i)   : size of x[]                   */
			   Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
			   )
{
	Word32 i;
	Word32 L_tmp;
	if(exp > 0)
	{
		for (i = lg - 1 ; i >= 0; i--)
		{
			//L_tmp = x[i] << 16;
			L_tmp = L_shl2(x[i], 16 + exp);         /* saturation can occur here */
			x[i] = extract_h(L_add(L_tmp, 0x8000));            
		}
	}
	else
	{
		exp = -exp;
		for (i = lg - 1; i >= 0; i--)
		{
			L_tmp = x[i] << 16;
			L_tmp >>= exp;         /* saturation can occur here */
			x[i] = (L_tmp + 0x8000)>>16;            
		}
	}
	return;
}
