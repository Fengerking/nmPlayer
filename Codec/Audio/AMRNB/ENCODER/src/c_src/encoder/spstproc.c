/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : subframePostProc.c
*      Purpose          : Subframe post processing
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "spstproc.h"
const char spstproc_id[] = "@(#)$Id $" spstproc_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "syn_filt.h"
#include "cnst.h"

#ifdef ARMv7_OPT
void vo_postProcessEnc_asm(
						   Word16 *pInput, 
						   Word16 code[],
						   Word16 gain_code,
						   Word16 pitch_fac,
						   Word16 tempShift
						   );
#endif
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
int voAMRNBEnc_subframePostProc(
					 Word16 *speech,   /* i   : speech segment                        */
					 enum Mode mode,   /* i   : coder mode                            */
					 Word16 i_subfr,   /* i   : Subframe nr                           */
					 Word16 gain_pit,  /* i   : Pitch gain                       Q14  */
					 Word16 gain_code, /* i   : Decoded innovation gain               */
					 Word16 *Aq,       /* i   : A(z) quantized for the 4 subframes    */
					 Word16 synth[],   /* i   : Local snthesis                        */
					 Word16 xn[],      /* i   : Target vector for pitch search        */
					 Word16 code[],    /* i   : Fixed codebook exitation              */
					 Word16 y1[],      /* i   : Filtered adaptive exitation           */
					 Word16 y2[],      /* i   : Filtered fixed codebook excitation    */
					 Word16 *mem_syn,  /* i/o : memory of synthesis filter            */
					 Word16 *mem_err,  /* o   : pointer to error signal               */
					 Word16 *mem_w0,   /* o   : memory of weighting filter            */
					 Word16 *exc,      /* o   : long term prediction residual         */
					 Word16 *sharp     /* o   : pitch sharpening value                */
					 )
{
	Word32  i, j, k;
	Word16  temp;
	Word32  L_temp;
	Word16  tempShift;
	Word16  kShift;
	Word16  pitch_fac;

	L_temp = 0;
	if (mode != MR122)
	{
		tempShift = 1;                     
		kShift = 2;                        
		pitch_fac = gain_pit;              
	}
	else
	{
		tempShift = 2;                     
		kShift = 4;                        
		pitch_fac = (gain_pit>> 1);
	}
	/*------------------------------------------------------------*
	* - Update pitch sharpening "sharp" with quantized gain_pit  *
	*------------------------------------------------------------*/
	*sharp = gain_pit;                   
	if (*sharp > SHARPMAX)
	{
		*sharp = SHARPMAX;                
	}
	/*------------------------------------------------------*
	* - Find the total excitation                          *
	* - find synthesis speech corresponding to exc[]       *
	* - update filters memories for finding the target     *
	*   vector in the next subframe                        *
	*   (update error[-m..-1] and mem_w_err[])             *
	*------------------------------------------------------*/
#ifdef ARMv7_OPT
	vo_postProcessEnc_asm(&exc[i_subfr],code,gain_code,pitch_fac,tempShift);
#else
	for (i = L_SUBFR - 1; i >= 0 ; i--){
		L_temp = (exc[i + i_subfr] * pitch_fac)<<1;
		L_temp += (code[i] * gain_code)<<1;
		L_temp = L_shl2 (L_temp, tempShift);
		exc[i + i_subfr] = (L_temp + 0x8000)>>16;           
	}
#endif

#ifdef ARMv7_OPT
	Syn_filt_1(Aq, &exc[i_subfr], &synth[i_subfr], mem_syn);
#else
	voAMRNBEnc_Syn_filt(Aq, &exc[i_subfr], &synth[i_subfr], L_SUBFR, mem_syn, 1);
#endif

	for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++) {
		mem_err[j] = sub(speech[i_subfr + i],synth[i_subfr + i]);        
		temp = extract_h(L_shl2((y1[i] * gain_pit)<<1, 1));
		k = extract_h(L_shl2((y2[i] * gain_code)<<1, kShift));
		mem_w0[j] = sub(xn[i], add(temp, k));       
	}
	return 0;
}
