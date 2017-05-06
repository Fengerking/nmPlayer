/*
*****************************************************************************
*
*      GSM AMR speech codec   Version 7.1.0   July 26, 1999
*
******************************************************************************
*
*      File             : cor_h.c
*      Purpose          : correlation functions for codebook search
*
*****************************************************************************
*/
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "cor_h.h"
const char cor_h_id[] = "@(#)$Id $" cor_h_h;
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "inv_sqrt.h"
#include "cnst.h" 

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  voAMRNBEnccor_h_x()
*
*  PURPOSE:  Computes correlation between target signal "x[]" and
*            impulse response"h[]".
*
*  DESCRIPTION:
*    The correlation is given by:
*       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
*
*    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
*    to each position track does not saturate.
*
*************************************************************************/
void voAMRNBEnccor_h_x (
			  Word16 h[],    /* (i): impulse response of weighted synthesis filter */
			  Word16 x[],    /* (i): target                                        */
			  Word16 dn[],   /* (o): correlation between target and h[]            */
			  Word16 sf      /* (i): scaling factor: 2 for 12.2, 1 for others      */
			  )
{
	Word16 i, j, k;
	Word32 s, y32[L_CODE], max, tot;

	/* first keep the result on 32 bits and find absolute maximum */
	tot = 5;  
	for (k = 0; k < NB_TRACK; k++)
	{
		max = 0; 
		for (i = k; i < L_CODE; i += STEP)
		{
			s = 0;  
			for (j = i; j < L_CODE; j++)
				s += (x[j] * h[j - i])<<1;

			y32[i] = s; 
			s = (s<0)? -s: s;
			if (s > max) max = s;
		}
		tot += (max >> 1);
	}
	j = norm_l(tot) - sf;

	for (i = 0; i < L_CODE; i++)
	{
		dn[i] = (L_shl(y32[i], j) + 0x8000)>>16; 
	}
}

/*************************************************************************
*
*  FUNCTION:  cor_h()
*
*  PURPOSE:  Computes correlations of h[] needed for the codebook search;
*            and includes the sign information into the correlations.
*
*  DESCRIPTION: The correlations are given by
*         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
*
*  and the sign information is included by
*         rr[i][j] = rr[i][j]*sign[i]*sign[j]
*
*************************************************************************/
void cor_h (
			Word16 h[],         /* (i) : impulse response of weighted synthesis filter  */
			Word16 sign[],      /* (i) : sign of d[n] */
			Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation  */
)
{
	Word16 i, j, k, dec, h2[L_CODE];
	Word32 s;
	/* Scaling for maximum precision */
	s = 2;
	for (i = 0; i < L_CODE; i++)
		s += (h[i]*h[i])<<1;

	j = extract_h(s) - 32767;
	if (j == 0)
	{
		for (i = 0; i < L_CODE; i++)
		{
			h2[i] = (h[i] >> 1); 
		}
	}
	else
	{
		s = s >> 1;
		k = extract_h ((Inv_sqrt (s) << 7));
		k = (k * 32440)>>15;                              /* k = 0.99*k */  
		for (i = 0; i < L_CODE; i++)
		{
			h2[i] = (((h[i] * k) << 10) + 0x8000)>>16;
		}
	}   
	/* build matrix rr[] */
	s = 0;                             
	i = L_CODE - 1;
	for (k = 0; k < L_CODE; k++, i--)
	{
		s += (h2[k] * h2[k])<<1;
		rr[i][i] = (s + 0x8000)>>16;             
	}

	for (dec = 1; dec < L_CODE; dec++)
	{
		s = 0;                           
		j = L_CODE - 1;
		i = j - dec;
		for (k = 0; k < (L_CODE - dec); k++, i--, j--)
		{
			s +=  (h2[k] * h2[k + dec])<<1;

			rr[j][i] = (((s + 0x8000)>>16) * ((sign[i] * sign[j])>>15))>>15;

			rr[i][j] = rr[j][i]; 
		}
	}
}
