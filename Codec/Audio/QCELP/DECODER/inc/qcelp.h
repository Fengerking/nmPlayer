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
* File:           qcelp.h
*
* Version:        DSPSE Release    2.0
*
* Description:    contains most of the parameters for the QCELP algorithm
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#ifndef __QCELP_H__
#define __QCELP_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include"types.h"
#include "voIndex.h"
#include "voQcelpDecID.h"

//extern "C" {
#include "basic_op.h"
//}

extern void *g_hQCELPDecInst;

#define QC13       1
#define QC8        0
#define PI         3.1415927
#define PF_OFF     0
#define PF_ON      1
#define PITCH      0
#define CB         1
#define ENCODER    0
#define DECODER    1
#define NOT_WGHTED 0
#define WGHTED     1
#define NO         0
#define YES        1
#define FLOAT      0
#define INT        1
#define NEGATIVE   1
#define POSITIVE   0
#define BOTH       0
#define ONLY_UNWGHTED 1
#define UNLIMITED  -1

#define FULL         4
#define HALF         3
#define QUARTER      2
#define EIGHTH       1
#define BLANK        0
#define ERASURE     14
#define FRL         15

#define QC13_RATE_OFFSET 5

#define QC13_FULL      9
#define QC13_HALF      8
#define QC13_QUARTER   7
#define QC13_EIGHTH    6
#define QC13_BLANK     5
#define QC13_ERASURE  16

/**************************/
/* Basic Coder Parameters */
/**************************/
#define FSIZE      160        /* Overall frame size                     */
#define FSIZE_16   (Int16) 20480 /* FSIZE in Q7 format */
#define INV_FSIZE_16 205      /* 1/160 in Q15 format */
#define AGCRATE     4
#define NUMRATES_EIGHT      5
#define NUMRATES_THIRTEEN   5
#define NUMRATES (NUMRATES_EIGHT + NUMRATES_THIRTEEN)

/**********************/
/* LPC/LSP Parameters */
/**********************/
#define LPCSIZE    160        /* LPC frame size                         */
#define LPCOFFSET   60        /* Offset of LPC frame to Fsize           */
#define LPCORDER    10
#define ACFORDER    17
#define INV_LPCORDER (Int16)3277     /* 1/10 in Q15 format */
#define INV_HALF_LPCORDER (Int16)23831    /* (0.5/11)*16 in Q15 format */
//#define LPC_CENTER LPCOFFSET+(LPCSIZE)/2
#define LPC_CENTER (LPCOFFSET+(LPCSIZE >> 1))
#define LPC_CENTER_16 (Int16)17920 /* LPC_CENTER in Q7 format */
#define CBRATE8     8

#define COSTABLE_BITS 9
#define COSTABLE_LEN (1<<COSTABLE_BITS)
#define LSP_SPREAD_FACTOR           .01
#define LSP_SPREAD_FACTOR_16         (Int16)328 /* Q15 */
#define BWE_FACTOR                  .9883
#define BWE_FACTOR_16               ((Int16) 32385)
#define AGC_FACTOR                  .9375
#define AGC_FACTOR_16               (Int16)30720

/****************************/
/* Rate Decision Parameters */
/****************************/
#define HIGH_THRESH_LIM      0.07539457082748
#define HIGH_THRESH_LIM_32   (Int32)5059644L               /* Q26 */

/********************/
/* Pitch Parameters */
/********************/
#define MAX_PITCH_SF 4             /* used for array size definitions */
#define LENGTH_OF_IMPULSE_RESPONSE  20 /* used in pitch & cb searches */
#define MINB                 0.0
#define MINB_16              (Int16)0 /* Q13 */
#define MAXB                 2.0
#define MAXB_16              (Int16)16384 /* Q13 */
#define INV_MAXB_SUB_MINB    0.5   /* 1/(MAXB - MINB) */
#define NUMBER_OF_B_LEVELS   9
#define NUMBER_OF_B_LEVELS_16   (Int16)18432 /* Q11 */
#define NUM_LEVELS_DIV_MM   4    /* (NUMBER_OF_B_LEVELS - 1)/(MAXB - MINB) */
#define NUM_LEVELS_DIV_MM_16 (Int16)16384 /* 4 in Q12 format */
#define MINLAG      17          /* The minimum and maximum pitch lags */
#define MAXLAG     143
/***********************/
/* Codebook Parameters */
/***********************/
#define INV_GORDER    (Int16)16384 // 0.5 in Q15

#define CBLENGTH   128
#define NOISE   0
#define TABLE   1
/***************************/
/* Data Packing Parameters */
/***************************/


/* struct.h - basic structures for celp coder   */

#define NO_UPDATE  0
#define UPDATE     1

//#define QCELP13_DEC_PRINTF printf
#define QCELP13_DEC_PRINTF 

typedef struct {
    Int16    min_lag;
    Int16    max_lag;
    Int16      b_16;		// (I16)(best match)
    Int16    lag;		// lag for best match
    Int16    frac;		// lag for best match is frac?
    Int16    qcode_b;
    Int16    qcode_lag;
    Int16    qcode_frac;
    Int16    last;
}PITCHPARAMS;

typedef struct
{
    Int16  qcode_k[LPCORDER];
    Int16  qcode_lsp[LPCORDER];
}LPCPARAMS;


typedef struct
{
    Int16    G_16;
    Int16    i;
    Int16    qcode_G;
    Int16    qcode_i;
    Int16    qcode_Gsign;
    Int32    sd;
    Int32    seed;
}CBPARAMS;

#define TRUEFALSE_MACRO 1

#ifdef TRUEFALSE_MACRO
#define truefalse(a, b) (((((a) >> (b)) & 0x01) == 0) ? 0 : 1)
#else
Int16 truefalse(Int16 word, Int16 bitloc);
#endif
//Void interp_lpcs(Int16 count, I16 *prev_lsp, I16 *curr_lsp, I16 *lpc_not_wghted, I16 *lpc_wghted, I16 *bright, Int16 loops, I16 per_wght_factor);
Void interp_lpcs(Int16 count, Int16 *prev_lsp, Int16 *curr_lsp, Int16 *lpc_not_wghted, Int16 *lpc_wghted, Int16 *bright, Int16 loops, Int16 per_wght_factor);
//Void wght_lpcs(I16 *lpc, I16 factor); 
Void wght_lpcs(Int16 *lpc, Int16 factor); 
//Void lsp2lpc(I16 *lsp, I16 *lpc, Int16 order);
Void lsp2lpc(Int16 *lsp, Int16 *lpc, Int16 order);
//Void lpc2lsp(I16 *Lpc, I16 *Lsp, Int16 order, I16 *last_qlsp);
Void update_acbk(Int32 * adptv_cbk, Int32 * pvect, Int16 length);
//Void update_acbk_16(I16 * adptv_cbk, I16 * pvect, Int16 length);
Void update_acbk_16(Int16 * adptv_cbk, Int16 * pvect, Int16 length);
//Void make_cw(I16 * vector, Int16 length,  CBPARAMS *cb_params, Int16 type);
Void make_cw(Int16 * vector, Int16 length,  CBPARAMS *cb_params, Int16 type);
//Void polefilt(I16 *data, I16 *coeffs, I16 *hist, Int16 len);
Void polefilt(Int16 *data, Int16 *coeffs, Int16 *hist, Int16 len);
//Void dp_polefilt(I16 *data, I16 *coeffs, I32 *hist, Int16 len);
Void  dp_polefilt(Int16 *data, Int16 *coeffs, Int32 *hist, Int16 len);
Void check_lsp_stab(Int16 *qlsp);
//Void unquantize_lsp(Int16 rate, I16 *qlsp, I16 *pred, Int16 *qcode);
Void unquantize_lsp(Int16 rate, Int16 *qlsp, Int16 *pred, Int16 *qcode);
//Int16 smooth_lsps(Int16 rate, I16 *prev_lsp, I16 *curr_lsp, Int16 *rate_cnt);
Int16 smooth_lsps(Int16 rate, Int16 *prev_lsp, Int16 *curr_lsp, Int16 *rate_cnt);
//void agc(I32 input_energy, I16 *output, Int16 length, I16 *factor, Int16 agc_flag);
Void agc(Int32 input_energy, Int16 *output, Int16 length, Int16 *factor, Int16 agc_flag);
//I16 postfilt(Int16 mode, I16 * data, I16 * lpc, I16 * pf_hist, I16 bright, I16 history, Int16 length);
Int16  postfilt(Int16 mode, Int16 * data, Int16 * lpc, Int16 * pf_hist, Int16 bright, Int16 history, Int16 length);

//Void wghtfilt(I16 *data, I16 * pcoeffs,I16 * zcoeffs, I16 * hist, Int16 order, Int16 len);
#ifdef ASM_OPT
Void  wghtfilt_asm(Int16 *data, Int16 * pcoeffs,Int16 * zcoeffs, Int16 * hist, Int16 order, Int16 len);
#else
Void  wghtfilt(Int16 *data, Int16 * pcoeffs,Int16 * zcoeffs, Int16 * hist, Int16 order, Int16 len);
#endif

//Void quantize_i( Int16 *i, Int16 *qcode_i);
Void unquantize_i( Int16 *i, Int16 *qcode_i);

//Int16 compute_lpc(I16 *speech, I16 *lpc, I16 *R);

//Void recursive_conv(I16 *resid, I16 *impulse_response, Int16 length);

//Void quantize_lsp(Int16 rate, I16 *lsp, I16 *qlsp,  LPCPARAMS *lpc_params, I16 * lsp_pred, I16 * last_qlsp);

//Void update(I16 *adptv_cbk, Int16 lag, I16 *h, I16 *out, Int16 length);


/* Utility functions */
//I16 Sqrt(I32 Num);
Int16 Sqrt(Int32 Num);
//Int16    blockscale(I16 * target, Int16 length, Int16 hroom);


/* Utility functions */
Int16 norm_s(Int16 var1);
Int16 norm_l(Int32 L_var1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__QCELP_H__
