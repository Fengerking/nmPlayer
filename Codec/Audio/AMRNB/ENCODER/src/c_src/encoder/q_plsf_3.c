/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
****************************************************************************
*
*      File             : voAMRNBEnc_Q_plsf_3.c
*      Purpose          : Quantization of LSF parameters with 1st order MA
*                         prediction and split by 3 vector quantization
*                         (split-VQ)
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "q_plsf.h"
const char q_plsf_3_id[] = "@(#)$Id $" q_plsf_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "lsp_lsf.h"
#include "reorder.h"
#include "lsfwt.h"
#include "copy.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
#include "q_plsf_3.tab"        /* Codebooks of LSF prediction residual */
#define PAST_RQ_INIT_SIZE 8
/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/
/* Quantization of a 4 dimensional subvector */

static Word16  Vq_subvec4(             /* o: quantization index,            Q0  */
						  Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
						  Word16 * dico,      /* i: quantization codebook,         Q15 */
						  Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
						  Word16 dico_size)   /* i: size of quantization codebook, Q0  */  
{
	Word32 i, index = 0;
	Word16 *p_dico, temp;
	Word32 dist_min, dist;
	Word16 lsf_r1_0, lsf_r1_1, lsf_r1_2, lsf_r1_3;
	Word16 wf1_0, wf1_1, wf1_2, wf1_3;  	

	dist_min = MAX_32;                                             
	p_dico = dico;                                                  

	lsf_r1_0 = lsf_r1[0];	lsf_r1_1 = lsf_r1[1];
	lsf_r1_2 = lsf_r1[2];	lsf_r1_3 = lsf_r1[3];

	wf1_0 = wf1[0]; wf1_1 = wf1[1];
	wf1_2 = wf1[2]; wf1_3 = wf1[3];

	for (i = 0; i < dico_size; i++)
	{
		temp = sub3 (lsf_r1_0, p_dico[0]);
		temp = mult3 (wf1_0, temp);
		dist = L_mult3 (temp, temp);

		temp = sub3 (lsf_r1_2, p_dico[2]);
		temp = mult3 (wf1_2, temp);
		dist = L_mac3 (dist, temp, temp);

		if(dist - dist_min > (Word32) 0)
		{
			p_dico += 4;
			continue;
		}

		temp = sub3 (lsf_r1_1, p_dico[1]);
		temp = mult3 (wf1_1, temp);
		dist = L_mac3 (dist, temp, temp);

		temp = sub3 (lsf_r1_3, p_dico[3]);
		temp = mult3 (wf1_3, temp);
		dist = L_mac3 (dist, temp, temp);

		if (dist - dist_min < (Word32) 0)
		{
			dist_min = dist;                                    
			index = i;                                           
		}

		p_dico += 4;

	}

	/* Reading the selected vector */

	p_dico = &dico[shl2 (index, 2)];                              
	lsf_r1[0] = *p_dico++;                                        
	lsf_r1[1] = *p_dico++;                                         
	lsf_r1[2] = *p_dico++;                                         
	lsf_r1[3] = *p_dico++;                                         

	return index;

}

/* Quantization of a 3 dimensional subvector */

static Word16 Vq_subvec3(             /* o: quantization index,            Q0  */
						 Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
						 Word16 * dico,      /* i: quantization codebook,         Q15 */
						 Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
						 Word16 dico_size,   /* i: size of quantization codebook, Q0  */
						 Flag use_half)      /* i: use every second entry in codebook */
{
	nativeInt i, index = 0;
	Word16 *p_dico, temp;
	Word32 dist_min, dist;
	Word16 lsf_r1_0, lsf_r1_1, lsf_r1_2;
	Word16 wf1_0, wf1_1, wf1_2;

	dist_min = MAX_32;                                           
	p_dico = dico;           

	lsf_r1_0 = lsf_r1[0]; lsf_r1_1 = lsf_r1[1]; lsf_r1_2 = lsf_r1[2]; 
	wf1_0 = wf1[0]; wf1_1 = wf1[1]; wf1_2 = wf1[2];                                                  

	if (use_half == 0) {
		for (i = 0; i < dico_size; i++)
		{
			temp = sub3(lsf_r1_0, *p_dico++);
			temp = mult3(wf1_0, temp);
			dist = L_mult3(temp, temp);

			if ((dist - dist_min) > (Word32) 0) {
				p_dico += 2;
				continue;
			}

			temp = sub3(lsf_r1_1, *p_dico++);
			temp = mult3(wf1_1, temp);
			dist = L_mac3(dist, temp, temp);

			temp = sub3(lsf_r1_2, *p_dico++);
			temp = mult3(wf1_2, temp);
			dist = L_mac3(dist, temp, temp);

			if ((dist - dist_min) < (Word32) 0) {
				dist_min = dist;                                  
				index = i;                                        
			}
		}
		p_dico = &dico[add3(index, add3(index, index))];            
	}
	else
	{
		for (i = 0; i < dico_size; i++)
		{
			temp = sub3(lsf_r1_0, *p_dico++);
			temp = mult3(wf1_0, temp);
			dist = L_mult3(temp, temp);

			if ((dist - dist_min) > (Word32) 0) {
				p_dico += 5; 
				continue;
			}

			temp = sub3(lsf_r1_1, *p_dico++);
			temp = mult3(wf1_1, temp);
			dist = L_mac3(dist, temp, temp);

			temp = sub3(lsf_r1_2, *p_dico++);
			temp = mult3(wf1_2, temp);
			dist = L_mac3(dist, temp, temp);

			if ((dist - dist_min) < (Word32) 0)
			{
				dist_min = dist;                                   
				index = i;                                         
			}
			p_dico = p_dico + 3; 
		}
		p_dico = &dico[shl3(add3(index, add3(index, index)),1)];     
	}


	/* Reading the selected vector */
	lsf_r1[0] = *p_dico++;                                         
	lsf_r1[1] = *p_dico++;                                         
	lsf_r1[2] = *p_dico++;                                         

	return index;
}

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/

/***********************************************************************
*
* routine:   voAMRNBEnc_Q_plsf_3()
*
* Quantization of LSF parameters with 1st order MA prediction and
* split by 3 vector quantization (split-VQ)
*
***********************************************************************/

void voAMRNBEnc_Q_plsf_3(
			  Q_plsfState *st,    /* i/o: state struct                             */
			  enum Mode mode,     /* i  : coder mode                               */
			  Word16 *lsp1,       /* i  : 1st LSP vector                      Q15  */
			  Word16 *lsp1_q,     /* o  : quantized 1st LSP vector            Q15  */
			  Word16 *indice,     /* o  : quantization indices of 3 vectors   Q0   */
			  Word16 *pred_init_i /* o  : init index for MA prediction in DTX mode */
			  )
{
	int  i, j;
	Word16 lsf1[M], wf1[M], lsf_p[M] = {0}, lsf_r1[M] = {0};
    Word16 lsf1_q[M];
    Word16 temp_r1[M];
    Word16 temp_p[M];
	Word32 L_pred_init_err;
	Word32 L_min_pred_init_err;

	/* convert LSFs to normalize frequency domain 0..16384 */
	voAMRNBEnc_Lsp_lsf(lsp1, lsf1, M);

	/* compute LSF weighting factors (Q13) */
	voAMRNBEnc_Lsf_wt(lsf1, wf1);

	/* Compute predicted LSF and prediction error */
	if (sub3(mode, MRDTX) != 0)
	{
		for (i = 0; i < M; i++)
		{
			lsf_p[i] = add3(mean_lsf3[i], 
				mult3(st->past_rq[i], 
				pred_fac3[i]));                           
			lsf_r1[i] = sub3(lsf1[i], lsf_p[i]);                         
		}
	}
	else
	{
		/* DTX mode, search the init vector that yields */
		/* lowest prediction resuidual energy           */
		*pred_init_i = 0;                                               
		L_min_pred_init_err = 0x7fffffff; /* 2^31 - 1 */                
		for (j = 0; j < PAST_RQ_INIT_SIZE; j++)
		{
			L_pred_init_err = 0;                                          
			for (i = 0; i < M; i++)
			{
				temp_p[i] = add3(mean_lsf3[i], past_rq_init3[j*M+i]);
				temp_r1[i] = sub3(lsf1[i],temp_p[i]);
				L_pred_init_err = L_mac3(L_pred_init_err, temp_r1[i], temp_r1[i]);
			}  /* next i */


			if (L_sub3(L_pred_init_err, L_min_pred_init_err) < (Word32) 0)
			{
				L_min_pred_init_err = L_pred_init_err;                  
				Copy(temp_r1, lsf_r1, M);
				Copy(temp_p, lsf_p, M);

				/* Set zerom */
				Copy(&past_rq_init3[j*M], st->past_rq, M);
				*pred_init_i = j;                                                   
			} /* endif */
		} /* next j */
	} /* endif MRDTX */

	/*---- Split-VQ of prediction error ----*/
	if (sub3 (mode, MR475) == 0 || sub3 (mode, MR515) == 0)
	{   /* MR475, MR515 */

		indice[0] = Vq_subvec3(&lsf_r1[0], (Word16*)dico1_lsf3, &wf1[0], DICO1_SIZE, 0);

		indice[1] = Vq_subvec3(&lsf_r1[3], (Word16*)dico2_lsf3, &wf1[3], DICO2_SIZE/2, 1);

		indice[2] = Vq_subvec4(&lsf_r1[6], (Word16*)mr515_3_lsf3, &wf1[6], MR515_3_SIZE);

	}
	else if (sub3 (mode, MR795) == 0)
	{   /* MR795 */

		indice[0] = Vq_subvec3(&lsf_r1[0], (Word16*)mr795_1_lsf3, &wf1[0], MR795_1_SIZE, 0);

		indice[1] = Vq_subvec3(&lsf_r1[3], (Word16*)dico2_lsf3, &wf1[3], DICO2_SIZE, 0);

		indice[2] = Vq_subvec4(&lsf_r1[6], (Word16*)dico3_lsf3, &wf1[6], DICO3_SIZE);

	}
	else 
	{   /* MR59, MR67, MR74, MR102 , MRDTX */

		indice[0] = Vq_subvec3(&lsf_r1[0], (Word16*)dico1_lsf3, &wf1[0], DICO1_SIZE, 0);

		indice[1] = Vq_subvec3(&lsf_r1[3], (Word16*)dico2_lsf3, &wf1[3], DICO2_SIZE, 0);

		indice[2] = Vq_subvec4(&lsf_r1[6], (Word16*)dico3_lsf3, &wf1[6], DICO3_SIZE);

	}


	/* Compute quantized LSFs and update the past quantized residual */

	for (i = 0; i < M; i++)
	{
		lsf1_q[i] = add3(lsf_r1[i], lsf_p[i]);                   
		st->past_rq[i] = lsf_r1[i];                             
	}

	/* verification that LSFs has mimimum distance of LSF_GAP Hz */

	voAMRNBEnc_Reorder_lsf(lsf1_q, LSF_GAP, M);

	/*  convert LSFs to the cosine domain */

	voAMRNBEnc_Lsf_lsp(lsf1_q, lsp1_q, M);
}
