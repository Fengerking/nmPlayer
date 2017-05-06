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
/* qcelp8.h - contains most of the parameters for the QCELP algorithm */

/*****************************************************************************
* File:           qcelp13.h
*
* Version:        DSPSE Release    2.0
*
* Description:    contains most of the parameters for the QCELP13 algorithm 
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#ifndef __QCELP13_H__
#define __QCELP13_H__

#include  "stream.h"
//#include "qcpdec.h"

#define DEC_DP_SYNTH_FILTER   1  /* double precision synthesis filter */
#define QC13_WORDS_PER_PACKET 18
/* Begin pack */
#define QC13_LSPs    0	/* Eighth Rate Only */
#define QC13_PGAIN   1	/* Pitch Gain */
#define QC13_PLAG    2	/* Pitch Lag */
#define BPFORDER     20   /* order of the band pass filter for 1/4 rate */
#define HPFORDER     2	 /* order of the high pass front end filter */
#define DECORDER     16  // order of filter used in refine_rate13

#define QC13_LSPVs   4	/* LSP Vectors Non-Eighth Rate	*/
#define QC13_PFRAC   5	/* Bit for Fractional Pitch  */

#define QC13_CBGAIN  6	/* Codebook Gain */
#define QC13_CBINDEX 7	/* Codebook Index */
#define QC13_CBSIGN  8	/* Codebook Sign for Gain?? */
#define QC13_CBSEED  9	/* Codebook Seed */
#define QC13_RESERVE 10 /* Reserved Bit */

/* End pack */
#define QC13_GPRED_ORDER 2  /* EIGHTH rate prediction */
#define QC13_MAX_GPRED 38 // Full rate prediction
#define QC13_MAX_GPRED_16  (Int16)19456 // Q9 value
#define QC13_MIN_GPRED	6 // Full rate prediction
#define QC13_MIN_GPRED_16  (Int16)3072 // Q9 value

#define QC13_FR_INTERP_FILTER_LENGTH  8
#define QC13_MAX_FR_RESOLUTION	      4
#define QC13_FR_RES		      4
#define QC13_EXY_HIST_ORDER	      3
#define QC13_EYY_HIST_ORDER	      2
#define QC13_RATE_HIST_ORDER	      4
#define QC13_INV_HALF_LPCORDER	      (Int16)1489 // 0.5/11 in Q15
#define QC13_INV_DIFF_16	      (Int16)25600 // 1/0.04 in Q10 format
#define QC13_calc_FE_const	      (Int16)26214 // 1.6 in Q14 format

#define QC13_PERCEPT_WGHT_FACTOR	 .78
#define QC13_PERCEPT_WGHT_FACTOR_16	 (Int16)25559  // 0.78 in Q15
#define QC13_PF_ZERO_WGHT_FACTOR	 .625
#define QC13_PF_ZERO_WGHT_FACTOR_16	 (Int16)20480
#define QC13_PF_POLE_WGHT_FACTOR	 .775
#define QC13_PF_POLE_WGHT_FACTOR_16	 (Int16)25395
#define QC13_GORDER  3
#define INV_QC13_GORDER (Int16)10923 // 1/3 in Q15 format
#define QC13_CBRATE4 5
#define QC13_PITCHSF8TH 8
#define QC13_PITCHSF_BLANK 4
#define QC13_PITCHSF_ERASURE 4
#define QC13_MAX_SF 20
#define QC13_MING 0.0
#define QC13_MING_16 (Int16)0
#define QC13_MAXG 60.0
#define QC13_DIFFG_16 (Int16)30720 //60.0 in Q9 format
#define QC13_INV_DIFFG_16 (Int16)17476 // 1/60.0 in Q20 format
#define QC13_NUMBER_OF_G_LEVELS 16
#define QC13_RELMING -6.0
#define QC13_RELMING_16   (Int16)-3072 //-6.0 in Q9 format
#define QC13_RELMAXG 22.0
#define QC13_REL_DIFFG_16 (Int16)14436 //28.0 in Q9 format
#define QC13_INV_RELDIFFG_16 (Int16)18725 // 1/28.0 in Q19 format
#define QC13_RELNUMBER_OF_G_LEVELS 8 // For full rate Gain predictor
#define QC13_GPRED_COEFF 1.0
#define QC13_SCALE_B  0.5  // For scaling down pitch gain when applying
#define QC13_SCALE_B_16 (Int16)16384 // Q15 format
#define QC13_LOWRATE_GAIN_FACTOR  0.5
#define QC13_G_FACTOR  1.2608  // 1/8th and qtr rate codebook gain calculation
#define QC13_G_FACTOR_16  (Int16)20657	// 1.2608 in Q14 format
#define QC13_NORM_SCALE  1.056920   /* normalization so that gain of filter at 1 kHz (PI/4) is 1.0 */
#define QC13_INV_NORM_SCALE_16	(Int16)31003  // 1/1.056920 in Q15 format
#define QC13_PI_QC   3.14159265358979323846 // extended precision PI ?
#define QC13_FREQBANDS	  2	  // QC13 rate select algorithm frequency bands
#define QC13_TLEVELS	  8
#define PMAX		120
#define PMIN		 20
#define DECIMATE_F	  2
#define PMAX_DIV_DEC_F	 PMAX/DECIMATE_F
#define PMIN_DIV_DEC_F	 PMIN/DECIMATE_F
#define FSIZE_DIV_DEC_F  FSIZE/DECIMATE_F
#define PITCH_NUM	  2
#define SMSNR		0.6 /* leaky integration constant used smooth snr estimate */
#define SMSNR_16	(Int16)19661  // 0.6  Q15
#define SMSNR_NOT_16	(Int16)13107  // 0.4  Q15
#define NACF_ADAP_BGN_THR_16  (Int16)12452   /* Q15 */
#define NACF_ADAP_BGN_THR  0.38 /* threshold signifying frame does	      */
			       /* not have any voiced speech in it	     */
			       /* so we might start to adapt thresholds      */
#define ADP		  8
#define INC_FACTOR     1.03
#define INC_FACTOR_16  (Int16)16876  // Q14
#define SNR_MAP_THRESHOLD 3
#define IS96_INC    1.00547
#define IS96_INC_16    (Int16)16474//	 1.00547 Q14
#define NACF_SOLID_VOICED_16  (Int16)16384 // Q15 0.5
#define NACF_SOLID_VOICED  0.5	/* threshold above which we are pretty sure   */
			       /* speech is present and thus SNRs can be     */
			       /* adjusted accordingly			     */
#define STATVOICED	  5
#define SCALE_DOWN_ENERGY 0.97
#define SCALE_DOWN_ENERGY_16 (Int16)31785  // 0.97 Q15
#define VOICE_INITIAL 65
#define VOICE_INITIAL_HI 55

#define VOICE_INIT_PL_16     (Int16)18432	//(65-20)/5=9 -> Q11
#define VOICE_INIT_PL_HI_16  (Int16)14336	//(55-20)/5=7 -> Q11
#define LOG10_ONE_Q26	     (Int16)16029	// log10(67108864) Q11


#define THR_SIL_1_2	  87.26779887 /* 9db below current energy is declared 1/2 rate silence */
#define THR_SIL_1_2_32	  (Int32)1464110711L // Q24
#define THR_SIL_1_4  93.26779887 /* 14db below current energy is declared 1/4 rate silence */
#define THR_SIL_1_4_32	  (Int32)1564774007L // Q24
#define THR_DIFFLSP 0.005 /* lsp must not change too quickly also for rate to
			    get bumped down				    */
#define THR_DIFFLSP_16 (Int16)164 //Q15 value

#define THR_NACF_BUMPUP 0.4   /* if nacf is lower than this and not declared*/
			     /* unvoiced then bump up rate		   */
#define THR_NACF_BUMPUP_16 (Int16)13107   /* Q15 */

#define THR_PRED_GAIN 5     /* in db, cc:08-23-94 */
#define THR_PRED_GAIN_16 (Int16)5120	 /* Q10 */
#define STATWINDOW (Int16)400	     /* the window over which rate control stats */
			      /* are calculated (non 1/8 rate)		  */
#define STAT_DIV_HALF (Int16)28444 /* STATWINDOW/HALFRATE @ Q9 */
#define FULLRATE_BPS 14.4
#define FULL_DIV_STAT	(Int16)18874  // 14.4/400 @ Q19
#define HALFRATE_BPS 7.2
#define HALF_DIV_STAT	(Int16)9437  // 7.2/400 @ Q19
#define ONE_HALFRATE_BPS_16  (Int16)4551  // 1/7.2 @ Q15
#define QUARTER_BPS 3.6
#define QRT_DIV_STAT   (Int16)4719  // 3.6/400 @ Q19


#define RATEM 0.02	   /* bounds around avg rate that are acceptable */
#define ONE_PLUS_RATEM	  (Int16)16712	// 1.02 @ Q14
#define ONE_MINUS_RATEM   (Int16)32113	// 0.98 @ Q15

#define HIGH_BND_SNR  (Int16)25    /*14.0*/  /* in db */
#define LOW_BND_SNR   (Int16)6	    /* in db */
#define EPS	   1E-08

#define PRED_GAIN_CONST 	20480	// 100/160  Q15
#define LAST_PRED_GAIN_CONST	12288	// 60/160   Q15
#define DIFF_LSP_TMP_CONST	20480	// 0.625    Q15
#define LAST_DIFF_LSP_CONST	12288	// 0.375    Q15

#define SM_CONST_LOW_32    (Int32)1 //1/67108864. Q26

#define Q13_HALF    (Int16)4096
#define Q13_QUARTER (Int16)2048
#define Q10_FIFTEEN (Int16)15360

#define FIVE_EIGTH_Q15 (Int16)20480
#define THREE_EIGTH_Q15 (Int16)12288

#define EAVG_CONST	    (Int16)28918  // .8825 Q15
#define NOT_EAVG_CONST	    (Int16)3850   // (1 - .8825) Q15
#define LOG_SCALE_CONSTANT  (Int32)154291103L // (0.1175 * (10*log10(67108864.)) Q26

extern Int16 QC13_byte_num_per_frame[15];

typedef struct
{
    Int16    b[QC13_MAX_SF];
	Int16    lag[QC13_MAX_SF];
	Int16    G[QC13_MAX_SF];
	Int16    i[QC13_MAX_SF];
    Int16    Gsign[QC13_MAX_SF];
	Int16    frac[QC13_MAX_SF];
    Int16    lpc[LPCORDER];
    Int32    sd_dec;
	Int32    sd_enc;
    Int8     rate;
    Int8     padding_1;
    Int8     *data;
    Int16    min_lag;
}QC13_PACKET;

typedef struct
{
    Int16   err_cnt;
    Int16   low_rate_cnt;
    Int16   last_rate;
    Int16   pf_flag;
    Int16   pitch_post_flag;
    Int16   *lsp_pred_16;
    Int16   *  G_pred;
    Int16     *  last_qlsp_16;
    Int16     last_b_16;
    Int16   last_frac;
    Int16   last_lag;
    Int16     *last_G_16;
    Int16     last_G_8th_rate_16; // used for Eighth rate prediction
    Int32     seed;

    Int16     lpc_16[MAX_PITCH_SF][2][LPCORDER];
    Int16     *  qlsp_16;
    Int16     agc_factor_16;
    Int16     *adptv_cbk_16;
    Int16     *prefilt_acbk_16;
    Int16     *  pf_hist_16;
    Int16     *  ring_pole_hist_16;
    Int32     *  ring_pole_hist_32;
    Int16     *  bpf_hist_16;
    Int16     *  bright_16;
    Int16     bright_hist_16;
}QC13_DObj;

typedef struct{
	QC13_PACKET	     q13_packet;
	QC13_DObj	     Decoder;
	FrameStream      *stream;
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;
    VO_PTR           hCheck;
	int              VoKey;
}QCP13_DecObj;


/* *************************************************************************
Class Definitions for QCELP 13 Decode
****************************************************************************/
//extern QC13_DObj	     Decode;

int  QC13_Decode(QCP13_DecObj *qc13_dec, Int16 *out_speech);
Void QC13_DecBlank(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect);
Void QC13_DecErase(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect);
int  QC13_DecEighth(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect);
Void QC13_DecQuarter(QCP13_DecObj *qc13_dec, Int16 rate, Int16 *outvect);
int  QC13_DecPSF(QCP13_DecObj *qc13_dec, Int16 psf, Int16 rate, Int16 *outvect);
Void agc_prefilter(Int32 input_energy, Int16 *output, Int16 length);
Void gen_params(QC13_DObj *Decode, PITCHPARAMS *pitch_params, CBPARAMS *cb_params, Int16 * interp_G);  // Used in DecErase

Void unpack_frame13( QC13_PACKET *qc13_packet);
Void clear_qc13_packet_params(QC13_PACKET *packet);
Void putbit13(QC13_PACKET *packet,Int16 type,Int16 number,Int16 loc, Int16 bit);

/* pack.c */
Void unpack_cb13(QC13_PACKET *qc13_packet,  CBPARAMS *cb_params, Int16 cbsf_number);
Int16 lsp_erasure_check(Int16 rate, Int16 *qcode);
Int16 G_erasure_check(Int16 *qcode_G);

/* filter.c */
Void  fir_filt(Int16 *data, Int16 * coeffs, Int16 * hist, Int16 len, Int16 order);

/* cb.c */
Void make_qc13_qtr_cw(Int16 *vector, Int16 length, CBPARAMS *cb_params, Int16 * hist);

Void make_qc13_full_cw(Int16 * vector, Int16 length,  CBPARAMS *cb_params);

/* lsp.c */
Void gen_qlsp(Int16 *qlsp, Int16 *lsp_pred, Int16 err_cnt, Int16 *last_qlsp); // Used in DecErase

/* quantize.c */
int  unquantize_b(Int16 *q_b, Int16 *qcode_b, Int16 lag);
Void unquantize_G_13(Int16 rate, Int16 *q_G, Int16 *qcode_G, Int16 qcode_Gsign, Int16 *lastG, Int16 cbsf, Int16 *G_pred);
int  unquantize_lag(Int16 *lag, Int16 *qcode_lag, Int16 *frac, Int16 *qcode_frac);
int  unquantize_G_8th(Int16 * q_G,Int16 * qcode_G, Int16 *G_pred);
Void unvq_lsp(Int16 * qlsp, Int16 * lsp_pred, Int16 * qcode);

/* pitch.c */
Void pitch_filter_13(Int16 * adptv_cbk,  PITCHPARAMS *pitch_params , Int16 * vector, Int16 length);

/* *************************************************************************
Class Definitions for QCELP 13 Encode
****************************************************************************/
//extern QC13_EObj	     Encode;
//void QC13_ENC_init();
/* encode.c */

/* pack.c */
Int16 getbit13(QC13_PACKET * QC13_packet, Int16 type, Int16 number, Int16 loc);

/* log10.cpp */
Int16   QC13_log10 (Int16 mant, Int16 shift);

#endif  //__QCELP13_H__

