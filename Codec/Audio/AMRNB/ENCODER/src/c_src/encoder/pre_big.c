/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : pre_big.c
*      Purpose          : Big subframe (2 subframes) preprocessing
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pre_big.h"
const char pre_big_id[] = "@(#)$Id $" pre_big_h;

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
#include "weight_a.h"
#include "residu.h"
#include "cnst.h"

/********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************/
int voAMRNBEnc_pre_big(
			enum Mode mode,            /* i  : coder mode                             */
			Word16 A_t[],              /* i  : A(z) unquantized, for 4 subframes, Q12 */
			Word16 frameOffset,        /* i  : Start position in speech vector,   Q0  */
            cod_amrState *amr_st
)
{
	Word16 Ap1[MP1];            /* A(z) with spectral expansion         */
	Word16 Ap2[MP1];            /* A(z) with spectral expansion         */
    Word16 *speech = amr_st->speech;
	Word16 *mem_w  = amr_st->mem_w;
	Word16 *wsp    = amr_st->wsp;
	const Word16 *g1 = gamma1_12k2;           /* Pointer to correct gammma1 vector    */
	Word32 aOffset = 0;

#if !ONLY_ENCODE_122   
	if (mode <= MR795)
	{
		g1 = gamma1;                        
	}
#endif  
	if (frameOffset > 0) {
		aOffset = 22;                     
	}
	/* process two subframes (which form the "big" subframe) */
	//for (i = 0; i < 2; i++)
	{
#ifdef  ARMv6_OPT
		Vo_weight_ai(&A_t[aOffset], g1, Ap1);
		Vo_weight_ai(&A_t[aOffset], gamma2, Ap2);
#else
		voAMRNBEncWeight_Ai(&A_t[aOffset], g1, Ap1);
		voAMRNBEncWeight_Ai(&A_t[aOffset], gamma2, Ap2);
#endif

#ifdef ARM
		Vo_Residu_asm(Ap1, &speech[frameOffset], &wsp[frameOffset], L_SUBFR);
#else
		voAMRNBEnc_Residu(Ap1, &speech[frameOffset], &wsp[frameOffset], L_SUBFR);
#endif

#ifdef ARMv5_OPT
		Syn_filt_1(Ap2, &wsp[frameOffset], &wsp[frameOffset], mem_w);
#else
		voAMRNBEnc_Syn_filt(Ap2, &wsp[frameOffset], &wsp[frameOffset], L_SUBFR, mem_w, 1);
#endif
		aOffset = (aOffset+ MP1);
		frameOffset =  (frameOffset+ L_SUBFR);

#ifdef  ARMv6_OPT
		Vo_weight_ai(&A_t[aOffset], g1, Ap1);
		Vo_weight_ai(&A_t[aOffset], gamma2, Ap2);
#else
		voAMRNBEncWeight_Ai(&A_t[aOffset], g1, Ap1);
		voAMRNBEncWeight_Ai(&A_t[aOffset], gamma2, Ap2);
#endif 

#ifdef  ARM
		Vo_Residu_asm(Ap1, &speech[frameOffset], &wsp[frameOffset], L_SUBFR);
#else
		voAMRNBEnc_Residu(Ap1, &speech[frameOffset], &wsp[frameOffset], L_SUBFR);
#endif


#ifdef ARMv5_OPT
		Syn_filt_1(Ap2, &wsp[frameOffset], &wsp[frameOffset], mem_w);
#else
		voAMRNBEnc_Syn_filt(Ap2, &wsp[frameOffset], &wsp[frameOffset], L_SUBFR, mem_w, 1);
#endif

		aOffset = (aOffset+ MP1);
		frameOffset = (frameOffset+ L_SUBFR);
	}   
	return 0;
}
