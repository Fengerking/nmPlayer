/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : q_plsf_5.c
*      Purpose          : Quantization of 2 sets of LSF parameters using 1st 
*                         order MA prediction and split by 5 matrix
*                         quantization (split-MQ)
*
********************************************************************************
*/


/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_plsf.h"
const char q_plsf_5_id[] = "@(#)$Id $" q_plsf_h;

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
#include "lsfwt.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

#include "q_plsf_5.tab"         /* Codebooks of LSF prediction residual */

//bit match
/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/ //bit match
/* Quantization of a 4 dimensional subvector */
#ifdef ARMv6_OPT
Word16 Vq_subvec_asm (/* o : quantization index,            Q0  */
				  Word16 *lsf_r1,      /* i : 1st LSF residual vector,       Q15 */
				  Word16 *lsf_r2,      /* i : 2nd LSF residual vector,       Q15 */
				  const Word16 *dico,  /* i : quantization codebook,         Q15 */
				  Word16 *wf1,         /* i : 1st LSF weighting factors      Q13 */
				  Word16 *wf2,         /* i : 2nd LSF weighting factors      Q13 */  
				  Word16 dico_size     /* i : size of quantization codebook, Q0  */
);
#else
static Word16 Vq_subvec (/* o : quantization index,            Q0  */
						 Word16 *lsf_r1,      /* i : 1st LSF residual vector,       Q15 */
						 Word16 *lsf_r2,      /* i : 2nd LSF residual vector,       Q15 */
						 const Word16 *dico,  /* i : quantization codebook,         Q15 */
						 Word16 *wf1,         /* i : 1st LSF weighting factors      Q13 */
						 Word16 *wf2,         /* i : 2nd LSF weighting factors      Q13 */  
						 Word16 dico_size     /* i : size of quantization codebook, Q0  */
						 )
{
	Word32   index = 0; /* initialization only needed to keep gcc silent */
	Word32   temp;
	const Word16 *p_dico;
	Word32 dist_min, dist;
	Word32 i;
	Word32 t0_lsf_r1  =  lsf_r1[0];
	Word32 t1_lsf_r1  =  lsf_r1[1];
	Word32 t0_wf1	 =  wf1[0];
	Word32 t1_wf1	 =  wf1[1];
	Word32 t0_lsf_r2  =  lsf_r2[0];
	Word32 t1_lsf_r2  =  lsf_r2[1];
	Word32 t0_wf2	 =  wf2[0];
	Word32 t1_wf2	 =  wf2[1];

	dist_min = MAX_32;                                  
	p_dico = dico;   
#ifdef C_OPT//very_safe TODO:judge the dis-dist_min after the second cal
	for (i = 0; i < dico_size; i++)
	{
		temp = (t0_lsf_r1 - p_dico[0]);
		temp =  (t0_wf1 * temp>>15);
		dist = (temp * temp<<1);

		temp =  (t0_lsf_r2 - p_dico[2]);
		temp =  (t0_wf2 * temp>>15);
		dist += (temp * temp<<1);//L_mac (dist, temp, temp);

		if ( dist >= dist_min)
		{
			p_dico += 4;
			continue;
		}
		temp =  (t1_lsf_r1 - p_dico[1]);
		temp =  (t1_wf1 * temp>>15);
		dist += (temp * temp<<1);//L_mac (dist, temp, temp);

		temp =  (t1_lsf_r2 - p_dico[3]);
		temp =  (t1_wf2 * temp>>15);
		dist += (temp * temp<<1);//L_mac (dist, temp, temp);

		if (dist < dist_min )
		{
			dist_min = dist;                           
			index = i;                                 
		}
		p_dico += 4;
	}
	/* Reading the selected vector */
	p_dico = &dico[(index << 2)];                     
	lsf_r1[0] = *p_dico++;                              
	lsf_r1[1] = *p_dico++;                              
	lsf_r2[0] = *p_dico++;                              
	lsf_r2[1] = *p_dico++;            
#else
	for (i = 0; i < dico_size; i++)
	{
		temp = (lsf_r1[0] -*p_dico++);
		temp = mult (wf1[0], temp);
		dist = L_mult (temp, temp);

		temp =  (lsf_r1[1] - *p_dico++);
		temp = mult (wf1[1], temp);
		dist = L_mac (dist, temp, temp);

		temp = (lsf_r2[0] - *p_dico++);
		temp = mult (wf2[0], temp);
		dist = L_mac (dist, temp, temp);

		temp =  (lsf_r2[1] -*p_dico++);
		temp = mult (wf2[1], temp);
		dist = L_mac (dist, temp, temp);

		if ( (dist - dist_min) <  0)
		{
			dist_min = dist;                           
			index = i;                                 
		}
	}
	/* Reading the selected vector */

	p_dico = &dico[shl2 (index, 2)];                     
	lsf_r1[0] = *p_dico++;                              
	lsf_r1[1] = *p_dico++;                              
	lsf_r2[0] = *p_dico++;                              
	lsf_r2[1] = *p_dico++;                              
#endif
	return index;

}
#endif

/* Quantization of a 4 dimensional subvector with a signed codebook */

static Word16 Vq_subvec_s ( /* o : quantization index            Q0  */
						   Word16 *lsf_r1,         /* i : 1st LSF residual vector       Q15 */
						   Word16 *lsf_r2,         /* i : and LSF residual vector       Q15 */
						   const Word16 *dico,     /* i : quantization codebook         Q15 */
						   Word16 *wf1,            /* i : 1st LSF weighting factors     Q13 */
						   Word16 *wf2,            /* i : 2nd LSF weighting factors     Q13 */
						   Word16 dico_size)       /* i : size of quantization codebook Q0  */  
{
	nativeInt index = 0;  /* initialization only needed to keep gcc silent */
	nativeInt sign = 0;   /* initialization only needed to keep gcc silent */
	nativeInt temp, temp1;
	const Word16 *p_dico;
	Word32 dist_min, dist, dist1;
	nativeInt i;
	dist_min = MAX_32;                                 
	p_dico = dico; 

#ifdef C_OPT//very_safe
	for (i = 0; i < dico_size; i++)
	{
		temp  = (lsf_r1[0] - p_dico[0]);
		temp1 = (lsf_r1[0] + p_dico[0]);
		temp  = (wf1[0] * temp >>15);
		dist  = (temp * temp <<1);
		temp1 = (wf1[0] * temp1 >>15);        
		dist1 = (temp1 * temp1 <<1);

		temp  = (lsf_r1[1] - p_dico[1]);
		temp1 = (lsf_r1[1] + p_dico[1]);
		temp  = (wf1[1] * temp>>15);
		dist  += (temp * temp<<1);
		temp1 = (wf1[1] * temp1 >>15);
		dist1 += (temp1 * temp1<<1);

		temp  =  (lsf_r2[0] - p_dico[2]);
		temp1 =  (lsf_r2[0] + p_dico[2]);
		temp  = (wf2[0] * temp>>15);
		dist  += (temp * temp<<1);
		temp1 = (wf2[0] * temp1>>15);
		dist1 += (temp1* temp1<<1);

		temp  =  (lsf_r2[1] - p_dico[3]);
		temp1 =  (lsf_r2[1] + p_dico[3]);
		temp  = (wf2[1] * temp>>15);
		dist  += (temp* temp<<1);
		temp1 = (wf2[1] * temp1>>15);
		dist1 += (temp1* temp1<<1);

		if (dist < dist_min)
		{
			dist_min = dist;                        
			index = i;                                 
			sign = 0;                                   
		}
		if (dist1 < dist_min)
		{
			dist_min = dist1;                             
			index = i;                                  
			sign = 1;                                  
		}
		p_dico += 4;
	}
#else
	for (i = 0; i < dico_size; i++)
	{
		/* test positive */
		temp =  (lsf_r1[0] - *p_dico++);
		temp = mult (wf1[0], temp);
		dist = L_mult (temp, temp);

		temp = (lsf_r1[1] - *p_dico++);
		temp = mult (wf1[1], temp);
		dist = L_mac (dist, temp, temp);

		temp =  (lsf_r2[0] - *p_dico++);
		temp = mult (wf2[0], temp);
		dist = L_mac (dist, temp, temp);

		temp =  (lsf_r2[1] -*p_dico++);
		temp = mult (wf2[1], temp);
		dist = L_mac (dist, temp, temp);

		if ((dist - dist_min) <  0)
		{
			dist_min = dist;                        
			index = i;                                 
			sign = 0;                                   
		}
		/* test negative */

		p_dico -= 4;                                   
		temp = add (lsf_r1[0], *p_dico++);
		temp = mult (wf1[0], temp);
		dist = L_mult (temp, temp);

		temp = add (lsf_r1[1], *p_dico++);
		temp = mult (wf1[1], temp);
		dist = L_mac (dist, temp, temp);

		temp = add (lsf_r2[0], *p_dico++);
		temp = mult (wf2[0], temp);
		dist = L_mac (dist, temp, temp);

		temp = add (lsf_r2[1], *p_dico++);
		temp = mult (wf2[1], temp);
		dist = L_mac (dist, temp, temp);
		if ((dist-dist_min) < (Word32) 0)
		{
			dist_min = dist;                             
			index = i;                                  
			sign = 1;                                  
		}
	}
#endif
	/* Reading the selected vector */
	p_dico = &dico[(index<<2)];                     
	if (sign == 0)
	{
		lsf_r1[0] = *p_dico++;                      
		lsf_r1[1] = *p_dico++;                         
		lsf_r2[0] = *p_dico++;                         
		lsf_r2[1] = *p_dico++;                          
	}
	else
	{
		lsf_r1[0] = - (*p_dico++);                 
		lsf_r1[1] = - (*p_dico++);                 
		lsf_r2[0] = - (*p_dico++);                 
		lsf_r2[1] = - (*p_dico++);                 
	}
	index = (index<<1);
	index = (index + sign);
	return index;
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
*   FUNCTION:  Q_plsf_5()
*
*   PURPOSE:  Quantization of 2 sets of LSF parameters using 1st order MA
*             prediction and split by 5 matrix quantization (split-MQ)
*
*   DESCRIPTION:
*
*        p[i] = pred_factor*past_rq[i];   i=0,...,m-1
*        r1[i]= lsf1[i] - p[i];           i=0,...,m-1
*        r2[i]= lsf2[i] - p[i];           i=0,...,m-1
*   where:
*        lsf1[i]           1st mean-removed LSF vector.
*        lsf2[i]           2nd mean-removed LSF vector.
*        r1[i]             1st residual prediction vector.
*        r2[i]             2nd residual prediction vector.
*        past_r2q[i]       Past quantized residual (2nd vector).
*
*   The residual vectors r1[i] and r2[i] are jointly quantized using
*   split-MQ with 5 codebooks. Each 4th dimension submatrix contains 2
*   elements from each residual vector. The 5 submatrices are as follows:
*     {r1[0], r1[1], r2[0], r2[1]};  {r1[2], r1[3], r2[2], r2[3]};
*     {r1[4], r1[5], r2[4], r2[5]};  {r1[6], r1[7], r2[6], r2[7]};
*                    {r1[8], r1[9], r2[8], r2[9]};
*
*************************************************************************/

void voAMRNBEnc_Q_plsf_5(
			   Q_plsfState *st,
			   Word16 *lsp1,      /* i : 1st LSP vector,                     Q15 */
			   Word16 *lsp2,      /* i : 2nd LSP vector,                     Q15 */   
			   Word16 *lsp1_q,    /* o : quantized 1st LSP vector,           Q15 */
			   Word16 *lsp2_q,    /* o : quantized 2nd LSP vector,           Q15 */
			   Word16 *indice     /* o : quantization indices of 5 matrices, Q0  */
			   )
{
	Word32  i;
    Word16 lsf1[M], lsf2[M], wf1[M], wf2[M], lsf_p[M], lsf_r1[M], lsf_r2[M];
    Word16 lsf1_q[M], lsf2_q[M];
	/* convert LSFs to normalize frequency domain 0..16384  */
	voAMRNBEnc_Lsp_lsf(lsp1, lsf1, M);
	voAMRNBEnc_Lsp_lsf(lsp2, lsf2, M);
	/* Compute LSF weighting factors (Q13) */
	voAMRNBEnc_Lsf_wt(lsf1, wf1);
	voAMRNBEnc_Lsf_wt(lsf2, wf2);
	/* Compute predicted LSF and prediction error */
#ifdef C_OPT
	for (i = 0; i < M; i++)
	{
		lsf_p[i] =  (mean_lsf[i] + (st->past_rq[i]* LSP_PRED_FAC_MR122>>15));
		lsf_r1[i] = (lsf1[i] - lsf_p[i]);          
		lsf_r2[i] = (lsf2[i] - lsf_p[i]);        
	}
#else
	for (i = 0; i < M; i++)
	{
		lsf_p[i] = add (mean_lsf[i], mult (st->past_rq[i], LSP_PRED_FAC_MR122));
		lsf_r1[i] = sub (lsf1[i], lsf_p[i]); 
		lsf_r2[i] = sub (lsf2[i], lsf_p[i]); 
	}
#endif
	//return;
	/*---- Split-MQ of prediction error ----*/
#ifdef ARMv6_OPT
	indice[0] = Vq_subvec_asm (&lsf_r1[0], &lsf_r2[0], dico1_lsf, &wf1[0], &wf2[0], DICO1_SIZE);
	indice[1] = Vq_subvec_asm (&lsf_r1[2], &lsf_r2[2], dico2_lsf, &wf1[2], &wf2[2], DICO2_SIZE);
#else
	indice[0] = Vq_subvec (&lsf_r1[0], &lsf_r2[0], dico1_lsf, &wf1[0], &wf2[0], DICO1_SIZE);
	indice[1] = Vq_subvec (&lsf_r1[2], &lsf_r2[2], dico2_lsf, &wf1[2], &wf2[2], DICO2_SIZE);
#endif 

	indice[2] = Vq_subvec_s (&lsf_r1[4], &lsf_r2[4], dico3_lsf, &wf1[4], &wf2[4], DICO3_SIZE);

#ifdef ARMv6_OPT
	indice[3] = Vq_subvec_asm (&lsf_r1[6], &lsf_r2[6], dico4_lsf, &wf1[6], &wf2[6], DICO4_SIZE);
	indice[4] = Vq_subvec_asm (&lsf_r1[8], &lsf_r2[8], dico5_lsf, &wf1[8], &wf2[8], DICO5_SIZE);
#else
	indice[3] = Vq_subvec (&lsf_r1[6], &lsf_r2[6], dico4_lsf, &wf1[6], &wf2[6], DICO4_SIZE);
	indice[4] = Vq_subvec (&lsf_r1[8], &lsf_r2[8], dico5_lsf, &wf1[8], &wf2[8], DICO5_SIZE);
#endif
#ifdef C_OPT
	/* Compute quantized LSFs and update the past quantized residual */
	for (i = 0; i < M; i++)
	{
		lsf1_q[i] =  (lsf_r1[i] +  lsf_p[i]);          
		lsf2_q[i] =  (lsf_r2[i] +  lsf_p[i]);          
		st->past_rq[i] = lsf_r2[i];                     
	}
#else
	for (i = 0; i < M; i++)
	{
		lsf1_q[i] = add (lsf_r1[i], lsf_p[i]); 
		lsf2_q[i] = add (lsf_r2[i], lsf_p[i]);  
		st->past_rq[i] = lsf_r2[i];            
	}
#endif
	/* verification that LSFs has minimum distance of LSF_GAP */
	voAMRNBEnc_Reorder_lsf(lsf1_q, LSF_GAP, M);
	voAMRNBEnc_Reorder_lsf(lsf2_q, LSF_GAP, M);
	/*  convert LSFs to the cosine domain */
	voAMRNBEnc_Lsf_lsp(lsf1_q, lsp1_q, M);
	voAMRNBEnc_Lsf_lsp(lsf2_q, lsp2_q, M);
}
