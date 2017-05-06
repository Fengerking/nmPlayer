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

/*****************************************************************************
* File:           cb.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    codebook search routines
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

//#include<stdio.h>
//#include<stdlib.h>
#include"qcelp.h"
#include"qcelp13.h"
#include"qc_tabl.h"
//#include<math.h>
extern Int16 CBSF[NUMRATES];
extern Int16 CODEBOOK_16[CBLENGTH];
extern Int16 CODEBOOK_FULL_16[CBLENGTH];
extern Int16 BPFPARAMS_16[BPFORDER + 1];
/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  make_cw(I16 * vector, Int16 length,  CBPARAMS *cb_params, Int16 type)
Void  make_cw(Int16 * vector, Int16 length,  CBPARAMS *cb_params, Int16 type)
{
	Int16 k;
	//Int32 unsgn_k;
	Int32 temp1;
	Int32 temp2;
	Int32 ar;
	Int32 temp3;

	if(type==NOISE)
	{
		for (k = 0; k < length; k++)
		{
			cb_params->seed = ((521L * (Int32)cb_params->seed + 259L) & (0xffffL));
			temp1 = ((((Int32)cb_params->seed + 0x7fffL) & 0xffffL) - 0x7fffL);
			ar = (Int32)0x2bf5L * (Int32)temp1 << 2; /* Q26 * Q15 = Q42 (with left shift) */
			temp3 = ar;
			//SAT(ar);
			temp2 = (Int32)ar; // Q42 number
			temp3 = L_add_MACRO(temp3, 0x00008000l); // rounding
			temp1 = (Int16)(temp3 >> 16); // Q26 number
			ar = (Int32)temp1 * (Int32)cb_params->G_16 >> 2; /* Q26 * Q4 = Q31 (with left shift) */
			ar = L_add_MACRO(ar, 0x00008000l); // rounding
			vector[k] = (Int16)(ar >> 16);
		}
	}
	else
	{
		for (k = 0; k < length; k++)
		{
			ar =(Int32)CODEBOOK_16[ (CBLENGTH - cb_params->i + k) % CBLENGTH] * (Int32)cb_params->G_16 << 1; /* Q11 * Q4 = Q29 (with left shift) */
			ar = L_add_MACRO(ar, 0x00008000l); // rounding
			vector[k] = (Int16)(ar >> 16);
		}
	}	
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
Void  make_qc13_qtr_cw(Int16 * vector, Int16 length,  CBPARAMS *cb_params, Int16 *hist)
{
	Int16 k;
	//Int32 unsgn_k;
	Int16 temp1;
	Int32 temp2;
	Int32 ar,temp3;
	for (k = 0; k < length; k++)
	{
		cb_params->seed = ((521L * (Int32)cb_params->seed + 259L) & (0xffffL));
		vector[k] = (Int16)((((Int32)cb_params->seed + 0x7fffL) & (0xffffL)) - 0x7fffL);
	}
	fir_filt(vector, BPFPARAMS_16, hist, length, BPFORDER);
	for (k = 0; k < length; k++)
	{
		ar = (Int32)0x2bf5L * (Int32)vector[k] << 2; 
		temp3 = ar;
		temp2 = ar; 
		temp3 = L_add_MACRO(temp3, 0x00008000l); // rounding
		temp1 = (Int16)(temp3 >> 16); 
		ar = (Int32)temp1 * (Int32)cb_params->G_16 >> 2;		
		ar = L_add_MACRO(ar, 0x00008000l); // rounding
		vector[k] = (Int16)(ar >> 16);
	}
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  make_qc13_full_cw(I16 * vector, Int16 length,  CBPARAMS *cb_params)
Void  make_qc13_full_cw(Int16 * vector, Int16 length,  CBPARAMS *cb_params)
{
	Int16 k;
	Int32 ar;
	for (k = 0; k < length; k++)
	{
		ar = (Int32)CODEBOOK_FULL_16[ (CBLENGTH - cb_params->i + k) % CBLENGTH] * (Int32)cb_params->G_16 << 1; /* Q11 * Q4 = Q29 (with left shift) */
		ar = L_add_MACRO(ar, 0x00008000l); // rounding
		vector[k] = (Int16)(ar >> 16);
	}
}

