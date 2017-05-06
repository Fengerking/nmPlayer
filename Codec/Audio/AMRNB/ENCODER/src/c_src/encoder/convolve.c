/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : convolve.c
*      Purpose          : Perform the convolution between two vectors x[]
*                       : and h[] and write the result in the vector y[].
*                       : All vectors are of length L and only the first
*                       : L samples of the convolution are computed.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "convolve.h"
const char convolve_id[] = "@(#)$Id $" convolve_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:   Convolve
*
*  PURPOSE:
*     Perform the convolution between two vectors x[] and h[] and
*     write the result in the vector y[]. All vectors are of length L
*     and only the first L samples of the convolution are computed.
*
*  DESCRIPTION:
*     The convolution is given by
*
*          y[n] = sum_{i=0}^{n} x[i] h[n-i],        n=0,...,L-1
*
*************************************************************************/
void Convolve (
			   Word16 x[],        /* (i)     : input vector                           */
			   Word16 h[],        /* (i)     : impulse response                       */
			   Word16 y[],        /* (o)     : output vector                          */
			   Word16 L           /* (i)     : vector size                            */
			   )
#ifdef C_OPT    //bit match
{
	Word32  i, n;
	Word16  *tmpH,*tmpX;
	Word32  s;
	for (n = 0; n < 40;)
	{
		tmpH = h+n;
		tmpX = x;
		i=n+1;
		s = (*tmpX++) * (*tmpH--);i--;
		while(i>0)
		{
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			i -= 4;
		}
		s = (s << 4); //L_shl2 (s, 4);
		y[n] = extract_h (s);   
		n++;

		tmpH = h+n;
		tmpX = x;
		i=n+1;
		s = (*tmpX++) * (*tmpH--);i--;
		s += (*tmpX++) * (*tmpH--);i--;

		while(i>0)
		{
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			i -= 4;
		}
		s = (s << 4);
		y[n] = extract_h (s);   
		n++;

		tmpH = h+n;
		tmpX = x;
		i=n+1;
		s = (*tmpX++) * (*tmpH--);i--;
		s += (*tmpX++) * (*tmpH--);i--;
		s += (*tmpX++) * (*tmpH--);i--;

		while(i>0)
		{
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			i -= 4;
		}
		s = (s << 4);
		y[n] = extract_h (s);   
		n++;

		s = 0;
		tmpH = h+n;
		tmpX = x;
		i=n+1;
		while(i>0)
		{
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			s += (*tmpX++) * (*tmpH--);
			i -= 4;
		}
		s = (s << 4);
		y[n] = extract_h (s);   
		n++;        
	}
	return;
}
#else
{
	Word16 i, n;
	Word32 s;
	for (n = 0; n < L; n++)
	{
		s = 0;
		for (i = 0; i <= n; i++)
		{
			s = L_mac (s, x[i], h[n - i]);
		}
		s = L_shl2(s, 3);
		y[n] = extract_h (s);
	}
	return;
}
#endif
