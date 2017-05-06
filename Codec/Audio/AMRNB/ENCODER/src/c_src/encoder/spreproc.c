/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : spreproc.c
*      Purpose          : Subframe preprocessing
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "spreproc.h"
const char spreproc_id[] = "@(#)$Id $" spreproc_h;
 
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
#include "weight_a.h"
#include "syn_filt.h"
#include "residu.h"
#include "copy.h"

/*********************************************************************************
*                         PUBLIC PROGRAM CODE
*********************************************************************************/
int voAMRNBEnc_subframePreProc(
	cod_amrState *amr_st,
    enum Mode mode,               /* i  : coder mode                            */
    Word16 *A,                    /* i  : A(z) unquantized for the 4 subframes  */
    Word16 *Aq,                   /* i  : A(z)   quantized for the 4 subframes  */
    Word16 i_subfr,
    Word16 xn[],                  /* o  : target vector for pitch search        */
    Word16 res2[]                 /* o  : long term prediction residual         */
)
{
   Word16 *mem_err = amr_st->mem_err;
   Word16 *mem_w0  = amr_st->mem_w0;
   Word16 *zero    = amr_st->zero;
   Word16 *ai_zero = amr_st->ai_zero;
   Word16 *h1      = amr_st->h1;
   Word16 *error   = amr_st->error;
   Word16 *speech  = &(amr_st->speech[i_subfr]);
   Word16 *exc     = &(amr_st->exc[i_subfr]);
   Word16 i;
   Word16 Ap1[MP1];               /* A(z) with spectral expansion         */
   Word16 Ap2[MP1];               /* A(z) with spectral expansion         */
   const  Word16 *g1;             /* Pointer to correct gammma1 vector    */
   /*---------------------------------------------------------------*
    * mode specific pointer to gamma1 values                        *
    *---------------------------------------------------------------*/
#if ONLY_ENCODE_122 
	g1 = gamma1_12k2; 
#else//ONLY_ENCODE_122 
	if ((mode == MR122)|| (mode == MR102))
    {
           g1 = gamma1_12k2; 
	}
    else
    {
           g1 = gamma1;      
	}
#endif//ONLY_ENCODE_122 
   /*---------------------------------------------------------------*
    * Find the weighted LPC coefficients for the weighting filter.  *
    *---------------------------------------------------------------*/
#ifdef ARMv6_OPT
   Vo_weight_ai(A, g1, Ap1);
   Vo_weight_ai(A, gamma2, Ap2);
#else
   voAMRNBEncWeight_Ai(A, g1, Ap1);
   voAMRNBEncWeight_Ai(A, gamma2, Ap2);
#endif
   
   /*---------------------------------------------------------------*
    * Compute impulse response, h1[], of weighted synthesis filter  *
    *---------------------------------------------------------------*/
   for (i = 0; i <= M; i++)
   {
      ai_zero[i] = Ap1[i];        
   }
#ifdef ARMv5_OPT
   Syn_filt_0(Aq, ai_zero, h1, zero);
   Syn_filt_0(Ap2, h1, h1, zero);
#else
   voAMRNBEnc_Syn_filt(Aq, ai_zero, h1, L_SUBFR, zero, 0);
   voAMRNBEnc_Syn_filt(Ap2, h1, h1, L_SUBFR, zero, 0);
#endif
 
   /*------------------------------------------------------------------------*
    *                                                                        *
    *          Find the target vector for pitch search:                      *
    *                                                                        *
    *------------------------------------------------------------------------*/  
   /* LPC residual */
#ifdef ARM
   Vo_Residu_asm(Aq, speech, res2, L_SUBFR); 
#else
   voAMRNBEnc_Residu(Aq, speech, res2, L_SUBFR);
#endif

   Copy(res2, exc, L_SUBFR);

#ifdef ARMv5_OPT
   Syn_filt_0(Aq, exc, error, mem_err);
#else
   voAMRNBEnc_Syn_filt(Aq, exc, error, L_SUBFR, mem_err, 0);
#endif

#ifdef ARM
   Vo_Residu_asm(Ap1, error, xn, L_SUBFR);
#else
   voAMRNBEnc_Residu(Ap1, error, xn, L_SUBFR);
#endif

   /* target signal xn[]*/
#ifdef ARMv5_OPT
   Syn_filt_0(Ap2, xn, xn, mem_w0); 
#else
   voAMRNBEnc_Syn_filt(Ap2, xn, xn, L_SUBFR, mem_w0, 0); 
#endif

   return 0;
}
