/**********************************************************************/
/* QCELP Variable Rate Speech Codec - Simulation of TIA IS96-A, service */
/*     option one for TIA IS95, North American Wideband CDMA Digital  */
/*     Cellular Telephony.					      */
/*								      */
/* (C) Copyright 1993, QUALCOMM Incorporated			      */
/* QUALCOMM Incorporated					      */
/* 10555 Sorrento Valley Road					      */
/* San Diego, CA 92121						      */
/*								      */
/* Note:  Reproduction and use of this software for the design and    */
/*     development of North American Wideband CDMA Digital	      */
/*     Cellular Telephony Standards is authorized by		      */
/*     QUALCOMM Incorporated.  QUALCOMM Incorporated does not	      */
/*     authorize the use of this software for any other purpose.      */
/*								      */
/*     The availability of this software does not provide any license */
/*     by implication, estoppel, or otherwise under any patent rights */
/*     of QUALCOMM Incorporated or others covering any use of the     */
/*     contents herein. 					      */
/*								      */
/*     Any copies of this software or derivative works must include   */
/*     this and all other proprietary notices.			      */
/**********************************************************************/
/* quantize.c - quantiztation routines */

/*****************************************************************************
* File:           quant13.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    quantization routines 
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

//#include<math.h>
#include<stdlib.h>
#include"qcelp.h"
#include"qcelp13.h"
//#include"qc13_deb.h"

#define  MATCH_1617  1

#define  LSP_DPCM_DECAY 0.90625

//#define MPY32_16(lvar, svar) ((svar*(I16)((I40)lvar>>(Int16)16))+(I40)(((Int32)svar*((Int32)lvar&(Int32)0xffffl))>>(Int16)15))
//Int16 norm_40(I40 L_var1);

extern Int16 QG[NUMRATES][4];
extern Int16 GA_16[73];
extern Int16 QCODE_DIV_NUM_LEVELS_16[4][16];
extern Int16 QCODE_DIV_FULL_NUM_LEVELS_16[8];
extern Int16 CosineTable_16[COSTABLE_LEN];

extern Int16 LSPVQ0_16[][2];

extern Int16 LSPVQ1_16[][2];

extern Int16 LSPVQ2_16[][2];

extern Int16 LSPVQ3_16[][2];

extern Int16 LSPVQ4_16[][2];

extern Int16 QC13_PITCH_GAIN[8];

/****************************************************************************
Integer Subroutines
****************************************************************************/
int  unquantize_G_8th(Int16 * q_G,Int16 * qcode_G, Int16 *G_pred)
{
    Int16 i;
    Int16 pred;
    Int16 ind, G_dB;
    Int32 ar;	
    if (*qcode_G < 0 || *qcode_G > 3)
    {
        return VO_ERR_WRONG_STATUS;
    }
	
    ar = 0l;
    for (i = 0; i < QC13_GPRED_ORDER; i++)
    {
		ar += (Int32)G_pred[i];
    }
    ar <<= 15; // divide by 2
    ar = L_add(ar, 0x00008000l);
    pred = (Int16)(ar >> 16);
	
    G_dB = pred;
	
    G_dB -= 1;
    if (G_dB < 4)
		G_dB = 4;
    else if (G_dB > 58)
		G_dB = 58;
	
    for (i = QC13_GPRED_ORDER - 1; i > 0; i--)
    {
		G_pred[i] = G_pred[i - 1];
    }
    G_pred[0] = (Int16)(G_dB + QG[1][ *qcode_G]);
	
    ind = G_dB + 6;	      /* 6 is offset required for GA[]	     */
    *q_G = GA_16[ind + QG[1][ *qcode_G]];
	
    return VO_ERR_NONE;
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  unquantize_G_13(Int16 rate, I16 *q_G, Int16 *qcode_G, Int16 qcode_Gsign, I16 *lastG, Int16 cbsf, Int16 *G_pred)
Void  unquantize_G_13(Int16 rate, Int16 *q_G, Int16 *qcode_G, Int16 qcode_Gsign, Int16 *lastG, Int16 cbsf, Int16 *G_pred)
{
    Int16 i;
    Int16 pred;
    Int32 ar;
	
    for (i = QC13_GPRED_ORDER - 1; i > 0; i--)
    {
		G_pred[i] = G_pred[i - 1];
    }
	
    if ((rate != QC13_FULL) || ((cbsf + 1) % 4 != 0))
    {
		ar = QC13_DIFFG_16 * QCODE_DIV_NUM_LEVELS_16[3][*qcode_G] << 1; // Q9 * Q15 = Q25
		ar += (QC13_MING_16 << 16); // bring up to Q25
		ar = L_add(ar, 0x00008000l);
		*q_G = (Int16)(ar >> 16);
		for (i = QC13_GORDER - 1; i >= 1; i--)
		{
			lastG[i] = lastG[i - 1];
		}
		lastG[0] = *q_G;
		
		ar = (Int32)(*q_G);
		ar >>= 9; // bring down to Q0
		*q_G = (Int16)ar;
		
		G_pred[0] = (Int16) (*q_G);
		
		*q_G = GA_16[ ((Int16) *q_G) + 6];
		
		if (qcode_Gsign == NEGATIVE)
		{
			ar = 0l;
			ar -= (Int32)(*q_G);
			*q_G = (Int16)ar;
			//	      *q_G *= -1;
		}
    }
    else
    {
		ar = 0l;
		for (i = 0; i < QC13_GORDER; i++)
		{
			ar += lastG[i]; // QC13_GPRED_COEFF is 1.0
		}
		ar <<= 14; //bring up to Q23
		ar += 0x00008000l; // rounding
		pred = (Int16)(ar >> 16);  // Q7 value
		ar = pred * INV_QC13_GORDER << 1; // Q7 * Q15 = Q23
		ar <<= 2; // bring up to Q25
		ar = L_add(ar, 0x00008000l);
		pred = (Int16)(ar >> 16);
		//	  pred = (Int16) (pred / QC13_GORDER);/* truncation			   */
		
		/* limit the prediction value */
		if (pred > QC13_MAX_GPRED_16)
			pred = QC13_MAX_GPRED_16;
		else if (pred < QC13_MIN_GPRED_16)
			pred = QC13_MIN_GPRED_16;
		
		ar = (Int32)QC13_REL_DIFFG_16 * (Int32)QCODE_DIV_FULL_NUM_LEVELS_16[*qcode_G] << 1; // Q9 * Q15 = Q25
		ar += (Int32)QC13_RELMING_16 << 16; // bring up to Q25
		//	  lin_unquant(q_G, QC13_RELMING, QC13_RELMAXG,
		//	      QC13_RELNUMBER_OF_G_LEVELS, *qcode_G);
		ar += (Int32)pred << 16; // bring up to Q25
		//	  *q_G += pred;
		ar = L_add(ar, 0x00008000l);    //rounding
		*q_G = (Int16)(ar >> 16);
		
		for (i = QC13_GORDER - 1; i >= 1; i--)
		{
			lastG[i] = lastG[i - 1];
		}
		lastG[0] = *q_G;
		
		ar = (Int32)*q_G;
		ar >>= 9; //bring down to Q0
		*q_G = (Int16)ar;
		
		G_pred[0] = (Int16) *q_G;
		
		*q_G = GA_16[ ((Int16) *q_G) + 6];
		
		if (qcode_Gsign == NEGATIVE)
		{
			ar = 0l;
			ar -= (Int32)*q_G;
			*q_G = (Int16)ar;
			//	      *q_G *= -1;
		}
    }
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  unvq_lsp(I16 * qlsp, I16 * lsp_pred, Int16 * qcode)
Void  unvq_lsp(Int16 * qlsp, Int16 * lsp_pred, Int16 * qcode)
{
    Int16 i;
    Int16 *VQ=NULL;
    Int16 temp;
    Int32 arA;
    Int32 arB;
	
    temp = (Int16)QC13_INV_HALF_LPCORDER;
	//    for (i = 0; i < LPCORDER / 2; i++)
    for (i = 0; i < (LPCORDER>>1); i++)
    {
		switch (i)
		{
		case 0 :
			VQ = &LSPVQ0_16[0][0];
			break;
		case 1 :
			VQ = &LSPVQ1_16[0][0];
			break;
		case 2 :
			VQ = &LSPVQ2_16[0][0];
			break;
		case 3 :
			VQ = &LSPVQ3_16[0][0];
			break;
		case 4 :
			VQ = &LSPVQ4_16[0][0];
			break;
		}
		if (i == 0)
		{
			qlsp[0] = VQ[2 * qcode[i]];
			arB = (Int32)qlsp[i] << 16; // Q31
			arB = L_sub(arB, ((Int32)temp << 16)); // Q31
			//arB <<= (Int16)3; // bring up to Q18
			//	    SAT(arB);
			lsp_pred[0] = (Int16)(arB >> 16);
#if MATCH_1617
			arB = (((Int32)29696) * (Int32)lsp_pred[0]) << 1; // decay by 0.90625
			arB = L_add(arB, 0x00008000l); // rounding
			lsp_pred[0] = (Int16)(arB >> 16);
#endif
			arB = (Int32)temp;
			arB += (Int32)QC13_INV_HALF_LPCORDER;
			temp = (Int16)arB;
		}
		else
		{
			arA  = (Int32)qlsp[2 * i - 1] << 16; // Q31
			arA = L_add(arA, ((Int32)VQ[2 * qcode[i]] << 16)); // Q31
			arB = arA;
			arB = L_sub(arB, ((Int32)temp << 16)); // Q31
			//arB <<= 3; // bring up to Q18
			arB = L_add(arB, 0x00008000l); // rounding
			lsp_pred[2 * i] = (Int16)(arB >> 16);
			qlsp[2 * i] = (Int16)(arA >> 16); //Q15
#if MATCH_1617
			arB = ((Int32)29696 * (Int32)lsp_pred[2 * i]) << 1; // decay by 0.90625
			arB = L_add(arB, 0x00008000l); // rounding
			lsp_pred[2 * i] = (Int16)(arB >> 16);
#endif
			arB = (Int32)temp;
			arB += (Int32)QC13_INV_HALF_LPCORDER;
			temp = (Int16)arB;
		}
		
		arA = (Int32)qlsp[2 * i] << 16; // Q31
		arA = L_add(arA, ((Int32)VQ[2 * qcode[i] + 1] << 16)); //Q31
		arB = arA;
		arB = L_sub(arB, ((Int32)temp << 16)); // Q31
		//arB <<= 3; // bring up to Q18
		arB = L_add(arB, 0x00008000l); // rounding
		lsp_pred[2 * i + 1] = (Int16)(arB >> 16);
		qlsp[2 * i + 1] = (Int16)(arA >> 16);
#if MATCH_1617
        arB = ((Int32)29696 * (Int32)lsp_pred[2 * i + 1]) << 1; // decay by 0.90625
        arB = L_add(arB, 0x00008000l);
        lsp_pred[2 * i + 1] = (Int16)(arB >> 16);
#endif
		arB = (Int32)temp;
		arB += (Int32)QC13_INV_HALF_LPCORDER;
		temp = (Int16)arB;
    }
}

//Void  unquantize_lag(Int16 *lag, Int16 *qcode_lag, Int16 *frac, Int16 *qcode_frac)
int unquantize_lag(Int16 *lag, Int16 *qcode_lag, Int16 *frac, Int16 *qcode_frac)
{
    if (*qcode_lag == 0)
    {
		*lag = MAXLAG;
		*frac = 0;
    }
    else if (*qcode_lag > 123) // This branch added to fix bug in badrate.pkt testing
    {
		*lag = (Int16)(*qcode_lag + 16);
		*frac = 0;
    }
    else if (*qcode_frac == 0)
    {
		*lag = (Int16)(*qcode_lag + 16);
		*frac = 0;
    }
    else if (*qcode_frac == 1)
    {
		*lag = (Int16)(*qcode_lag + 16);
		*frac = 2;
    }
    else
    {
        return VO_ERR_WRONG_STATUS;
    }
    
    return VO_ERR_NONE;
}

int  unquantize_b(Int16 *q_b, Int16 *qcode_b, Int16 lag)
{
    if (*qcode_b < 0 || *qcode_b > 7)
    {
		return VO_ERR_WRONG_STATUS;
    }
	
    if (*qcode_b == 0)
    {
		if (lag == 0)
			*q_b = (Int16)0;
		else
			*q_b = (Int16)4096;
    }
    else
    {
		*q_b = QC13_PITCH_GAIN[*qcode_b]; // Q14 format
		//	  *q_b = 0.25 * (*qcode_b + 1);
    }
	
    return VO_ERR_NONE;
}

