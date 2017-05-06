#ifndef proto_func_fx_h
#define proto_func_fx_h

#include <stdio.h>

#include "typedef.h"
#include "mem_fx.h"


/* AMR-WB+ Function prototypes */
Word16 Coder_amrwb_plus_stereo(
  Word16 channel_right[],     /* input: used on mono and stereo */
  Word16 channel_left[],      /* input: used on stereo only */
  Word16 codec_mode,    /* input: AMR-WB+ mode (see cnst.h) */
  Word16 L_frame,       /* input: 80ms frame size */
  Word16 serial[],    /* output: serial parameters */
  Coder_State_Plus_fx * st,  /* i/o : coder memory state */
  Word16 useCaseB, 
  Word16 bwe_flag,      /* AriL: for 32kHz NBWE */
  Word16 StbrMode
);
void Init_coder_amrwb_plus(Coder_State_Plus_fx *st, Word16 num_chan, Word16 fscale, Word16 use_case_mode, Word16 full_reset);

Word16 Coder_amrwb_plus_mono(
  Word16 channel_right[],       /* input: used on mono and stereo */
  Word16 codec_mode,      /* input: AMR-WB+ mode (see cnst.h) */
  Word16 L_frame, /* input: 80ms frame size */
  Word16 serial[],      /* output: serial parameters */
  Coder_State_Plus_fx * st,    /* i/o : coder memory state */
  Word16 useCaseB, 
  Word16 bwe_flag        /* AriL: for 32kHz NBWE */
);
void Coder_amrwb_plus_mono_first(
  Word16 channel_right[], /* input: used on mono and stereo */
  Word16 n_channel, /* input: 1 or 2 (mono/stereo) */
  Word16 L_frame,   /* input: frame size */
  Word16 L_next,    /* input: lookahead */
  Word16 bwe_flag,  /* AriL: for 32kHz NBWE */
  Coder_State_Plus_fx * st       /* i/o : coder memory state */
);

Word16 Coder_amrwb_plus_first(
  Word16 channel_right[],      /* input: used on mono and stereo */
  Word16 channel_left[],       /* input: used on stereo only */
  Word16 n_channel,      /* input: 1 or 2 (mono/stereo) */
  Word16 L_frame,        /* input: frame size */
  Word16 L_next, /* input: lookahead */
  Word16 bwe_flag,       /* AriL: for 32kHz NBWE */
  Coder_State_Plus_fx * st    /* i/o : coder memory state */
);
void Coder_lf(  
  Word16 codec_mode,  /* (i) : AMR-WB+ mode (see cnst.h)             */ 
  Word16 speech[],    /* (i) : speech vector [-M..L_FRAME_PLUS+L_NEXT]    */ 
  Word16 synth[],     /* (o) : synthesis vector [-M..L_FRAME_PLUS]        */ 
  Word16 mod[],       /* (o) : mode for each 20ms frame (mode[4]     */
  Word16 AqLF[],      /* (o) : quantized coefficients (AdLF[16])     */ 
  Word16 window[],    /* (i) : window for LPC analysis               */
  Word16 param[],     /* (o) : parameters (NB_DIV*NPRM_DIV)          */
  Word16 ol_gain[],   /* (o) : open-loop LTP gain                    */  
  Word16 ave_T_out[], /* (o) : average LTP lag                       */ 
  Word16 ave_p_out[], /* (o) : average LTP gain                      */ 
  Word16 coding_mod[],/* (i) : selected mode for each 20ms           */
  Word16 pit_adj,     /* (i) : indicate pitch adjustment             */
  Coder_State_Plus_fx *st/* i/o : coder memory state               */
  ) ;
/* Decoder files */
Word16 Decoder_amrwb_plus(  /* output: number of sample processed */
  Word16 codec_mode,    /* input: AMR-WB+ mode (see cnst.h)         */
  Word16 serial[],    /* input: serial parameters (4x20ms)        */
  Word16 bad_frame[],   /* input: bfi (bad_frame[4])                */
  Word16 L_frame,       /* input: frame size of synthesis           */
  Word16 n_channel,     /* input: 1 or 2 (mono/stereo)              */
  Word16 channel_right[], /* (o): used on mono and stereo         */
  Word16 channel_left[],  /* (o): used on stereo only             */
  Decoder_State_Plus_fx *st,  /* i/o:  decoder memory state              */
  Word16 fscale,      /* AriL */
  Word16 StbrMode,
  Word16 mono_dec_stereo,
  Word16 upsamp_fscale);

void Init_decoder_amrwb_plus(
    Decoder_State_Plus_fx *st, 
    Word16 num_chan,
    Word16 fscale, 
    Word16 full_reset);
void Find_wsp(
    const Word16 *Az,           /* i:   A(z) filter coefficients                Q12 */
    const Word16 *speech_ns,    /* i:   pointer to the denoised speech frame        */
    Word16       *wsp,          /* o:   poitnter to the weighted speech frame       */
    Word16       *mem_wsp,       /* i/o: W(z) denominator memory                     */
    Word16       lg
);

/* params*/
void Dec_prm_stereo_x(
  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
  Word16 serial[],    /* (i) : serial bits stream               */
  Word16  nbits_pack,    /* (i) : number of bits per packet of 20ms*/
  Word16  nbits_bwe,     /* (i) : number of BWE bits per 20ms (AriL) */
  Word16  param[],       /* (o) : decoded parameters               */
  Word16  brMode,
  Decoder_State_Plus_fx* st);

void Dec_prm(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 frames)   */
  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
  Word16 serial[],    /* (i) : serial bits stream               */
  Word16 nbits_pack,    /* (i) : number of bits per packet of 20ms*/
  Word16 codec_mode,    /* (i) : AMR-WB+ mode (see cnst.h)        */
  Word16 param[],       /* (o) : decoded parameters               */
  Word16 nbits_AVQ[]);   /* (o) : nb of bits for AVQ (4 division)  */
void Dec_prm_hf(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 frames)   */
  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
  Word16 serial[],    /* (i) : serial bits stream               */
  Word16 nbits_pack,    /* (i) : number of bits per packet of 20ms*/
  Word16 param[]);       /* (o) : decoded parameters               */


void Writ_data(
  Word16 data[],  /* input : data              */
  Word16 size,    /* input : number of samples */
  FILE  *fp,      /* output: file pointer      */
  Word16 Frame_scaling
  
);


/* decoder routines*/
void Decoder_amrwb_plus_1(
  Word16* chan_right,
	Word16* chan_left,
	Word16* mod, 
	Word16* param,
	Word16* prm_hf_right,
	Word16* prm_hf_left,
	Word16* nbits_AVQ, 
	Word16 codec_mode, 
	Word16* bad_frame, 
	Word16 *bad_frame_hf,
	Word16* AqLF, 
	Word16* synth, 
	Word16* pitch, 
	Word16* pit_gain, 
	Decoder_State_Plus_fx* st, 
	Word16 n_channel,
	Word16 L_frame,
	Word16 bwe_flag,
  Word16 mono_dec_stereo);

void Init_decoder_lf(Decoder_State_Plus_fx *st);
void Decoder_lf(
  Word16 mod[],         /* (i)  : mode for each 20ms frame (mode[4]     */
  Word16 param[],       /* (i)  : parameters                            */
  Word16 nbits_AVQ[],   /* (i)  : for each frame (nbits_AVQ[4])         */
  Word16 codec_mode,    /* (i)  : AMR-WB+ mode (see cnst.h)             */ 
  Word16 bad_frame[],   /* (i)  : for each frame (bad_frame[4])         */
  Word16 AzLF[],        /* (o)  : decoded coefficients (AdLF[16])       */ 
  Word16 exc1[],        /* (o)  : decoded excitation                    */
  Word16 syn[],         /* (o)  : decoded synthesis                     */
  Word16 pitch[],       /* (o)  : decoded pitch (pitch[16])             */
  Word16 pit_gain[],    /* (o)  : decoded pitch gain (pit_gain[16])     */
  Word16 pit_adj,
  Decoder_State_Plus_fx *st     /* i/o : coder memory state                     */
  
);
void Decoder_acelp(
  Word16 prm[],         /* input: parameters               */
  Word16 lg,            /* input: frame length             */
  Word16 codec_mode,    /* input: AMR-WB+ mode (see cnst.h)*/
  Word16 bfi,           /* input: 1=bad frame              */
  Word16 *pT,           /* out:   pitch for all subframe   */
  Word16 *pgainT,     /* out:   pitch gain for all subfr */
  Word16 Az[],
  Word16 wexc[],
  Word16 wsynth[],
  Word16 pit_adj,
  Word16 len,
  Word16 stab_fac,
  Decoder_State_Plus_fx *st /* i/o :  coder memory state       */
);

void   Init_decoder_hf(Decoder_StState_fx * st);
void Decoder_hf(
  Word16 mod[],         /* (i)  : mode for each 20ms frame (mode[4]     */
  Word16 param[],       /* (i)  : parameters                            */
  Word16 param_other[], /* (i)  : parameters for the right channel in case of mono decoding with stereo bitstream */
  Word16 mono_dec_stereo, /* 1=Mono decoding with stereo bitstream */
  Word16 bad_frame[],   /* (i)  : for each frame (bad_frame[4])         */
  Word16 AqLF[],      /* (i)  : decoded coefficients (AdLF[16])       */ 
  Word16 exc[],       /* (i)  : decoded excitation                    */
  Word16 synth_hf[],  /* (o)  : decoded synthesis                     */
  Word16 mem_lpc_hf[], 
  Word16 *mem_gain_hf,
  Word16 *ramp_state,
  Decoder_StState_fx *st,  /* i/o : decoder memory state                */
  Word16 old_subfr_q
  );


void Decoder_tcx(
  Word16 prm[],         /* input:  parameters              */
  Word16 nbits_AVQ[],   /* input:  nbits in parameters of AVQ */
  Word16 A[],         /* input:  coefficients NxAz[M+1]  */
  Word16 L_frame,       /* input:  frame length            */
  Word16 bad_frame[],   /* input:  Bad frame indicator     */
  Word16  exc[],       /* output: exc[-lg..lg]            */
  Word16 synth[],     /* in/out: synth[-M..lg]           */
  Decoder_State_Plus_fx *st) /* i/o : coder memory state        */
;

/* stereo */
void Init_dec_hi_stereo(Decoder_State_Plus_fx *st);

void Decoder_stereo_x(
  Word16 param[],			/* (i)	:	Codebooks indices		*/
	Word16 bad_frame[],		/* (i)	:	Bad frame index			*/
	Word16 sig_left_[],		/* (o)	:	Decoded left channel	*/
	Word16 synth_[],		/* (o)	:	Decoded right channel	*/
	Word16 AqLF_[],
	Word16 StbrMode,
	Word16 fscale,
	Decoder_State_Plus_fx *st		/* (i/o):	Decoder states			*/		
);	

void Decim_2k(Word16 *in, Word16 *out, Word16 L_frame, const Word16 *filter);
void Oversamp_2k(Word16 *in, Word16 *out, Word16 L_frame, const Word16 *filter);

Word16 my_max_fx(Word16 x, Word16 y);
Word16 my_min_fx(Word16 x, Word16 y);
void Dec_filt(
  Word16  *prm,
  Word16 wh[],        // Q15
  Word16 old_wh[],    // Q15
  Word16 bfi,
  const PMSVQ_fx* st);

void Dec_gain(
  Word16  *prm,
  Word16 *gain_left,  // Q10
  Word16 *gain_right, // Q10
  Word16 *old_gain,   // Q10
  Word16  bfi,
  const PMSVQ_fx* st);

void Get_exc_win(Word16 *exc_ch,        // out
                    Word16 *buf,           // in
                    Word16 *exc_mono,      // in
                    Word16 *side_buf,      // in
                    Word16 *win,           // in
                    Word16 gain[],         // gain
                    Word16 N,              // N samples
                    Word16 doSum);          // 1 = sum, 0 = subtract

void Get_exc(Word16 *exc_ch,        // out
                Word16 *exc_mono,      // in
                Word16 *side_buf,      // in
                Word16 gain,           // gain
                Word16 N,              // N samples
                Word16 doSum);          // 1 = sum, 0 = subtract

Word16 Comp_hi_gain(Word16 gcode0);   // Input in Q8, output Q14

void Dec_hi_stereo(Word16 synth_hi_t0[],  // Q0
  Word16 right_hi[],                // Q1
  Word16 left_hi[],                 // Q1 
  Word16 AqLF[],                    // Q12
  Word16   param[],
  Word16 bad_frame[],
  Word16 fscale,
  Decoder_State_Plus_fx *st);

void Init_tcx_stereo_decoder(Decoder_State_Plus_fx *st);
void Coder_stereo_x(
  Word16 AqLF[],            /* (i)  : Mono frequencies coefficient  */
  Word16 brMode,            /* (i)  : Stereo bit rate index         */
  Word16 param[],			/* (o)	: Encoded parameters            */	
  Word16 fscale,            /* (i)  : Internal Frequency scaling    */
  Coder_State_Plus_fx *st,  /* i/o  : Encoder states	            */
  Word16 wspeech_hi[],	    /* (i)	: Mixed channel, hi	            */
  Word16 wchan_hi[],		/* (i)	: Right channel, hi	            */
  Word16 wspeech_2k[],	    /* (i)	: Mixed channel, lo             */
  Word16 wchan_2k[]		    /* (i)	: Right channel, lo	            */
);
void Cod_tcx_stereo(
  Word16 wmono_2k[],
  Word16 wright_2k[],  
  Word16 param[],
  Word16 brMode,
  Word16 mod[],
  Word16 fscale,
  Coder_State_Plus_fx *st
  );

Word16 D_gain_pan(				   // Q14
				 Word16 index);        // 

void Comp_gain_shap(       
         Word16 *wm,           // Q0
         Word16 *gain_shap,    // Q14
         Word16 lg,
         Word16 Qwm);

void Windowing(       
         Word16 length, 
         Word16 *vec,          // Q0
         Word16 *window,       // Q15
         Word16 offset_vec, 
         Word16 offset_win);

void Apply_xnq_gain(
         Word16 lg,           
         Word16 gain_pan_fx,   // Q14
         Word16 gain_fx,       // Q0 
         Word16 *xnq_fx,       // Q floating, given by ifft3_fx 
         Word16 *wm_fx,        // Q0 
         Word16 xnq_scale);    // scale factor from ifft3_fx 

void Apply_tcx_overlap(
         Word16 *xnq_fx,       // Q0
         Word16 *wovlp_fx,     // Q0
         Word16 lext, 
         Word16 L_frame);

void Apply_gain_shap(
         Word16 lg, 
         Word16 *xnq_fx,       // Q floating, given by ifft3_fx
         Word16 *gain_shap_fx);// Q14

void Apply_wien_filt(       // Not VERIFIED !!
         Word16 lg, 
         Word16 *xnq_fx,       // Q0
         Word16 *h_fx,         // Q?
         Word16 *wm_fx);       // Q0

void Ch_sep(
         Word16 *synth_side_fx,// Q0 
         Word16 alpha_fx,      // Q15
         Word16 *synth_2k_fx,  // Q0
         Word16 *left_2k_fx,   // Q0
         Word16 *right_2k_fx,  // Q0
         Word16 start, 
         Word16 end);

Word16 D_gain_tcx_st(          // Q scale - 14
         Word16 index,     
         Word16 code[],        // Q floating, given by scalefactor from ifft_fx
         Word16 lcode,         
         Word16 bfi,
         Word16 *oldrms,       // Q3
         Word16 *scale);       // scale factor from ifft3_fx, scalefactor modified by this function. 

Word16 Ifft3_st(               // scale factor given by Ifft3
         Word16 xri[],         // Q9 
         Word16 xnq[],         // Q floating, given by scale factor 
         Word16 lg,
         Word16 Scale1);

Word16 Get_alpha(           // Q15
         Decoder_State_Plus_fx* st, 
         Word16 mod[], 
         Word16 k, 
         Word16 bad_frame[]);

Word16 Gain_pan(            // Q14
         Word16 bad_frame,
         Decoder_State_Plus_fx *st,
         Word16 prm);

void Cos_window2(
         Word16 window[],      // Q15
         Word16 n1, 
         Word16 n2);

void Dtcx_stereo(
         Word16 synth[],     // in/out: synth[-M..lg]           */
         Word16 mono[],
         Word16 wovlp[],     /* i/o:    wovlp[0..127]           */
         Word16 ovlp_size,     /* input:  0, 64 or 128 (0=acelp)  */
         Word16 L_frame,       /* input:  frame length            */
         Word16 prm[],
         Word16 pre_echo,
         Word16 bad_frame[],
         Decoder_State_Plus_fx *st);

void Dec_tcx_stereo(
         Word16 synth_2k[],
				 Word16 left_2k[],
				 Word16 right_2k[],
				 Word16 param[],
				 Word16 bad_frame[],
				 Decoder_State_Plus_fx *st);
void Init_decoder_stereo_x(Decoder_State_Plus_fx *st);
void Mix_ch(
  Word16 *ch_left,	/* input: samples from left channel */
	Word16 *ch_right,	/* input: samples from right channel */
	Word16 *ch_sum,		/* output: mixed mono signal */
	Word16 n,				  /* input: length of frame */	
	Word16 gA,			  /* input: weight factor for left channel Q14 */
	Word16 gB			    /* input: weight factor for right channel Q14 (avoid multiply by 0.5) */
);

Word16 Balance(Word16 bad_frame,Decoder_State_Plus_fx *st,Word16 prm);
void Apply_xnq_gain2(Word16 lg, Word16 wbalance, Word32 Lgain, Word16 *xnq_fx, Word16 *wm_fx, Word16 Q_syn);
/* tcx_ecu.c */
void Adapt_low_freq_deemph_ecu(
  Word16 xri[],
  Word16 lg,  
  Word16 Q_ifft,
  Decoder_State_Plus_fx *st
  );
void Deemph1k6(
  Word16 *xri, 
  Word16 e_max,
  Word16 m_max,
  Word16 lg4
);
Word32 SpPeak1k6(
  Word16 *xri, 
  Word16 *exp_m,
  Word16 lg4
);
Word16 Atan2(
  Word16 ph_x,
  Word16 ph_y
);
void Find_x_y(
  Word16 angle,
  Word16 *ph        /*x, y Q15 */
);


void Adap_low_freq_deemph(Word16 xri[], Word16 lg);
void Reconst_spect(
  Word16 xri[],
  Word16 old_xri[],
  Word16 n_pack,
  Word16 bfi[],
  Word16 lg,
  Word16 last_mode,
  Word16 Q_ifft
);
/* bit packing and unpacking functions in bits.c */
void Int2bin(
  Word16 value,         /* input : value to be converted to binary      */
  Word16 no_of_bits,    /* input : number of bits associated with value */
  Word16 *bitstream     /* output: address where bits are written       */
);
Word16 Bin2int(         /* output: recovered integer value              */
  Word16 no_of_bits,    /* input : number of bits associated with value */
  Word16 *bitstream     /* input : address where bits are read          */
);

Word16 Unpack4bits(Word16 nbits, Word16 *prm, Word16 *ptr);
void Enc_prm_stereo_x(
  Word16 param[],       /* (i) : parameters                       */
  Word16 sserial[],    /* (o) : serial bits stream               */
  Word16 nbits_pack,     /* (i) : number of bits per packet of 20ms*/
  Word16 nbits_bwe,	/* (i) : number of BWE bits per 20ms (AriL) */
  Word16 brMode
);

/* decimation functions*/
Word16 Over_fs(        /* number of sample oversampled       */
  Word16 sig_in[],     /* input:  signal to oversample       */
  Word16 sig_out[],    /* output: signal oversampled         */
  Word16 lg,           /* input:  length of output           */
  Word16 fac_down,     /* input:  fs*12/fac_down = 44k/48k   */
  Word16 mem[],        /* in/out: mem[2*L_FILT_OVER_FS]      */
  Word16 *frac_mem     /* in/out: interpol fraction memory   */
);
Word16 Decim_fs(       /* number of sample oversampled       */
  Word16 sig_in[],     /* input:  signal to decimate         */
  Word16 lg,           /* input:  length of input            */
  Word16 sig_out[],    /* output: signal decimated           */
  Word16 fac_up,         /* input:  44k/48k *fac_up/12 = fs    */
  Word16 mem[],        /* in/out: mem[2*L_FILT_DECIM_FS]     */
  Word16 *frac_mem     /* in/out: interpol fraction memory   */
);

Word16 Join_over_12k8(   /* number of sample oversampled       */
  Word16 sig12k8_lf[], /* input:  LF signal (fs=12k8)        */
  Word16 sig12k8_hf[], /* input:  HF signal (fs=12k8)        */
  Word16 lg,             /* input:  length of LF and HF        */
  Word16 sig_fs[],     /* output: oversampled signal         */
  Word16 lg_output,      /* (i)   : L_FRAME44k if 44kHz        */
  Word16 fac_fs,         /* input:  at 48kHz, ovclk fac = fac_fs/OVCLK_DENOM */
  Word16 mem[],        /* in/out: mem[L_MEM_JOIN_OVER]       */
  Word16 *frac_mem       /* in/out: interpol fraction memory   */
  
);
void Oversamp_12k8(
  Word16 sig12k8[],    /* input:  signal to oversampling  */
  Word16 sig_fs[],     /* output: oversampled signal      */
  Word16 lg,             /* input:  length of output        */
  Word16 mem[],        /* in/out: memory (2*L_FILT)       */
  Word16 band,           /* input:  0/2/3=0..6.4k, 1=6.4..10.8k */
  Word16 add             /* input:  1 if result is added to  */
  		      /*         output vector, 0 if not */
  );

void Decim_12k8_p(
  Word16 sig_fs[],     /* (i)  : signal to decimate      */
  Word16 lg,           /* (i)  : length of input         */
  Word16 sig12k8[],    /* (o)  : decimated signal        */
  Word16 mem[],        /* (i/o): memory (2*L_FILT_FS)    */
  Word16 band         /* (i)  : 0=0..6.4k, 1=6.4..10.8k */
);
Word16 Read_data(      /* return: number of data successfully read */
  FILE  *fp,        /* input : data file (16-bit words)         */
  Word16 data[],     /* output: speech data                      */
  Word16 size        /* input : number of samples                */
);
  
// @shanrong modified
#if (!FUNC_INTERPOL_MEM_ASM)
void Interpol_mem(
  Word16 *signal,         /* (i)  : Signal    */
  Word16 *signal_int,     /* (o)  : interpolated signal           */
  Word16 L_frame_int,     /* (i)  : Length of interpolated signal */
  const Word16 *filter,         /* (i)  : Filter                    Q14 */ 
  Word16 nb_coef,         /* (i)  : Number of coeff               */
  Word16 fac_up,          /* (i)  : Upsampling factor             */
  Word16 fac_down,        /* (i)  : Denumerator on numerator      */
  Word16 gain,            /* (i)  : Gain to apply                 */  
  Word16 *mem_frac        /* (i/o): Fraction memory              */
);
#endif
// end

Word16 Decim_split_12k8(  /* number of sample decimated         */
  Word16 sig_fs[],        /* (i)  : signal to decimate         */
  Word16 lg_input,        /* input:  2*L_FRAME44k if 44kHz      */
  Word16 sig12k8_lf[],    /* (o)  : LF decimated signal        */
  Word16 sig12k8_hf[],    /* (o)  : HF decimated signal        */
  Word16 lg,              /* (i)  : length of LF and HF        */
  Word16 fac_fs,          /* (i)  : at 48kHz, scale fac = fac_fs/FSCALE_DENOM */
  Word16 mem[],           /* (i/o): mem[L_MEM_DECIM_SPLIT]     */
  Word16 *frac_mem        /* (i/o): interpol fraction memory   */
);
/* FFT routines in fft3.c and fft9.c */
void Fft3(Word16 X[], Word16 Y[], Word16 n);
void Ifft3(Word16  Y[], Word16  X[], Word16  n);
void ifft9_fx(Word16 X[],Word16 Y[],Word16 n);
void Ifft_reorder(Word16 xri[], Word16 lg);
void init_r_fft_fx(
  Word16 size,
  Word16 wSizeByTwo,
  Word16 wNumberStage,
  Word16 *ii_table,
  Word16 *phs_tbl
);
/* called functions */
void r_fft_fx(
  Word16 *farray_ptr, /* coeffients in the order re[0], re[1], ... re[n/2], im[1], im[2], ..., im[n/2-1] */
  Word16 wSize,
  Word16 wSizeByTwo,
  Word16 wNumberStage,
  Word16 *ii_table,
  Word16 *phs_tbl,
  Word16 isign
);                    /* 1=fft, otherwize it's ifft                 */ 

/* TCX gain functions in gaintcx.c */
Word32 D_gain_tcx(   /* (o)  : gain                     */
  Word16 index,       /* (i)  : index                    */
  Word16 code[],      /* (i)  : quantized vector         */
  Word16 lcode,
  Word16 bfi,
  Word16 *oldrms
);
/* hf_func.c */
Word16 Match_gain_6k4(Word16 *AqLF, Word16 *AqHF);
void voAMRWBDecIntGain(Word16 old_gain, Word16 new_gain,const Word16* Int_wind, Word16 *gain, Word16 nb_subfr);
void voAMRWBDecSoft_exc_hf(Word16 *exc_hf, Word32 *mem);
void Smooth_ener_hf(Word16 *HF, Word32 *threshold);

/* int_lpc.c */
void Int_lpc(Word16 isp_old[], Word16 isp_new[],
                        const Word16 frac[], Word16 Az[], Word16 n_subfr, Word16 m);
    

/* q_gn_hf.c */

void D_gain_hf(
  Word16 indice,     /* input:  quantization indices */
  Word16 *gain_q,    /* output: quantized gains      */
  Word16 *past_q,    /* i/o   : past quantized gain (1 word) */
  Word16 bfi         /* input : Bad frame indicator  */
);
void Q_gain_hf(
  Word16 *gain,      /* input : gain of 4 subfr */
  Word16 *gain_q,    /* output: quantized gains */
  Word16 *indice       /* output: indices         */
);
/* q_isf_hf.c */

void D_isf_hf(
  Word16 *indice,    /* input:  quantization indices                  */
  Word16 *isf_q,     /* output: quantized ISFs in the cosine domain   */
  Word16 *past_q,    /* i/o   : past quantized isf (for AR prediction)*/
  Word16 bfi,          /* input : Bad frame indicator                  */
  const Word16 *Mean,
  const Word16 *Dico
);
/* qpisf_2s.c */
void Dpisf_2s_46b_2(
  Word16 *indice,    /* input:  quantization indices                  */
  Word16 *isf_q,     /* output: quantized ISFs in the cosine domain   */
  Word16 *past_isfq, /* i/0   : past ISF quantizer                    */
  Word16 *isfold,    /* input : past quantized ISF                    */
  Word16 *isf_buf,   /* input : isf buffer                            */
  Word16 bfi,        /* input : Bad frame indicator                   */
  Word16 bfi_2nd_st, /* input : 2nd stage bfi mask (bin: 011111)      */
  Word16 enc_dec
);
/* RE8 lattice quantiser functions in re8_*.c */
void AVQ_Demuxdec(Word16 n_pack, Word16 *param, Word16 *n_bits, Word16 *xriq, Word16 Nsv, Word16 *bfi);
void RE8_Cod(Word16 x[],Word16 *n, UWord16 *I,Word16 k[]);
void RE8_Vor(Word16 y[], Word16 *n, Word16 k[], Word16 c[], Word16 *ka);
void Re8_k2y(Word16 *k,Word16 m,Word16 *y);
void RE8_Dec(Word16 n, UWord16 I, Word16 k[], Word16 y[]);
void RE8_PPV_FX(Word32 x[] /*Q8*/, Word16 y[]);
void AVQ_Encmux(Word16 n_pack, Word16 *xriq, Word16 *param, Word16 *n_bits, Word16 Nsv);
void Enc_prm(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 division) */
  Word16 codec_mode,    /* (i) : AMR-WB+ mode (see cnst.h)        */
  Word16 sparam[],      /* (i) : Parameters to encode             */  
  Word16 serial[],      /* (o) : serial bits stream               */
  Word16 nbits_pack     /* (i) : number of bits per packet of 20ms*/
);
void Enc_prm_hf(
  Word16 mod[],         /* (i) : frame mode (mode[4], 4 division) */
  Word16 param[],       /* (i) : parameters                       */
  Word16 serial[],    /* (o) : serial bits stream               */
  Word16 nbits_pack     /* (i) : number of bits per packet of 20ms*/
);

/* rnd_ph16.c */
void Rnd_ph16(Word16 *seed, Word16 *xri, Word16 lg, Word16 Qifft);

/* scaling routine*/
void Scale_tcx_ifft( 
  Word16 exc[],         /* (i/o)  : excitation to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_exc        /* (i/o)  : State structure                   */
);
void Rescale_mem(Word16 *mem_syn2, Decoder_State_Plus_fx *st);
void Scale_mem2(Word16 Signal[], Word16* Old_q, Decoder_State_Plus_fx *st, Word16 Syn);
Word16 Scale_exc( 
  Word16 exc[],         /* (i/o)  : excitation to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word32 L_gain_code,   /* (i)    : decoded codebook gain       Q16   */      
  Word16 *Q_exc,        /* (i/o)  : State structure                   */
  Word16 *Q_exsub,      /*                                            */
  Word16 tcx
);
void Scale_mem_tcx(
  Word16 xnq[],         /* (i/o)  : xnq to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word32 Lgain,         /* (i)    : Q16 */ 
  Word16 mem_syn[], 
  Decoder_State_Plus_fx *st
);
void Updt_mem_q(Word16* old_sub_q, Word16 n, Word16 new_Q);


Word16 Scale_speech( 
  Word16 speech[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_speech,     /* (i/o)  : State structure                   */
  Word16 *Old_Q,        /* (i/o)  : Last frame Scaling                */
  Word16 BitToRemove
); 
Word16 Scale_speech_st( 
  Word16 sp_right[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 sp_left[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_speech,     /* (i/o)  : State structure                   */
  Word16 *Old_Q,        /* (i/o)  : Last frame Scaling                */
  Word16 BitToRemove
);
void Rescale_enc_mem(Coder_State_Plus_fx *st, Word16 scale_fac, Word16 deb);
void Rescale_enc_mem_st(Coder_State_Plus_fx *st, Word16 scale_fac, Word16 deb);

void Rescale_enc_mem_hf(Coder_StState_fx *st, Word16 scale_fac);

void Preemph_scaled( 
    Word16 new_speech[],     /* i:   Speech to scale                 */
    Word16 *Q_new,           /* o:   Scaling factor                  */
    Word16 *exp,             /* o:   Diff scaling factor             */ 
    Word16 *mem_preemph,     /* i/o: Preemph memory                  */
    Word16 *Q_max,           /* i/o: Q_new limitation                */  
    Word16 *Q_old,           /* i/o: Q_new memory                    */  
    Word16 Preemph_factor,   /* i:   Preemphasis factor         Q15  */
    Word16 bits,             /* i:   Limit output to (15-bits) bits  */  
    Word16 Lframe            /* i:   Frame length                    */
);
void Scale_mem_enc_tcx(
  Word16 xnq[],         /* (i/o)  : xnq to rescale       Q_exc */
  Word16 *wmem_wsyn,
  Word16 *wwovlp,
  Word16 lg,            /* (i)    : frame size                        */
  Word32 Lgain,         /* (i)    : Q16 */ 
  Word16 mem_syn[], 
  Coder_State_Plus_fx *st
);
void Coder_acelp(
  Word16 Az[],          /* i  : coefficients 4xAz[M+1]    */
  Word16 Azq[],         /* i  : coefficients 4xAz_q[M+1]  */
  Word16 speech[],      /* i  : speech[-M..lg]            */
  /*Word16 *mem_wsp,    i/o: wsp memory , computed in Find_wsp()   */
  Word16 *mem_wsyn_,    /* i/o: wsyn memory               */
  Word16 synth_[],      /* i/o: synth[-M..lg]             */
  Word16 exc[],         /* i/o: exc[-(PIT_max+L_INTERPOL)..lg+1] */
  Word16 wovlp_[],      /* o  : wovlp[0..128]             */
  Word16 lg,            /* i  : frame length              */
  Word16 codec_mode,    /* i  : AMR_WB+ mode (see cnst.h) */
  Word16 norm_corr,     /* i  : Normalized correlation    */
  Word16 norm_corr2,    /* i  : Normalized correlation    */
  Word16 T_op,          /* i  : open-loop LTP             */  
  Word16 T_op2,         /* i  : open-loop LTP             */  
  Word16 T_out[],       /* o  : integer pitch-lag         */  
  Word16 p_out[],       /* o  : pitch gain                */ 
  Word16 c_out_[],	    /* o  : fixed codebook gain       */
  Word16 pit_adj,       /* i  : indicate pitch adjustment */ 
  Word16 *sprm,         /* o  : acelp parameters          */
  Word16 xn_in[],       /* i  : Target                    */
  Coder_State_Plus_fx* st /* i/o : encoder static memory    */
  );

/* utils*/

void Init_bass_postfilter(Decoder_State_Plus_fx *st);
void Bass_postfilter(
  Word16 *synth_in,     /* (i) : 12.8kHz synthesis to postfilter             */
  Word16 *T_sf,           /* (i) : Pitch period for all subframe (T_sf[16])    */
  Word16 *gainT_sf,     /* (i) : Pitch gain for all subframe (gainT_sf[16])  */
  Word16 *synth_out,    /* (o) : filtered synthesis (with delay=L_SUBFR+L_FILT) */
                       /* delay = (2*L_SUBFR)+L_FILT   using EXTENSION_VA   */
  Word16 pit_adj,
  Decoder_State_Plus_fx *st   /* i/o : decoder memory state                        */
);

Word16 D_gain2_plus(
  Word16 index,         /* (i)  : index of quantizer                      */   //A verifier....
  Word16  code[],       /* (i)  : Innovative code vector    Q9            */
  Word16  lcode,        /* (i)  : Subframe size                           */
  Word16  *gain_pit,    /* (o)  : Quantized pitch gain      Q14           */
  Word32  *gain_code,   /* (o)  : Quantized codebook gain   W32Q16        */
  Word16 bfi,          /* (i)  : Bad frame indicato                      */
  Word16  mean_ener,    /* (i)  : mean_ener defined in open-loop (2 bits) */
  Word16  *past_gpit,   /* (i)  : past gain of pitch         Q14          */
  Word32  *past_gcode);  /* (i/o): past gain of code          Q16          */

#endif
