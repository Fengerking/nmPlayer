/*------------------------------------------------------------------------*
 *                         AUTOCORR.C									  *
 *------------------------------------------------------------------------*
 *   Compute autocorrelations of signal with windowing					  *
 *																		  *
 *------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "acelp_fx.h"
#include "count.h"


#define L_WINDOW_PLUS 512    /* 448 low rate, 512 using EXTENSION_VA */


void voAMRWBPDecAutocorr(
     Word16 x[],              /* (i)    : Input signal                      */
     Word16 m,                /* (i)    : LPC order                         */
     Word16 r_h[],            /* (o) Q15: Autocorrelations  (msb)           */
     Word16 r_l[],            /* (o)    : Autocorrelations  (lsb)           */
     Word16 l_window,         /* (i)    : window size             */
     Word16 window[]          /* (i)    : analysis window         */
     )
{
    Word16 i, j, norm, shift, y[L_WINDOW_PLUS];
    Word32 L_sum, L_tmp;

    /* Windowing of signal */

    for (i = 0; i < l_window; i++)
    {
        y[i] = mult_r(x[i], window[i]);   
    }

    /* calculate energy of signal */

    L_sum = L_deposit_h(16);               /* sqrt(256), avoid overflow after rounding */
    for (i = 0; i < l_window; i++)
    {
        L_tmp = L_mult(y[i], y[i]);
        L_tmp = L_shr(L_tmp, 8);
        L_sum = L_add(L_sum, L_tmp);
    }

    /* scale signal to avoid overflow in autocorrelation */

    norm = norm_l(L_sum);
    shift = 4 - (norm >> 1);

    if (shift < 0 )
    {
        shift = 0;                         
    }
    for (i = 0; i < l_window; i++)
    {
        y[i] = shr_r(y[i], shift);         
    }

    /* Compute and normalize r[0] */

    L_sum = 1;                             
    for (i = 0; i < l_window; i++)
        L_sum = L_mac(L_sum, y[i], y[i]);

    norm = norm_l(L_sum);
    L_sum = L_shl(L_sum, norm);
    voAMRWBPDecL_Extract(L_sum, &r_h[0], &r_l[0]);    /* Put in DPF format (see oper_32b) */

    /* Compute r[1] to r[m] */

    for (i = 1; i <= m; i++)
    {
        L_sum = 0;                         
        for (j = 0; j < l_window - i; j++)
            L_sum = L_mac(L_sum, y[j], y[j + i]);

        L_sum = L_shl(L_sum, norm);
        voAMRWBPDecL_Extract(L_sum, &r_h[i], &r_l[i]);
    }

    return;
}
