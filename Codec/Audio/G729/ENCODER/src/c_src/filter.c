/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-------------------------------------------------------------------*
 * Function  Convolve:                                               *
 *           ~~~~~~~~~                                               *
 *-------------------------------------------------------------------*
 * Perform the convolution between two vectors x[] and h[] and       *
 * write the result in the vector y[].                               *
 * All vectors are of length N.                                      *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

void Convolve(
  Word16 x[],      /* (i)     : input vector                           */
  Word16 h[],      /* (i) Q12 : impulse response                       */
  Word16 y[],      /* (o)     : output vector                          */
  Word16 L         /* (i)     : vector size                            */
)
{
	Word16 i, n;
	Word32 s;

	for (n = 0; n < L; n++)
	{
		s = 0;
		for (i = 0; i <= n; i++)
			s += ((x[i]*h[n-i])<<1);
		s    = L_shl2(s, 3);                   /* h is in Q12 and saturation */
		y[n] = extract_h(s);
	}

	return;
}

/*-----------------------------------------------------*
 * procedure Syn_filt:                                 *
 *           ~~~~~~~~                                  *
 * Do the synthesis filtering 1/A(z).                  *
 *-----------------------------------------------------*/

void Syn_filt(
  Word16 a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  Word16 x[],     /* (i)     : input signal                             */
  Word16 y[],     /* (o)     : output signal                            */
  Word16 lg,      /* (i)     : size of filtering                        */
  Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
  Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
	Word16 i;
	Word32 s;
	Word16 tmp[100];     /* This is usually done by memory allocation (lg+M) */
	Word16 *yy;
	/* Copy mem[] to yy[] */
	yy = tmp;

	for(i=0; i<M; i++)
	{
		*yy++ = mem[i];
	}
	/* Do the filtering. */
	for (i = 0; i < lg; i++)
	{
		s = x[i]*a[0];
		s -= a[1]* yy[-1];
		s -= a[2]* yy[-2];
		s -= a[3]* yy[-3];
		s -= a[4]* yy[-4];
		s -= a[5]* yy[-5];
		s -= a[6]* yy[-6];
		s -= a[7]* yy[-7];
		s -= a[8]* yy[-8];
		s -= a[9]* yy[-9];
		s -= a[10]* yy[-10];
		s = L_shl2(s, 4);

		*(y+i) = (Word16)((s + 0x8000) >>16);

		*yy++ = *(y+i);
	}

	/* Update of memory if update==1 */
	if(update != 0)
		for (i = 0; i < M; i++)
		{
			mem[i] = y[lg-M+i];
		}

	return;
}


/*-----------------------------------------------------------------------*
 * procedure Residu:                                                     *
 *           ~~~~~~                                                      *
 * Compute the LPC residual  by filtering the input speech through A(z)  *
 *-----------------------------------------------------------------------*/
void Residu(
			Word16 a[],    /* (i) Q12 : prediction coefficients                     */
			Word16 x[],    /* (i)     : speech (values x[-m..-1] are needed         */
			Word16 y[],    /* (o)     : residual signal                             */
			Word16 lg      /* (i)     : size of filtering                           */
			)
{
	Word16 i;
	Word32 s;
	Word16 *tmp_a, *tmp_x;
	for (i = 0; i < lg; i++)
	{
		tmp_a = a;
		tmp_x = x+i;
		s = (*(tmp_x--)) * (*(tmp_a++));

		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*(tmp_a++)) * (*(tmp_x--));
		s +=  (*tmp_a) * (*tmp_x);

		s = L_shl2(s, 4);
		y[i] = (Word16)((s + 0x8000) >>16);

	}
	return;
}

