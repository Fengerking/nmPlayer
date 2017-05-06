/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : autocorr.c
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "autocorr.h"
const char autocorr_id[] = "@(#)$Id $" autocorr_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "lag_wind.tab" 
/*********************************************************************************
*                         LOCAL VARIABLES AND TABLES
*********************************************************************************/

/*
**************************************************************************
*
*  Function    : autocorr
*  Purpose     : Compute autocorrelations of signal with windowing
*
**************************************************************************
*/
Word16 voAMRNBEncAutocorr(
				Word16 x[],            /* (i)    : Input signal (L_WINDOW)            */
				Word16 m,              /* (i)    : LPC order                          */
				Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
				Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
				const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINDOW) */
)
{
	Word16 i, j, norm;
	Word16 y[L_WINDOW];
	Word32 sum , temp;
	Word16 overfl, overfl_shft;
	/* Windowing of signal */
	for (i = 0; i < L_WINDOW; i++)
	{
		//y[i] = mult_r (x[i], wind[i]);
		y[i] = (x[i] * wind[i] + 0x4000) >> 15; 
	}
	/* Compute r[0] and test for overflow */
	overfl_shft = 0;                 
	do
	{
		overfl = 0;                  
		sum = 0L;                   
		for (i = 0; i < L_WINDOW; i++)
		{
			temp = (y[i] * y[i])<<1;
			sum = L_add(sum, temp);
		}
		/* If overflow divide y[] by 4 */
		if ((sum == MAX_32))
		{
			overfl_shft += 4;
			overfl = 1;                                      /* Set the overflow flag */
			for (i = 0; i < L_WINDOW; i++)
			{
				y[i] = (y[i] >> 2); 
			}
		}
	}while (overfl != 0);

	sum += 1;                                               /* Avoid the case of all zeros */
	/* Normalization of r[0] */
	norm = norm_l (sum);
	sum = (sum << norm);
	r_h[0] = sum >> 16;
	r_l[0] = (sum - (r_h[0] << 16)) >> 1;

	//L_Extract (sum, &r_h[0], &r_l[0]); /* Put in DPF format (see oper_32b) */
	/* r[1] to r[m] */
	for (i = 1; i <= m; i++)
	{
		sum = 0;                    
		for (j = 0; j < L_WINDOW - i; j++)
		{
			sum += ((y[j] * y[j + i]));
		}
		sum = (sum << (norm + 1));
		r_h[i] = sum >>16;
		r_l[i] = (sum - (r_h[i]<<16))>>1;
		//L_Extract (sum, &r_h[i], &r_l[i]);
	}
	norm -= overfl_shft;
	return norm;
}
