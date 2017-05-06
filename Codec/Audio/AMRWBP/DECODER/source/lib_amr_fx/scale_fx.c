/*-------------------------------------------------------------------*
 *                         SCALE.C									 *
 *-------------------------------------------------------------------*
 * Scale signal to get maximum of dynamic.							 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"

// @shanrong modified
#if (!FUNC_SCALE_SIG_ASM)
void voAMRWBPDecScale_sig(
     Word16 x[],                           /* (i/o) : signal to scale               */
     Word16 lg,                            /* (i)   : size of x[]                   */
     Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
)
{
#if (FUNC_SCALE_SIG_OPT)
    Word16 i;
    Word32 L_tmp;
    for (i = 0; i < lg; i++)
    {
        L_tmp = x[i] << 16;

//        L_tmp = L_shl(L_tmp, exp);
        if (exp >= 0)
    	{
    		if (L_tmp > ((Word32)0x7fffffffL >> exp))
			{
				L_tmp = MAX_32;
			}
    		else if (L_tmp < ((Word32)0x80000000 >> exp))
			{
				L_tmp = MIN_32;
			}
			else
			{
				L_tmp <<= exp;
			}
    	}
        else	//exp < 0
    	{
    		L_tmp >>= (-exp);
    	}

        x[i] = round16(L_tmp);
    }
    
    
    
#else

    Word16 i;
    Word32 L_tmp;

    for (i = 0; i < lg; i++)
    {
        L_tmp = L_deposit_h(x[i]);
        L_tmp = L_shl(L_tmp, exp);         /* saturation can occur here */
        x[i] = round16(L_tmp);               
    }

    
#endif
#if AMR_DUMP
  {
	Dumploop2(AMR_DEBUG_Scale_sig,"after Scale_sig",4,L_SUBFR/4,x,d16);
  }
#endif//AMR_DUMP
	return;
}
#endif
// end
