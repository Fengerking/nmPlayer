/*-------------------------------------------------------------------*
*                         DECIM54.C								 *
*-------------------------------------------------------------------*
* Decim_12k8   : decimation of 16kHz signal to 12.8kHz.             *
* Oversamp_16k : oversampling from 12.8kHz to 16kHz.                *
*-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "acelp_fx.h"
#include "cnst_wb_fx.h"

#define FAC4   4
#define FAC5   5
#define INV_FAC5   6554                    /* 1/5 in Q15 */
#define DOWN_FAC  26215                    /* 4/5 in Q15 */
#define UP_FAC    20480                    /* 5/4 in Q14 */

#define NB_COEF_DOWN  15
#define NB_COEF_UP    12

/* Local functions */
static void Up_samp(
					Word16 * sig_d,                       /* input:  signal to oversampling  */
					Word16 * sig_u,                       /* output: oversampled signal      */
					Word16 L_frame                        /* input:  length of output        */
					);

#ifdef ASM_OPT
Word16 Interpol_asm(                    /* return result of interpolation */
					   Word16 * x,                           /* input vector                   */
					   Word16 * fir,                         /* filter coefficient             */
					   Word16 frac,                          /* fraction (0..resol)            */
					   Word16 resol,                         /* resolution                     */
					   Word16 nb_coef                        /* number of coefficients         */
					   );
#else
static Word16 Interpol(                    /* return result of interpolation */
					   Word16 * x,                           /* input vector                   */
					   Word16 * fir,                         /* filter coefficient             */
					   Word16 frac,                          /* fraction (0..resol)            */
					   Word16 resol,                         /* resolution                     */
					   Word16 nb_coef                        /* number of coefficients         */
					   );
#endif
/* 1/5 resolution interpolation filter  (in Q14)  */
/* -1.5dB @ 6kHz, -6dB @ 6.4kHz, -10dB @ 6.6kHz, -20dB @ 6.9kHz, -25dB @ 7kHz, -55dB @ 8kHz */
static Word16 fir_up[120] =
{
	-1, 12, -33, 68, -119, 191, -291, 430, -634, 963, -1616, 3792, 15317, -2496, 1288, -809, 542, -369, 247, -160, 96, -52, 23, -6,
	-4, 24, -62, 124, -213, 338, -510, 752, -1111, 1708, -2974, 8219, 12368, -3432, 1881, -1204, 812, -552, 368, -235, 139, -73, 30, -7,
	-7, 30, -73, 139, -235, 368, -552, 812, -1204, 1881, -3432, 12368, 8219, -2974, 1708, -1111, 752, -510, 338, -213, 124, -62, 24, -4,
	-6, 23, -52, 96, -160, 247, -369, 542, -809, 1288, -2496, 15317, 3792, -1616, 963, -634, 430, -291, 191, -119, 68, -33, 12, -1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#if FILE_DECIM54_FX_OPT
//static Word16 signal[L_FRAME16k + (2 * NB_COEF_DOWN)];
#endif

void Init_Oversamp_16k(
					   Word16 mem[]                          /* output: memory (2*NB_COEF_UP) set to zeros  */
)
{
	AMRWBDecSetZero(mem, 2 * NB_COEF_UP);
	return;
}

void Oversamp_16k(
				  Word16 sig12k8[],                     /* input:  signal to oversampling  */
				  Word16 lg,                            /* input:  length of input         */
				  Word16 sig16k[],                      /* output: oversampled signal      */
				  Word16 mem[]                          /* in/out: memory (2*NB_COEF_UP)   */
)
{
#if FILE_DECIM54_FX_OPT
	Word16 lg_up;
	Word16 signal[L_SUBFR + (2 * NB_COEF_UP)];

	Copy(mem, signal, NB_COEF_UP<<1);
	Copy(sig12k8,signal + (2 * NB_COEF_UP),lg);

	//memcpy(signal, mem, 4 * NB_COEF_UP);
	//memcpy(signal + (2 * NB_COEF_UP), sig12k8, lg<<1);
	//  lg_up = shl(mult(lg, UP_FAC), 1);
	lg_up = (((Word32)lg * UP_FAC) >> 15) << 1;  
	Up_samp(signal + NB_COEF_UP, sig16k, lg_up);
	Copy(signal + lg, mem,NB_COEF_UP<<1);

	//memcpy(mem, signal + lg, 4 * NB_COEF_UP); 
	return;   
#else
	Word16 lg_up;
	Word16 signal[L_SUBFR + (2 * NB_COEF_UP)];

	Copy(mem, signal, 2 * NB_COEF_UP);
	Copy(sig12k8, signal + (2 * NB_COEF_UP), lg);

	lg_up = shl(mult(lg, UP_FAC), 1);

	Up_samp(signal + NB_COEF_UP, sig16k, lg_up);

	Copy(signal + lg, mem, 2 * NB_COEF_UP);
	return;
#endif
}

static void Up_samp(
					Word16 * sig_d,                       /* input:  signal to oversampling  */
					Word16 * sig_u,                       /* output: oversampled signal      */
					Word16 L_frame                        /* input:  length of output        */
					)
{
#if FILE_DECIM54_FX_OPT
	Word16 i, j, pos, frac;
	pos = 0;										             /* position with 1/5 resolution */
	for (j = 0; j < L_frame; j++)
	{
		i = (pos * INV_FAC5) >> 15;						         /* integer part = pos * 1/5 */
		frac = pos - (i << 2) - i;                               /* frac = pos - (pos/5)*5   */

#ifdef ASM_OPT
		sig_u[j] = Interpol_asm(&sig_d[i], fir_up, frac, FAC5, NB_COEF_UP);
#else
		sig_u[j] = Interpol(&sig_d[i], fir_up, frac, FAC5, NB_COEF_UP);
#endif

		pos += FAC4;                                             /* position + 4/5 */
	}
	return;

#else

	Word16 i, j, pos, frac;
	pos = 0;                                                    /* position with 1/5 resolution */
	for (j = 0; j < L_frame; j++)
	{
		i = mult(pos, INV_FAC5);                                /* integer part = pos * 1/5 */
		frac = sub(pos, add(shl(i, 2), i));                     /* frac = pos - (pos/5)*5   */
		sig_u[j] = Interpol(&sig_d[i], fir_up, frac, FAC5, NB_COEF_UP); 
		pos = add(pos, FAC4);                                   /* position + 4/5 */
	}
	return;
#endif
}

/* Fractional interpolation of signal at position (frac/resol) */

#ifndef ASM_OPT
static Word16 Interpol(                    /* return result of interpolation */
					   Word16 * x,                           /* input vector                   */
					   Word16 * fir,                         /* filter coefficient             */
					   Word16 frac,                          /* fraction (0..resol)            */
					   Word16 resol,                         /* resolution                     */
					   Word16 nb_coef                        /* number of coefficients         */
					   )
{
#if 0//(!FUNC_INTERPOL_WB_OPT)
	Word16 i, k;
	Word32 L_sum;
	x = x - nb_coef + 1; 
	L_sum = 0L;

	for (i = 0, k = sub(sub(resol, 1), frac); i < 2 * nb_coef; i++, k = (Word16) (k + resol))
	{
		L_sum = L_mac(L_sum, x[i], fir[k]);
	}
	L_sum = L_shl(L_sum, 1);                                       /* saturation can occur here */
	return (vo_round(L_sum));
#else
	Word16 k;
	Word32 L_sum;
	x -= nb_coef - 1;    //   nb_coef = 12
	k = resol - 1 - frac;
	k = k * 24;
    if(k == 96)
	{
    L_sum = x[11] * fir[k+11];
	}
	else
	{
	L_sum = x[0] * fir[k++];
	L_sum += x[1] * fir[k++];
	L_sum += x[2] * fir[k++];
	L_sum += x[3] * fir[k++];
	L_sum += x[4] * fir[k++];
	L_sum += x[5] * fir[k++];
	L_sum += x[6] * fir[k++];
	L_sum += x[7] * fir[k++];
	L_sum += x[8] * fir[k++];
	L_sum += x[9] * fir[k++];
	L_sum += x[10] * fir[k++];
	L_sum += x[11] * fir[k++];
	L_sum += x[12] * fir[k++];
	L_sum += x[13] * fir[k++];
	L_sum += x[14] * fir[k++];
	L_sum += x[15] * fir[k++];
	L_sum += x[16] * fir[k++];
	L_sum += x[17] * fir[k++];
	L_sum += x[18] * fir[k++];
	L_sum += x[19] * fir[k++];
	L_sum += x[20] * fir[k++];
	L_sum += x[21] * fir[k++];
	L_sum += x[22] * fir[k++];
	L_sum += x[23] * fir[k];
	}
	L_sum = L_shl2(L_sum, 2);               /* saturation can occur here */

	return (vo_round(L_sum));              //can't be modified

#endif
}
#endif
