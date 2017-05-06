/**********************************************************************/
/* QCELP Variable Rate Speech Codec - Simulation of TIA IS96-A, service */
/*     option one for TIA IS95, North American Wideband CDMA Digital  */
/*     Cellular Telephony.                                            */
/*                                                                    */
/* (C) Copyright 1993, QUALCOMM Incorporated                          */
/* QUALCOMM Incorporated                                              */
/* 10555 Sorrento Valley Road                                         */
/* San Diego, CA 92121                                                */
/*                                                                    */
/* Note:  Reproduction and use of this software for the design and    */
/*     development of North American Wideband CDMA Digital            */
/*     Cellular Telephony Standards is authorized by                  */
/*     QUALCOMM Incorporated.  QUALCOMM Incorporated does not         */
/*     authorize the use of this software for any other purpose.      */
/*                                                                    */
/*     The availability of this software does not provide any license */
/*     by implication, estoppel, or otherwise under any patent rights */
/*     of QUALCOMM Incorporated or others covering any use of the     */
/*     contents herein.                                               */
/*                                                                    */
/*     Any copies of this software or derivative works must include   */
/*     this and all other proprietary notices.                        */
/**********************************************************************/

/*****************************************************************************
* File:           lpc.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    linear predictive coding functions
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

//#include<math.h>
#include "qcelp.h"
#include "qcelp13.h"
//#include "qc13_deb.h"


extern void durbin(
				   Int32 *L_R, 
				   Int16 *rc, 
				   Int16 *pc, 
				   Int16 order
				   );

#define  BWE_FACTOR_16  ((Int16) 32385) /* .9883 q15*/
//#define MPY32_16(lvar, svar) ((svar*(I16)((I40)lvar>>(Int16)16))+(I40)(((Int32)svar*((Int32)lvar&(Int32)0xffffl))>>(Int16)15))


//extern Int16 PITCHSF[NUMRATES];
//extern Int16 ham_window[LPCSIZE];
//extern Int16 PITCHSF_16[NUMRATES];
extern Int16 FSIZE_DIV_LOOPS_16[NUMRATES];

/****************************************************************************
Integer Subroutines
****************************************************************************/
void  interp_lpcs (
				   Int16 count,
				   Int16 *prev_lsp,
				   Int16 *curr_lsp,
				   Int16 *lpc_not_wghted,             /* unweighted lpc coefficients */
				   Int16 *lpc_wghted,                 /* weighted lpc coefficients */
				   Int16 *bright,
				   Int16 loops,
				   Int16 per_wght_factor            /* percept weight factor */
				   )
{
    Int16 j;
    Int16 wght_factor;
    Int16 tmp_lsp[LPCORDER];
    Int16 current_center;
    Int16 interp_factor;
    Int32 ar;
	
    /*---------------------------------------------------------------------*/
    /* current_center = (Float) (count + 0.5) * (Float) (FSIZE / loops);   */
    /*---------------------------------------------------------------------*/
	
    ar = (Int32)count << 13;
    ar += 4096; // 0.5 in Q13 format
    current_center = (Int16)ar;
	
    /* Q13 * Q7 = Q21 (with left shift) */
    ar = (Int32)current_center * (Int32)FSIZE_DIV_LOOPS_16[loops];
    ar <<= 3;
    ar += (Int32)FSIZE_16 << 16;
    ar -= (Int32)LPC_CENTER_16 << 16;
    interp_factor = (Int16)(ar >> 16);
	
    /* Q7 * Q15 = Q23 (with left shift) */
    ar = (Int32)interp_factor * (Int32)INV_FSIZE_16;
    ar = L_shl2(ar, 9);
    ar = L_add(ar, 0x00008000l);    //can't be modified
    //ar += 0x00008000l;
    interp_factor = (Int16)(ar >> 16);
	
    bright[count] = 0;
    for (j = 0; j < LPCORDER; j++)
    {
        //following can't be modified--shrchen
		ar = (Int32)prev_lsp[j] << 16;
		ar = L_add(ar, (Int32)curr_lsp[j] * (Int32)interp_factor << 1); /* Q15 * Q15 = Q31 (with left shift) */
        ar = L_sub(ar, (Int32)prev_lsp[j] * (Int32)interp_factor << 1); /* Q15 * Q15 = Q31 (with left shift) */
        ar = L_add(ar, 0x00008000l);
		tmp_lsp[j] = (Int16)(ar >> 16);
		ar >>= 2;
		ar = L_add(ar, (Int32)bright[count] << 16);
		ar = L_add(ar, 0x00008000l);
		bright[count] = (Int16)(ar >> 16);
    }
	
	//    ar = (Int32)bright[count] * (Int32)INV_LPCORDER << 1; /* Q13 * Q15 = Q29 (no left shift) */
	//    ar <<= 2;
	//    ar = L_add(ar, 0x00008000l);
	//    bright[count] = (Int16)(ar >> 16);
    ar = (Int32)bright[count] * (Int32)INV_LPCORDER; /* Q13 * Q15 = Q29 (no left shift) */
    ar <<= 2;
    ar += 0x00004000l;
    bright[count] = (Int16)(ar >> 15);
	
    if (bright[count] < 7864) // 0.24 in Q15 format
    {
		bright[count] = 8192; // 0.25 in Q15 format
    }
    else if (bright[count] > 8520) // 0.26 in Q15 format
    {
		bright[count] = (Int16)57344L; // -0.25 in Q15 format
    }
    else
    {
		//	ar = (Int32)bright[count] << 16;
		//	ar = L_sub(ar, (Int32)8192 << 16); // 0.25 in Q15 format
		//    ar = L_add(ar, 0x00008000l);
		//	bright[count] = (Int16)(ar >> 16);
		//	ar = (Int32)bright[count] * 39936L << 1; /* Q15 * Q10 = Q26 (with left shift) */
		//	ar = L_shl(ar, 5);
		//	//ar <<= 5;
		//	ar = L_add(ar, 0x00008000l);
		//	bright[count] = (Int16)(ar >> 16);
		ar = (Int32)bright[count] << 16;
		ar -= (Int32)8192 << 16; // 0.25 in Q15 format
        ar += 0x00008000l;
		bright[count] = (Int16)(ar >> 16);
		ar = (Int32)bright[count] * 39936L;
		//ar <<= 5;
		ar += 0x00000200l;
		bright[count] = (Int16)(ar >> 11);
    }
	
    lsp2lpc(tmp_lsp, lpc_not_wghted, LPCORDER);
	
    wght_factor = BWE_FACTOR_16;
    for (j = 0; j < LPCORDER; j++)
    {
		//    	ar = (Int32)lpc_not_wghted[j] * (Int32)wght_factor << 1; /* Q15 * Q15 = Q31 (with left shift) */
		//    	ar += 0x00008000l;
		//    	lpc_not_wghted[j] = (Int16)(ar >> 16);
		//    
		//    	ar = (Int32)wght_factor * (Int32)BWE_FACTOR_16 << 1; /*Q15 * Q15 = Q31 (with left shift) */
		//    	ar += 0x00008000l;
		//    	wght_factor = (Int16)(ar >> 16);
		ar = (Int32)lpc_not_wghted[j] * (Int32)wght_factor;
		ar += 0x00004000l;
		lpc_not_wghted[j] = (Int16)(ar >> 15);
		
		ar = (Int32)wght_factor * (Int32)BWE_FACTOR_16;
		ar += 0x00004000l;
		wght_factor = (Int16)(ar >> 15);
    }
	
//    wght_factor = per_wght_factor;
//    for (j = 0; j < LPCORDER; j++)
//    {
//		//    	ar = (Int32)lpc_not_wghted[j] * (Int32)wght_factor << 1; /* Q15 * Q15 = Q31 (with left shift) */
//		//    	ar += 0x00008000l;
//		//    	lpc_wghted[j] = (Int16)(ar >> 16);
//		//    
//		//    	ar = (Int32)wght_factor * (Int32)per_wght_factor << 1; /*Q15 * Q15 = Q31 (with left shift) */
//		//    	ar += 0x00008000l;
//		//    	wght_factor = (Int16)(ar >> 16);
//		
//		ar = (Int32)lpc_not_wghted[j] * (Int32)wght_factor;
//		ar += 0x00004000l;
//		lpc_wghted[j] = (Int16)(ar >> 15);
//		
//		ar = (Int32)wght_factor * (Int32)per_wght_factor;
//		ar += 0x00004000l;
//		wght_factor = (Int16)(ar >> 15);
//    }
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
void  wght_lpcs (
				 Int16 *lpc,             /* (i/o) pointer to lpc-coefficients */
				 Int16 factor            /* (i)   weighting factor */
				 )
				 
{
    Int16 j;
    Int16 wght_factor;
    Int32 ar;
	
    wght_factor = factor;
    for (j = 0; j < LPCORDER; j++)
    {
		//    	ar = (Int32)lpc[j] * (Int32)wght_factor << 1; // Q12 * Q15 = Q28 with left shift
		//    	ar = L_add(ar, 0x00008000l); // rounding
		//    	lpc[j] = (Int16)(ar >> 16);
		//    
		//    	ar = (Int32)wght_factor * (Int32)factor << 1; // Q15 * Q15 = Q31 with left shift
		//    	ar = L_add(ar, 0x00008000l); // rounding
		//    	wght_factor = (Int16)(ar >> 16);
		ar = (Int32)lpc[j] * (Int32)wght_factor;
		ar += 0x00004000l; // rounding
		lpc[j] = (Int16)(ar >> 15);
		
		ar = (Int32)wght_factor * (Int32)factor;
		ar += 0x00004000l; // rounding
		wght_factor = (Int16)(ar >> 15);
    }
}

