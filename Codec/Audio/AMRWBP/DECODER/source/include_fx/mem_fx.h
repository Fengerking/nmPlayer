#ifndef mem_fx_h
#define mem_fx_h

#include "typedef.h"
#include "nclass_fx.h"
#include "wb_vad_fx.h"
#include "cnst_fx.h"
#include "consts_fx.h"
#include "cnst_tcx_stereo_fx.h"
#include "util_stereo_x_fx.h"

#define L_INTERPOL  (16+1)

typedef struct {
  Word16 mode;			/* AMR_WB core mode: 0..8 */
  Word16 extension;		/* 0=AMRWB, 1=mono, 2=stereo20%, 3=stereo25% */
  Word16 st_mode;		/* stereo mode 0..13 (not used, for ericsson cmd line?) */
  Word16 fscale;		/* frequency scaling */
  Word16 use_case_mode;	/* use case (for AMRWB+ only) */
  Word16 allow_dtx;		/* dtx (for AMRWB only) */
  Word16 FileFormat;
  Word16 mode_index;
  Word16 fscale_index;
  Word16 bc;             /* Backward compatible file format*/
} EncoderConfig;

//shanrong
#if 0
typedef struct {

  Word16 mode;        /* AMR_WB core mode: 0..8 */
  Word16 extension;   /* 0=AMRWB, 1=mono, 2=stereo20%, 3=stereo25% */
  Word16 st_mode;     /* stereo mode 0..13 (not used, for ericsson cmd line?) */
  Word16 fscale;   /* Frequency scaling */
  Word32 fs; 
  Word32 mono_dec_stereo; 
  Word32 limiter_on ;
  Word16 FileFormat;
  Word16 fer_sim;                /* frame errasures simulation */

} DecoderConfig;
#endif

/*---------------------------------------------------------------*
 * Decoder Static RAM		  								     *
 *---------------------------------------------------------------*/
typedef struct {

    /* cod_main.c */
    /* FIP */
    Word16 mem_decim[L_MEM_DECIM_SPLIT];    /* speech decimated filter memory */
    Word16 decim_frac;
    Word16 mem_sig_in[6];                 /* hp50 filter memory */
    Word16 mem_preemph;                   /* speech preemph filter memory */

    Word16 mem_decim_hf[2*L_FILT24k];     /* HF speech decimated filter memory */
    Word16 old_speech_hf[L_OLD_SPEECH_ST];/* HF old speech vector at 12.8kHz */

    /* cod_hf.c */
    Word16 past_q_isf_hf[MHF];            /* HF past quantized isf */
    Word16 ispold_hf[MHF];                /* HF old isp (immittance spectral pairs) */
    Word16 ispold_q_hf[MHF];              /* HF quantized old isp */
    Word16 old_gain;                      /* HF old gain (match amplitude at 6.4kHz) */
    Word16 mem_hf1[MHF];                  /* HF memory for gain calculcation */
    Word16 mem_hf2[MHF];                  /* HF memory for gain calculcation */
    Word16 mem_hf3[MHF];                  /* HF memory for gain calculcation */
    Word16 mem_lev_hf[18];

    Word16 old_exc[PIT_MAX_MAX];
    Word16 Q_sp_hf;
    Word16 OldQ_sp_hf[2];

    /* Table pointers */
    const Word16 *mean_isf_hf;
    const Word16 *dico1_isf_hf;


} Coder_StState_fx;

typedef struct {
    /* memory for both channels */
    Coder_StState_fx left;
    Coder_StState_fx right;

    /* memory for the  stereo */

    /*FIP */
    Word16 	old_chan[L_OLD_SPEECH_ST];
    Word16 	old_chan_2k[L_OLD_SPEECH_2k];
    Word16 	old_chan_hi[L_OLD_SPEECH_hi];

    Word16 	old_speech_2k[L_OLD_SPEECH_2k];
    Word16 	old_speech_hi[L_OLD_SPEECH_hi];

    Word16 	old_speech_pe[L_OLD_SPEECH_ST];
    // NMBC
    Word16 old_wh[HI_FILT_ORDER];
    Word16 old_wh_q[HI_FILT_ORDER];
    Word16 old_gm_gain[2];
    Word16 old_exc_mono[HI_FILT_ORDER];
    Word16 filt_energy_threshold;
    Word16 w_window[L_SUBFR];
    const PMSVQ_fx *filt_hi_pmsvq;
    const PMSVQ_fx *gain_hi_pmsvq;
    // E_STEREO_TCX
    Word16 mem_stereo_ovlp_size;
    Word16 mem_stereo_ovlp[L_OVLP_2k];
    /* cod_main.c */
    Word16 old_ovlp_size;                   /* last tcx overlap size */
    // Memory of past gain for WB+ -> WB switching
    Word16 SwitchFlagPlusToWB;
    Word16 prev_mod;

    /* cod_main.c */
    NCLASSDATA_FX *_stClass;
    VadVars *_vadSt;						
    Word16 vad_hist;						

    Word16 old_speech[L_OLD_SPEECH_ST];   /* old speech vector at 12.8kHz */
    Word16 old_synth[M];                  /* synthesis memory */

    /* cod_lf.c */
    Word16 past_isfq[M];                  /* past isf quantizer */
    Word16 old_wovlp[128];                /* last tcx overlap synthesis */
    Word16 old_d_wsp[PIT_MAX_MAX/OPL_DECIM];  /* Weighted speech vector */
    Word16 old_exc[PIT_MAX_MAX+L_INTERPOL];   /* old excitation vector */

    Word16 old_mem_wsyn;                  /* weighted synthesis memory */
    Word16 old_mem_w0;                    /* weighted speech memory */
    Word16 old_mem_xnq;                   /* quantized target memory */

    Word16 isfold[M];                     /* old isf (frequency domain) */
    Word16 ispold[M];                     /* old isp (immittance spectral pairs) */
    Word16 ispold_q[M];                   /* quantized old isp */
    Word16 mem_wsp;                       /* wsp vector memory */
    Word16 mem_lp_decim2[3];              /* wsp decimation filter memory */

    /* memory of open-loop LTP */

    Word16 ada_w;
    Word16 ol_gain;
    Word16 ol_wght_flg;

    Word16 old_ol_lag[5];
    Word16 old_T0_med;
    Word16 hp_old_wsp[L_FRAME_PLUS/OPL_DECIM+(PIT_MAX_MAX/OPL_DECIM)];
    Word16 hp_ol_ltp_mem[9/* HP_ORDER*2 *//* 3*2+1*/];

    //	Mem_OL mem_ol;

    /* LP analysis window */
    Word16 window[L_WINDOW_HIGH_RATE];



    // Memory of past gain for WB+ -> WB switching
    Word16 mem_gain_code[4];
    //	Word16 wprev_mod;

    Word16 Q_sp;
    Word16 OldQ_sp;
    Word16 i_offset;
    Word16 pit_max;

    Word16 lev_mem[18];
    Word16 old_wsp_max[4];
    Word16 old_wsp_shift;
    Word16 scale_fac;

    Word16 Q_new;           /* First scaling */
    Word16 Q_max[2];
    Word16 OldQ_sp_deci[2];       
    Word16 Q_exc;
    Word16 Old_Qexc;
    Word16 LastQMode;

} Coder_State_Plus_fx;



typedef struct {
  /* dec_main.c */
  Word16 wmem_oversamp[L_MEM_JOIN_OVER];     /* memory for core oversampling */
  Word16 wover_frac;
  Word16 wmem_oversamp_hf[2*L_FILT];     /* memory for HF oversampling */  

  /* dec_hf.c */
  Word16 wpast_q_isf_hf[MHF];            /* HF past quantized isf */
  Word16 wpast_q_isf_hf_other[MHF];      /* HF past quantized isf for the other channel when mono decoding stereo */
  Word16 wpast_q_gain_hf;                /* HF past quantized gain */     
  Word16 wpast_q_gain_hf_other;          /* HF past quantized gain for the other channel when mono decoding stereo */     
  Word16 wold_gain;                      /* HF old gain (match amplitude at 6.4kHz) */
  Word16 wispold_hf[MHF];                /* HF old isp (immittance spectral pairs) */
  Word32 Lp_amp;                        /* HF memory for soft exc */
  Word32 Lthreshold;                     /* HF memory for smooth ener */
  Word16 wmem_syn_hf[MHF];               /* HF synthesis memory */  

  Word16 wold_synth_hf[D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5];        /* HF memory for synchronisation */
  // NMBC
  Word16 wmem_d_nonc[D_NC];
  Word16 wmem_sig_out[6];                /* hp50 filter memory for synthesis */

  Word16 Q_synHF;
  const Word16 *Mean_isf_hf;
  const Word16 *Dico1_isf_hf;
 
  /* stereo */
  Word16 mem_synth_hi_fx[M];
  Word16 mem_d_tcx_fx[D_NC+(D_STEREO_TCX*32/5)];
 

} Decoder_StState_fx;

/* Memory structure for parametric stereo decoding states. */

typedef struct {
  /* memory for both channels */
  Decoder_StState_fx left;
  Decoder_StState_fx right;

  /* memory for parametric stereo */
  Word16 mem_left_2k_fx[2*L_FDEL_2k];
  Word16 mem_right_2k_fx[2*L_FDEL_2k];
  Word16 mem_left_hi_fx[L_FDEL];
  Word16 mem_right_hi_fx[L_FDEL];

  Word16 my_old_synth_2k_fx[L_FDEL_2k + D_STEREO_TCX + 2*(D_NC*5)/32];
  Word16 my_old_synth_hi_fx[2*L_FDEL];
  Word16 my_old_synth_fx[2*L_FDEL+20];

  Word16 old_AqLF_fx[5*(M+1)];
  Word16 old_wh_fx[HI_FILT_ORDER];
  Word16 old_wh2_fx[HI_FILT_ORDER];
  Word16 old_exc_mono_fx[HI_FILT_ORDER];
  Word16 old_gain_left_fx[4];
  Word16 old_gain_right_fx[4];
  Word16 old_wh_q_fx[HI_FILT_ORDER];
  Word16 old_gm_gain_fx[2];
  Word16 W_window[L_SUBFR];
  const PMSVQ_fx *Filt_hi_pmsvq_fx;
  const PMSVQ_fx *Gain_hi_pmsvq_fx;

  Word16 mem_stereo_ovlp_size_fx;
  Word16 mem_stereo_ovlp_fx[L_OVLP_2k];
  Word16 last_stereo_mode;
  Word16 side_rms_fx;
  Word16 h_fx[ECU_WIEN_ORD+1];
  Word16 mem_balance_fx;

  // ECU
  Word16 wold_xri[L_TCX];
  /* memory for lower band (mono) */
  /* dec_main.c */
  Word16 last_mode;                       /* last mode in previous 80ms frame */
  Word16 wmem_sig_out[6];                /* hp50 filter memory for synthesis */
  Word16 wmem_deemph;                    /* speech deemph filter memory      */  
  /* dec_lf.c */
  Word16 prev_lpc_lost;                   /* previous lpc is lost when = 1 */
  Word16 wold_synth[M];                  /* synthesis memory */

  Word16 wold_exc[PIT_MAX_MAX+L_INTERPOL];                /* old excitation vector (20ms) */
  
  Word16 wisfold[M];                     /* old isf (frequency domain) */
  Word16 wispold[M];                     /* old isp (immittance spectral pairs) */
  Word16 wpast_isfq[M];                  /* past isf quantizer */
  Word16 wwovlp[128];                    /* last weighted synthesis for overlap */
  Word16 ovlp_size;                       /* overlap size */
  Word16 wisf_buf[L_MEANBUF*(M+1)];      /* old isf (for frame recovery) */
  Word16 wold_T0;                          /* old pitch value (for frame recovery) */
  Word16 wold_T0_frac;                     /* old pitch value (for frame recovery) */  
  Word16 seed_ace;                      /* seed memory (for random function) */ 
  Word16 wmem_wsyn;                      /* TCX synthesis memory */
  Word16 seed_tcx;                      /* seed memory (for random function) */ 
  Word16 wwsyn_rms;                      /* rms value of weighted synthesis */
  Word16 wpast_gpit;                     /* past gain of pitch (for frame recovery) */
  Word32 Lpast_gcode;                    /* past gain of code (for frame recovery) */
  Word16 pitch_tcx;                       /* for bfi on tcx20 */
  Word32 L_gc_thres;
  
  /* bass_pf.c */
  Word16 wold_synth_pf[PIT_MAX_MAX+(2*L_SUBFR)];      /* bass post-filter: old synthesis  */    
  Word16 wold_noise_pf[2*L_FILT];        /* bass post-filter: noise memory   */    
  Word16 wold_T_pf[2];                        /* bass post-filter: old pitch      */
  Word16 wold_gain_pf[2];                   /* bass post-filter: old pitch gain */ 

  // For WB <-> WB+ switching
  Word16 wmem_gain_code[4];
  Word16 wmem_lpc_hf[MHF+1];
  Word16 wmem_gain_hf;
  Word16 wramp_state;
  Word16 cp_old_synth[M];

  /* FIP static */
  Word16 Q_old;
  Word16 Q_exc;
  Word16 Q_syn;
  Word16 Old_Q_syn;
  Word16 Old_Q_exc;
  Word16 prev_Q_syn;
  Word16 mem_syn2[M];
  Word16 Old_Qxnq;
  Word16 Old_QxnqMax;
  
  Word16 Old_Qxri;
  Word16 Old_bpf_scale;

  Word16 mem_subfr_q[7];
  Word16 old_subfr_q[16];
  Word16 old_syn_q[16];
  Word16 i_offset;

} Decoder_State_Plus_fx;


#endif /* mem_fx_h */
