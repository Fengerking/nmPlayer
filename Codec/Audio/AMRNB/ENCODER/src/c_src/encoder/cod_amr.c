/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : cod_amr.c
*      Purpose          : Main encoder routine operating on a frame basis.
*
*****************************************************************************
*/
#include "cod_amr.h"
const char cod_amr_id[] = "@(#)$Id $" cod_amr_h;

/******************************************************************************
*                         INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"
#include "set_zero.h"
#include "qua_gain.h"
#include "lpc.h"
#include "lsp.h"
#include "pre_big.h"
#include "ol_ltp.h"
#include "p_ol_wgh.h"
#include "spreproc.h"
#include "cl_ltp.h"
#include "pred_lt.h"
#include "spstproc.h"
#include "cbsearch.h"
#include "gain_q.h"
#include "copy.h"
#include "convolve.h"
#include "ton_stab.h"
#include "vad.h"
#include "dtx_enc.h"


/******************************************************************************
*                         PUBLIC PROGRAM CODE
******************************************************************************/
/***************************************************************************
*
*  Function    : cod_amr_init
*  Purpose     : Allocates memory and initializes state variables
*
***************************************************************************/
int voAMRNBEnc_cod_amr_init (cod_amrState **state, Flag dtx, VO_MEM_OPERATOR *pMemOP)
{
	cod_amrState* s;
	if (state == (cod_amrState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (cod_amrState *) voAMRNBEnc_mem_malloc(pMemOP, sizeof(cod_amrState), 32)) == NULL){
		return -1;
	}
	s->lpcSt = NULL;
	s->lspSt = NULL;
	s->clLtpSt = NULL;
	s->gainQuantSt = NULL;
	s->pitchOLWghtSt = NULL;
	s->tonStabSt = NULL;    
	s->vadSt = NULL;
	s->dtx_encSt = NULL;
	s->dtx = dtx;
	s->last_pmax = 19;

	/* Init sub states */
	if (voAMRNBEnc_cl_ltp_init(&s->clLtpSt, pMemOP) || voAMRNBEnc_lsp_init(&s->lspSt, pMemOP) ||
		voAMRNBEnc_gainQuant_init(&s->gainQuantSt, pMemOP) || voAMRNBEnc_p_ol_wgh_init(&s->pitchOLWghtSt, pMemOP) ||
		voAMRNBEnc_ton_stab_init(&s->tonStabSt, pMemOP) ||      
#ifndef VAD2
		voAMRNBEnc_vad1_init(&s->vadSt, pMemOP) ||
#else
		vad2_init(&s->vadSt, pMemOP) ||
#endif
		dtx_enc_init(&s->dtx_encSt, pMemOP) || voAMRNBEnc_lpc_init(&s->lpcSt, pMemOP)) {
			voAMRNBEnc_cod_amr_exit(&s, pMemOP);
			return -1;
	}
	voAMRNBEnc_cod_amr_reset(s);
	*state = s;
	return 0;
}

/***************************************************************************
*
*  Function    : voAMRNBEnc_cod_amr_reset
*  Purpose     : Resets state memory
*
***************************************************************************/

int voAMRNBEnc_cod_amr_reset (cod_amrState *st)
{
	Word16 i;    
	if (st == (cod_amrState *) NULL){
		return -1;
	} 
	/*-----------------------------------------------------------------------*
	*          Initialize pointers to speech vector.                        *
	*-----------------------------------------------------------------------*/    
	st->new_speech = st->old_speech + L_TOTAL - L_FRAME;   /* New speech     */ 
	st->speech = st->new_speech - L_NEXT;                  /* Present frame  */  
	st->p_window = st->old_speech + L_TOTAL - L_WINDOW;    /* For LPC window */
	st->p_window_12k2 = st->p_window - L_NEXT; /* EFR LPC window: no lookahead */

	/* Initialize static pointers */
	st->wsp = st->old_wsp + PIT_MAX;
	st->exc = st->old_exc + PIT_MAX + L_INTERPOL;
	st->zero = st->ai_zero + MP1;
	st->error = st->mem_err + M;
	st->h1 = &st->hvec[L_SUBFR];

	/* Static vectors to zero */
	Set_zero(st->old_speech, L_TOTAL);
	Set_zero(st->old_exc,    PIT_MAX + L_INTERPOL);
	Set_zero(st->old_wsp,    PIT_MAX);
	Set_zero(st->mem_syn,    M);
	Set_zero(st->mem_w,      M);
	Set_zero(st->mem_w0,     M);
	Set_zero(st->mem_err,    M);
	Set_zero(st->zero,       L_SUBFR);
	Set_zero(st->hvec,       L_SUBFR);    /* set to zero "h1[-L_SUBFR..-1]" */

	/* OL LTP states */
	for (i = 0; i < 5; i++)
	{
		st->old_lags[i] = 40; 
	}
	/* Reset lpc states */
	voAMRNBEnc_lpc_reset(st->lpcSt);
	/* Reset lsp states */
	voAMRNBEnc_lsp_reset(st->lspSt);
	/* Reset clLtp states */
	voAMRNBEnc_cl_ltp_reset(st->clLtpSt);
	voAMRNBEnc_gainQuant_reset(st->gainQuantSt);
	voAMRNBEnc_p_ol_wgh_reset(st->pitchOLWghtSt);
	voAMRNBEnc_ton_stab_reset(st->tonStabSt);   
#ifndef VAD2
	voAMRNBEnc_vad1_reset(st->vadSt);
#else
	vad2_reset(st->vadSt);
#endif  
	dtx_enc_reset(st->dtx_encSt);
	st->sharp = SHARPMIN;
	return 0;
}

/*
**************************************************************************
*
*  Function    : voAMRNBEnc_cod_amr_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_cod_amr_exit (cod_amrState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* dealloc members */
	voAMRNBEnc_lpc_exit(&(*state)->lpcSt, pMemOP);
	voAMRNBEnc_lsp_exit(&(*state)->lspSt, pMemOP);
	voAMRNBEnc_gainQuant_exit(&(*state)->gainQuantSt, pMemOP);
	voAMRNBEnc_cl_ltp_exit(&(*state)->clLtpSt, pMemOP);
	voAMRNBEnc_p_ol_wgh_exit(&(*state)->pitchOLWghtSt, pMemOP);
	voAMRNBEnc_ton_stab_exit(&(*state)->tonStabSt, pMemOP);
#ifndef VAD2
	voAMRNBEnc_vad1_exit(&(*state)->vadSt, pMemOP);
#else
	vad2_exit(&(*state)->vadSt, pMemOP);
#endif 
	dtx_enc_exit(&(*state)->dtx_encSt, pMemOP);
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;

	return;
}

/***************************************************************************
*   FUNCTION:   voAMRNBEnc_cod_amr_first
*
*   PURPOSE:  Copes with look-ahead.
*
*   INPUTS:
*       No input argument are passed to this function. However, before
*       calling this function, 40 new speech data should be copied to the
*       vector new_speech[]. This is a global pointer which is declared in
*       this file (it points to the end of speech buffer minus 200).
*
***************************************************************************/

int voAMRNBEnc_cod_amr_first(cod_amrState *st,     /* i/o : State struct           */
				  Word16 new_speech[])  /* i   : speech input (L_FRAME) */
{ 
	Copy(new_speech,&st->new_speech[-L_NEXT], L_NEXT);
	/*   Copy(new_speech,st->new_speech,L_FRAME); */
	return 0;
}


/***************************************************************************
*   FUNCTION: cod_amr
*
*   PURPOSE:  Main encoder routine.
*
*   DESCRIPTION: This function is called every 20 ms speech frame,
*       operating on the newly read 160 speech samples. It performs the
*       principle encoding functions to produce the set of encoded parameters
*       which include the LSP, adaptive codebook, and fixed codebook
*       quantization indices (addresses and gains).
*
*   INPUTS:
*       No input argument are passed to this function. However, before
*       calling this function, 160 new speech data should be copied to the
*       vector new_speech[]. This is a global pointer which is declared in
*       this file (it points to the end of speech buffer minus 160).
*
*   OUTPUTS:
*
*       ana[]:     vector of analysis parameters.
*       synth[]:   Local synthesis speech (for debugging purposes)
*
***************************************************************************/

int voAMRNBEnc_cod_amr(
			cod_amrState *st,          /* i/o : State struct                   */
			enum Mode mode,            /* i   : AMR mode                       */
			Word16 new_speech[],       /* i   : speech input (L_FRAME)         */
			Word16 ana[],              /* o   : Analysis parameters            */
			enum Mode *usedMode,       /* o   : used mode                    */
			Word16 synth[]             /* o   : Local synthesis                */
)
{  
/* LPC coefficients */
    Word16 A_t[(MP1) * 4];      /* A(z) unquantized for the 4 subframes */
    Word16 Aq_t[(MP1) * 4];     /* A(z)   quantized for the 4 subframes */
    Word16 *A, *Aq;             /* Pointer on A_t and Aq_t              */
    Word16 lsp_new[M];

/* Other vectors */
    Word16 xn[L_SUBFR];         /* Target vector for pitch search       */
    Word16 xn2[L_SUBFR];        /* Target vector for codebook search    */
    Word16 code[L_SUBFR];       /* Fixed codebook excitation            */
    Word16 y1[L_SUBFR];         /* Filtered adaptive excitation         */
    Word16 y2[L_SUBFR];         /* Filtered fixed codebook excitation   */
    Word16 gCoeff[6];           /* Correlations between xn, y1, & y2:   */
    Word16 res[L_SUBFR];        /* Short term (LPC) prediction residual */
    Word16 res2[L_SUBFR];       /* Long term (LTP) prediction residual  */

/* Vector and scalars needed for the MR475 */
    Word16 xn_sf0[L_SUBFR];     /* Target vector for pitch search       */
    Word16 y2_sf0[L_SUBFR];     /* Filtered codebook innovation         */   
    Word16 code_sf0[L_SUBFR];   /* Fixed codebook excitation            */
    Word16 h1_sf0[L_SUBFR];     /* The impulse response of sf0          */
    Word16 T_op[L_FRAME/L_FRAME_BY2];
    Word16 mem_syn_save[M];     /* Filter memory                        */
    Word16 mem_w0_save[M];      /* Filter memory                        */
	Word16 mem_err_save[M] = {0};     /* Filter memory                        */

	Word16 sharp_save;          /* Sharpening                           */
	Word16 evenSubfr;           /* Even subframe indicator              */ 
	Word16 T0_sf0 = 0;          /* Integer pitch lag of sf0             */  
	Word16 T0_frac_sf0 = 0;     /* Fractional pitch lag of sf0          */  
	Word16 i_subfr_sf0 = 0;     /* Position in exc[] for sf0            */
	Word16 gain_pit_sf0;        /* Quantized pitch gain for sf0         */
	Word16 gain_code_sf0;       /* Quantized codebook gain for sf0      */  
	Word32 i_subfr, subfrNr;    /* Scalars */
	Word16 T0, T0_frac;
	Word16 gain_pit, gain_code; /* Flags */
	Word16 lsp_flag = 0;        /* indicates resonance in LPC filter */   
	Word16 gp_limit;            /* pitch gain limit value            */
	Word16 compute_sid_flag;    /* SID analysis  flag                 */

#if !DISABLE_DTX
	Word16 vad_flag;            /* VAD decision flag                 */
#endif

	Copy(new_speech, st->new_speech, L_FRAME);
	*usedMode = mode;   

#if !DISABLE_DTX
	/* DTX processing */
	if (st->dtx)
	{  /* no test() call since this if is only in simulation env */
		/* Find VAD decision */
#ifdef  VAD2
		vad_flag = vad2 (st->new_speech,    st->vadSt);
		vad_flag = vad2 (st->new_speech+80, st->vadSt) || vad_flag;      
#else//VAD2
		vad_flag = voAMRNBEnc_vad1(st->vadSt, st->new_speech);     
#endif//VAD2
		/* NB! usedMode may change here */
		compute_sid_flag = tx_dtx_handler(st->dtx_encSt,vad_flag, usedMode);                                  
		//printf("DTX processing \n");
	}
	else //!DISABLE_DTX
#endif//!DISABLE_DTX
	{
		compute_sid_flag = 0;              
	}  
	/*------------------------------------------------------------------------*
	*  - Perform LPC analysis:                                               *
	*       * autocorrelation + lag windowing                                *
	*       * Levinson-durbin algorithm to find a[]                          *
	*       * convert a[] to lsp[]                                           *
	*       * quantize and code the LSPs                                     *
	*       * find the interpolated LSPs and convert to a[] for all          *
	*         subframes (both quantized and unquantized)                     *
	*------------------------------------------------------------------------*/
	/* LP analysis */
	voAMRNBEnc_lpc(st, mode, A_t);
	/* From A(z) to lsp. LSP quantization and interpolation */
	voAMRNBEnc_lsp(st->lspSt, mode, *usedMode, A_t, Aq_t, lsp_new, &ana); 
	/* Buffer lsp's and energy */
	dtx_buffer(st->dtx_encSt,lsp_new,st->new_speech);

#if !DISABLE_DTX
	/* Check if in DTX mode */
	if(*usedMode == MRDTX)
	{
		dtx_enc(st->dtx_encSt,
			compute_sid_flag,
			st->lspSt->qSt, 
			st->gainQuantSt->gc_predSt,
			&ana);     
		Set_zero(st->old_exc,    PIT_MAX + L_INTERPOL);
		Set_zero(st->mem_w0,     M);
		Set_zero(st->mem_err,    M);
		Set_zero(st->zero,       L_SUBFR);
		Set_zero(st->hvec,       L_SUBFR);    /* set to zero "h1[-L_SUBFR..-1]" */
		/* Reset lsp states */
		voAMRNBEnc_lsp_reset(st->lspSt);
		Copy(lsp_new, st->lspSt->lsp_old, M);
		Copy(lsp_new, st->lspSt->lsp_old_q, M);

		/* Reset clLtp states */
		voAMRNBEnc_cl_ltp_reset(st->clLtpSt);
		st->sharp = SHARPMIN;       
	}
	else
#endif//!DISABLE_DTX
	{
		/* check resonance in the filter */
		lsp_flag = voAMRNBEnc_check_lsp(st->tonStabSt, st->lspSt->lsp_old);  
	}
	/*----------------------------------------------------------------------*
	* - Find the weighted input speech w_sp[] for the whole speech frame   *
	* - Find the open-loop pitch delay for first 2 subframes               *
	* - Set the range for searching closed-loop pitch in 1st subframe      *
	* - Find the open-loop pitch delay for last 2 subframes                *
	*----------------------------------------------------------------------*/
#if !DISABLE_DTX
#ifdef VAD2
	if (st->dtx)
	{  /* no test() call since this if is only in simulation env */
		st->vadSt->L_Rmax = 0;			
		st->vadSt->L_R0 = 0;			
	}
#endif
#endif//!DISABLE_DTX
	for(subfrNr = 0, i_subfr = 0; subfrNr < 2; subfrNr++, i_subfr += L_FRAME_BY2)
	{
		/* Pre-processing on 80 samples */
		voAMRNBEnc_pre_big(mode, A_t, i_subfr, st); 
		if (mode > MR515)
		{
			/* Find open loop pitch lag for two subframes */
			voAMRNBEnc_ol_ltp(st, mode, &st->wsp[i_subfr],&T_op[subfrNr],subfrNr);
		}
	}
	if (mode <= MR515)
	{
		/* Find open loop pitch lag for ONE FRAME ONLY */ /* search on 160 samples */ 
		voAMRNBEnc_ol_ltp(st, mode, &st->wsp[0],&T_op[0], 1);
		T_op[1] = T_op[0];                                    
	}         
#if !DISABLE_DTX   
#ifdef VAD2
	if (st->dtx)
	{  /* no test() call since this if is only in simulation env */
		voAMRNBEnc_LTP_flag_update(st->vadSt, mode);
	}
#endif
#ifndef VAD2
	/* run VAD pitch detection */
	if (st->dtx)
	{  /* no test() call since this if is only in simulation env */
		voAMRNBEnc_vad_pitch_detection(st->vadSt, T_op);
	} 
#endif
	if(*usedMode == MRDTX)
	{
		goto the_end;
	}
#endif //#if !DISABLE_DTX   

	/*------------------------------------------------------------------------*
	*          Loop for every subframe in the analysis frame                 *
	*------------------------------------------------------------------------*
	*  To find the pitch and innovation parameters. The subframe size is     *
	*  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.               *
	*     - find the weighted LPC coefficients                               *
	*     - find the LPC residual signal res[]                               *
	*     - compute the target signal for pitch search                       *
	*     - compute impulse response of weighted synthesis filter (h1[])     *
	*     - find the closed-loop pitch parameters                            *
	*     - encode the pitch dealy                                           *
	*     - update the impulse response h1[] by including fixed-gain pitch   *
	*     - find target vector for codebook search                           *
	*     - codebook search                                                  *
	*     - encode codebook address                                          *
	*     - VQ of pitch and codebook gains                                   *
	*     - find synthesis speech                                            *
	*     - update states of weighting filter                                *
	*------------------------------------------------------------------------*/
	A  = A_t;      /* pointer to interpolated LPC parameters */
	Aq = Aq_t;     /* pointer to interpolated quantized LPC parameters */
	evenSubfr = 0;                                                  
	subfrNr   = -1;                                                   
	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		subfrNr = (subfrNr + 1);
		evenSubfr = (1 - evenSubfr);
		/* Save states for the MR475 mode */ 
		if ((evenSubfr != 0) && ((*usedMode == MR475)))
		{
			Copy(st->mem_syn, mem_syn_save, M);
			Copy(st->mem_w0, mem_w0_save, M);         
			Copy(st->mem_err, mem_err_save, M);         
			sharp_save = st->sharp;
		}

		/*-----------------------------------------------------------------*
		* - Preprocessing of subframe                                     *
		*-----------------------------------------------------------------*/
#if !ONLY_ENCODE_122      
		if(*usedMode != MR475)
		{
			voAMRNBEnc_subframePreProc(st, *usedMode, A, Aq, i_subfr, xn, res);
		}
		else
		{ /* MR475 */
			voAMRNBEnc_subframePreProc(st, *usedMode,A, Aq, i_subfr, xn, res);
			/* save impulse response (modified in cbsearch) */ 
			if (evenSubfr != 0)
			{
				Copy (st->h1, h1_sf0, L_SUBFR);
			}
		}
#else// if !ONLY_ENCODE_122  
		voAMRNBEnc_subframePreProc(st, *usedMode, A, Aq, i_subfr, xn, res);
#endif// if !ONLY_ENCODE_122   
		/* copy the LP residual (res2 is modified in the CL LTP search)    */
		Copy(res, res2, L_SUBFR);
		/*-----------------------------------------------------------------*
		* - Closed-loop LTP search                                        *
		*-----------------------------------------------------------------*/
		voAMRNBEnc_cl_ltp(st->clLtpSt, st->tonStabSt, *usedMode, i_subfr, T_op, st->h1, 
			&st->exc[i_subfr], res2, xn, lsp_flag, xn2, y1, 
			&T0, &T0_frac, &gain_pit, gCoeff, &ana, &gp_limit);

		/* update LTP lag history */
		if ((subfrNr == 0) && (st->ol_gain_flg[0] > 0))
		{
			st->old_lags[1] = T0;     
		}
		if ((subfrNr == 3) && (st->ol_gain_flg[1] > 0))
		{
			st->old_lags[0] = T0;     
		}        
		/*-----------------------------------------------------------------*
		* - Inovative codebook search (find index and gain)               *
		*-----------------------------------------------------------------*/
		voAMRNBEnc_cbsearch(xn2, st->h1, T0, st->sharp, gain_pit, res2, 
			code, y2, &ana, *usedMode, subfrNr);
		/*------------------------------------------------------*
		* - Quantization of gains.                             *
		*------------------------------------------------------*/
		voAMRNBEnc_gainQuant(st->gainQuantSt, *usedMode, res, &st->exc[i_subfr], code,
			xn, xn2,  y1, y2, gCoeff, evenSubfr, gp_limit, &gain_pit_sf0, &gain_code_sf0,
			&gain_pit, &gain_code, &ana);
		/* update gain history */
		voAMRNBEnc_update_gp_clipping(st->tonStabSt, gain_pit);
#if !ONLY_ENCODE_122
		if (*usedMode != MR475)
		{
			/* Subframe Post Porcessing */
			voAMRNBEnc_subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
				gain_code, Aq, synth, xn, code, y1, y2, st->mem_syn,
				st->mem_err, st->mem_w0, st->exc, &st->sharp);
		}
		else
		{
			if (evenSubfr != 0)
			{
				i_subfr_sf0 = i_subfr; 
				Copy(xn, xn_sf0, L_SUBFR);
				Copy(y2, y2_sf0, L_SUBFR);          
				Copy(code, code_sf0, L_SUBFR);
				T0_sf0 = T0; 
				T0_frac_sf0 = T0_frac;       
				/* Subframe Post Porcessing */
				voAMRNBEnc_subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
					gain_code, Aq, synth, xn, code, y1, y2, mem_syn_save, st->mem_err, 
					mem_w0_save, st->exc, &st->sharp);
				st->sharp = sharp_save; 
			}
			else
			{
				/* update both subframes for the MR475 */  
				/* Restore states for the MR475 mode */
				Copy(mem_err_save, st->mem_err, M);         
				/* re-build excitation for sf 0 */
#ifdef ARMv6_OPT //have error
				Pred_lt_3or6_asm(&st->exc[i_subfr_sf0], T0_sf0, T0_frac_sf0,L_SUBFR, 1);
#else
				voAMRNBEnc_Pred_lt_3or6(&st->exc[i_subfr_sf0], T0_sf0, T0_frac_sf0,L_SUBFR, 1);
#endif

#ifdef ARMv6_OPT
				Convolve_asm(&st->exc[i_subfr_sf0], h1_sf0, y1, L_SUBFR);
#else
				Convolve(&st->exc[i_subfr_sf0], h1_sf0, y1, L_SUBFR);
#endif

				Aq -= MP1;
				voAMRNBEnc_subframePostProc(st->speech, *usedMode, i_subfr_sf0,
					gain_pit_sf0, gain_code_sf0, Aq,
					synth, xn_sf0, code_sf0, y1, y2_sf0,
					st->mem_syn, st->mem_err, st->mem_w0, st->exc,
					&sharp_save); /* overwrites sharp_save */
				Aq += MP1;

				/* re-run pre-processing to get xn right (needed by postproc) */
				/* (this also reconstructs the unsharpened h1 for sf 1)       */

				voAMRNBEnc_subframePreProc(st, *usedMode, A, Aq, i_subfr, xn, res);
				/* re-build excitation sf 1 (changed if lag < L_SUBFR) */
#ifdef ARMv6_OPT //have error
				Pred_lt_3or6_asm(&st->exc[i_subfr], T0, T0_frac, L_SUBFR, 1);
#else
				voAMRNBEnc_Pred_lt_3or6(&st->exc[i_subfr], T0, T0_frac, L_SUBFR, 1);
#endif

#ifdef ARMv6_OPT
				Convolve_asm(&st->exc[i_subfr], st->h1, y1, L_SUBFR);
#else
				Convolve(&st->exc[i_subfr], st->h1, y1, L_SUBFR);
#endif 

				voAMRNBEnc_subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
					gain_code, Aq, synth, xn, code, y1, y2,
					st->mem_syn, st->mem_err, st->mem_w0,
					st->exc, &st->sharp);
			}
		}      
#else//if !ONLY_ENCODE_122
		voAMRNBEnc_subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
			gain_code, Aq, synth, xn, code, y1, y2, st->mem_syn,
			st->mem_err, st->mem_w0, st->exc, &st->sharp);
#endif//if !ONLY_ENCODE_122    
		A += MP1;    /* interpolated LPC parameters for next subframe */
		Aq += MP1;
	}
	Copy(&st->old_exc[L_FRAME], &st->old_exc[0], PIT_MAX + L_INTERPOL);  

the_end:  
	/*--------------------------------------------------*
	* Update signal for next frame.                    *
	*--------------------------------------------------*/
	Copy(&st->old_wsp[L_FRAME], &st->old_wsp[0], PIT_MAX);      
	Copy(&st->old_speech[L_FRAME], &st->old_speech[0], L_TOTAL - L_FRAME);         
	return 0;
}

