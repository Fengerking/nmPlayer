/***********************************************************************
*                                                                      *
*        VisualOn, Inc. Confidential and Proprietary, 2003-2010        *
*                                                                      *
************************************************************************/
/***********************************************************************
*  File: residu.c                                                      *
*                                                                      *
*  Description: Compute the LPC residual by filtering                  *
*             the input speech through A(z)                            *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"

void Residu(
			Word16 a[],                           /* (i) Q12 : prediction coefficients                     */
			Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed         */
			Word16 y[],                           /* (o) x2  : residual signal                             */
			Word16 lg                             /* (i)     : size of filtering                           */
			)
{
	Word16 i,*p1, *p2;
	Word32 s;
#if 1 //C_OPT
	for (i = 0; i < lg; i++)
	{
		p1 = a;
		p2 = &x[i];
		s  = *p1++ * *p2--;
        s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1++ * *p2--;
		s += *p1 * *p2;

		s = L_shl2(s, 5);               /* saturation can occur here */
		y[i] = extract_h(L_add(s, 0x8000));
	}

#else
	for (i = 0; i < lg; i++)
	{
		s = (x[i] * a[0])<<1;
		for (j = 1; j <= m; j++)
			s = L_mac(s, a[j], x[i - j]);
		s = L_shl(s, 4);               /* saturation can occur here */
		y[i] = voround(s);
	}
#endif
	return;
}
