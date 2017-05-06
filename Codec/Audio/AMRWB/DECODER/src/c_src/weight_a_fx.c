/*-------------------------------------------------------------------*
*                         WB_VAD.C									 *
*-------------------------------------------------------------------*
* Weighting of LPC coefficients.									 *
*   ap[i]  =  a[i] * (gamma ** i)									 *
*-------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"

void Weight_a(
			  const Word16 a[],                     /* (i) Q12 : a[m+1]  LPC coefficients             */
			  Word16 ap[],                          /* (o) Q12 : Spectral expanded LPC coefficients   */
			  Word16 gamma,                         /* (i) Q15 : Spectral expansion factor.           */
			  Word16 m                              /* (i)     : LPC order.                           */
			  )
#if 1
{
	Word32 num = m - 1, fac;
	*ap++ = *a++;
	fac = gamma;
	do{
		*ap++ =(Word16)(((((*a++) * fac)<<1) + 0x8000) >> 16);
		fac = (((fac * gamma)<<1) + 0x8000) >> 16;
	}while(--num != 0);

	*ap++ = (Word16)(((((*a++) * fac)<<1) + 0x8000) >> 16);
	return;
}
#else
{
	Word16 i, fac;
	ap[0] = a[0]; 
	fac = gamma; 
	for (i = 1; i < m; i++)
	{
		ap[i] = vo_round(L_mult(a[i], fac));
		fac = vo_round(L_mult(fac, gamma));
	}
	ap[m] = vo_round(L_mult(a[m], fac));
	return;
}
#endif
