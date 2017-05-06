/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/* Functions Corr_xy2() and Cor_h_x()   */

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

/*---------------------------------------------------------------------------*
 * Function corr_xy2()                                                       *
 * ~~~~~~~~~~~~~~~~~~~                                                       *
 * Find the correlations between the target xn[], the filtered adaptive      *
 * codebook excitation y1[], and the filtered 1st codebook innovation y2[].  *
 *   g_coeff[2]:exp_g_coeff[2] = <y2,y2>                                     *
 *   g_coeff[3]:exp_g_coeff[3] = -2<xn,y2>                                   *
 *   g_coeff[4]:exp_g_coeff[4] = 2<y1,y2>                                    *
 *---------------------------------------------------------------------------*/
void Corr_xy2(
      Word16 xn[],           /* (i) Q0  :Target vector.                  */
      Word16 y1[],           /* (i) Q0  :Adaptive codebook.              */
      Word16 y2[],           /* (i) Q12 :Filtered innovative vector.     */
      Word16 g_coeff[],      /* (o) Q[exp]:Correlations between xn,y1,y2 */
      Word16 exp_g_coeff[]   /* (o)       :Q-format of g_coeff[]         */
)
{
	Word16   exp;
	Word16   exp_y2y2,exp_xny2,exp_y1y2;
	Word16   y2y2,    xny2,    y1y2;
	Word32   i, L_acc1, L_acc2, L_acc3;
	//Word16   scaled_y2[L_SUBFR];       /* Q9 */

	/*------------------------------------------------------------------*
	 * Scale down y2[] from Q12 to Q9 to avoid overflow                 *
	 *------------------------------------------------------------------*/
	/* Compute scalar product <y2[],y2[]> */
	L_acc1 = 1;                       /* Avoid case of all zeros */
	L_acc2 = 1;
	L_acc3 = 1;

	for(i=0; i<L_SUBFR;)
	{
		L_acc1 += ((y2[i]>>3) * (y2[i]>>3))<<1; 
		L_acc2 += (xn[i] * (y2[i]>>3))<<1;
		L_acc3 += (y1[i] * (y2[i]>>3))<<1;
		i++;
		L_acc1 += ((y2[i]>>3) * (y2[i]>>3))<<1; 
		L_acc2 += (xn[i] * (y2[i]>>3))<<1;
		L_acc3 += (y1[i] * (y2[i]>>3))<<1; 
		i++;
		L_acc1 += ((y2[i]>>3) * (y2[i]>>3))<<1; 
		L_acc2 += (xn[i] * (y2[i]>>3))<<1;
		L_acc3 += (y1[i] * (y2[i]>>3))<<1; 
		i++;
		L_acc1 += ((y2[i]>>3) * (y2[i]>>3))<<1; 
		L_acc2 += (xn[i] * (y2[i]>>3))<<1;
		L_acc3 += (y1[i] * (y2[i]>>3))<<1; 
		i++;
		L_acc1 += ((y2[i]>>3) * (y2[i]>>3))<<1; 
		L_acc2 += (xn[i] * (y2[i]>>3))<<1;
		L_acc3 += (y1[i] * (y2[i]>>3))<<1; 
		i++;
	}

	exp      = norm_l(L_acc1);
	y2y2     = (Word16)(((L_acc1 << exp) + 0x8000)>>16);
	exp_y2y2 = exp + 3;      
	g_coeff[2]     = y2y2;
	exp_g_coeff[2] = exp_y2y2;


	exp      = norm_l(L_acc2);
	xny2     = (Word16)(((L_acc2 << exp) + 0x8000)>>16);
	exp_xny2 = exp - 7;
	g_coeff[3]     = -xny2;
	exp_g_coeff[3] = exp_xny2;                   /* -2<xn,y2> */


	exp      = norm_l(L_acc3);
	y1y2     = (Word16)(((L_acc3 << exp) +0x8000)>>16);
	exp_y1y2 = exp - 7; 
	g_coeff[4]     = y1y2;
	exp_g_coeff[4] = exp_y1y2;                 /* 2<y1,y2> */

	return;
}


/*--------------------------------------------------------------------------*
 *  Function  Cor_h_X()                                                     *
 *  ~~~~~~~~~~~~~~~~~~~                                                     *
 * Compute correlations of input response h[] with the target vector X[].   *
 *--------------------------------------------------------------------------*/
void Cor_h_X(
     Word16 h[],        /* (i) Q12 :Impulse response of filters      */
     Word16 X[],        /* (i)     :Target vector                    */
     Word16 D[]         /* (o)     :Correlations between h[] and D[] */
                        /*          Normalized to 13 bits            */
)
{
	Word32 i, j;
	Word32 s, max;
	Word32 y32[L_SUBFR];

	/* first keep the result on 32 bits and find absolute maximum */
	max = 0;
	for (i = 0; i < L_SUBFR; i++)
	{
		s = 0;
		for (j = i; j <  L_SUBFR; j++)
			s += (X[j] * h[j-i])<<1;
		y32[i] = s;
		s = (s < 0)? -s:s;
		if(s>max) {
			max = s;
		}
	}
	/* Find the number of right shifts to do on y32[]  */
	/* so that maximum is on 13 bits                   */
	j = norm_l(max);
	if( j > 16 ) {
		j = 16;
	}
	j = 18 - j;
	for(i=0; i<L_SUBFR; i++) {
		D[i] =  (Word16)(y32[i] >> j) ;
	}
	return;
}

