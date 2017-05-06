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
/* quantize.c - quantiztation routines */

/*****************************************************************************
* File:           quant.cpp
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

#define MATCH_1617  1

extern Int16 FG[73];

extern Int16 GA_16[73];

extern  Int16 NUM_LSP_QLEVELS[NUMRATES];

extern  Int16 LSP_DPCM_DECAY_16[NUMRATES];

extern  Int16 MIN_DELTA_LSP_16[NUMRATES][LPCORDER];

extern  Int16 MAX_DELTA_LSP_16[NUMRATES][LPCORDER];


extern  Int16 DIFF_DELTA_LSP_16[NUMRATES][LPCORDER];

extern  Int16 QG[NUMRATES][4];

extern Int16 QCODE_DIV_NUM_LEVELS_16[4][16];

#define LSP_BIAS_16 ((Int16)1489)


/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  unquantize_lsp(Int16 rate, I16 *qlsp, I16 *pred, Int16 *qcode)
Void  unquantize_lsp(Int16 rate, Int16 *qlsp, Int16 *pred, Int16 *qcode)
{
    Int16 i;
    Int16 diff;
    Int16 min;
    Int16 temp;
    Int32 arA = 0;
    Int32 arB;
    Int16 decay;
	
    temp = (Int16)2048; // Q11 number
	
    decay =  LSP_DPCM_DECAY_16[rate]; // Q15 number
	
    for (i = 0; i < LPCORDER; i++)
    {
        if(rate==QC13_EIGHTH)
        {
            if (qcode[i] == 1)
                arA = (Int32)328 << 16;    
            else
                arA = (Int32)(-328) << 16;   
        }
        else
        {
            diff = DIFF_DELTA_LSP_16[rate][i];
            min  = MIN_DELTA_LSP_16[rate][i];
            switch(rate)
            {
            case 0:
                arA = 0l;
                break;
            case 1:
            case 2:
                arA = (Int32)diff * (Int32)(QCODE_DIV_NUM_LEVELS_16[0][qcode[i]]) << 1; /* Q14 * Q15 = Q30 */
                break;
				
            case 3:
                arA = (Int32)diff * (Int32)(QCODE_DIV_NUM_LEVELS_16[1][qcode[i]]) << 1; /* Q14 * Q15 = Q30 */
                break;
				
            case 4:
                arA = (Int32)diff * (Int32)(QCODE_DIV_NUM_LEVELS_16[3][qcode[i]]) << 1; /* Q15 * Q15 = Q31 */
                break;
            }
            arA += (Int32)min << 15;
            arA <<= 1;
        }
		
#if MATCH_1617
        arA = L_add(arA, ((Int32)pred[i] << 16));     // err + pred[i]
        arB = arA;          // save arA for qlsp[i] computation
        pred[i] = (Int16)(arB >> 16); // Q18 number (store in pred[i] temporarily)
        arB = (Int32)decay * (Int32)pred[i] << 1; /* Q15 * Q18 = Q34 (with left shift) */
        arB = L_add(arB, 0x00008000l);
        pred[i] = (Int16)(arB >> 16); // Q18 number
		
        //arA >>= (Int16)3; // bring down to Q31
        arA = L_add(arA, ((Int32)temp * (Int32)INV_HALF_LPCORDER << 1)); /* Q11 * Q19 = Q31 */
        temp += 2048;
        arA = L_add(arA, 0x00008000l); // rounding
        qlsp[i] = (Int16)(arA >> 16);
#else
        arA = L_add(arA, ((Int32)decay * (Int32)pred[i] << 1)); /* Q15 * Q18 = Q34 (with left shift) */
        arB = arA;
        arB = L_add(arB, 0x00008000l); // rounding (DJY)
        pred[i] = (Int16)(arB >> 16); // Q18 number
		
        //arA >>= 3; // bring down to Q31
        arA = L_add(arA, ((Int32)temp * (Int32)INV_HALF_LPCORDER << 1)); /* Q11 * Q19 = Q31 */
        temp += 2048;
        arA = L_add(arA, 0x00008000l); // rounding (DJY)
        qlsp[i] = (Int16)(arA >> 16);
#endif
    }
    check_lsp_stab(qlsp);
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
Void  check_lsp_stab(Int16 *qlsp)
{
    Int16 i;
    Int16 temp;
    Int32 ar;
	
    if (qlsp[0] < LSP_SPREAD_FACTOR_16) qlsp[0] = LSP_SPREAD_FACTOR_16;
	
    for (i = 1; i < LPCORDER; i++)
    {
        ar = (Int32)qlsp[i] << 16;
        ar -= (Int32)qlsp[i-1] << 16;
        temp = (Int16)(ar >> 16);
        if (temp < LSP_SPREAD_FACTOR_16)
        {
            ar = (Int32)qlsp[i-1] << 16;
            ar += (Int32)LSP_SPREAD_FACTOR_16 << 16;
            qlsp[i] = (Int16)(ar >> 16);
        }
    }
	
    ar = (Int32)16384 << 16;
    ar -= (Int32)LSP_SPREAD_FACTOR_16 << 16;
    temp = (Int16)(ar >> 16);
    if (qlsp[LPCORDER - 1] > temp)
    {
        qlsp[LPCORDER - 1] = temp;
    }
	
    for (i = LPCORDER - 2; i >= 0; i--)
    {
        ar = (Int32)qlsp[i+1] << 16;
        ar -= (Int32)qlsp[i] << 16;
        temp = (Int16)(ar >> 16);
		
        if (temp < LSP_SPREAD_FACTOR_16)
        {
            ar = (Int32)qlsp[i+1] << 16;
            ar -= (Int32)LSP_SPREAD_FACTOR_16 << 16;
            qlsp[i] = (Int16)(ar >> 16);
        }
    }
}


Void  unquantize_i( Int16 *i, Int16 *qcode_i)
{
    *i = *qcode_i;
}

