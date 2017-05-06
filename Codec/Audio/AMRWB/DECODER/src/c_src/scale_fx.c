/*-------------------------------------------------------------------*
*                         SCALE.C									 *
*-------------------------------------------------------------------*
* Scale signal to get maximum of dynamic.							 *
*-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

// @shanrong modified
#if (!FUNC_SCALE_SIG_ASM)
void Scale_sig(
			   Word16 x[],                           /* (i/o) : signal to scale               */
			   Word16 lg,                            /* (i)   : size of x[]                   */
			   Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
			   )
{
#if (FUNC_SCALE_SIG_OPT)
	Word16 i;
	Word32 L_tmp;
	if(exp <= 0 )
		for(i = lg-1; i >=0; i--)
		{
			L_tmp = x[i] << 16;
			L_tmp >>= (-exp);
			x[i] = (L_tmp + 0x8000)>>16;
		}  
	else
		for(i = lg-1; i >=0; i--)
		{
			L_tmp = x[i] << 16;
			L_tmp <<= exp;
			x[i] =(L_tmp + 0x8000)>>16;
		}
#else
	Word16 i;
	Word32 L_tmp;
	for (i = 0; i < lg; i++)
	{
		L_tmp = L_deposit_h(x[i]);
		L_tmp = L_shl(L_tmp, exp);         /* saturation can occur here */
		x[i] = vo_round(L_tmp); 
	}   
#endif
	return;
}
#endif
// end
