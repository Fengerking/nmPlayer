/**********************************************************************
Each of the companies; Qualcomm, and Lucent (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the QCELP-13 Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the QCELP-13 be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the QCELP-13 or the field of use of practice of the 
QCELP-13 Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/* celp.h - basic structures for celp coder   */
#ifndef  __CELP_H__
#define  __CELP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include   <stdio.h>
#include   "filter.h"
#include   "lpc.h"
#include   "coder.h"
#include   "stream.h"
#include   "voQcelpEncID.h"

extern void *g_hQCELPEncInst;

#define USE_CALLOC      0
#define PREV_LSP_FIX    1

#define ABS(A)  (((A) > 0) ? (A) : (-(A)))

#define LPC_CENTER   140
	//(LPCOFFSET+((LPCSIZE)>>1))

struct PITCHPARAMS {
    short    min_lag;
    short    max_lag;
    short	   b;
    short    lag;
    short    frac;
    short    qcode_b;
    short    qcode_lag;
    short    qcode_frac;
    short    last;
};

struct DECODER_MEM {
    short  lastG[GORDER];
	short  G_pred[GPRED_ORDER];
	short  last_G;
	short  type;   /* To tell whether this is the Tx or Rx decoder */
    short  last_mode;
    short  pred_qlsp[LPCORDER];
    short  last_qlsp[LPCORDER];
    /* these pitch variables are needed for the pitch post filter */
    short  pre_factor;
    short  pw_speech_out[FSIZE];
    struct PITCHPARAMS pitch_post_params;
    short  post_lag[8];
    short  post_pgain[8];
    short  index;
    struct POLE_FILTER       lpc_filt;
    struct POLE_FILTER       wght_syn_filt;
    struct POLE_FILTER       jt_gain_opt;
    struct POLE_FILTER_1_TAP pitch_filt;
    struct POLE_FILTER_1_TAP pitch_filt_per;
    struct POLE_FILTER       post_filt_p;
    struct ZERO_FILTER       post_filt_z;
    struct ZERO_FILTER       pitch_sm;
    struct ZERO_FILTER       bpf_unv;
    struct POLE_FILTER       bright_filt;
    short  agc_factor;
    short  low_rate_cnt;
    short  last_lag;
    short  last_b;
    short  last_frac;
    short  err_cnt;  

};

struct UNV_FEATURES {  /* features used for unvoiced classification etc */
    int  rate[FREQBANDS];
	int  band_energy[FREQBANDS];
	int  frame_energy_db;
    int  frame_energy_last;
    int  voiced_energy;
    int  log_pred_gain;
    short  nacf0;    
    short  nacf1;
    short  nacf_frame;
    short  open_pitch;
    short  zero;
    /*float  res_energy_diff0;*/
    /*float  res_energy_diff1;*/
    /*float  last_res_energy;*/
    short  last_nacf;
    short  pred_gain;
    short  pred_gain_mean,pred_gain_var;
    short  pred_gain_diff;
    short  pred_gain_frame;
    short  last_pred_gain;
    short  last_pred_gain_frame;
    short  diff_lsp;
	short  last_diff_lsp;
	short  last_lsp[LPCORDER];    
};

struct ENCODER_MEM {
    struct UNV_FEATURES      features;
    struct DECODER_MEM       dec;

    struct ZERO_FILTER       unq_form_res_filt;
    struct ZERO_FILTER       form_res_filt;
    struct POLE_FILTER       spch_wght_syn_filt;
    struct POLE_FILTER       target_syn_filter;
    struct POLE_FILTER     hipass_p;
    struct ZERO_FILTER       hipass_z;
    struct ZERO_FILTER       decimate;
    int    band_noise_sm[FREQBANDS];
    int	   band_power[4];
    int    signal_energy[FREQBANDS];
    int    frame_energy_sm[FREQBANDS];
    int    band_power_last[FREQBANDS];
    int    snr[FREQBANDS];
    int    pitch_target_energy[MAXSF];
    int    pitch_target_energy_after[MAXSF];
    int    codebook_target_energy[MAXSF];
    int    codebook_target_energy_after[MAXSF];
	int    frame_num;
     /* these are the new variables needed for rate control */
    int    block_cnt,full_cnt,full_force,half_cnt,half_force;
    /* total counts */
    int    block_cnt_t,full_cnt_t,full_force_t,half_cnt_t,half_force_t,
                     quarter_cnt_t,total_speech_frames;
	int     quarter_cnt /*, hist_full[4],hist_half[4]*/;
	int    hangover, hangover_in_progress;
    short  last_k[LPCORDER];
    short  last_lsp[LPCORDER];
    short  last_rate;   /* rate decisision after 2nd stage select_mode2() */
    short  last_rate_1st_stage; /* rate decision after 1st stage */
    short  num_full_frames;   
    short  band_rate[FREQBANDS];
    short  resid_mem[PMAX];

    short  adaptcount;
    short  pitchrun;
    short  snr_stat_once;
    short  snr_map[FREQBANDS];


    short  target_snr,        /* target_snr for frame */
           target_snr_thr;    /* target snr threshold */
    short  target_snr_mean,
           target_snr_var;
    short  avg_rate,   /* average rate over STATWINDOW active frames */
           avg_rate_thr;
 
    short  hist_above[4], hist_below[4];

    short  pitch_gain[MAXPITSF]; /* Changed from [MAXSF/MAXCBPERPIT];*/
    short  pitch_lag[MAXPITSF];  /* Changed from [MAXSF/MAXCBPERPIT];*/
    short  target_after_save[FSIZE];  

};


struct LPCPARAMS {
    short  k[LPCORDER];
    short  qcode_k[LPCORDER];
    short  lsp[LPCORDER];
    short  qcode_lsp[LPCORDER];
};


struct CBPARAMS {
    int      sd;
    short	 G;
    short    i;
    short    qcode_G;
    short    qcode_i;
    short    qcode_Gsign;
    short    seed;
};

struct PACKET {
	int    sd_enc;
    char*  data;
    short  mode;
    short  lpc[LPCORDER];
    short  lsp[LPCORDER];
    short  min_lag;
    short  b[MAXSF], lag[MAXSF], G[MAXSF][MAXNUMCB], i[MAXSF][MAXNUMCB];
    short  sd_dec;
    short  Gsign[MAXSF][MAXNUMCB], frac[MAXSF];
};

struct SNR {
    int   num_frames[2];
    int   signal_energy[2];
    int   noise_energy[2];
    int   seg_snr[2];
};

typedef struct {
	struct ENCODER_MEM*     encoder_memory;
	struct PACKET*          packet;
	FrameStream      *stream;
	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
	short*  in_speech;
	short*  out_speech;
	short*	lpc_ir; 
	short   rda_flag;
	short   reduced_rate_flag;
	short   hysteresis;
	short	last_solid_voiced;	
	VO_PTR  hCheck;
}QCP13_ENCOBJ;
   

/***********************************************************************
*
* Function Prototypes
*
************************************************************************/

extern short anticosvalue(short val, short scale);

extern void adjust_rate_down(
    struct ENCODER_MEM  *e_mem
);

extern void adjust_rate_up(
    struct ENCODER_MEM  *e_mem 
);

extern void band_energy_fcn(
    int                 *R,
	int                 *energy
);

extern void clear_packet_params(
     struct PACKET *packet
);

extern void create_target_speech(
    short  mode,
    short  *input,
    short  *resid,
    short  *target,
    short  lpc[2][MAXSF][2][LPCORDER],
    struct ZERO_FILTER *err_filt,
    struct POLE_FILTER *wght_filt
);

extern int compute_autocorr(
    short   *signal,
    short   length,
    short   shift
);

#ifdef ARMv7_OPT
extern void comp_corr40_pitch( 
					  short *scal_sig,   /* i   : scaled signal.                          */
					  short *target,
					  short L_frame,     /* i   : length of frame to compute pitch        */
					  short lag_max,     /* i   : maximum lag                             */
					  short lag_min,     /* i   : minimum lag                             */
					  int	*corr);       /* o   : correlation of selected lag             */
#endif
extern void comp_corr( 
					  short *scal_sig,   /* i   : scaled signal.                          */
					  short *target,
					  short L_frame,     /* i   : length of frame to compute pitch        */
					  short lag_max,     /* i   : maximum lag                             */
					  short lag_min,     /* i   : minimum lag                             */
					  int	*corr);       /* o   : correlation of selected lag             */

extern int Lag_max(  
				   int   *corr,      
				   short scal_sig[],   
				   int   L_frame,      
				   int   lag_max,      
				   int   lag_min,    
				   int  *cor_max      
				   );

extern int compute_cb(
					  short                  mode,
					  short                  *target,
					  struct ENCODER_MEM     *e_mem,
					  short                  *lpc_ir,
					  struct CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
					  short                  cbsf 
					  );

extern int compute_cb_gain(
     short                  mode,
     struct ENCODER_MEM     *e_mem,
     struct CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
     short                  *speech,
     short                  cbsf 
);

extern void compute_features(
    int                  *pitch_cor,
    short                *pitch,
    short                *resid,
    short                *speech,
    struct UNV_FEATURES  *unv_features,
    int                  *acf,
    short                *lpc,
    short                *lsp,
    int                  *band_energy,
    struct ENCODER_MEM     *e_mem
);

extern void compute_lpc(
    short		       *speech,
    short              windowsize,
    short              windowtype,
    short              order,
    short	           *lpc, 
    int		           *R
);

extern void compute_pitch(
    short                 mode,
    short                 *target,
    struct ENCODER_MEM    *e_mem,
//    short                 *lpc,
	short				  *lpc_ir,	 
    struct PITCHPARAMS    *pitch_params 
);

//extern void compute_snr(
//     struct SNR     *snr,
//     struct CONTROL *control 
// );

extern void compute_sns(
						int	  *R,
						int   *sns,
						short *lpc,
						short *lsp
						);

extern void compute_target_snr(
							   short               mode,
							   struct ENCODER_MEM  *e_mem
							   );
#ifdef ASM_OPT
extern void do_pole_filter_asm( 
						   short                *input,
						   short                *output,
						   short                numsamples,
    					   struct POLE_FILTER   *filter,
						   short                update_flag
						   );
extern void do_pole_filter_high_asm( 
						   short                *input,
						   short                *output,
						   short                numsamples,
						   struct POLE_FILTER   *filter,
						   short                update_flag
						   );
#endif 
extern void do_pole_filter( 
    short                *input,
    short                *output,
    short                numsamples,
    struct POLE_FILTER   *filter,
    short                update_flag
);

// add by lhp
extern void do_pole_filter_response( 
    short                *output,
    short                numsamples,
    struct POLE_FILTER   *filter,
    short                update_flag
);

extern void do_pole_filter_high( 
    short                *input,
    short                *output,
    short                numsamples,
    struct POLE_FILTER   *filter,
    short                update_flag
);


extern void debug_do_pole_filter( 
    short                *input,
    short                *output,
    short                numsamples,
    struct POLE_FILTER   *filter,
    short                update_flag
);

extern void do_zero_filter(
    short               *input,
    short               *output,
    short               numsamples,
    struct ZERO_FILTER  *filter,
    short               update_flag 
);


extern void do_fir_linear_filter( 
    short               *input,
    short               *output,
    short               numsamples,
    struct ZERO_FILTER  *filter,
    short               update_flag 
);

#ifdef ASM_OPT
extern void do_zero_filter_asm(
		short               *input,
		short               *output,
		short               numsamples,
	    struct ZERO_FILTER  *filter,
		short               update_flag);

extern void do_fir_linear_filter_asm( 
		short               *input,
		short               *output,
		short               numsamples,
	    struct ZERO_FILTER  *filter,
		short               update_flag);

void filter_1_tap_interp_asm(
		short                     *input,
		short                     *output,
		short                     numsamples,
	    struct POLE_FILTER_1_TAP  *filter,
		short                     update_flag 
);
#endif

extern void do_pole_filter_1_tap_interp(
    short                     *input,
    short                     *output,
    short                     numsamples,
    struct POLE_FILTER_1_TAP  *filter,
    short                     update_flag 
);

extern void durbin(
    int	  *R,
    short *lpc,
    short order
);

extern void encoder(
    QCP13_ENCOBJ* qcpenc
);

extern void free_encoder(
    struct  ENCODER_MEM *e_mem,
	VO_MEM_OPERATOR *pMemOP
);

extern void free_pole_filter(
    struct  POLE_FILTER  *filter,
	VO_MEM_OPERATOR *pMemOP
);

extern void free_zero_filter(
    struct ZERO_FILTER  *filter,
	VO_MEM_OPERATOR *pMemOP
);

extern void front_end_filter(
    short               *speech,
    struct ENCODER_MEM  *e_mem 
);


extern void get_zero_input_response_pole(
    short               *response,
    short               length,
    struct POLE_FILTER  *filter 
);

extern void get_impulse_response_pole(
    short               *response,
    short               length,
    struct POLE_FILTER  *filter 
);

extern void get_zero_input_response_pole_1_tap_interp(
    short                     *response,
    short                     length,
    struct POLE_FILTER_1_TAP  *filter 
);

extern int getbit(
    struct PACKET *packet,
    short         type,
    short         number,
    short         loc
);

// extern short hammsinc(short input);

extern void HAMMINGwindow(
    short *input,
    short *output,
    short   length
);

extern short initialize_encoder(struct  ENCODER_MEM *e_mem, VO_MEM_OPERATOR  *pMemOP);
extern void reset_encoder(struct ENCODER_MEM *e_mem);

extern int initialize_pole_1_tap_filter(
    struct POLE_FILTER_1_TAP  *filter,
    short                     max_order,
	VO_MEM_OPERATOR           *pMemOP
);


extern int initialize_d_pole_filter(
    struct  POLE_FILTER    *filter,
    short                   order
);

extern int initialize_pole_filter(
    struct  POLE_FILTER     *filter,
    short                   order,
	VO_MEM_OPERATOR         *pMemOP
);

extern void initial_recursive_conv(
    short   *resid,
    short   length,
    short   *impulse_response
);

extern int initialize_zero_filter(
    struct ZERO_FILTER  *filter,
    short               order,
	VO_MEM_OPERATOR     *pMemOP
);

extern void lin_quant(
    short   *qcode,
    short	min,
    short	max,
    short   num_levels,
    short	input
);

extern void lin_unquant(
    short   *output,
    short   min,
    short   max,
    short   num_levels,
    short   qcode
);

extern void lpc2lsp(
    short *lpc,
    short *lsp,
#if PREV_LSP_FIX
    short *prev_lsp,
#endif
    short   order 
);

extern void lsp2lpc(
    short  *lsp,
    short  *lpc,
    short  order 
);

extern void interp_lpcs(
    short            mode,
    short            *prev_lsp,
    short            *curr_lsp,
    short            lpc[2][MAXSF][2][LPCORDER],
    short            type
);

extern void pack_cb(
    short           mode,
    struct CBPARAMS cb_params[MAXCBPERPIT][MAXNUMCB],
    struct PACKET   *packet,
    short           psf, /* pitch sub-frame */
    short           cbsf  /* codebook sub-frame */
);

extern void pack_pitch(
    struct PITCHPARAMS *pitch_params,
    struct PACKET      *packet,
    short              sf
);

extern void pack_lpc(
    short              mode,
    struct LPCPARAMS *lpc_params,
    struct PACKET    *packet
);

extern void pack_frame(
     short         mode,
     struct PACKET *packet
);


extern void quantize_b(
    short   unq_b,
    short   *q_b,
    short   *qcode_b 
);

extern void quantize_lag(
    short   *lag,
    short   *qcode_lag,
    short   *frac,
    short   *qcode_frac 
);

extern void quantize_lpc(
    short                 mode,
    short               lpc[LPCORDER],
    short               *lsp,
    short               *qlsp,
    int                 *R,
    struct LPCPARAMS    *lpc_params,
    struct ENCODER_MEM  *e_mem
);

extern void quantize_min_lag(
    short  min_lag,
    short  *qcode_min_lag
);

extern void quantize_G(
    short   mode,
    int     unq_G,
    short   *q_G,
    short   *qcode_G,
    short   *qcode_Gsign,
    short   *lastG, /* only needed for FULLRATE every fourth CB subframe */
    short   cbsf,
    short   *G_pred 
);

extern void quantize_G_8th(
						   short     unq_G,            /* input Gain value before quantization  */
						   short     *q_G,             /* recontructed CB Gain */
						   short     *qcode_G,         /* quant. code for CB Gain */
						   short     *G_pred           /* prediction for CB Gain */
						   );

extern void quantize_i(
    short   *i,
    short   *qcode_i
);

extern void recursive_conv(
    short  *resid,
    short  *impulse_response,
    short   length 
);

extern void recursive_conv_10(
    short  *resid,
    short  *impulse_response,
    short   length 
);

#ifdef ASM_OPT
extern void initial_recursive_conv_asm(
								   short   *resid,
								   short   length,
								   short   *impulse_response
								   );

extern void recursive_conv_40_Opt(
							  short  *resid,
							  short  *impulse_response,
							  short   length 
							  );
#endif
extern void run_decoder(
    short                     mode,
    struct  DECODER_MEM     *d_mem,
    short                   lpc[2][LPCORDER],
    struct  PITCHPARAMS     *pitch_params,
    struct  CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
    short                   *out_buffer,
    short                   length,
    short                   numcbsf,
    short                   numcb
);

extern void save_pitch(
					   struct PITCHPARAMS  *pitch_params,
					   short               *pitch_lag,
					   short               *pitch_gain
					   );

extern void save_target(
						short *target,
						short *gt_target,
						short length
						);

extern void select_mode1(
    short               *rate,
	int 	            *acf,
    struct ENCODER_MEM  *e_mem
);

extern void select_mode2(
    short               *rate,
    short               *form_resid,
    int		            *acf,
    struct ENCODER_MEM  *e_mem,
    short               *speech,
    short               *lpc,
    short               *lsp,
	short				*last_solid_voiced,	
	short				reduced_rate
);

extern void set_lag_range(
    struct PITCHPARAMS  *pitch_params,
    short               *qcode_min_lag 
);

//extern float small_sinc(float val);

extern void target_reduction(
							 short *after_search,
							 short *before_search,
							 int *energy_before,
							 int *energy_after,
							 short   length 
							 );

extern short truefalse(
    short word,
    short bitloc
);


extern void unquantize_b(
    short   *q_b,
    short   *qcode_b,
    short   lag 
);

extern void unquantize_G(
    short   mode,
    short   *q_G,
    short   *qcode_G,
    short   *qcode_Gsign,
    short   *lastG,
    short   cbsf,
    short   *G_pred 
);

extern void unquantize_G_8th(
    short   *q_G,          /* reconstructed G value */
    short   *qcode_G,      /* quantization code for Gain */
    short   *G_pred        /* predictor */
);

extern void unquantize_i(
    short   *i,
    short   *qcode_i 
);

extern void unquantize_lag(
    short   *lag,
    short   *qcode_lag,
    short   *frac,
    short   *qcode_frac 
);

extern void unquantize_lsp(
    short               mode,
    short               *qlsp,          /* output unquantized lsp's */
    short               *last_qlsp,     /* input - unquantized last lsp */
    short               *qcode,
    struct DECODER_MEM  *d_mem 
);

void unquantize_min_lag(short *, short);

extern void update_form_resid_mems(
    short               *input,
    struct ZERO_FILTER  *err_filt
);

extern void update_hist_cnt(
    struct ENCODER_MEM  *e_mem,
    short               rate 
);

//extern void update_snr(
//     int         type,
//     float       *signal,
//     float       *sig_and_noise,
//     struct SNR  *snr
// );

extern void update_target_cb(
    short               mode,
    short               *pw_speech,
    short               *target,
    short               *lpc,
    struct PITCHPARAMS  *pitch_params,
    struct DECODER_MEM  *d_mem
);

extern void update_target_pitch(
    short              mode,
    short              *pw_speech,
    short              *target,
    short              *lpc,
    struct DECODER_MEM *d_mem
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   //__CELP_H__












