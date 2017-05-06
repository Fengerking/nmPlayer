#ifndef table_decl_fx_h
#define table_decl_fx_h

#include "consts_fx.h"

/* RE8 lattice quantiser tables */
extern const Word16 Tab_factorial[8];
extern const Word16 Tab_pow2[],Da_[][8],Ds_[],Ns_[],A3_[],A4_[],Ia_[];
extern const UWord16 I3_[],I4_[],Is_[];
extern const Word16 Da_nq_[];
extern const Word16 Da_pos_[], Da_nb_[], Da_id_[];


/* LF ISF quantiser codebooks */
extern const Word16 Isf_init[M];
extern const Word16 Isp_init[M];
extern const Word16 Isp_init_HF[MHF] ;
extern const Word16 Mean_isf[16];
extern const Word16 Dico1_isf[SIZE_BK1 * 9];
extern const Word16 Dico2_isf[SIZE_BK2 * 7];
extern const Word16 Dico21_isf[SIZE_BK21 * 3];
extern const Word16 Dico22_isf[SIZE_BK22 * 3];
extern const Word16 Dico23_isf[SIZE_BK23 * 3];
extern const Word16 Dico24_isf[SIZE_BK24 * 3];
extern const Word16 Dico25_isf[SIZE_BK25 * 4];
extern const Word16 Dico21_isf_36b[SIZE_BK21_36b * 5];
extern const Word16 Dico22_isf_36b[SIZE_BK22_36b * 4];
extern const Word16 Dico23_isf_36b[SIZE_BK23_36b * 7];

/* HF ISF quantiser codebooks */
extern const Word16 Isf_init_hf[M];
extern const Word16 Mean_isf_hf_low_rate[Q_ISF_ORDER];
extern const Word16 Dico1_isf_hf_low_rate[SIZE_BK1_HF * Q_ISF_ORDER];
extern const Word16 Dico2_isf_hf[SIZE_BK2_HF * Q_ISF_ORDER];
extern const Word16 Dico1_isf_hf_low_rate[SIZE_BK1_HF*Q_ISF_ORDER];   

extern const Word16 Mean_isf_hf_12k8[Q_ISF_ORDER];
extern const Word16 Dico1_isf_hf_12k8[SIZE_BK1_HF*Q_ISF_ORDER];   

/* HF gain quantiser codebook */
extern const Word16 Dico_gain_hf[SIZE_BK_HF * Q_GN_ORDER];

/* Resampling filters */
extern const Word16 Filter_8k[61];
extern const Word16 Filter_28k8[109];
extern const Word16 Filter_28k8_hf[109];
extern const Word16 Filter_32k[61];
extern const Word16 Filter_32k_hf[61];
extern const Word16 Filter_32k_7k[61];
extern const Word16 Filter_48k[185];
extern const Word16 Filter_48k_hf[185];
extern const Word16 Filter_48k_hf_high_rate[185];

/* FFT sin and cos tables */
extern const Word16 t_sinFxS4[];

/* For phase random generator */
extern const Word16 Sin20[20];

/* For pitch predictor */
extern const Word16 Inter4_2[PIT_FIR_SIZE2];

/* For bass postfilter */
extern const Word16 Filt_lp[1 + L_FILT];

/* 2k LP filter sed by stereo code */
extern const Word16 Filter_2k_fxQ14_32[];
extern const Word16 Filter_2k_fxQ14_5[];

/* stereo */
extern const int Nprm_stereo_hi_x[4];

/* Bit allocation tables for stereo */
extern const Word16 StereoNbits_FX[];
/* number of bits (for core codec) per 80ms frame according to the mode */
extern const Word16 NBITS_CORE_FX[];
extern const Word16 NBITS_CORE_AMR_WB_FX[];

/* High stereo codebooks */
// average filter
extern const Word16 cb_filt_hi_mean_fx[HI_FILT_ORDER];
extern const Word16 filt_hi_mscb_4a_fx[SIZE_FILT_HI_MSVQ_4A][HI_FILT_ORDER];
extern const Word16 filt_hi_mscb_7a_fx[SIZE_FILT_HI_MSVQ_7A][HI_FILT_ORDER];
extern const Word16 filt_hi_mscb_7b_fx[SIZE_FILT_HI_MSVQ_7B][HI_FILT_ORDER];

// average gain vector
extern const Word16 cb_gain_hi_mean_fx[HI_GAIN_ORDER];
extern const Word16 gain_hi_mscb_2a_fx[SIZE_GAIN_HI_MSVQ_2A][HI_GAIN_ORDER];
extern const Word16 gain_hi_mscb_5a_fx[SIZE_GAIN_HI_MSVQ_5A][HI_GAIN_ORDER];

// code books 
// 4 bit VQ (filter)
extern const Word16 size_filt_hi_msvq_4_fx[NSTAGES_FILT_HI_MSVQ4];
extern const Word16 *cbs_filt_hi_msvq_4_fx[NSTAGES_FILT_HI_MSVQ4];
extern const PMSVQ_fx filt_hi_pmsvq4_fx;
// 4+3 bit msvq (filter)
extern const Word16 size_filt_hi_msvq_7_fx[NSTAGES_FILT_HI_MSVQ7];
extern const Word16 *cbs_filt_hi_msvq_7_fx[NSTAGES_FILT_HI_MSVQ7];
extern const PMSVQ_fx filt_hi_pmsvq7_fx;
// 2 bit VQ (gain)
extern const Word16 size_gain_hi_msvq_2_fx[NSTAGES_GAIN_HI_MSVQ2];
extern const Word16 *cbs_gain_hi_msvq_2_fx[NSTAGES_GAIN_HI_MSVQ2];
extern const PMSVQ_fx gain_hi_pmsvq2_fx;
// 5 bit VQ (gain)
extern const Word16 size_gain_hi_msvq_5_fx[NSTAGES_GAIN_HI_MSVQ5];
extern const Word16 *cbs_gain_hi_msvq_5_fx[NSTAGES_GAIN_HI_MSVQ5];
extern const PMSVQ_fx gain_hi_pmsvq5_fx;

extern const Word16 Gain_hf_ramp[64];

extern const Word16 Inter2_coef[12];
#ifdef FILTER_48kHz
extern const Word16 Filter_LP180[1+((12+1)*180)];
#endif
/*Tables for function q_gain2()*/
extern const Word16 t_qua_gain7b[128*2];
extern const Word16 t_qua_gain6b[64*2]; 

/*dec_ace_fx.tab*/
extern const Word16 Ovelap_wind[];
extern const Word16 Cos_wind[];
extern const Word16 Cos_WindLR[];

/*tcx ecu tables */
extern const Word16 Len_tbl[]; 
extern const Word16 TXV[];


/* int lpc table */
extern const Word16 interpol_frac4[];
extern const Word16 interpol_frac8[];
extern const Word16 interpol_frac16[];


extern const Word16 Gain_hf_ramp[];


/* for open-loop classifier */
extern const Word16 VADFiltBandFreqs[COMPLEN2];
extern const Word16 Bw_inv[COMPLEN];
extern const Word16 Lwg[LPHAVELEN];


extern const Word16 miMode_fx[];
extern const Word16 isfIndex_fx[];
extern const Word16  MonoRate[];
extern const Word16  StereoRate[];

extern const Word16 phs_tbl128[2*SIZE]; 

extern const Word16 Filter_LP12[1+(L_FILT_OVER_FS*12)];
#ifdef FILTER_44kHz
extern const Word16 Filter_LP165[1+((12+1)*165)];
#endif

#endif
