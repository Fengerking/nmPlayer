/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-----------------------------------------------------*
 * Function Autocorr()                                 *
 *                                                     *
 *   Compute autocorrelations of signal with windowing *
 *                                                     *
 *-----------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "ld8a.h"
#include "tab_ld8a.h"

void Autocorr(
  Word16 x[],      /* (i)    : Input signal                      */
  Word16 m,        /* (i)    : LPC order                         */
  Word16 r_h[],    /* (o)    : Autocorrelations  (msb)           */
  Word16 r_l[]     /* (o)    : Autocorrelations  (lsb)           */
)
{
	//static Word16 number;
	Word16 i, j, norm;
	Word16 y[L_WINDOW];
	Word32 sum,tmp;
	Word16 overflag;
	/* Windowing of signal */
	for(i=0; i < L_WINDOW ;)
	{
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
		y[i] = (x[i]*hamwindow[i] + 16384)>>15;i++;
	}

	/* Compute r[0] and test for overflow */
	do {
		overflag = 0;
		sum = 1;                   /* Avoid case of all zeros */
		for(i= L_WINDOW-1; i>=0; i--)
		{
			tmp= (y[i] * y[i])<<1;
			sum = L_add(sum,tmp);
		}
		/* If overflow divide y[] by 4 */
		if(sum == MAX_32)
		{
			overflag = 1;
			for(i=L_WINDOW-1; i>=0; i--)
			{
				y[i] = y[i]>>2;
			}
		}
	}while (overflag != 0);
	/* Normalization of r[0] */
	norm = norm_l(sum);
	sum <<= norm;
	r_h[0] = (Word16)(sum >>16);
	r_l[0] = (Word16)((sum - (r_h[0]<<16))>>1);

	/* r[1] to r[m] */

	for (i = 1; i <= m; i++)
	{
		sum = 0;
		for(j=0; j<L_WINDOW-i; j++)
			sum += ((y[j]*y[j+i]));
		sum <<= (norm + 1);
		r_h[i] = (Word16)(sum >>16);
		r_l[i] = (Word16)((sum - (r_h[i]<<16))>>1);
	}
	return;
}


/*-------------------------------------------------------*
 * Function Lag_window()                                 *
 *                                                       *
 * Lag_window on autocorrelations.                       *
 *                                                       *
 * r[i] *= lag_wind[i]                                   *
 *                                                       *
 *  r[i] and lag_wind[i] are in special double precision.*
 *  See "oper_32b.c" for the format                      *
 *                                                       *
 *-------------------------------------------------------*/

void Lag_window(
  Word16 m,         /* (i)     : LPC order                        */
  Word16 r_h[],     /* (i/o)   : Autocorrelations  (msb)          */
  Word16 r_l[]      /* (i/o)   : Autocorrelations  (lsb)          */
)
{
	Word32 i;
	Word32 x;

	for(i=m; i>=1; i--)
	{
		x = (r_h[i]*lag_h[i-1])<<1;
		x += ((r_h[i] * lag_l[i-1])>>15)<<1;
		x += ((r_l[i] * lag_h[i-1])>>15)<<1;
		r_h[i] = (Word16)(x >>16);
		r_l[i] = (Word16)(((x>>1)-(r_h[i]<<15)));
	}
	return;
}



/*___________________________________________________________________________
 |                                                                           |
 |      LEVINSON-DURBIN algorithm in double precision                        |
 |      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        |
 |---------------------------------------------------------------------------|
 |                                                                           |
 | Algorithm                                                                 |
 |                                                                           |
 |       R[i]    autocorrelations.                                           |
 |       A[i]    filter coefficients.                                        |
 |       K       reflection coefficients.                                    |
 |       Alpha   prediction gain.                                            |
 |                                                                           |
 |       Initialization:                                                     |
 |               A[0] = 1                                                    |
 |               K    = -R[1]/R[0]                                           |
 |               A[1] = K                                                    |
 |               Alpha = R[0] * (1-K**2]                                     |
 |                                                                           |
 |       Do for  i = 2 to M                                                  |
 |                                                                           |
 |            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]                      |
 |                                                                           |
 |            K = -S / Alpha                                                 |
 |                                                                           |
 |            An[j] = A[j] + K*A[i-j]   for j=1 to i-1                       |
 |                                      where   An[i] = new A[i]             |
 |            An[i]=K                                                        |
 |                                                                           |
 |            Alpha=Alpha * (1-K**2)                                         |
 |                                                                           |
 |       END                                                                 |
 |                                                                           |
 | Remarks on the dynamics of the calculations.                              |
 |                                                                           |
 |       The numbers used are in double precision in the following format :  |
 |       A = AH <<16 + AL<<1.  AH and AL are 16 bit signed integers.         |
 |       Since the LSB's also contain a sign bit, this format does not       |
 |       correspond to standard 32 bit integers.  We use this format since   |
 |       it allows fast execution of multiplications and divisions.          |
 |                                                                           |
 |       "DPF" will refer to this special format in the following text.      |
 |       See oper_32b.c                                                      |
 |                                                                           |
 |       The R[i] were normalized in routine AUTO (hence, R[i] < 1.0).       |
 |       The K[i] and Alpha are theoretically < 1.0.                         |
 |       The A[i], for a sampling frequency of 8 kHz, are in practice        |
 |       always inferior to 16.0.                                            |
 |                                                                           |
 |       These characteristics allow straigthforward fixed-point             |
 |       implementation.  We choose to represent the parameters as           |
 |       follows :                                                           |
 |                                                                           |
 |               R[i]    Q31   +- .99..                                      |
 |               K[i]    Q31   +- .99..                                      |
 |               Alpha   Normalized -> mantissa in Q31 plus exponent         |
 |               A[i]    Q27   +- 15.999..                                   |
 |                                                                           |
 |       The additions are performed in 32 bit.  For the summation used      |
 |       to calculate the K[i], we multiply numbers in Q31 by numbers        |
 |       in Q27, with the result of the multiplications in Q27,              |
 |       resulting in a dynamic of +- 16.  This is sufficient to avoid       |
 |       overflow, since the final result of the summation is                |
 |       necessarily < 1.0 as both the K[i] and Alpha are                    |
 |       theoretically < 1.0.                                                |
 |___________________________________________________________________________|
*/


/* Last A(z) for case of unstable filter */

static Word16 old_A[M+1]={4096,0,0,0,0,0,0,0,0,0,0};
static Word16 old_rc[2]={0,0};
void Levinson(
  Word16 Rh[],      /* (i)     : Rh[M+1] Vector of autocorrelations (msb) */
  Word16 Rl[],      /* (i)     : Rl[M+1] Vector of autocorrelations (lsb) */
  Word16 A[],       /* (o) Q12 : A[M]    LPC coefficients  (m = 10)       */
  Word16 rc[]       /* (o) Q15 : rc[M]   Reflection coefficients.         */
)
{
	Word32 i, j;
	Word16 hi, lo;
	Word16 Kh, Kl;                /* reflection coefficient; hi and lo           */
	Word16 alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent         */
	Word16 Ah[M+1], Al[M+1];      /* LPC coef. in double prec.                   */
	Word16 Anh[M+1], Anl[M+1];    /* LPC coef.for next iteration in double prec. */
	Word32 t0, t1, t2, t3;            /* temporary variable                          */

	/* K = A[1] = -R[1] / R[0] */
	t1  = L_deposit_h(Rh[1]);
	t1 += (Rl[1] << 1);

	t2 = t1>0?t1:-t1;  
	t0  = Div_32(t2, Rh[0], Rl[0]);       /* R[1]/R[0] in Q31 */
	if(t1 > 0) t0= -(t0);          /* -R[1]/R[0]       */
	Kh = (Word16)(t0 >>16);
	Kl = (Word16)((t0 - (Kh<<16))>>1);
	rc[0] = Kh;
	t0 = t0 >>4;

	Ah[1] = (Word16)(t0 >>16);
	Al[1] = (Word16)((t0 - (Ah[1]<<16))>>1);

	/*  Alpha = R[0] * (1-K**2) */

	t0 = (Kh * Kh)<<1;

	t0 +=(((Kh * Kl)>>15)<<2);
	t0 = (t0 <0 )? -t0 :t0;

	t0 = ( (Word32)0x7fffffffL - t0 ); /* 1 - K*K  in Q31 */

	hi = (Word16)(t0 >>16);
	lo = (Word16)((t0 -(hi <<16))>>1);

	t0 = (Rh[0] * hi)<<1;
	t0 +=(((Rh[0] * lo)>>15)<<1);
	t0 +=(((Rl[0] * hi)>>15)<<1);
	/* Normalize Alpha */

	alp_exp = norm_l(t0);
	t0 = t0 << alp_exp;
	alp_h = (Word16)(t0 >>16);
	alp_l = (Word16)((t0 - (alp_h<<16))>>1);

	//L_Extract(t0, &alp_h, &alp_l);         /* DPF format    */

	/*--------------------------------------*
	 * ITERATIONS  I=2 to M                 *
	 *--------------------------------------*/

	for(i= 2; i<=M; i++)
	{
		/* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */
		t0 = 0;
		for(j=1; j<i; j++)
		{
			t3 = (Rh[j] * Ah[i-j])<<1;
			t3 += (((Rh[j] * Al[i-j])>>15)<<1);
			t3 += (((Rl[j] * Ah[i-j])>>15)<<1);
			t0 = (t0 + t3);
		}

		t0 = (t0 <<4);                  /* result in Q27 -> convert to Q31 */
		/* No overflow possible            */
		t1  = L_deposit_h(Rh[i]);
		t1 += (Rl[i] << 1);
		t0 +=( t1);                /* add R[i] in Q31                 */

		t1 =(t0 <0)? -t0:t0;
		t2 = Div_32(t1, alp_h, alp_l);     /* abs(t0)/Alpha                   */
		if(t0 > 0) t2= -(t2);       /* K =-t0/Alpha                    */
		t2 = (t2 << alp_exp);           /* denormalize; compare to Alpha   */

		Kh = (Word16)(t2 >>16);
		Kl = (Word16)((t2 - (Kh<<16))>>1);

		rc[i-1] = Kh;

		/* Test for unstable filter. If unstable keep old A(z) */
		if (abs_s(Kh) > 32750)
		{
			for(j=0; j<=M; j++)
			{
				A[j] = old_A[j];
			}
			rc[0] = old_rc[0];        /* only two rc coefficients are needed */
			rc[1] = old_rc[1];
			return;
		}

		/*------------------------------------------*
		 *  Compute new LPC coeff. -> An[i]         *
		 *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
		 *  An[i]= K                                *
		 *------------------------------------------*/
		for(j=1; j<i; j++)
		{
			t0 = (Kh * Ah[i-j])<<1;
			t0 +=(((Ah[i-j] * Kl)>>15)<<1);
			t0 +=(((Kh * Al[i-j])>>15)<<1);

			t1  = L_deposit_h(Ah[j]);
			t1 += (Al[j])<<1;
			t0 = (t0 + t1);
			Anh[j] = (Word16)(t0 >>16);
			Anl[j] = (Word16)((t0 - (Anh[j]<<16))>>1);
		}
		t2 = t2 >> 4;                  /* t2 = K in Q31 ->convert to Q27  */
		Anh[i] = (Word16)(t2 >>16);
		Anl[i] = (Word16)((t2 - (Anh[i]<<16))>>1);

		/*  Alpha = Alpha * (1-K**2) */

		t0 = (Kh * Kh)<<1;
		t0 +=(((Kh * Kl)>>15)<<2);
		t0 = (t0 <0)? -t0:t0;                      /* Some case <0 !! */
		t0 = ( (Word32)0x7fffffffL - t0 ); /* 1 - K*K  in Q31 */
		hi = (Word16)(t0 >>16);
		lo = (Word16)((t0 - (hi <<16))>>1);

		t0 = (alp_h * hi)<<1;
		t0 += (((alp_h * lo)>>15)<<1);
		t0 += (((alp_l * hi)>>15)<<1);

		/* Normalize Alpha */

		j = norm_l(t0);
		t0 = t0 << j;
		alp_h = (Word16)(t0 >>16);
		alp_l = (Word16)((t0 - (alp_h<<16))>>1);

		alp_exp = alp_exp + j;             /* Add normalization to alp_exp */

		/* A[j] = An[j] */

		for(j=1; j<=i; j++)
		{
			Ah[j] =Anh[j];
			Al[j] =Anl[j];
		}
	}

	/* Truncate A[i] in Q27 to Q12 with rounding */
	A[0] = 4096;
	for(i=1; i<=M; i++)
	{
		t0   = Ah[i]<<16;
		t0   += (Al[i]<<1);
		// t0   = L_Comp(Ah[i], Al[i]);
		old_A[i] = A[i] = (Word16)(((t0 << 1)+0x8000)>>16);
	}
	old_rc[0] = rc[0];
	old_rc[1] = rc[1];

	return;
}



/*-------------------------------------------------------------*
 *  procedure Az_lsp:                                          *
 *            ~~~~~~                                           *
 *   Compute the LSPs from  the LPC coefficients  (order=10)   *
 *-------------------------------------------------------------*/

/* local function */

static Word16 Chebps_11(Word16 x, Word16 f[], Word16 n);
static Word16 Chebps_10(Word16 x, Word16 f[], Word16 n);

void Az_lsp(
  Word16 a[],        /* (i) Q12 : predictor coefficients              */
  Word16 lsp[],      /* (o) Q15 : line spectral pairs                 */
  Word16 old_lsp[]   /* (i)     : old lsp[] (in case not found 10 roots) */
)
{
	Word16 i, j, nf, ip;
	Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
	Word16 x, y, sign, exp;
	Word16 *coef;
	Word16 f1[M/2+1], f2[M/2+1];
	Word32 t0;
	Flag   ovf_coef;
	Word16 (*pChebps)(Word16 x, Word16 f[], Word16 n);

	/*-------------------------------------------------------------*
	 *  find the sum and diff. pol. F1(z) and F2(z)                *
	 *    F1(z) <--- F1(z)/(1+z**-1) & F2(z) <--- F2(z)/(1-z**-1)  *
	 *                                                             *
	 * f1[0] = 1.0;                                                *
	 * f2[0] = 1.0;                                                *
	 *                                                             *
	 * for (i = 0; i< NC; i++)                                     *
	 * {                                                           *
	 *   f1[i+1] = a[i+1] + a[M-i] - f1[i] ;                       *
	 *   f2[i+1] = a[i+1] - a[M-i] + f2[i] ;                       *
	 * }                                                           *
	 *-------------------------------------------------------------*/
	ovf_coef = 0;
	pChebps = Chebps_11;

	f1[0] = 2048;          /* f1[0] = 1.0 is in Q11 */
	f2[0] = 2048;          /* f2[0] = 1.0 is in Q11 */

	for (i = 0; i< NC; i++)
	{
		t0 =  a[i+1]<<15;
		t0 += a[M-i]<<15;
		x = (Word16)(t0 >>16);

		f1[i+1] = sub(x, f1[i]);    /* f1[i+1] = a[i+1] + a[M-i] - f1[i] */

		t0 =  a[i+1] <<15;
    	        t0 -= a[M-i] <<15;
		x = (Word16)(t0 >>16);

		f2[i+1] = add(x, f2[i]);    /* f2[i+1] = a[i+1] - a[M-i] + f2[i] */
	}
	/*-------------------------------------------------------------*
	 * find the LSPs using the Chebichev pol. evaluation           *
	 *-------------------------------------------------------------*/

	nf=0;          /* number of found frequencies */
	ip=0;          /* indicator for f1 or f2      */

	coef = f1;

	xlow = grid[0];
	ylow = (*pChebps)(xlow, coef, NC);

	j = 0;
	while ( (nf < M) && (j < GRID_POINTS) )
	{

		j ++;    //j =add(j,1); 
		xhigh = xlow;
		yhigh = ylow;
		xlow  = grid[j];
		ylow  = (*pChebps)(xlow,coef,NC);

		//L_temp = L_mult(ylow ,yhigh);
		if ( (ylow * yhigh) <= 0)
		{
			/* divide 2 times the interval */
#if 1
			for (i = 0; i < 2; i++)
			{
				xmid = (xlow >>1) + (xhigh >>1);   //xmid = add( shr(xlow, 1) , shr(xhigh, 1)); /* xmid = (xlow + xhigh)/2 */
				ymid = (*pChebps)(xmid,coef,NC);
			//	L_temp = (ylow *ymid)<<1;
				if ( (ylow *ymid)<=0)
				{
					yhigh = ymid;
					xhigh = xmid;
				}
				else
				{
					ylow = ymid;
					xlow = xmid;
				}
			}
#endif
			/*-------------------------------------------------------------*
			 * Linear interpolation                                        *
			 *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
			 *-------------------------------------------------------------*/
			x   = (xhigh - xlow);
			y   = (yhigh - ylow);
			if(y == 0)
			{
				xint = xlow;
			}
			else
			{
				sign= y;
				y =(y<0)?-y:y;
				exp = norm_s(y);
				y   =y << exp;
				y   = div_s( (Word16)16383, y);

				t0 = (x*y)<<1;
				t0 = t0 >>(20 - exp);
				y   = (Word16)(t0);            /* y= (xhigh-xlow)/(yhigh-ylow) in Q11 */

				if(sign < 0) y = -(y);
				t0   = (ylow*y)<<1;
				t0   = t0 >>11;

				xint = (xlow - (Word16)(t0));         /* xint = xlow - ylow*y */
			}

			lsp[nf] = xint;
			xlow    = xint;

			nf++;

			if(ip == 0)
			{
				ip = 1;
				coef = f2;
			}
			else
			{
				ip = 0;
				coef = f1;
			}
			ylow = (*pChebps)(xlow,coef,NC);
		}
	}
	/* Check if M roots found */
	if(nf < M)
	{
		for(i=0; i<M; i++)
		{
			lsp[i] = old_lsp[i];
		}
		/* printf("\n !!Not 10 roots found in Az_lsp()!!!\n"); */
	}
	return;
}

/*--------------------------------------------------------------*
 * function  Chebps_11, Chebps_10:                              *
 *           ~~~~~~~~~~~~~~~~~~~~                               *
 *    Evaluates the Chebichev polynomial series                 *
 *--------------------------------------------------------------*
 *                                                              *
 *  The polynomial order is                                     *
 *     n = M/2   (M is the prediction order)                    *
 *  The polynomial is given by                                  *
 *    C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2 *
 * Arguments:                                                   *
 *  x:     input value of evaluation; x = cos(frequency) in Q15 *
 *  f[]:   coefficients of the pol.                             *
 *                         in Q11(Chebps_11), in Q10(Chebps_10) *
 *  n:     order of the pol.                                    *
 *                                                              *
 * The value of C(x) is returned. (Saturated to +-1.99 in Q14)  *
 *                                                              *
 *--------------------------------------------------------------*/
static __inline Word16 Chebps_11(Word16 x, Word16 f[], Word16 n)
{
	Word16 cheb;
	Word16 b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
	Word32 t0;

	/* Note: All computation are done in Q24. */

	b2_h = 256;                           /* b2 = 1.0 in Q24 DPF */
	b2_l = 0;

	t0 = x << 10;
	t0 += (f[1] <<13);
	b1_h = (Word16)(t0>>16);
	b1_l = (Word16)((t0 -( b1_h<<16))>>1);

	//for (i = 2; i<n; i++)
	{

		t0 = (b1_h * x) <<1;
		t0 += (((b1_l * x)>>15)<<1);
		t0 = t0 <<1;

		t0 += (b2_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

		t0 -= (b2_l <<1);
		t0 += (f[2]<<13);

		b0_h = (Word16)(t0>>16);
		b0_l = (Word16)((t0 - (b0_h<<16))>>1);


		t0 = (b0_h * x) <<1;
		t0 += (((b0_l * x)>>15)<<1);
		t0 = t0 <<1;

		t0 += (b1_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

		t0 -= (b1_l <<1);
		t0 += (f[3]<<13);

		b1_h = (Word16)(t0>>16);
		b1_l = (Word16)((t0 - (b1_h<<16))>>1);

		t0 = (b1_h * x) <<1;
		t0 += (((b1_l * x)>>15)<<1);
		t0 = t0 <<1;

		t0 += (b0_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

		t0 -= (b0_l <<1);
		t0 += (f[4]<<13);
		b0_h = (Word16)(t0>>16);
		b0_l = (Word16)((t0 - (b0_h<<16))>>1);

	}
	t0 = (b0_h * x)<<1;
	t0 += (((b0_l * x)>>15)<<1);
	t0 +=( b1_h *(Word16)-32768L)<<1; /* t0 = x*b1 - b2          */
	t0 -= (b1_l <<1);
	t0 += (f[5]<<12);

	t0 = L_shl2(t0, 6);                    /* Q24 to Q30 with saturation */
	cheb = extract_h(t0);                 /* Result in Q14              */
	return(cheb);
}


static __inline Word16 Chebps_10(Word16 x, Word16 f[], Word16 n)
{
	Word16 cheb;
	Word16 b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
	Word32 t0;

	/* Note: All computation are done in Q23. */

	b2_h = 128;                           /* b2 = 1.0 in Q23 DPF */
	b2_l = 0;

	t0 =  x <<9;
	t0 += f[1] << 13;
	b1_h = (Word16)(t0>>16);
	b1_l = (Word16)((t0 -(b1_h<<16))>>1);

	t0 = (b1_h * x) <<1;
	t0 += (((b1_l * x)>>15)<<1);
	t0 = t0 <<1;

	t0 += (b2_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

	t0 -= (b2_l <<1);
	t0 += (f[2]<<13);

	b0_h = (Word16)(t0>>16);
	b0_l = (Word16)((t0 - (b0_h<<16))>>1);


	t0 = (b0_h * x) <<1;
	t0 += (((b0_l * x)>>15)<<1);
	t0 = t0 <<1;

	t0 += (b1_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

	t0 -= (b1_l <<1);
	t0 += (f[3]<<13);

	b1_h = (Word16)(t0>>16);
	b1_l = (Word16)((t0 - (b1_h<<16))>>1);

	t0 = (b1_h * x) <<1;
	t0 += (((b1_l * x)>>15)<<1);
	t0 = t0 <<1;

	t0 += (b0_h *(Word16)-32768L)<<1;/* t0 = 2.0*x*b1 - b2         */

	t0 -= (b0_l <<1);
	t0 += (f[4]<<13);

	b0_h = (Word16)(t0>>16);
	b0_l = (Word16)((t0 - (b0_h<<16))>>1);
	t0 = (b0_h * x)<<1;
	t0 += (((b0_l * x)>>15)<<1);
	t0 += (b1_h * (Word16)-32768L)<<1; /* t0 = x*b1 - b2          */
	t0 -= (b1_l <<1);
	t0 += (f[5]<<12);

	t0 = L_shl2(t0, 7);                    /* Q23 to Q30 with saturation */
	cheb = extract_h(t0);                 /* Result in Q14              */
	return(cheb);
}

