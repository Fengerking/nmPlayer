/*-------------------------------------------------------------------*
 *                         PREEMPH.C								 *
 *-------------------------------------------------------------------*
 * Preemphasis: filtering through 1 - g z^-1                         *
 *                                                                   *
 * Preemph2 --> signal is multiplied by 2.                           *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"


void voAMRWBPDecPreemph(
     Word16 x[],                           /* (i/o)   : input signal overwritten by the output */
     Word16 mu,                            /* (i) Q15 : preemphasis coefficient                */
     Word16 lg,                            /* (i)     : lenght of filtering                    */
     Word16 * mem                          /* (i/o)   : memory (x[-1])                         */
)
{
#if (FILE_PREEMPH_FX_OPT)
    Word16 i, temp;
    Word32 L_tmp;

    temp = x[lg - 1];

    for (i = (Word16) (lg - 1); i > 0; i--)
    {
        L_tmp = (((Word32)x[i]) << 15) - x[i - 1] * mu;
        x[i] = (L_tmp + 0x4000) >> 15;
    }

    L_tmp = (((Word32)x[0]) << 15) - (*mem) * mu;
    x[0] = (L_tmp + 0x4000) >> 15;

    *mem = temp;

   

#else//(FILE_PREEMPH_FX_OPT)

    Word16 i, temp;
    Word32 L_tmp;

    temp = x[lg - 1];                      

    for (i = (Word16) (lg - 1); i > 0; i--)
    {
        L_tmp = L_deposit_h(x[i]);
        L_tmp = L_msu(L_tmp, x[i - 1], mu);
        x[i] = round16(L_tmp);               
    }

    L_tmp = L_deposit_h(x[0]);
    L_tmp = L_msu(L_tmp, *mem, mu);
    x[0] = round16(L_tmp);                   

    *mem = temp;                           


#endif//(FILE_PREEMPH_FX_OPT)

   return;
}
