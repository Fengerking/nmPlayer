/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*---------------------------------------------------------------------------*
 * Pitch related functions                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                                   *
 *---------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*---------------------------------------------------------------------------*
 * Function  Pitch_ol_fast                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                                   *
 * Compute the open loop pitch lag. (fast version)                           *
 *                                                                           *
 *---------------------------------------------------------------------------*/


Word16 Pitch_ol_fast(  /* output: open loop pitch lag                        */
   Word16 signal[],    /* input : signal used to compute the open loop pitch */
                       /*     signal[-pit_max] to signal[-1] should be known */
   Word16   pit_max,   /* input : maximum pitch lag                          */
   Word16   L_frame    /* input : length of frame to compute pitch           */
)
{
	Word32  i, j;
	Word16  max1, max2, max3;
	Word16  max_h, max_l, ener_h, ener_l;
	Word16  T1, T2, T3;
	Word16  *p, *p1;
	Word32  max, sum, L_temp;

	/* Scaled signal */

	Word16 scaled_signal[L_FRAME+PIT_MAX];
	Word16 *scal_sig;
	scal_sig = &scaled_signal[pit_max];

	/*--------------------------------------------------------*
	 *  Verification for risk of overflow.                    *
	 *--------------------------------------------------------*/

	sum = 0;
	for(i= -pit_max; i< L_frame; i+=2)
	{
		L_temp = (signal[i] * signal[i])<<1;
		sum = L_add(sum, L_temp);
	}
	/*--------------------------------------------------------*
	 * Scaling of input signal.                               *
	 *                                                        *
	 *   if Overflow        -> scal_sig[i] = signal[i]>>3     *
	 *   else if sum < 1^20 -> scal_sig[i] = signal[i]<<3     *
	 *   else               -> scal_sig[i] = signal[i]        *
	 *--------------------------------------------------------*/
	if(sum == MAX_32)
	{
		for(i=-pit_max; i<L_frame; i++)
		{
			scal_sig[i] = signal[i] >> 3;
		}
	}
	else {
		if ( sum < 0x100000 )  /* if (sum < 2^20) */
		{
			for(i=-pit_max; i<L_frame; i++)
			{
				scal_sig[i] = signal[i] << 3;
			}
		}
		else
		{
			for(i=-pit_max; i<L_frame; i++)
			{
				scal_sig[i] = signal[i];
			}
		}
	}

	/*--------------------------------------------------------------------*
	 *  The pitch lag search is divided in three sections.                *
	 *  Each section cannot have a pitch multiple.                        *
	 *  We find a maximum for each section.                               *
	 *  We compare the maxima of each section by favoring small lag.      *
	 *                                                                    *
	 *  First section:  lag delay = 20 to 39                              *
	 *  Second section: lag delay = 40 to 79                              *
	 *  Third section:  lag delay = 80 to 143                             *
	 *--------------------------------------------------------------------*/
	/* First section */
	max = MIN_32;
	T1  = 20;    /* Only to remove warning from some compilers */
	for (i = 20; i < 40; i++) {
		p  = scal_sig;
		p1 = &scal_sig[-i];
		sum = 0;
		for (j=0; j<L_frame; j+=8, p+=8, p1+=8)
		{
			sum += ((*p)*(*p1));
			sum += ((*(p+2))*(*(p1+2)));
			sum += ((*(p+4))*(*(p1+4)));
			sum += ((*(p+6))*(*(p1+6)));
		}
		sum <<=1;
		if (sum > max) { max = sum; T1 = i;   }
	}
	/* compute energy of maximum */
	sum = 0;                   /* to avoid division by zero */
	p = &scal_sig[-T1];
	for(i=0; i<L_frame; i+=8, p+=8)
	{
		sum += ((*p)*(*p));
		sum += ((*(p+2))*(*(p+2)));
		sum += ((*(p+4))*(*(p+4)));
		sum += ((*(p+6))*(*(p+6)));
	}
	sum = (sum <<1) + 1;

	/* max1 = max/sqrt(energy)                  */
	/* This result will always be on 16 bits !! */
#ifdef ASM_OPT 
	sum = Inv_sqrt_asm(sum);
#else
	sum = Inv_sqrt(sum);            /* 1/sqrt(energy),    result in Q30 */
#endif
	max_h = (Word16)(max >>16);
	max_l = (Word16)((max - (max_h<<16))>>1); 

	ener_h = (Word16)(sum >>16);
	ener_l = (Word16)((sum - (ener_h <<16))>>1);
	sum = (max_h * ener_h);
	sum +=(((max_h * ener_l)>>15));
	sum +=(((max_l * ener_h)>>15));
	//sum  = Mpy_32(max_h, max_l, ener_h, ener_l);
	sum <<=1;
	max1 = (Word16)sum;

	/* Second section */
	max = MIN_32;
	T2  = 40;    /* Only to remove warning from some compilers */
	for (i = 40; i < 80; i++) {
		p  = scal_sig;
		p1 = &scal_sig[-i];
		sum = 0;
		for (j=0; j<L_frame; j+=8, p+=8, p1+=8)
		{
			sum += ((*p)*(*p1));
			sum += ((*(p+2))*(*(p1+2)));
			sum += ((*(p+4))*(*(p1+4)));
			sum += ((*(p+6))*(*(p1+6)));
		}
		sum <<=1;
		//L_temp = L_sub(sum, max);
		if (sum > max) { max = sum; T2 = i;   }
	}
	/* compute energy of maximum */
	sum = 0;                   /* to avoid division by zero */
	p = &scal_sig[-T2];
	for(i=0; i<L_frame; i+=8, p+=8)
	{
		sum += ((*p)*(*p));
		sum += ((*(p+2))*(*(p+2)));
		sum += ((*(p+4))*(*(p+4)));
		sum += ((*(p+6))*(*(p+6)));

	}
	sum = (sum <<1) + 1;
	/* max2 = max/sqrt(energy)                  */
	/* This result will always be on 16 bits !! */

#ifdef ASM_OPT
	sum = Inv_sqrt_asm(sum);
#else
	sum = Inv_sqrt(sum);            /* 1/sqrt(energy),    result in Q30 */
#endif           /* 1/sqrt(energy),    result in Q30 */

	max_h = (Word16)(max >>16);
	max_l = (Word16)((max - (max_h<<16))>>1); 

	ener_h = (Word16)(sum >>16);
	ener_l = (Word16)((sum - (ener_h <<16))>>1);

	sum = (max_h * ener_h);
	sum +=(((max_h * ener_l)>>15));
	sum +=(((max_l * ener_h)>>15));
	sum <<=1; 
	max2 = (Word16)(sum);

	/* Third section */
	max = MIN_32;
	T3  = 80;    /* Only to remove warning from some compilers */
	for (i = 80; i < 143; i+=2) {
		p  = scal_sig;
		p1 = &scal_sig[-i];
		sum = 0;
		for (j=0; j<L_frame; j+=8, p+=8, p1+=8)
		{
			sum += ((*p)*(*p1));
			sum += ((*(p+2))*(*(p1+2)));
			sum += ((*(p+4))*(*(p1+4)));
			sum += ((*(p+6))*(*(p1+6)));
		}
		sum <<=1;
		//L_temp = L_sub(sum, max);
		if (sum > max) { max = sum; T3 = i;   }
	}
	/* Test around max3 */
	sum = 0;                   /* to avoid division by zero */
	p = &scal_sig[-T3];
	for(i=0; i<L_frame; i+=8, p+=8)
	{
		sum += ((*p)*(*p));
		sum += ((*(p+2))*(*(p+2)));
		sum += ((*(p+4))*(*(p+4)));
		sum += ((*(p+6))*(*(p+6)));

	}
	sum = (sum <<1) + 1;

	/* max1 = max/sqrt(energy)                  */
	/* This result will always be on 16 bits !! */
#ifdef ASM_OPT
	sum = Inv_sqrt_asm(sum);
#else
	sum = Inv_sqrt(sum);            /* 1/sqrt(energy),    result in Q30 */
#endif
	max_h = (Word16)(max >>16);
	max_l = (Word16)((max - (max_h<<16))>>1); 

	ener_h = (Word16)(sum >>16);
	ener_l = (Word16)((sum - (ener_h <<16))>>1);

	sum = (max_h * ener_h);
	sum +=(((max_h * ener_l)>>15));
	sum +=(((max_l * ener_h)>>15));
	sum <<=1;
	max3 = (Word16)sum;

	/*-----------------------*
	 * Test for multiple.    *
	 *-----------------------*/

	/* if( abs(T2*2 - T3) < 5)  */
	/*    max2 += max3 * 0.25;  */
	i = (T2 <<1) - T3;
	j = (i<0) ? -i :i;
	if(j < 5)
		max2 += (max3 >>2);

	// max2 = add(max2, shr(max3, 2));

	/* if( abs(T2*3 - T3) < 7)  */
	/*    max2 += max3 * 0.25;  */

	i += T2;
	j = (i < 0)? -i :i;

	if(j < 7)
		max2 += (max3 >>2 );

	i = (T1 <<1) - T2;
	j = (i < 0)? -i:i;

	if(j < 5)
		max1 = max1 + ((max2 * 6554)>>15);
	//max1 = add(max1, (max2 * 6554)>>15);

	/* if( abs(T1*3 - T2) < 7)  */
	/*    max1 += max2 * 0.20;  */
	i += T1;
	j = (i < 0)? -i :i;

	if(j < 7) 
		max1 = max1 + ((max2 * 6554)>>15);
	//max1 = add(max1, (max2 * 6554)>>15);

	/*--------------------------------------------------------------------*
	 * Compare the 3 sections maxima.                                     *
	 *--------------------------------------------------------------------*/

	if((max1< max2)) {max1 = max2; T1 = T2;  }
	if((max1< max3))  {T1 = T3; }

	return T1;
}




/*--------------------------------------------------------------------------*
 *  Function  Dot_Product()                                                 *
 *  ~~~~~~~~~~~~~~~~~~~~~~                                                  *
 *--------------------------------------------------------------------------*/
Word32 Dot_Product(      /* (o)   :Result of scalar product. */
       Word16   x[],     /* (i)   :First vector.             */
       Word16   y[],     /* (i)   :Second vector.            */
       Word16   lg       /* (i)   :Number of point.          */
)
{
	Word32 i;
	Word32 sum;

	sum = 0;
	for(i=0; i<lg; i++)
	sum += (x[i]*y[i])<<1;
	return sum;
}

/*--------------------------------------------------------------------------*
 *  Function  Pitch_fr3_fast()                                              *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~                                              *
 * Fast version of the pitch close loop.                                    *
 *--------------------------------------------------------------------------*/

Word16 Pitch_fr3_fast(/* (o)     : pitch period.                          */
  Word16 exc[],       /* (i)     : excitation buffer                      */
  Word16 xn[],        /* (i)     : target vector                          */
  Word16 h[],         /* (i) Q12 : impulse response of filters.           */
  Word16 L_subfr,     /* (i)     : Length of subframe                     */
  Word16 t0_min,      /* (i)     : minimum value in the searched range.   */
  Word16 t0_max,      /* (i)     : maximum value in the searched range.   */
  Word16 i_subfr,     /* (i)     : indicator for first subframe.          */
  Word16 *pit_frac    /* (o)     : chosen fraction.                       */
)
{
	Word16 t, t0, i;
	Word16 Dn[L_SUBFR];
	Word16 exc_tmp[L_SUBFR];
	Word32 max, corr;
	Word16 *p;

	/*-----------------------------------------------------------------*
	 * Compute correlation of target vector with impulse response.     *
	 *-----------------------------------------------------------------*/
#ifdef ASM_OPT //Wm
	Cor_h_X_asm(h, xn, Dn);
#else
	Cor_h_X(h, xn, Dn);
#endif
	/*-----------------------------------------------------------------*
	 * Find maximum integer delay.                                     *
	 *-----------------------------------------------------------------*/
	max = MIN_32;
	t0 = t0_min; /* Only to remove warning from some compilers */

	for(t=t0_min; t<=t0_max; t++)
	{
		p = &exc[-t];
		corr = 0;
		for(i=0; i< L_subfr; i++)
			corr += (Dn[i]*(*(p+i)))<<1;
		if(corr > max) {max = corr; t0 = t;}
	}
	/*-----------------------------------------------------------------*
	 * Test fractions.                                                 *
	 *-----------------------------------------------------------------*/
	/* Fraction 0 */
	Pred_lt_30(exc, t0, 0, L_subfr);
	max  = 0;
	for(i=0; i<L_subfr; i++)
		max  += (Dn[i]*exc[i])<<1;
	*pit_frac = 0;
	/* If first subframe and lag > 84 do not search fractional pitch */

	if((i_subfr == 0) && (t0 > 84))
		return t0;

	memcpy(exc_tmp,exc,2*L_subfr);
	//Copy(exc, exc_tmp, L_subfr);
	/* Fraction -1/3 */
#ifdef ASM_OPT
	Pred_lt_3_asm(exc, t0, -1, L_subfr);
#else
	Pred_lt_3(exc, t0, -1, L_subfr);
#endif
	corr  = 0;
	for(i=0; i<L_subfr; i++)
		corr  += (Dn[i]*exc[i])<<1;

	if(corr > max) {
		max = corr;
		*pit_frac = -1;
		memcpy(exc_tmp,exc, 2*L_subfr);
		// Copy(exc, exc_tmp, L_subfr);
	}
	/* Fraction +1/3 */
#ifdef ASM_OPT
	Pred_lt_3_asm(exc, t0, 1, L_subfr);
#else
	Pred_lt_3(exc, t0, 1, L_subfr);
#endif

	corr  = 0;
	for(i=0; i<L_subfr; i++)
		corr  += (Dn[i]*exc[i])<<1;
	if(corr > max) {
		max = corr;
		*pit_frac =  1;
	}
	else
		memcpy(exc,exc_tmp,2*L_subfr);

	return t0;
}


/*---------------------------------------------------------------------*
 * Function  G_pitch:                                                  *
 *           ~~~~~~~~                                                  *
 *---------------------------------------------------------------------*
 * Compute correlations <xn,y1> and <y1,y1> to use in gains quantizer. *
 * Also compute the gain of pitch. Result in Q14                       *
 *  if (gain < 0)  gain =0                                             *
 *  if (gain >1.2) gain =1.2                                           *
 *---------------------------------------------------------------------*/


Word16 G_pitch(      /* (o) Q14 : Gain of pitch lag saturated to 1.2       */
  Word16 xn[],       /* (i)     : Pitch target.                            */
  Word16 y1[],       /* (i)     : Filtered adaptive codebook.              */
  Word16 g_coeff[],  /* (i)     : Correlations need for gain quantization. */
  Word16 L_subfr     /* (i)     : Length of subframe.                      */
)
{
	Word32 i;
	Word16 xy, yy, exp_xy, exp_yy, gain;
	Word32 s;
	Word16 scaled_y1[L_SUBFR];

	/* divide "y1[]" by 4 to avoid overflow */
	for(i=L_subfr-1; i>=0; i--)
		scaled_y1[i] = y1[i]>>2;
	/* Compute scalar product <y1[],y1[]> */
	s = 1;                    /* Avoid case of all zeros */
	for(i=L_subfr-1; i>=0; i--)
		s = L_mac(s, y1[i], y1[i]);

	if(s == MAX_32)
	{
		s = 1;                  /* Avoid case of all zeros */
		for(i=L_subfr-1; i>=0; i--)
			s += ((scaled_y1[i]*scaled_y1[i])<<1);
		exp_yy = norm_l(s);
		yy = (Word16)(((s <<exp_yy)+0x8000)>>16);
		exp_yy = exp_yy - 4;
	}
	else
	{
		exp_yy = norm_l(s);
		yy     = vo_round( s << exp_yy);
	}

	/* Compute scalar product <xn[],y1[]> */
	s = 0;
	for(i=0; i<L_subfr; i++)
		s = L_mac(s, xn[i], y1[i]);

	if (s == MAX_32) {
		s = 0;
		for(i=0; i<L_subfr; i++)
			s += ((xn[i]*scaled_y1[i])<<1);
		exp_xy = norm_l(s);
		xy     = (Word16)(((s<<exp_xy)+0x8000)>>16);
		exp_xy = exp_xy - 2;
	}
	else {
		exp_xy = norm_l(s);
		xy     = (Word16)(((s << exp_xy) + 0x8000)>>16);
	}

	g_coeff[0] = yy;
	g_coeff[1] = 15 - exp_yy;
	g_coeff[3] = 15 - exp_xy;
	g_coeff[2] = xy;
	/* If (xy <= 0) gain = 0 */
	if (xy <= 0)
	{
		g_coeff[3] = -15;   /* Force exp_xy to -15 = (15-30) */
		return((Word16)0);
	}
	/* compute gain = xy/yy */
	xy >>= 1;                         /* Be sure xy < yy */
	gain = div_s( xy, yy);

	i = exp_xy - exp_yy;
	gain = shr(gain, i);               /* saturation if > 1.99 in Q14 */

	/* if(gain >1.2) gain = 1.2  in Q14 */
	if( gain > 19661)
	{
		gain = 19661;
	}
	return(gain);
}

/*----------------------------------------------------------------------*
 *    Function Enc_lag3                                                 *
 *             ~~~~~~~~                                                 *
 *   Encoding of fractional pitch lag with 1/3 resolution.              *
 *----------------------------------------------------------------------*
 * The pitch range for the first subframe is divided as follows:        *
 *   19 1/3  to   84 2/3   resolution 1/3                               *
 *   85      to   143      resolution 1                                 *
 *                                                                      *
 * The period in the first subframe is encoded with 8 bits.             *
 * For the range with fractions:                                        *
 *   index = (T-19)*3 + frac - 1;   where T=[19..85] and frac=[-1,0,1]  *
 * and for the integer only range                                       *
 *   index = (T - 85) + 197;        where T=[86..143]                   *
 *----------------------------------------------------------------------*
 * For the second subframe a resolution of 1/3 is always used, and the  *
 * search range is relative to the lag in the first subframe.           *
 * If t0 is the lag in the first subframe then                          *
 *  t_min=t0-5   and  t_max=t0+4   and  the range is given by           *
 *       t_min - 2/3   to  t_max + 2/3                                  *
 *                                                                      *
 * The period in the 2nd subframe is encoded with 5 bits:               *
 *   index = (T-(t_min-1))*3 + frac - 1;    where T[t_min-1 .. t_max+1] *
 *----------------------------------------------------------------------*/
Word16 Enc_lag3(     /* output: Return index of encoding */
  Word16 T0,         /* input : Pitch delay              */
  Word16 T0_frac,    /* input : Fractional pitch delay   */
  Word16 *T0_min,    /* in/out: Minimum search delay     */
  Word16 *T0_max,    /* in/out: Maximum search delay     */
  Word16 pit_min,    /* input : Minimum pitch delay      */
  Word16 pit_max,    /* input : Maximum pitch delay      */
  Word16 pit_flag    /* input : Flag for 1st subframe    */
)
{
	Word16 index, i;

	if (pit_flag == 0)   /* if 1st subframe */
	{
		/* encode pitch delay (with fraction) */
		if (T0 <= 85)
		{
			/* index = t0*3 - 58 + t0_frac   */
			i = (T0<<1) + T0;
			index = i - 58 + T0_frac;
		}
		else {
			index = T0 + 112;
		}
		/* find T0_min and T0_max for second subframe */
		*T0_min = T0 - 5;
		if (*T0_min < pit_min)
		{
			*T0_min = pit_min;
		}

		*T0_max = *T0_min + 9;
		if (*T0_max > pit_max)
		{
			*T0_max = pit_max;
			*T0_min = *T0_max - 9;
		}
	}
	else      /* if second subframe */
	{
		/* i = t0 - t0_min;               */
		/* index = i*3 + 2 + t0_frac;     */
		i = T0 - *T0_min;
		i = (i <<1) + i;
		index = (i + 2) + T0_frac;
	}
	return index;
}

