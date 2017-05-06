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
/* pitch.c - perform pitch functions, including open loop pitch */
/*    estimation, and the pitch search				*/

/*****************************************************************************
* File:           pitch.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    perform pitch functions, including open loop pitch 
*                 estimation, and the pitch search				
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

//#include<stdlib.h>
//#include<stdio.h>

#include"qcelp.h"
#include"qcelp13.h"
//#include"qc13_deb.h"

extern Int16 HAMMSINC_16[QC13_FR_INTERP_FILTER_LENGTH];
extern Int16 SMALL_SINC_16[QC13_FR_RES];
extern Int16 PITCHSF[NUMRATES];
//extern Int16 pgain_16[8];
//extern struct DEMO_CONTROL control;
//extern Int16 pk1_16[8];
//extern Int16 pk2_16[8];

/****************************************************************************
Integer
****************************************************************************/
#define T_HEADROOM 0
#define P_HEADROOM 4


/****************************************************************************
Integer Subroutines
****************************************************************************/
Void  pitch_filter_13(Int16 * adptv_cbk,  PITCHPARAMS *pitch_params , Int16 * vector, Int16 length)
{
    Int16 i,j;
    Int16 frac_hist[QC13_FR_INTERP_FILTER_LENGTH];
    Int32 ar,arB;
	
    if (pitch_params->frac == 0)
    {
		if(length<=pitch_params->lag)
		{
			for (i = 0; i < length; i++)
			{
				ar = (Int32)vector[i] << 15;
				ar += (Int32)adptv_cbk[pitch_params->lag - i] * (Int32)pitch_params->b_16 << 1; /* Q14 * Q14 = Q29 (with left shift) */
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
			}
		}
		else
		{
			for (i = 0; i < pitch_params->lag; i++)
			{
				ar = (Int32)vector[i] << 15;
				ar += (Int32)adptv_cbk[pitch_params->lag - i] * (Int32)pitch_params->b_16 << 1; /* Q14 * Q14 = Q29 (with left shift) */
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
			}
			
			for (i = pitch_params->lag; i < length; i++)
			{
				ar = (Int32)vector[i] << 15;
				ar +=  (Int32)vector[i - pitch_params->lag] * (Int32)pitch_params->b_16 << 1; /* Q14 * Q14 = Q30 (with left shift) */
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
			}
		}
    }
    else
    {
		for (j = 0 ; j < QC13_FR_INTERP_FILTER_LENGTH; j++)
		{
			//	    arB = (Int32)adptv_cbk[pitch_params->lag - j + QC13_FR_INTERP_FILTER_LENGTH/2] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
			//	    arB = L_add(arB, 0x00008000L);
			//	    frac_hist[QC13_FR_INTERP_FILTER_LENGTH - j -1] = (Int16)(arB >> 16);
			//	    arB = (Int32)adptv_cbk[pitch_params->lag - j + QC13_FR_INTERP_FILTER_LENGTH/2] * (Int32)pitch_params->b_16;
			arB = (Int32)adptv_cbk[pitch_params->lag - j + (QC13_FR_INTERP_FILTER_LENGTH>>1)] * (Int32)pitch_params->b_16;
			arB += 0x00004000L;
			frac_hist[QC13_FR_INTERP_FILTER_LENGTH - j -1] = (Int16)(arB >> 15);
		}
		
		//	if (length <= (pitch_params->lag - QC13_FR_INTERP_FILTER_LENGTH /2))
		if (length <= (pitch_params->lag - (QC13_FR_INTERP_FILTER_LENGTH>>1)))
		{
			for (i = 0; i < length; i++)
			{
				ar = (Int32)vector[i] << 15;	     // Q30
				for (j = QC13_FR_INTERP_FILTER_LENGTH - 1; j > 0; j--)
				{
					//		      ar += frac_hist[j] * pitch_params->b * HAMMSINC[j];
					ar += (Int32)frac_hist[j] * (Int32)HAMMSINC_16[j] << 1;	 // Q30
					frac_hist[j] = frac_hist[j -1];
				}
				//		  ar += frac_hist[0] * pitch_params->b * HAMMSINC[0];
				ar += (Int32)frac_hist[0] * (Int32)HAMMSINC_16[0] << 1;    // Q30
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
				//		arB = (Int32)adptv_cbk[pitch_params->lag - QC13_FR_INTERP_FILTER_LENGTH/2 - i] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = (Int32)adptv_cbk[pitch_params->lag - (QC13_FR_INTERP_FILTER_LENGTH>>1) - i] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = L_add(arB, 0x00008000L);
				frac_hist[0] = (Int16)(arB >> 16);
			}
		}
		else
		{
			//	    for (i = 0; i < pitch_params->lag - QC13_FR_INTERP_FILTER_LENGTH /2; i++)
			for (i = 0; i < pitch_params->lag - (QC13_FR_INTERP_FILTER_LENGTH>>1); i++)
			{
				ar = (Int32)vector[i] << 15;	     // Q30
				for (j = QC13_FR_INTERP_FILTER_LENGTH - 1; j > 0; j--)
				{
					//		      ar += frac_hist[j] * pitch_params->b * HAMMSINC[j];
					ar += (Int32)frac_hist[j] * (Int32)HAMMSINC_16[j] << 1;	 // Q30
					frac_hist[j] = frac_hist[j -1];
				}
				//		  ar += frac_hist[0] * pitch_params->b * HAMMSINC[0];
				ar += (Int32)frac_hist[0] * (Int32)HAMMSINC_16[0] << 1;    // Q30
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
				//		arB = (Int32)adptv_cbk[pitch_params->lag - QC13_FR_INTERP_FILTER_LENGTH/2 - i] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = (Int32)adptv_cbk[pitch_params->lag - (QC13_FR_INTERP_FILTER_LENGTH>>1) - i] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = L_add(arB, 0x00008000L);
				frac_hist[0] = (Int16)(arB >> 16);
			}
			
			//	    for (i = (Int16)(pitch_params->lag - QC13_FR_INTERP_FILTER_LENGTH/2);
			for (i = (Int16)(pitch_params->lag - (QC13_FR_INTERP_FILTER_LENGTH>>1));
			i < length; i++)
			{
				ar = (Int32)vector[i] << 15;	     // Q30
				for (j = QC13_FR_INTERP_FILTER_LENGTH - 1; j > 0; j--)
				{
					//		      ar += frac_hist[j] * pitch_params->b * HAMMSINC[j];
					ar += (Int32)frac_hist[j] * (Int32)HAMMSINC_16[j] << 1;	 // Q30
					frac_hist[j] = frac_hist[j -1];
				}
				//		  ar += frac_hist[0] * pitch_params->b * HAMMSINC[0];
				ar += (Int32)frac_hist[0] * (Int32)HAMMSINC_16[0] << 1;    // Q30
				ar = L_add(ar, ar);
				ar = L_add(ar, 0x00008000L);
				vector[i] = (Int16)(ar >> 16);
				//		arB = (Int32)vector[i	+ QC13_FR_INTERP_FILTER_LENGTH/2 - pitch_params->lag] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = (Int32)vector[i	+ (QC13_FR_INTERP_FILTER_LENGTH>>1) - pitch_params->lag] * (Int32)pitch_params->b_16 << 1; /* Q15 * Q14 = Q30 (with left shift) */
				arB = L_add(arB, 0x00008000L);
				frac_hist[0] = (Int16)(arB >> 16);
			}
		}
    }
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
Void  update_acbk_16(Int16 * adptv_cbk, Int16 * pvect, Int16 length)
{
    Int16 i;
    Int16 * r1ptr, *r2ptr;
    Int16 * p_ptr = pvect;
	
    /* shift adptv_cbk vector by length samples */
    r1ptr = adptv_cbk + MAXLAG;
    r2ptr = adptv_cbk + (MAXLAG - length);
    for (i = 0; i < MAXLAG - length; i++)
		*r1ptr--= *r2ptr--;
	
    /* update last length samples of adptv_cbk[n] with scaled vectors */
    for (i = 0; i < length; i++)
		*r1ptr--= (*p_ptr++);
}

Void  update_acbk(Int32 * adptv_cbk, Int32 * pvect, Int16 length)
{
    Int16 i;
    Int32 * r1ptr, *r2ptr;
    Int32 * p_ptr = pvect;
	
    /* shift adptv_cbk vector by length samples */
    r1ptr = adptv_cbk + MAXLAG;
    r2ptr = adptv_cbk + (MAXLAG - length);
    for (i = 0; i < MAXLAG - length; i++)
		*r1ptr--= *r2ptr--;
	
    /* update last length samples of adptv_cbk[n] with scaled vectors */
    for (i = 0; i < length; i++)
		*r1ptr--= (*p_ptr++);
	
}
