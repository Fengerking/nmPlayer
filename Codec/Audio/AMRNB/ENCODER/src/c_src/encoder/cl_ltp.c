/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : cl_ltp.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "cl_ltp.h"
const char cl_ltp_id[] = "@(#)$Id $" cl_ltp_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "convolve.h"
#include "g_pitch.h"
#include "pred_lt.h"
#include "pitch_fr.h"
#include "enc_lag3.h"
#include "enc_lag6.h"
#include "q_gain_p.h"
#include "ton_stab.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   cl_ltp_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
Word16 div_s_v7 (Word16 var1, Word16 var2)
{
	int var_out;
	int iteration;
	Word32 L_num;
	Word32 L_denom;
	var_out = MAX_16;
	if (var1!= var2)//var1!= var2
	{
		var_out = 0;
		L_num = (Word32) var1;
		L_denom = (Word32) var2;
		for (iteration = 0; iteration < 15; iteration++)
		{
			var_out <<= 1;
			L_num <<= 1;
			if (L_num >= L_denom)
			{
				L_num -= L_denom;
				var_out +=1;
			}
		}
	}
	return (var_out);
}

int voAMRNBEnc_cl_ltp_init (clLtpState **state, VO_MEM_OPERATOR *pMemOP)
{
	clLtpState* s;
	if (state == (clLtpState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (clLtpState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(clLtpState), 32)) == NULL){
		return -1;
	}
	/* init the sub state */
	if (voAMRNBEnc_Pitch_fr_init(&s->pitchSt, pMemOP)) {
		voAMRNBEnc_cl_ltp_exit(&s, pMemOP);
		return -1;
	}
	voAMRNBEnc_cl_ltp_reset(s);
	*state = s;

	return 0;
}

/*************************************************************************
*
*  Function:   cl_ltp_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int voAMRNBEnc_cl_ltp_reset (clLtpState *state)
{
	if (state == (clLtpState *) NULL){
		return -1;
	}

	/* Reset pitch search states */
	voAMRNBEnc_Pitch_fr_reset (state->pitchSt);

	return 0;
}

/*************************************************************************
*
*  Function:   cl_ltp_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_cl_ltp_exit (clLtpState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;

	/* dealloc members */
	voAMRNBEnc_Pitch_fr_exit(&(*state)->pitchSt, pMemOP);

	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;

	return;
}

/*************************************************************************
*
*  Function:   cl_ltp
*  Purpose:    closed-loop fractional pitch search
*
**************************************************************************
*/
int voAMRNBEnc_cl_ltp (
			clLtpState *clSt,    /* i/o : State struct                              */
			tonStabState *tonSt, /* i/o : State struct                              */
			enum Mode mode,      /* i   : coder mode                                */
			Word16 frameOffset,  /* i   : Offset to subframe                        */
			Word16 T_op[],       /* i   : Open loop pitch lags                      */
			Word16 *h1,          /* i   : Impulse response vector               Q12 */
			Word16 *exc,         /* i/o : Excitation vector                      Q0 */
			Word16 res2[],       /* i/o : Long term prediction residual          Q0 */
			Word16 xn[],         /* i   : Target vector for pitch search         Q0 */
			Word16 lsp_flag,     /* i   : LSP resonance flag                        */
			Word16 xn2[],        /* o   : Target vector for codebook search      Q0 */
			Word16 y1[],         /* o   : Filtered adaptive excitation           Q0 */
			Word16 *T0,          /* o   : Pitch delay (integer part)                */
			Word16 *T0_frac,     /* o   : Pitch delay (fractional part)             */
			Word16 *gain_pit,    /* o   : Pitch gain                            Q14 */
			Word16 g_coeff[],    /* o   : Correlations between xn, y1, & y2         */
			Word16 **anap,       /* o   : Analysis parameters                       */
			Word16 *gp_limit     /* o   : pitch gain limit                          */
			)
{
	nativeInt i;
	nativeInt index;
	Word32 L_temp;     /* temporarily variable */
	nativeInt resu3;      /* flag for upsample resolution */
	nativeInt gpc_flag;
	/*----------------------------------------------------------------------*
	*                 Closed-loop fractional pitch search                  *
	*----------------------------------------------------------------------*/
	*T0 = voAMRNBEnc_Pitch_fr(clSt->pitchSt,mode, T_op, exc, xn, h1, L_SUBFR, frameOffset,T0_frac, (Word16*)&resu3, (Word16*)&index);
	*(*anap)++ = index;                              
	/*-----------------------------------------------------------------*
	*   - find unity gain pitch excitation (adapitve codebook entry)  *
	*     with fractional interpolation.                              *
	*   - find filtered pitch exc. y1[]=exc[] convolve with h1[])     *
	*   - compute pitch gain and limit between 0 and 1.2              *
	*   - update target vector for codebook search                    *
	*   - find LTP residual.                                          *
	*-----------------------------------------------------------------*/
#ifdef ARMv6_OPT
    Pred_lt_3or6_asm(exc, *T0, *T0_frac, L_SUBFR, resu3);
#else
	voAMRNBEnc_Pred_lt_3or6(exc, *T0, *T0_frac, L_SUBFR, resu3);
#endif

#ifdef ARMv6_OPT
	Convolve_asm(exc, h1, y1, L_SUBFR);
#else
	Convolve(exc, h1, y1, L_SUBFR);
#endif 

	/* gain_pit is Q14 for all modes */
#ifdef ARMv7_OPT
	*gain_pit = G_pitch_asm(mode, xn, y1, g_coeff, L_SUBFR);  
#else
	*gain_pit = voAMRNBEnc_G_pitch(mode, xn, y1, g_coeff, L_SUBFR);  
#endif
	/* check if the pitch gain should be limit due to resonance in LPC filter */
	gpc_flag = 0;                                       
	*gp_limit = MAX_16;                                  
	if ((lsp_flag != 0) && (*gain_pit > GP_CLIP))
	{
		gpc_flag = voAMRNBEnc_check_gp_clipping(tonSt, *gain_pit);
	}
	/* special for the MR475, MR515 mode; limit the gain to 0.85 to */
	/* cope with bit errors in the decoder in a better way.         */
	if (mode<= MR515) {

		if (*gain_pit > 13926) {
			*gain_pit = 13926;   /* 0.85 in Q14 */    
		}
		if (gpc_flag != 0) {
			*gp_limit = GP_CLIP;                    
		}
	}
	else
	{
		if (gpc_flag != 0)
		{
			*gp_limit = GP_CLIP;                  
			*gain_pit = GP_CLIP;                   
		}           
		/* For MR122, gain_pit is quantized here and not in gainQuant */
		if (mode == MR122)
		{
			*(*anap)++ = voAMRNBEnc_q_gain_pitch(MR122, *gp_limit, gain_pit, NULL, NULL);                                          
		}
	}

	for (i = 0; i < L_SUBFR; i++) {
		L_temp = y1[i]*(*gain_pit);
		L_temp = (L_temp << 2);
		xn2[i] = sub3(xn[i], extract_h(L_temp));    

		L_temp = exc[i]*(*gain_pit);
		L_temp = (L_temp << 2);
		res2[i] = sub3(res2[i], extract_h(L_temp));  
	}

	return 0;
}
