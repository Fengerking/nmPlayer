/*-----------------------------------------------------------------------*
*                         RESIDU.C										 *
*-----------------------------------------------------------------------*
* Compute the LPC residual by filtering the input speech through A(z)   *
*-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

#if (!FUNC_RESIDU_ASM)
void voAMRWBDecResidu(
			const Word16 a[],                           /* (i) Q12 : prediction coefficients                     */
			Word16 m,                             /* (i)     : order of LP filter                          */
			Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed         */
			Word16 y[],                           /* (o) x2  : residual signal                             */
			Word16 lg                             /* (i)     : size of filtering                           */
			)
{
#if (FUNC_RESIDU_OPT)
	Word16 i, j;
	Word32 s;
	for (i = 0; i < lg; i++)
	{
		s = 0;
		for (j = 0; j <= m; j++)
		{
			s += a[j] * x[i-j];
		}
		s = L_shl(s, 5);               /* saturation can occur here */
		y[i] = (Word16)((s + 0x00008000L) >> 16);
	}
	return;

#else
	Word16 i, j;
	Word32 s;
	for (i = 0; i < lg; i++)
	{
		s = L_mult(x[i], a[0]);
		for (j = 1; j <= m; j++)
			s = L_mac(s, a[j], x[i - j]);
		s = L_shl(s, 3 + 1);               /* saturation can occur here */
		y[i] = vo_round(s); 
	}
	return;
#endif
}
#endif

/*--------------------------------------------------------------------*
* procedure residu2:                                                  
* Compute the LP residual by filtering the input speech through A(z) 
* Output is in Qx
*--------------------------------------------------------------------*/

// @shanrong modified
#if (!FUNC_RESIDU_ASM)
void voAMRWBDecResidu2(
			 const Word16 a[],  /* i:   prediction coefficients                 Q12 */
			 const Word16 m,    /* i:   order of LP filter                      Q0  */
			 const Word16 x[],  /* i:   input signal (usually speech)           Qx  */
			 /*      (note that values x[-m..-1] are needed)     */
			 Word16       y[],  /* o:   output signal (usually residual)        Qx  */
			 const Word16 lg    /* i:   vector size                             Q0  */
			 )
{
#if (FUNC_RESIDU2_OPT)
	Word16 i, j;
	Word32 s;
	for (i = 0; i < lg; i++)
	{
		s = 0;
		for (j = m; j >= 0; j--)
		{
			s += a[j] * x[i - j];
		}
		s = L_shl(s, 4);                 /* saturation can occur here */
		y[i] = (Word16)((s + 0x00008000L) >> 16);
	}
	return;

#else

	Word16 i, j;
	Word32 s;

	for (i = 0; i < lg; i++)
	{
		s = L_mult(x[i], a[0]);

		for (j = 1; j <= m; j++)
			s = L_mac(s, a[j], x[i - j]);

		s = L_shl(s, 3 );                 /* saturation can occur here */
		y[i] = vo_round(s); 
	}
	return;
#endif
}
#endif
// end

