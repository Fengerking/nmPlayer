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
/* lsp.c - lsp routines */

/*****************************************************************************
* File:           lsp.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    lsp routines
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

//#include<math.h>
#include"qcelp.h"
#include"qcelp13.h"


extern Int16 PITCHSF[NUMRATES];
extern Int16 CosineTable_16[];
extern Int16 ERA_LSP_DEC_16[4];

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  lsp2lpc(I16 *Lsp, I16 *Lpc, Int16 order)
Void  lsp2lpc(Int16 *Lsp, Int16 *Lpc, Int16 order)
{
    Int16 i;
    Int16 j;
    Int32 unsgn_k;
    Int16 temp1,temp2;
//    Int16 Floor;
//    I16 Fac[(LPCORDER/2)-2] = {FtoI16(((Float)1.0),12),FtoI16(((Float)0.5),12),FtoI16(((Float)0.25),12)};
//    Int16 Fac[(LPCORDER/2)-2] = {4096, 2048, 1024};
//    Int32 P[LPCORDER/2+1];
//    Int32 Q[LPCORDER/2+1];
    Int16 Fac[(LPCORDER>>1)-2] = {4096, 2048, 1024};
    Int32 P[(LPCORDER>>1) +1];
    Int32 Q[(LPCORDER>>1) +1];
    Int32 arA,arB;
    Int16 Lpc_Overflow = (Int16)0;
    Int16 raw_factor, factor;

    for (i=0; i < order; i++)
    {
//        arA = (Int32)Lsp[i] << 10;
//        Floor = (Int16)(arA >> 16);
//        j = Floor;
//        arA = L_sub(arA, (Int32)Floor << 16);
//        arA <<= 15;
//        arA = L_add(arA, 0x00008000l); // rounding
//        temp1 = (Int16)(arA >> 16);
//
//        arA = (Int32)CosineTable_16[j+1] << 16;
//        arA = L_sub(arA, (Int32)CosineTable_16[j] << 16);
//        arA = L_add(arA, 0x00008000l); // rounding
//        temp2 = (Int16)(arA >> 16);
//
//        arA = 0l;
//        arA = L_sub(arA, (Int32)CosineTable_16[j] << 16);
//        arA = L_sub(arA, (Int32)temp2 * (Int32)temp1 << 1);
//        arA = L_add(arA, 0x00008000l); // rounding
//        Lsp[i] = (Int16)(arA >> 16);

////1
//        arA = (Int32)Lsp[i] << 10;
//        Floor = (Int16)(arA >> 16);
//        j = Floor;
//        arA -= (Int32)Floor << 16;
//        arA <<= 15;
//        arA += 0x00008000l; // rounding
//        temp1 = (Int16)(arA >> 16);
////2
//        arA = (Int32)Lsp[i] << 10;
//        j = Lsp[i] >> 6;
//        arA -= (Int32)j << 16;
//        arA <<= 15;
//        arA += 0x00008000l; // rounding
//        temp1 = (Int16)(arA >> 16);
//3
        arA = (Int32)Lsp[i];
        j = Lsp[i] >> 6;
        arA = (arA & 0x3f) << 25;
        arA += 0x00008000l; // rounding
        temp1 = (Int16)(arA >> 16);

        arA = (Int32)CosineTable_16[j+1] << 16;
        arA -= CosineTable_16[j] << 16;
        arA += 0x00008000l; // rounding
        temp2 = (Int16)(arA >> 16);

        //can't modify following 4
        arA = L_sub(0, (Int32)CosineTable_16[j] << 16);
        arA = L_sub(arA, (Int32)temp2 * (Int32)temp1 << 1);
        arA = L_add(arA, 0x00008000l); // rounding
        Lsp[i] = (Int16)(arA >> 16);
    }


/* Init P and Q, All entries in Q13 format */

    P[0] = 134217728;                // 0.5 Q28
    arA = (Int32)Lsp[0] << 13;
    arA += (Int32)Lsp[2] << 13;
    P[1] = arA;

//    arA = (Int32)Lsp[0] * (Int32)Lsp[2] << 1; /* Q15 * Q15 = Q31 (with left shift) */
//    arA >>= 2; /* 2 * Lsp[0] * Lsp[2] in Q28 format */
//    arA = L_add(arA, (Int32)4096 << 16);
//    P[2] = arA;
    arA = (Int32)Lsp[0] * (Int32)Lsp[2];
    arA >>= 1;
    arA += (Int32)4096 << 16;
    P[2] = arA;

//    Q[0] = 134217728;
//    arA = (Int32)Lsp[1] << 13;
//    arA = L_add(arA, (Int32)Lsp[3] << 13);
//    Q[1] = arA;
    Q[0] = 134217728;
    arA = (Int32)Lsp[1] << 13;
    arA += (Int32)Lsp[3] << 13;
    Q[1] = arA;

//    arA = (Int32)Lsp[1] * (Int32)Lsp[3] << 1; /* Q15 * Q15 = Q31 (with left shift) */
//    arA >>= (Int16) 2;  /*  2 * Lsp[1] * Lsp[3] in Q28 format */
//    arA = L_add(arA, (Int32)4096 << 16); /* 1.0 in Q28 format */
//    Q[2] = arA;
    arA = (Int32)Lsp[1] * (Int32)Lsp[3];
    arA >>= 1;
    arA += (Int32)4096 << 16; /* 1.0 in Q28 format */
    Q[2] = arA;

//    for (i=2; i < order/2; i++)
    for (i=2; i < (order>>1); i++)
    {
/* Compute coefficient (i+1) */
//       arA = P[i-1];
//       temp1 = (Int16)(P[i] >> 16);  // TCMC_PPA 14.07.97
//       arA = L_add(arA, (Int32)temp1 * (Int32)Lsp[2*i+0] << 1); /* Q12 * Q15 = Q28 (with left shift) */
//       unsgn_k = (Int32)(P[i] & (0x0000ffffl));	// TCMC_PPA 14.07.97
//	   arB = (Int32)(unsgn_k * Lsp[2*i+0]);
//       arB >>= 15;
//       arA = L_add(arA, arB);
//       P[i+1] = arA;
//       arA = Q[i-1];
//       temp1 = (Int16)(Q[i] >> 16);  // TCMC_PPA 14.07.97
//       arA = L_add(arA, (Int32)temp1 * (Int32)Lsp[2*i+1] << 1); /* Q12 * Q15 = Q28 (with left shift) */
//       unsgn_k = (Q[i] & (0x0000ffffl)); // TCMC_PPA 14.07.97
//	   arB = (Int32)(unsgn_k * Lsp[2*i+1]); 
//	   arB >>= (Int16)15;
//	   arA = L_add(arA, arB);
//       Q[i+1] = arA;
       arA = P[i-1];
       temp1 = (Int16)(P[i] >> 16);  // TCMC_PPA 14.07.97
       arA += (Int32)temp1 * (Int32)Lsp[2*i+0] << 1; /* Q12 * Q15 = Q28 (with left shift) */
       unsgn_k = (Int32)(P[i] & (0x0000ffffl));	// TCMC_PPA 14.07.97
	   arB = (Int32)(unsgn_k * Lsp[2*i+0]);
       arB >>= 15;
       arA += arB;
       P[i+1] = arA;
       arA = Q[i-1];
       temp1 = (Int16)(Q[i] >> 16);  // TCMC_PPA 14.07.97
       arA += (Int32)temp1 * (Int32)Lsp[2*i+1] << 1; /* Q12 * Q15 = Q28 (with left shift) */
       unsgn_k = (Q[i] & (0x0000ffffl)); // TCMC_PPA 14.07.97
	   arB = (Int32)(unsgn_k * Lsp[2*i+1]); 
	   arB >>= (Int16)15;
	   arA += arB;
       Q[i+1] = arA;

/* Compute coefficients i, i-1, ..., 2 */

       for (j=i; j >= 2; j--)
       {
//           arA = P[j];
//           arA = L_add(arA, P[j-2]);
//           arA >>= 1; /* 0.5 * (P[j] + P[j-2]) */
//    	   temp1 = (Int16)(P[j-1] >> 16);    // TCMC_PPA 14.07.97
//           arA = L_add(arA, (Int32)temp1 * (Int32)Lsp[2*i+0] << 1); /* Q12 * Q15 = Q28 (with left shift) */
//    	   unsgn_k = (Int32)(P[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
//    	   arB = (Int32)(unsgn_k * Lsp[2*i+0]);  
//		   arB >>= (Int16)15;
//		   arA = L_add(arA, arB);
//           P[j] = arA;
           arA = P[j];
           arA += P[j-2];
           arA >>= 1; /* 0.5 * (P[j] + P[j-2]) */
    	   temp1 = (Int16)(P[j-1] >> 16);    // TCMC_PPA 14.07.97
           arA += (Int32)temp1 * (Int32)Lsp[2*i+0] << 1; /* Q12 * Q15 = Q28 (with left shift) */
    	   unsgn_k = (Int32)(P[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
    	   arB = (Int32)(unsgn_k * Lsp[2*i+0]);  
		   arB >>= (Int16)15;
		   arA += arB;
           P[j] = arA;

//           arA = Q[j];
//           arA = L_add(arA, Q[j-2]);
//           arA >>= 1; /* 0.5 * (Q[j] + Q[j-2]) */
//    	   temp1 = (Int16)(Q[j-1] >> 16);  // TCMC_PPA 14.07.97
//           arA = L_add(arA, (Int32)temp1 * (Int32)Lsp[2*i+1] << 1); /* Q12 * Q15 = Q28 (with left shift) */
//    	   unsgn_k = (Int32)(Q[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
//    	   arB = (Int32)(unsgn_k * Lsp[2*i+1]);
//           arB >>= (Int16)15;
//           arA = L_add(arA, arB);
//           Q[j] = arA;
           arA = Q[j];
           arA += Q[j-2];
           arA >>= 1; /* 0.5 * (Q[j] + Q[j-2]) */
    	   temp1 = (Int16)(Q[j-1] >> 16);  // TCMC_PPA 14.07.97
           arA += (Int32)temp1 * (Int32)Lsp[2*i+1] << 1; /* Q12 * Q15 = Q28 (with left shift) */
    	   unsgn_k = (Int32)(Q[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
    	   arB = (Int32)(unsgn_k * Lsp[2*i+1]);
           arB >>= (Int16)15;
           arA += arB;
           Q[j] = arA;
       }

/* Compute coefficients 1, 0 */
//       arA = P[0]; /* 0.5 * P[0] in Q28 */
//       arA >>= 1;
//       P[0] = arA;
       P[0] = P[0] >> 1;

//       arA = Q[0]; /* 0.5 * Q[0] in Q28 */
//       arA >>= 1;
//       Q[0] = arA;
       Q[0] = Q[0] >> 1;

//       arA = P[1];
//       arA = L_add(arA, (Int32)Lsp[2*i+0] * (Int32)Fac[i-2] << 1); /* Q15 * Q12 = Q28 (with left shift) */
//       arA >>= 1; /* 0.5 *(P[1] + Lsp[]*Fac[]) in Q28 */
//       P[1] = arA;
       arA = P[1];
       arA += (Int32)Lsp[2*i+0] * (Int32)Fac[i-2] << 1; /* Q15 * Q12 = Q28 (with left shift) */
       P[1] = arA >> 1;

//       arA = Q[1];
//       arA = L_add(arA, (Int32)Lsp[2*i+1] * (Int32)Fac[i-2] << 1); /* Q15 * Q12 = Q28 (with left shift) */
//       arA >>= 1; /* 0.5 *(Q[1] + Lsp[]*Fac[]) in Q28 */
//       Q[1] = arA;
       arA = Q[1];
       arA += (Int32)Lsp[2*i+1] * (Int32)Fac[i-2] << 1; /* Q15 * Q12 = Q28 (with left shift) */
       Q[1] = arA >> 1;
    }

//    for (i=0; i < order/2; i++)
    for (i=0; i < (order>>1); i++)
    {
//      arA = Q[i];
//      arA = L_sub(arA, Q[i+1]);
//      arA = L_sub(arA, P[i]);
//      arA = L_sub(arA, P[i+1]);
//      arB = arA;
//      arA = L_shl(arA, 3);
//      arA = L_add(arA, 0x00008000l);
//      Lpc[i] = (Int16)(arA >> 16);
//      if (arB < 0) arB = -arB;
//      if (arB > 0xfffffffl)
//          Lpc_Overflow = 1;
      arA = Q[i] - Q[i+1] - P[i] - P[i+1];
      arB = arA;
      arA += 0x00001000l;
      Lpc[i] = (Int16)(arA >> 13);
      if (arB < 0) arB = -arB;
      if (arB > 0xfffffffl)
          Lpc_Overflow = 1;

//      arA = Q[i + 1];
//      arA = L_sub(arA, Q[i]);
//      arA = L_sub(arA, P[i]);
//      arA = L_sub(arA, P[i+1]);
//      arB = arA;
//      arA = L_shl(arA, 3);
//      arA = L_add(arA, 0x00008000l);
//      Lpc[order-1-i] = (Int16)(arA >> 16);
//      if (arB < 0) arB = -arB;
//      if (arB > 0xfffffffl)
//          Lpc_Overflow = 1;
      arA = Q[i + 1] - Q[i] - P[i] - P[i+1];
      arB = arA;
      arA += 0x00001000l;
      Lpc[order-1-i] = (Int16)(arA >> 13);
      if (arB < 0) arB = -arB;
      if (arB > 0xfffffffl)
          Lpc_Overflow = 1;
    }

    if (Lpc_Overflow == (Int16)1)
    {
        arB = 0x10000000l;
//        for (i=0; i < order/2; i++)
        for (i=0; i < (order>>1); i++)
        {
//            arA = Q[i];
//            arA = L_sub(arA, Q[i+1]);
//            arA = L_sub(arA, P[i]);
//            arA = L_sub(arA, P[i+1]);
//            Lpc[i] = (Int16)(arA >> 16);
//            if (arA < 0) arA = -arA;
//            if (arA > arB)
//                arB = arA;
            arA = Q[i] - Q[i+1] - P[i] - P[i+1];
            Lpc[i] = (Int16)(arA >> 16);
            if (arA < 0) arA = -arA;
            if (arA > arB)
                arB = arA;

            arA = Q[i + 1] - Q[i] - P[i] - P[i+1];
            Lpc[order-1-i] = (Int16)(arA >> 16);
            if (arA < 0) arA = -arA;
            if (arA > arB)
                arB = arA;
        }

        arB -= 0x10000000l;
        arB = -arB;
        arB <<= 3;
        arB += 0x7fffffffl;
        raw_factor = (Int16)(arB > 16);

        factor = raw_factor;

        for (i=0; i < LPCORDER; i++)
        {
//            arA = (Int32)Lpc[i] * (Int32)factor << 1; // Q9 * Q15 = Q25
//            arA <<= (Int16)3; // Q28
//            Lpc[i] = (Int16)(arA >> 16);
//            arA = (Int32)raw_factor * (Int32)factor << 1; // Q15 * Q15 = Q31
//            factor = (Int16)(arA >> 16);
            arA = (Int32)Lpc[i] * (Int32)factor;
            Lpc[i] = (Int16)(arA >> 12);
            arA = (Int32)raw_factor * (Int32)factor;
            factor = (Int16)(arA >> 15);
        }
    }
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Int16  smooth_lsps(Int16 rate, I16 *prev_lsp, I16 *curr_lsp, Int16 *rate_cnt)
Int16  smooth_lsps(Int16 rate, Int16 *prev_lsp, Int16 *curr_lsp, Int16 *rate_cnt)
{
    Int16 i;
    Int16 smooth;
    Int16 loops;
    Int32 ar;

    if (rate == BLANK || rate == QC13_BLANK)
    {
        loops = 0;
        return(loops);
    }

    smooth = (Int16)4096;

    if ((rate == QUARTER) || (rate == EIGHTH) || (rate == QC13_EIGHTH))
    {
        *rate_cnt += (Int16) 1;
        if (*rate_cnt >= 10)
        {
            smooth = (Int16)29491;
        }
    }
    else
    {
        *rate_cnt = 0;
    }

    if (rate < QC13_BLANK || rate == QC13_EIGHTH)
    {
        if (rate != FULL)
            for (i = 0; i < LPCORDER; i++)
            {
//                ar = (Int32)curr_lsp[i] << 16;
//                ar += (Int32)smooth * (Int32)prev_lsp[i] << 1;/* Q15 * Q15 = Q31 (with left shift) */
//                ar -= (Int32)smooth * (Int32)curr_lsp[i] << 1;/* Q15 * Q15 = Q31 (with left shift) */
//                curr_lsp[i] = (Int16)(ar >> 16);
                ar = (Int32)curr_lsp[i] << 15;
                ar += (Int32)smooth * (Int32)prev_lsp[i];
                ar -= (Int32)smooth * (Int32)curr_lsp[i];
                curr_lsp[i] = (Int16)(ar >> 15);
            }
    }

    if (rate == EIGHTH || rate == QC13_EIGHTH)
    {
        loops = 1;
    }
    else
    {
        loops = (Int16)PITCHSF[rate];
    }
    return(loops);

}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  gen_qlsp(I16 *qlsp, I16 *lsp_pred, Int16 err_cnt, I16 *last_qlsp) // Used in DecErase
Void  gen_qlsp(Int16 *qlsp, Int16 *lsp_pred, Int16 err_cnt, Int16 *last_qlsp) // Used in DecErase
{
    Int16 i;
    Int32 arA, arB;

    arB = 0l;
    for (i = 0; i < LPCORDER; i++)
    {
//        arA = (Int32)lsp_pred[i] * (Int32)ERA_LSP_DEC_16[err_cnt] << 1; // Q18 * Q15 = Q34 (with left shift)
//        arA = L_add(arA, 0x00008000l);
//        qlsp[i] = (Int16)(arA >> 16);
//        lsp_pred[i] = qlsp[i]; // Q18 value
//        arA = (Int32)qlsp[i] * (Int32)29696 << 1; // Q18 * Q15 = Q34 (with left shift)
        arA = (Int32)lsp_pred[i] * (Int32)ERA_LSP_DEC_16[err_cnt];
        arA += 0x00004000l;
        qlsp[i] = (Int16)(arA >> 15);
        lsp_pred[i] = qlsp[i]; // Q18 value
        arA = (Int32)qlsp[i] * (Int32)29696 << 1; // Q18 * Q15 = Q34 (with left shift)

//        /* add in bias */
//        arB = L_add(arB, (Int32)QC13_INV_HALF_LPCORDER << 16); // Q31 value        
//        arA >>= 3; // bring down to Q31
//        arA = L_add(arA, arB);
//        arA = L_add(arA, 0x00008000l); // rounding
//        qlsp[i] = (Int16)(arA >> 16); // Q15 value
        /* add in bias */
        arB += (Int32)QC13_INV_HALF_LPCORDER << 16; // Q31 value
        arA >>= 3; // bring down to Q31
        arA += arB;
        arA += 0x00008000l; //rounding
        qlsp[i] = (Int16)(arA >> 16); // Q15 value
    }

    /* Enforce LSP separation */
    if (qlsp[0] < LSP_SPREAD_FACTOR_16) qlsp[0] = LSP_SPREAD_FACTOR_16;

    for (i = 1; i < LPCORDER; i++)
    {
        arA = qlsp[i]; // Q15
        arA -= qlsp[i - 1]; // Q15
        if (arA < LSP_SPREAD_FACTOR_16)
        {
            arA = qlsp[i -1]; // Q15
            arA += LSP_SPREAD_FACTOR_16;
            qlsp[i] = (Int16)arA;
//            qlsp[i] = qlsp[i - 1] + LSP_SPREAD_FACTOR;
        }
    }

    if (qlsp[LPCORDER - 1] > 16056)
    {
        qlsp[LPCORDER - 1] = 16056;
    }

    for (i = LPCORDER - 2; i >= 0; i--)
    {
        arA = qlsp[i + 1]; // Q15
        arA -= qlsp[i]; // Q15
        if (arA < LSP_SPREAD_FACTOR_16)
        {
            arA = qlsp[i + 1]; // Q15
            arA -= LSP_SPREAD_FACTOR_16;
            qlsp[i] = (Int16)arA;
//            qlsp[i] = qlsp[i + 1] - LSP_SPREAD_FACTOR;
        }
    }

    /* Smooth LSP's */
    for (i = 0; i < LPCORDER; i++)
    {
//        arA = (Int32)last_qlsp[i] << 16;
//        arA = L_sub(arA, (Int32)last_qlsp[i] << 13); // arA = (1 - 0.125) * last_qlsp  (Q31)
//        arA = L_add(arA, (Int32)qlsp[i] << 13); // arA += 0.125 * qlsp[i] (Q31)
//        arA = L_add(arA, 0x00008000l); // rounding
//        qlsp[i] = (Int16)(arA >> 16);
////        qlsp[i] = 0.875 * last_qlsp[i] + 0.125 * qlsp[i];
        arA = (Int32)last_qlsp[i] << 3;
        arA -= (Int32)last_qlsp[i];
        arA += (Int32)qlsp[i];
        arA += 0x00000004l; // rounding
        qlsp[i] = (Int16)(arA >> 3);
    }

}
