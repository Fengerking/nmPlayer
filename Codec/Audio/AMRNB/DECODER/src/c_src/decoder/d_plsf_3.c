/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : d_plsf_3.c
*      Purpose          : Decodes the LSP parameters using the received
*                         quantization indices. 1st order MA prediction and
*                         split by 3 vector quantization (split-VQ)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d_plsf.h"
const char d_plsf_3_id[] = "@(#)$Id $" d_plsf_h;


/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "lsp_lsf.h"
#include "reorder.h"
#include "copy.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "q_plsf_3.tab"    /* Codebooks of LSF prediction residual */

#define ALPHA     29491       /* ALPHA    ->  0.9  */
#define ONE_ALPHA 3277        /* ONE_ALPHA-> (1.0-ALPHA)  */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:   D_plsf_3()
*
*  PURPOSE: Decodes the LSP parameters using the received quantization
*           indices.1st order MA prediction and split by 3 vector
*           quantization (split-VQ)
*
*************************************************************************/
void D_plsf_3(
			  D_plsfState *st,   /* i/o: State struct                               */
			  enum Mode mode,    /* i  : coder mode                                 */
			  Word16 bfi,        /* i  : bad frame indicator (set to 1 if a bad frame is received)  */
			  Word16 * indice,   /* i  : quantization indices of 3 submatrices, Q0  */
			  Word16 * lsp1_q    /* o  : quantized 1st LSP vector,              Q15 */
			  )
{
	int    i, index;
	const  Word16 *p_cb1, *p_cb2, *p_cb3, *p_dico;
	Word16 lsf1_r[M], temp;
	Word16 lsf1_q[M];

	if (bfi != 0)   /* if bad frame */
	{
		/* use the past LSFs slightly shifted towards their mean */
		for (i = 0; i < M; i++)
		{
			/* lsfi_q[i] = ALPHA*past_lsf_q[i] + ONE_ALPHA*voAMRNBDecmean_lsf[i]; */
			lsf1_q[i] = add(mult(st->past_lsf_q[i], ALPHA), mult(voAMRNBDecmean_lsf3[i], ONE_ALPHA));
		}

		/* estimate past quantized residual to be used in next frame */
		if(mode != MRDTX) {
			for (i = 0; i < M; i++) {
				/* temp  = voAMRNBDecmean_lsf[i] +  past_r2_q[i] * PRED_FAC; */
				temp = add(voAMRNBDecmean_lsf3[i], mult(st->past_r_q[i], voAMRNBDecpred_fac3[i]));
				st->past_r_q[i] = sub(lsf1_q[i], temp);                   
			}
		} else {
			for (i = 0; i < M; i++) {
				/* temp  = voAMRNBDecmean_lsf[i] +  past_r2_q[i]; */
				temp = add(voAMRNBDecmean_lsf3[i], st->past_r_q[i]);
				st->past_r_q[i] = sub(lsf1_q[i], temp);                   
			}	  
		}
	}
	else  /* if good LSFs received */
	{
		if ((mode == MR475)||(mode == MR515))
		{   /* MR475, MR515 */
			p_cb1 = voAMRNBDecdico1_lsf3;                  
			p_cb2 = voAMRNBDecdico2_lsf3;                  
			p_cb3 = voAMRNBDecmr515_3_lsf3;                
		}
		else if(mode == MR795)
		{   /* MR795 */
			p_cb1 = voAMRNBDecmr795_1_lsf3;                
			p_cb2 = voAMRNBDecdico2_lsf3;                  
			p_cb3 = voAMRNBDecdico3_lsf3;                  
		}
		else 
		{   /* MR59, MR67, MR74, MR102, MRDTX */ 
			p_cb1 = voAMRNBDecdico1_lsf3;                  
			p_cb2 = voAMRNBDecdico2_lsf3;                  
			p_cb3 = voAMRNBDecdico3_lsf3;                  
		}

		/* decode prediction residuals from 3 received indices */
		index = *indice++;                      
		p_dico = &p_cb1[(index << 1) + index];  
		lsf1_r[0] = *p_dico++;                  
		lsf1_r[1] = *p_dico++;                  
		lsf1_r[2] = *p_dico++;                  

		index = *indice++;                      

		if ((mode == MR475) || (mode == MR515))
		{   /* MR475, MR515 only using every second entry */
			index = (index << 1);
		}

		p_dico = &p_cb2[(index << 1) + index];              
		lsf1_r[3] = *p_dico++;                  
		lsf1_r[4] = *p_dico++;                  
		lsf1_r[5] = *p_dico++;                  

		index = *indice++;                      
		p_dico = &p_cb3[(index << 2)];         
		lsf1_r[6] = *p_dico++;                  
		lsf1_r[7] = *p_dico++;                  
		lsf1_r[8] = *p_dico++;                  
		lsf1_r[9] = *p_dico++;                  

		/* Compute quantized LSFs and update the past quantized residual */
		if(mode != MRDTX) 
			for (i = 0; i < M; i++) {
				temp = add(voAMRNBDecmean_lsf3[i], mult(st->past_r_q[i], voAMRNBDecpred_fac3[i]));
				lsf1_q[i] = add(lsf1_r[i], temp);   
				st->past_r_q[i] = lsf1_r[i];        
			}
		else
			for (i = 0; i < M; i++) {
				temp = add(voAMRNBDecmean_lsf3[i], st->past_r_q[i]);
				lsf1_q[i] = add(lsf1_r[i], temp);   
				st->past_r_q[i] = lsf1_r[i];        
			}
	}

	/* verification that LSFs has minimum distance of LSF_GAP Hz */

	Reorder_lsf(lsf1_q, LSF_GAP, M);
	voAMRNBDecCopy (lsf1_q, st->past_lsf_q, M);
	/*  convert LSFs to the cosine domain */
	Lsf_lsp(lsf1_q, lsp1_q, M);

	return;
}

void Init_D_plsf_3(D_plsfState *st,  /* i/o: State struct                */
				   Word16 index      /* i  : past_rq_init[] index [0, 7] */)
{
	voAMRNBDecCopy(&voAMRNBDecpast_rq_init3[index * M], st->past_r_q, M);
}
