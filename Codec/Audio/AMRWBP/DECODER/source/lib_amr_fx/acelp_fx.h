/*--------------------------------------------------------------------------*
 *                         ACELP.H                                          *
 *--------------------------------------------------------------------------*
 *       Function			 												*
 *--------------------------------------------------------------------------*/
#include "typedef.h"

#define NB_QUA_GAIN7B 128
/*-----------------------------------------------------------------*
 *                        LPC prototypes                           *
 *-----------------------------------------------------------------*/
void voAMRWBPDecIsf_Extrapolation(Word16 HfIsf[]);
void voAMRWBPDecInit_Lagconc(Word16 lag_hist[]);
void voAMRWBPDeclagconc(
     Word16 gain_hist[],                   /* (i) : Gain history     */
     Word16 lag_hist[],                    /* (i) : Subframe size         */
     Word16 * T0,
     Word16 * old_T0,
     Word16 * seed,
     Word16 unusable_frame
);

void voAMRWBPDecagc2(
     Word16 * sig_in,                      /* input : postfilter input signal  */
     Word16 * sig_out,                     /* in/out: postfilter output signal */
     Word16 l_trm                          /* input : subframe size            */
);

void voAMRWBPDecInit_Filt_7k(Word16 mem[]);
void voAMRWBPDecFilt_7k(
     Word16 signal[],                      /* input:  signal                  */
     Word16 lg,                            /* input:  length of input         */
     Word16 mem[]                          /* in/out: memory (size=30)        */
);

Word16 voAMRWBPDecmedian5(Word16 x[]);


void voAMRWBPDecAutocorr(
     Word16 x[],              /* (i)    : Input signal                      */
     Word16 m,                /* (i)    : LPC order                         */
     Word16 r_h[],            /* (o) Q15: Autocorrelations  (msb)           */
     Word16 r_l[],            /* (o)    : Autocorrelations  (lsb)           */
     Word16 l_window,         /* (i)    : window size             */
     Word16 window[]          /* (i)    : analysis window         */
);

void voAMRWBPDecIsp_Az(
     Word16 isp[],                         /* (i) Q15 : Immittance spectral pairs            */
     Word16 a[],                           /* (o) Q12 : predictor coefficients (order = M)   */
     Word16 m,
     Word16 adaptive_scaling               /* (i) 0   : adaptive scaling disabled */
                                           /*     1   : adaptive scaling enabled  */
);

void voAMRWBPDecIsf_isp(
     Word16 isf[],                         /* (i) Q15 : isf[m] normalized (range: 0.0<=val<=0.5) */
     Word16 isp[],                         /* (o) Q15 : isp[m] (range: -1<=val<1)                */
     Word16 m                              /* (i)     : LPC order                                */
);
void voAMRWBPDecInt_isp(
     Word16 isp_old[],                     /* input : isps from past frame              */
     Word16 isp_new[],                     /* input : isps from present frame           */
     Word16 frac[],                        /* input : fraction for 3 first subfr (Q15)  */
     Word16 Az[]                           /* output: LP coefficients in 4 subframes    */
);
void voAMRWBPDecWeight_a(
     const Word16 a[],                           /* (i) Q12 : a[m+1]  LPC coefficients             */
     Word16 ap[],                          /* (o) Q12 : Spectral expanded LPC coefficients   */
     Word16 gamma,                         /* (i) Q15 : Spectral expansion factor.           */
     Word16 m                              /* (i)     : LPC order.                           */
);


/*-----------------------------------------------------------------*
 *                        isf quantizers                           *
 *-----------------------------------------------------------------*/
void voAMRWBPDecDpisf_2s_46b(
     Word16 * indice,                      /* input:  quantization indices                       */
     Word16 * isf_q,                       /* output: quantized ISF in frequency domain (0..0.5) */
     Word16 * past_isfq,                   /* i/0   : past ISF quantizer                    */
     Word16 * isfold,                      /* input : past quantized ISF                    */
     Word16 * isf_buf,                     /* input : isf buffer                                                        */
     Word16 bfi,                           /* input : Bad frame indicator                   */
     Word16 enc_dec
);
void voAMRWBPDecDpisf_2s_36b(
     Word16 * indice,                      /* input:  quantization indices                       */
     Word16 * isf_q,                       /* output: quantized ISF in frequency domain (0..0.5) */
     Word16 * past_isfq,                   /* i/0   : past ISF quantizer                    */
     Word16 * isfold,                      /* input : past quantized ISF                    */
     Word16 * isf_buf,                     /* input : isf buffer                                                        */
     Word16 bfi,                           /* input : Bad frame indicator                   */
     Word16 enc_dec
);

void voAMRWBPDecDisf_ns(
     Word16 * indice,                      /* input:  quantization indices                  */
     Word16 * isf_q                        /* input : ISF in the frequency domain (0..0.5)  */
);
Word16 voAMRWBPDecSubVQ(                             /* output: return quantization index     */
     Word16 * x,                           /* input : ISF residual vector           */
     const Word16 * dico,                        /* input : quantization codebook         */
     Word16 dim,                           /* input : dimention of vector           */
     Word16 dico_size,                     /* input : size of quantization codebook */
     Word32 * distance                     /* output: error of quantization         */
);
void voAMRWBPDecReorder_isf(
     Word16 * isf,                         /* (i/o) Q15: ISF in the frequency domain (0..0.5) */
     Word16 min_dist,                      /* (i) Q15  : minimum distance to keep             */
     Word16 n                              /* (i)      : number of ISF                        */
);

/*-----------------------------------------------------------------*
 *                       filter prototypes                         *
 *-----------------------------------------------------------------*/
void voAMRWBPDecInit_Oversamp_16k(
     Word16 mem[]                          /* output: memory (2*NB_COEF_UP) set to zeros  */
);
void voAMRWBPDecOversamp_16k(
     Word16 sig12k8[],                     /* input:  signal to oversampling  */
     Word16 lg,                            /* input:  length of input         */
     Word16 sig16k[],                      /* output: oversampled signal      */
     Word16 mem[]                          /* in/out: memory (2*NB_COEF_UP)   */
);

void voAMRWBPDecInit_HP50_12k8(Word16 mem[]);
void voAMRWBPDecHP50_12k8(
     Word16 signal[],                      /* input/output signal */
     Word16 lg,                            /* lenght of signal    */
     Word16 mem[]                          /* filter memory [6]   */
);
void voAMRWBPDecInit_HP400_12k8(Word16 mem[]);
void voAMRWBPDecHP400_12k8(
     Word16 signal[],                      /* input/output signal */
     Word16 lg,                            /* lenght of signal    */
     Word16 mem[]                          /* filter memory [6]   */
);

void voAMRWBPDecInit_Filt_6k_7k(Word16 mem[]);
void voAMRWBPDecFilt_6k_7k(
     Word16 signal[],                      /* input:  signal                  */
     Word16 lg,                            /* input:  length of input         */
     Word16 mem[]                          /* in/out: memory (size=30)        */
);

void voAMRWBPDecPreemph(
     Word16 x[],                           /* (i/o)   : input signal overwritten by the output */
     Word16 mu,                            /* (i) Q15 : preemphasis coefficient                */
     Word16 lg,                            /* (i)     : lenght of filtering                    */
     Word16 * mem                          /* (i/o)   : memory (x[-1])                         */
);

void voAMRWBPDecDeemph(
     Word16 x[],                           /* (i/o)   : input signal overwritten by the output */
     Word16 mu,                            /* (i) Q15 : deemphasis factor                      */
     Word16 L,                             /* (i)     : vector size                            */
     Word16 * mem                          /* (i/o)   : memory (y[-1])                         */
);
void voAMRWBPDecDeemph2(
     Word16 x[],                           /* (i/o)   : input signal overwritten by the output */
     Word16 mu,                            /* (i) Q15 : deemphasis factor                      */
     Word16 L,                             /* (i)     : vector size                            */
     Word16 * mem                          /* (i/o)   : memory (y[-1])                         */
);
void voAMRWBPDecDeemph_32(
     Word16 x_hi[],                        /* (i)     : input signal (bit31..16) */
     Word16 x_lo[],                        /* (i)     : input signal (bit15..4)  */
     Word16 y[],                           /* (o)     : output signal (x16)      */
     Word16 mu,                            /* (i) Q15 : deemphasis factor        */
     Word16 L,                             /* (i)     : vector size              */
     Word16 * mem                          /* (i/o)   : memory (y[-1])           */
);

// @shanrong modified
#if (!FUNC_RESIDU_ASM)
void voAMRWBPDecResidu(
     const Word16 a[],                           /* (i) Q12 : prediction coefficients                     */
     Word16 m,                             /* (i)     : order of LP filter                          */
     Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed         */
     Word16 y[],                           /* (o)     : residual signal                             */
     Word16 lg                             /* (i)     : size of filtering                           */
);

// VAILLANCOURT
void voAMRWBPDecResidu2(
     const Word16 a[],                           /* (i) Q12 : prediction coefficients                     */
     const Word16 m,                             /* (i)     : order of LP filter                          */
     const Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed         */
     Word16 y[],                           /* (o)     : residual signal                             */
     const Word16 lg                             /* (i)     : size of filtering                           */
);
#endif
// end

void voAMRWBPDecSynFilt(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients           */
     Word16 m,                             /* (i)     : order of LP filter                       */
     Word16 x[],                           /* (i)     : input signal                             */
     Word16 y[],                           /* (o)     : output signal                            */
     Word16 lg,                            /* (i)     : size of filtering                        */
     Word16 mem[],                         /* (i/o)   : memory associated with this filtering.   */
     Word16 update                         /* (i)     : 0=no update, 1=update of memory.         */
);
void voAMRWBPDecSyn_filt_32(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients */
     Word16 m,                             /* (i)     : order of LP filter             */
     Word16 exc[],                         /* (i) Qnew: excitation (exc[i] >> Qnew)    */
     Word16 Qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
     Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
     Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
     Word16 lg                             /* (i)     : size of filtering              */
);

void voAMRWBPDecSyn_filt_s(
  Word16 shift,           /* (i) : scaling to apply                           Q0  */
  Word16 a[],             /* (i) : a[m+1] prediction coefficients             Q12 */
  Word16 m,               /* (i) : order of LP filter                         Q0  */
  Word16 x[],             /* (i) : input signal                               Qx  */
  Word16 y[],             /* (o) : output signal                              Qx-s*/  
  Word16 lg,              /* (i) : size of filtering                          Q0  */
  Word16 mem[],           /* (i/o) :memory associated with this filtering.    Qx-s*/
  Word16 update           /* (i) : 0=no update, 1=update of memory.           Q0  */
);
/*-----------------------------------------------------------------*
 *                       pitch prototypes                          *
 *-----------------------------------------------------------------*/\
void voAMRWBPDecPredlt4(
     Word16 exc[],                         /* in/out: excitation buffer */
     Word16 T0,                            /* input : integer pitch lag */
     Word16 frac,                          /* input : fraction of lag   */
     Word16 L_subfr                        /* input : subframe size     */
);


/*-----------------------------------------------------------------*
 *                       gain prototypes                           *
 *-----------------------------------------------------------------*/
void voAMRWBPDecInit_D_gain2(
     Word16 * mem                          /* output  :static memory (4 words)      */
);

void voAMRWBPDecD_gain2(
     Word16 index,                         /* (i)     :index of quantization.       */
     Word16 nbits,                         /* (i)     : number of bits (6 or 7)     */
     Word16 code[],                        /* (i) Q9  :Innovative vector.           */
     Word16 L_subfr,                       /* (i)     :Subframe lenght.             */
     Word16 * gain_pit,                    /* (o) Q14 :Pitch gain.                  */
     Word32 * gain_cod,                    /* (o) Q16 :Code gain.                   */
     Word16 bfi,                           /* (i)     :bad frame indicator          */
     Word16 prev_bfi,                      /* (i) : Previous BF indicator      */
     Word16 state,                         /* (i) : State of BFH               */
     Word16 unusable_frame,                /* (i) : UF indicator            */
     Word16 vad_hist,                      /* (i)         :number of non-speech frames  */
     Word16 * mem                          /* (i/o)   :static memory (4 words)      */
);

/*-----------------------------------------------------------------*
 *                       acelp prototypes                          *
 *-----------------------------------------------------------------*/
void voAMRWBPDecACELP_2t64_fx(
     Word16 index,                         /* (i) :    12 bits index                                  */
     Word16 code[]                         /* (o) :Q9  algebraic (fixed) codebook excitation          */
);

void voAMRWBPDecACELP_4t64_fx(
     Word16 index[],                       /* (i) : index (20): 5+5+5+5 = 20 bits.                 */
										   /* (i) : index (36): 9+9+9+9 = 36 bits.                 */
										   /* (i) : index (44): 13+9+13+9 = 44 bits.               */
										   /* (i) : index (52): 13+13+13+13 = 52 bits.             */
										   /* (i) : index (64): 2+2+2+2+14+14+14+14 = 64 bits.     */
										   /* (i) : index (72): 10+2+10+2+10+14+10+14 = 72 bits.   */
										   /* (i) : index (88): 11+11+11+11+11+11+11+11 = 88 bits. */
     Word16 nbbits,                        /* (i) : 20, 36, 44, 52, 64, 72 or 88 bits              */
     Word16 code[]                         /* (o) Q9: algebraic (fixed) codebook excitation        */
);

void voAMRWBPDecPit_shrp(
     Word16 * x,                           /* in/out: impulse response (or algebraic code) */
     Word16 pit_lag,                       /* input : pitch lag                            */
     Word16 sharp,                         /* input : pitch sharpening factor (Q15)        */
     Word16 L_subfr                        /* input : subframe size                        */
);


/*-----------------------------------------------------------------*
 *                        others prototypes                        *
 *-----------------------------------------------------------------*/
void Set_zero(
     Word16 x[],                           /* (o)    : vector to clear     */
     Word16 L                              /* (i)    : length of vector    */
);

/*-------------------------------------------------------------------*
 * Function  Copy:                                                   *
 *           ~~~~~                                                   *
 * Copy vector x[] to y[]                                            *
 *-------------------------------------------------------------------*/
void voAMRWBPDecCopy(
     const Word16 x[],                           /* (i)   : input vector   */
     Word16 y[],                           /* (o)   : output vector  */
     Word16 L                              /* (i)   : vector length  */
);

Word16 voAMRWBPDecVfactor(                       /* (o) Q15 : factor (-1=unvoiced to 1=voiced) */
     Word16 exc[],                         /* (i) Q_exc: pitch excitation                */
     Word16 Q_exc,                         /* (i)     : exc format                       */
     Word16 gain_pit,                      /* (i) Q14 : gain of pitch                    */
     Word16 code[],                        /* (i) Q9  : Fixed codebook excitation        */
     Word16 gain_code,                     /* (i) Q0  : gain of code                     */
     Word16 L_subfr                        /* (i)     : subframe length                  */
);

#if (!FUNC_SCALE_SIG_ASM)
void voAMRWBPDecScale_sig(
     Word16 x[],                           /* (i/o) : signal to scale               */
     Word16 lg,                            /* (i)   : size of x[]                   */
     Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
);
#endif


#if (!FUNC_RANDOM_MACRO)
#ifdef MSVC_AMRWBPLUS
Word16 Random(Word16 * seed);
#elif EVC_AMRWBPLUS
Word16 Random_evc(Word16 * seed);
#endif
#else
#ifdef MSVC_AMRWBPLUS
#define Random(seed) (*((Word16 *)(seed)) = (Word16)((*(seed)) * 31821 + 13849L))
#elif EVC_AMRWBPLUS
#define Random_evc(seed) (*((Word16 *)(seed)) = (Word16)((*(seed)) * 31821 + 13849L))
#endif
#endif

void voAMRWBPDecInit_Phase_dispersion(
     Word16 disp_mem[]                     /* (i/o): static memory (size = 8) */
);
void voAMRWBPDecPhase_dispersion(
     Word16 gain_code,                     /* (i) Q0  : gain of code             */
     Word16 gain_pit,                      /* (i) Q14 : gain of pitch            */
     Word16 code[],                        /* (i/o)   : code vector              */
     Word16 mode,                          /* (i)     : level, 0=hi, 1=lo, 2=off */
     Word16 disp_mem[]                     /* (i/o)   : static memory (size = 8) */
);
