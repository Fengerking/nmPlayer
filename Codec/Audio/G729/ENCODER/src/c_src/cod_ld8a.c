/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-----------------------------------------------------------------*
 *   Functions Coder_ld8a and Init_Coder_ld8a                      *
 *             ~~~~~~~~~~     ~~~~~~~~~~~~~~~                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *                                                                 *
 *                                                                 *
 *  Coder_ld8a(Word16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 speech data should have beee copy to vector new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "ld8a.h")        *
 *-----------------------------------------------------------*
 *   L_WINDOW    : LPC analysis window size.                 *
 *   L_NEXT      : Samples of next frame needed for autocor. *
 *   L_FRAME     : Frame size.                               *
 *   L_SUBFR     : Sub-frame size.                           *
 *   M           : LPC order.                                *
 *   MP1         : LPC order+1                               *
 *   L_TOTAL     : Total size of speech buffer.              *
 *   PIT_MIN     : Minimum pitch lag.                        *
 *   PIT_MAX     : Maximum pitch lag.                        *
 *   L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/
/* Speech vector */
static Word16 old_speech[L_TOTAL];
static Word16 *speech, *p_window;
Word16 *new_speech;                    /* Global variable */

/* Weighted speech vector */
static Word16 old_wsp[L_FRAME+PIT_MAX];
static Word16 *wsp;

/* Excitation vector */
static Word16 old_exc[L_FRAME+PIT_MAX+L_INTERPOL];
static Word16 *exc;

/* Lsp (Line spectral pairs) */

static Word16 lsp_old[M]={
              30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};
static Word16 lsp_old_q[M];

/* Filter's memory */
static Word16  mem_w0[M], mem_w[M], mem_zero[M];
static Word16  sharp;

/*-----------------------------------------------------------------*
 *   Function  Init_Coder_ld8a                                     *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *       - initialize pointers to speech buffer                    *
 *       - initialize static  pointers                             *
 *       - set static vectors to zero                              *
 *                                                                 *
 *-----------------------------------------------------------------*/

void Init_Coder_ld8a(void)
{
	/*----------------------------------------------------------------------*
	 *      Initialize pointers to speech vector.                            *
	 *                                                                       *
	 *                                                                       *
	 *   |--------------------|-------------|-------------|------------|     *
	 *     previous speech           sf1           sf2         L_NEXT        *
	 *                                                                       *
	 *   <----------------  Total speech vector (L_TOTAL)   ----------->     *
	 *   <----------------  LPC analysis window (L_WINDOW)  ----------->     *
	 *   |                   <-- present frame (L_FRAME) -->                 *
	 * old_speech            |              <-- new speech (L_FRAME) -->     *
	 * p_window              |              |                                *
	 *                     speech           |                                *
	 *                             new_speech                                *
	 *-----------------------------------------------------------------------*/
	new_speech = old_speech + L_TOTAL - L_FRAME;         /* New speech     */
	speech     = new_speech - L_NEXT;                    /* Present frame  */
	p_window   = old_speech + L_TOTAL - L_WINDOW;        /* For LPC window */

	/* Initialize static pointers */
	wsp    = old_wsp + PIT_MAX;
	exc    = old_exc + PIT_MAX + L_INTERPOL;
	/* Static vectors to zero */
	Set_zero(old_speech, L_TOTAL);
	Set_zero(old_exc, PIT_MAX+L_INTERPOL);
	Set_zero(old_wsp, PIT_MAX);
	Set_zero(mem_w,   M);
	Set_zero(mem_w0,  M);
	Set_zero(mem_zero, M);
	sharp = SHARPMIN;

	/* Initialize lsp_old_q[] */
	memcpy(lsp_old_q,lsp_old,2*M);
	//Copy(lsp_old, lsp_old_q, M);
	Lsp_encw_reset();
	Init_exc_err();

	return;
}

/*-----------------------------------------------------------------*
 *   Functions Coder_ld8a                                          *
 *            ~~~~~~~~~~                                           *
 *  Coder_ld8a(Word16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 speech data should have beee copy to vector new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/

void Coder_ld8a(
     Word16 ana[]       /* output  : Analysis parameters */
)
{
	// static Word16 number;
	/* LPC analysis */
	Word16 Aq_t[(MP1)*2];         /* A(z)   quantized for the 2 subframes */
	Word16 Ap_t[(MP1)*2];         /* A(z/gamma)       for the 2 subframes */
	Word16 *Aq, *Ap;              /* Pointer on Aq_t and Ap_t             */
	/* Other vectors */
	Word16 h1[L_SUBFR];            /* Impulse response h1[]              */
	Word16 xn[L_SUBFR];            /* Target vector for pitch search     */
	Word16 xn2[L_SUBFR];           /* Target vector for codebook search  */
	Word16 code[L_SUBFR];          /* Fixed codebook excitation          */
	Word16 y1[L_SUBFR];            /* Filtered adaptive excitation       */
	Word16 y2[L_SUBFR];            /* Filtered fixed codebook excitation */
	Word16 g_coeff[4];             /* Correlations between xn & y1       */
	Word16 g_coeff_cs[5];
	Word16 exp_g_coeff_cs[5];      /* Correlations between xn, y1, & y2
					  <y1,y1>, -2<xn,y1>,
					  <y2,y2>, -2<xn,y2>, 2<y1,y2> */
	/* Scalars */
	Word16 i, j, k, i_subfr;
	Word16 T_op, T0, T0_min, T0_max, T0_frac;
	Word16 gain_pit, gain_code, index;
	Word16 temp, taming;
	Word32 L_temp;

	/*------------------------------------------------------------------------*
	 *  - Perform LPC analysis:                                               *
	 *       * autocorrelation + lag windowing                                *
	 *       * Levinson-durbin algorithm to find a[]                          *
	 *       * convert a[] to lsp[]                                           *
	 *       * quantize and code the LSPs                                     *
	 *       * find the interpolated LSPs and convert to a[] for the 2        *
	 *         subframes (both quantized and unquantized)                     *
	 *------------------------------------------------------------------------*/
	{
		/* Temporary vectors */
		Word16 r_l[MP1], r_h[MP1];       /* Autocorrelations low and hi          */
		Word16 rc[M];                    /* Reflection coefficients.             */
		Word16 lsp_new[M], lsp_new_q[M]; /* LSPs at 2th subframe                 */
#ifdef ASM_OPT // asm optimization
		Autocorr_asm(p_window, M, r_h, r_l);              /* Autocorrelations */
#else
		Autocorr(p_window, M, r_h, r_l);              /* Autocorrelations */
#endif
		Lag_window(M, r_h, r_l);                      /* Lag windowing    */
		Levinson(r_h, r_l, Ap_t, rc);                 /* Levinson Durbin  */
		Az_lsp(Ap_t, lsp_new, lsp_old);               /* From A(z) to lsp */

		/* LSP quantization */
		Qua_lsp(lsp_new, lsp_new_q, ana);
		ana += 2;                         /* Advance analysis parameters pointer */

		/*--------------------------------------------------------------------*
		 * Find interpolated LPC parameters in all subframes                  *
		 * The interpolated parameters are in array Aq_t[].                   *
		 *--------------------------------------------------------------------*/
		Int_qlpc(lsp_old_q, lsp_new_q, Aq_t);

		/* Compute A(z/gamma) */
#ifdef  ASM_OPT //Wm
		Weight_Az_asm(&Aq_t[0],   GAMMA1, M, &Ap_t[0]);
		Weight_Az_asm(&Aq_t[MP1], GAMMA1, M, &Ap_t[MP1]);
#else
		Weight_Az(&Aq_t[0],   GAMMA1, M, &Ap_t[0]);
		Weight_Az(&Aq_t[MP1], GAMMA1, M, &Ap_t[MP1]);
#endif
		/* update the LSPs for the next frame */
		memcpy(lsp_old, lsp_new, 2*M);
		memcpy(lsp_old_q, lsp_new_q, 2*M);
	}

	/*----------------------------------------------------------------------*
	 * - Find the weighted input speech w_sp[] for the whole speech frame   *
	 * - Find the open-loop pitch delay                                     *
	 *----------------------------------------------------------------------*/
#ifdef ASM_OPT
	Residu_asm(&Aq_t[0], &speech[0], &exc[0], L_SUBFR);
	Residu_asm(&Aq_t[MP1], &speech[L_SUBFR], &exc[L_SUBFR], L_SUBFR);
#else
	Residu(&Aq_t[0], &speech[0], &exc[0], L_SUBFR);
	Residu(&Aq_t[MP1], &speech[L_SUBFR], &exc[L_SUBFR], L_SUBFR);
#endif

	{
		Word16 Ap1[MP1];
		Ap = Ap_t;
		Ap1[0] = 4096;
		for(i=M; i>=1; i--)    /* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
			Ap1[i] = sub(Ap[i], (Ap[i-1] * 22938)>>15);

#ifdef ASM_OPT
		Syn_filt4_1(Ap1, &exc[0], &wsp[0], mem_w);
#else
		Syn_filt(Ap1, &exc[0], &wsp[0], L_SUBFR, mem_w, 1);
#endif

		Ap += MP1;
		for(i=M; i>=1; i--)    /* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
			Ap1[i] = sub(Ap[i], (Ap[i-1] * 22938)>>15);

#ifdef ASM_OPT
		Syn_filt4_1(Ap1, &exc[L_SUBFR], &wsp[L_SUBFR], mem_w);
#else
		Syn_filt(Ap1, &exc[L_SUBFR], &wsp[L_SUBFR], L_SUBFR, mem_w, 1);
#endif
	}
	/* Find open loop pitch lag */

#ifdef ASM_OPT
	T_op= Pitch_ol_fast_asm(wsp, PIT_MAX, L_FRAME);
#else
	T_op = Pitch_ol_fast(wsp, PIT_MAX, L_FRAME);
#endif
	T0_min = T_op - 3;

	if (T0_min < PIT_MIN) {
		T0_min = PIT_MIN;
	}

	T0_max = T0_min + 6;
	if (T0_max  > PIT_MAX)
	{
		T0_max = PIT_MAX;
		T0_min = T0_max - 6;
	}
	/*------------------------------------------------------------------------*
	 *          Loop for every subframe in the analysis frame                 *
	 *------------------------------------------------------------------------*
	 *  To find the pitch and innovation parameters. The subframe size is     *
	 *  L_SUBFR and the loop is repeated 2 times.                             *
	 *     - find the weighted LPC coefficients                               *
	 *     - find the LPC residual signal res[]                               *
	 *     - compute the target signal for pitch search                       *
	 *     - compute impulse response of weighted synthesis filter (h1[])     *
	 *     - find the closed-loop pitch parameters                            *
	 *     - encode the pitch delay                                           *
	 *     - find target vector for codebook search                           *
	 *     - codebook search                                                  *
	 *     - VQ of pitch and codebook gains                                   *
	 *     - update states of weighting filter                                *
	 *------------------------------------------------------------------------*/
	Aq = Aq_t;    /* pointer to interpolated quantized LPC parameters */
	Ap = Ap_t;    /* pointer to weighted LPC coefficients             */

	for (i_subfr = 0;  i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		/*---------------------------------------------------------------*
		 * Compute impulse response, h1[], of weighted synthesis filter  *
		 *---------------------------------------------------------------*/
		h1[0] = 4096;
		Set_zero(&h1[1], L_SUBFR-1);

#ifdef ASM_OPT
		Syn_filt4_0(Ap, h1, h1, &h1[1]);
#else
		Syn_filt(Ap, h1, h1, L_SUBFR, &h1[1], 0);
#endif
		/*----------------------------------------------------------------------*
		 *  Find the target vector for pitch search:                            *
		 *----------------------------------------------------------------------*/
#ifdef ASM_OPT
		Syn_filt4_0(Ap, &exc[i_subfr], xn, mem_w0);
#else
		Syn_filt(Ap, &exc[i_subfr], xn, L_SUBFR, mem_w0, 0);
#endif
		/*---------------------------------------------------------------------*
		 *                 Closed-loop fractional pitch search                 *
		 *---------------------------------------------------------------------*/

		T0 = Pitch_fr3_fast(&exc[i_subfr], xn, h1, L_SUBFR, T0_min, T0_max,
				i_subfr, &T0_frac);

		index = Enc_lag3(T0, T0_frac, &T0_min, &T0_max,PIT_MIN,PIT_MAX,i_subfr);

		*ana++ = index;

		if (i_subfr == 0) {
			*ana++ = Parity_Pitch(index);
		}

		/*-----------------------------------------------------------------*
		 *   - find filtered pitch exc                                     *
		 *   - compute pitch gain and limit between 0 and 1.2              *
		 *   - update target vector for codebook search                    *
		 *-----------------------------------------------------------------*/
#ifdef ASM_OPT
		Syn_filt4_0(Ap, &exc[i_subfr], y1, mem_zero);
#else
		Syn_filt(Ap, &exc[i_subfr], y1, L_SUBFR, mem_zero, 0);
#endif

		gain_pit = G_pitch(xn, y1, g_coeff, L_SUBFR);
		/* clip pitch gain if taming is necessary */
		taming = test_err(T0, T0_frac);
		if( taming == 1){
			if (gain_pit > GPCLIP) {
				gain_pit = GPCLIP;
			}
		}
		/* xn2[i]   = xn[i] - y1[i] * gain_pit  */
		for (i = L_SUBFR-1; i >=0 ; i--)
		{
			L_temp = (y1[i] * gain_pit)<<1;
			L_temp = L_temp <<1;
			xn2[i] = sub(xn[i], extract_h(L_temp));
		}


		/*-----------------------------------------------------*
		 * - Innovative codebook search.                       *
		 *-----------------------------------------------------*/
		index = ACELP_Code_A(xn2, h1, T0, sharp, code, y2, &i);
		*ana++ = index;        /* Positions index */
		*ana++ = i;            /* Signs index     */
		/*-----------------------------------------------------*
		 * - Quantization of gains.                            *
		 *-----------------------------------------------------*/

		g_coeff_cs[0]     = g_coeff[0];            /* <y1,y1> */
		exp_g_coeff_cs[0] = negate(g_coeff[1]);    /* Q-Format:XXX -> JPN */
		g_coeff_cs[1]     = negate(g_coeff[2]);    /* (xn,y1) -> -2<xn,y1> */
		exp_g_coeff_cs[1] = negate(add(g_coeff[3], 1)); /* Q-Format:XXX -> JPN */

		Corr_xy2( xn, y1, y2, g_coeff_cs, exp_g_coeff_cs );  /* Q0 Q0 Q12 ^Qx ^Q0 */
		/* g_coeff_cs[3]:exp_g_coeff_cs[3] = <y2,y2>   */
		/* g_coeff_cs[4]:exp_g_coeff_cs[4] = -2<xn,y2> */
		/* g_coeff_cs[5]:exp_g_coeff_cs[5] = 2<y1,y2>  */

		*ana++ = Qua_gain(code, g_coeff_cs, exp_g_coeff_cs,
				L_SUBFR, &gain_pit, &gain_code, taming);


		/*------------------------------------------------------------*
		 * - Update pitch sharpening "sharp" with quantized gain_pit  *
		 *------------------------------------------------------------*/
		sharp = gain_pit;
		if (sharp > SHARPMAX)         { sharp = SHARPMAX;         }
		if (sharp < SHARPMIN)         { sharp = SHARPMIN;         }

		/*------------------------------------------------------*
		 * - Find the total excitation                          *
		 * - update filters memories for finding the target     *
		 *   vector in the next subframe                        *
		 *------------------------------------------------------*/
		for (i = L_SUBFR-1; i >=0 ;  i--)
		{
			/* exc[i] = gain_pit*exc[i] + gain_code*code[i]; */
			/* exc[i]  in Q0   gain_pit in Q14               */
			/* code[i] in Q13  gain_cod in Q1                */
			L_temp  = (exc[i+i_subfr] * gain_pit)<<1;
			L_temp += (code[i] * gain_code)<<1;
			L_temp = L_temp << 1;
			exc[i+i_subfr] = (Word16)((L_temp + 0x8000) >>16);
		}
		update_exc_err(gain_pit, T0);

		for (i = L_SUBFR-M, j = 0; i < L_SUBFR; i++, j++)
		{
			temp = ((y1[i] * gain_pit)<<2)>>16;
			k = ((y2[i] * gain_code)<<3)>>16;
			mem_w0[j] = xn[i] - (temp + k);

		}
		Aq += MP1;           /* interpolated LPC parameters for next subframe */
		Ap += MP1;
	}
	/*--------------------------------------------------*
	 * Update signal for next frame.                    *
	 * -> shift to the left by L_FRAME:                 *
	 *     speech[], wsp[] and  exc[]                   *
	 *--------------------------------------------------*/
	memcpy(&old_speech[0],&old_speech[L_FRAME],2*(L_TOTAL-L_FRAME));
	memcpy(&old_wsp[0],&old_wsp[L_FRAME],2*PIT_MAX);
	memcpy(&old_exc[0],&old_exc[L_FRAME],2*(PIT_MAX+L_INTERPOL));
	return;
}

