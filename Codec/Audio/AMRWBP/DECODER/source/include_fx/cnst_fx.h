/*--------------------------------------------------------------------------*
 *                         CNST_FX.H                                           *
 *--------------------------------------------------------------------------*
 *       Codec constant parameters (coder and decoder)                      *
 *--------------------------------------------------------------------------*/

#ifndef cnst_fx_h
#define cnst_fx_h

#include "voAMRWPDecID.h"
#include "options_fx.h" // do all setting in there

//#define CODEC_VERSION "AMR.WB+ ver 3.00 stereo (fixed point)"




#define FSCALE_DENOM      96     /* filter into decim_split.h */
#define FSCALE_DENOM_NORMS 8     /* filter into decim_split.h */
#define FAC_FSCALE_MAX    (FSCALE_DENOM*41/24)
#define FAC_FSCALE_MIN	 (FSCALE_DENOM/2)




#define USE_CASE_A       0
#define USE_CASE_B       1


#define L_FRAME_FSMAX 2*L_FRAME48k


#define L_FRAME48k   ((L_FRAME_PLUS/4)*15)
#define L_FRAME44k   ((L_FRAME_PLUS/128)*441)
#define L_FRAME32k   ((L_FRAME_PLUS/2)*5)
#define L_FRAME24k   ((L_FRAME_PLUS/8)*15)
#define L_FRAME22k   ((L_FRAME_PLUS/256)*441)
#define L_FRAME16kPLUS   ((L_FRAME_PLUS/4)*5)
#define L_FRAME8k    ((L_FRAME_PLUS/8)*5)

#define L_NEXT24k   ((L_NEXT/8)*15)
#define L_NEXT16k   ((L_NEXT/4)*5)
#define L_NEXT8k    ((L_NEXT/8)*5)

#define L_FRAME11k   ((L_FRAME_PLUS/512)*441)
#define L_FILT_DECIM_FS   (L_FILT_OVER_FS*6)
#define L_FILT_OVER_FS    12

#define L_FILT_DECIM   ((L_FILT_OVER+1)*60)
#define L_FILT_OVER    12
#define L_FILT_SPLIT   24
#define L_FILT_JOIN    12
#define L_MEM_DECIM_SPLIT (2*(L_FILT_SPLIT+L_FILT_DECIM))
#define L_MEM_JOIN_OVER   (2*(L_FILT_OVER+L_FILT_JOIN+L_FILT_JOIN))

#define L_FILT24k    23


/* codec constant assuming at fs=12.8kHz */

#define L_OVLP       128     /* 80ms TCX overlap size (10 ms)              */
#define L_TCX        (1024+128)  /* 80ms TCX Frame size (80ms + 10 ms overlap) */

#define L_FRAME_PLUS 1024    /* 80ms frame size (long TCX frame)           */
#define L_DIV        256     /* 20ms frame size (ACELP or short TCX frame) */
#define NB_DIV       4       /* number of division (20ms) per 80ms frame   */
#define NB_SUBFR     16      /* number of 5ms subframe per 80ms frame      */
#define L_SUBFR      64      /* subframe size (5ms)                        */


#define L_NEXT       256     /* overhead in LP analysis (20ms)             */
#define L_WINDOW     448     /* 35ms window size in LP analysis            */

#define L_NEXT_HIGH_RATE       288     /* overhead in LP analysis                    */
#define L_WINDOW_HIGH_RATE     512     /* window size in LP analysis (50% overlap)   */

#define L_TOTAL_PLUS			(M+L_FRAME_PLUS+L_NEXT)   /* total size of speech buffer.  */
#define L_TOTAL_HIGH_RATE	(M+L_FRAME_PLUS+L_NEXT_HIGH_RATE) 

#define M            16      /* order of LP filter                         */
#define MHF          8       /* order of LP filter for HF band             */

#define L_FILT       12      /* Delay of up-sampling filter                */
#define DELAY_SPLIT  54      /* Additional delay of mid band signal due to split filter */


#define DELAY_PF     (L_FILT+L_SUBFR)     /* decoder delay from post-filter e.tc*/

#define DELAY_PF_HIGH_RATE     (L_FILT+(2*L_SUBFR)) /* decoder delay from post-filter e.tc*/

#define BWE          60.0f   /* Bandwidth expansion in lag windowing       */
#define WNC          1.0001f /* White noise correction factor in LPC       */


//delay of STEREO need to be ajusted to new bass_postfilter delay (+64 samples)
//before adding these new pitch limit for higher rate...

#define PIT_MIN_12k8	34      /* Minimum pitch lag with resolution 1/4      */
#define PIT_FR2_12k8	128     /* Minimum pitch lag with resolution 1/2      */
#define PIT_FR1_12k8	160     /* Minimum pitch lag with resolution 1        */
#define PIT_MAX_12k8	231     /* Maximum pitch lag                          */

/* Maximum pitch lag for highest freq. scaling factor  */
#define PIT_MAX_MAX     (PIT_MAX_12k8 + (6*((((FAC_FSCALE_MAX*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8)))


//#define L_INTERPOL (16+1)  /* Length of filter for interpolation         */

#define OPL_DECIM    2       /* Decimation in open-loop pitch analysis     */

#define RANDOM_INITSEED  21845      /* own random init value */

#define L_MEANBUF    3        /* for isf recovery */


/* AMR_WB+ mode relative to AMR-WB core */
#define MODE_9k6     0
#define MODE_11k2    1
#define MODE_12k8    2
#define MODE_14k4    3
#define MODE_16k     4
#define MODE_18k4    5
#define MODE_20k     6
#define MODE_23k2    7

/* number of bits for parametric bandwidth extension (BWE) */
#define  NBITS_BWE    (4*16)     /* 4 packets x 16 bits = 0.8 kbps */
#define  NPRM_BWE_DIV  6         /* 12 on 40ms frame, 24 on 80ms frame */

/* maximum number of bits (to set buffer size of bitstream vector) */
#define  NBITS_MAX    (48*80)    /* define the buffer size at 32kbps */

/* number of packets per frame (4 packets of 20ms) */
#define  N_PACK_MAX   4

/* codec mode: 0=ACELP, 1=TCX20, 2=TCX40, 3=TCX80 */
#define  NBITS_MODE   (4*2)      /* 4 packets x 2 bits */
#define  NBITS_LPC    (46)       /* AMR-WB LPC quantizer */

#define  SYNC_WORD    (short)0x6b21    /* packet sync transmitted every 20ms */
#define  BIT_0        (short)0x007F
#define  BIT_1        (short)0x0081

/* AMRWB+ core parameters constants */
#define  NPRM_LPC     7                /* number of prm for LPC */
#define  NPRM_RE8     (L_TCX+(L_TCX/8))
#define  NPRM_TCX80   (2+NPRM_RE8)           /* TCX 80ms */
#define  NPRM_TCX40   (2+(NPRM_RE8/2))       /* TCX 40ms */
#define  NPRM_TCX20   (2+(NPRM_RE8/4))       /* TCX 20ms */
#define  NPRM_DIV     (NPRM_LPC+NPRM_TCX20)  /* buffer size = NB_DIV*NPRM_DIV */
/* number of parameters on the decoder side (AVQ use 4 bits per parameter) */
#define  DEC_NPRM_DIV (((24*80)/4)/NB_DIV)   /* set for max of 24kbps (TCX) */

/* Stereo Constants */
/* Stereo Constants */

#define L_NEXT_ST24k   ((L_NEXT_ST/8)*15)
#define L_NEXT_ST16k   ((L_NEXT_ST/4)*5)
#define L_NEXT_ST8k    ((L_NEXT_ST/8)*5)
#define L_NEXT_ST_2k   ((L_NEXT_ST * 5 / 32))


#define L_ANA		(12800 * 30 / 1000)
#define L_ANA_2k	      (2000 * 80/1000)

#define L_B			((L_ANA - L_DIV)/2)
#define L_B_2k		((L_ANA_2k - L_DIV_2k)/2)

#define L_A			L_B
#define L_A_2k		L_B_2k
#define L_A_MAX		(L_A > ((L_A_2k * 32)/5) ? L_A : ((L_A_2k * 32)/5))

#define N_COEFF_F2K 321
#define L_FDEL_64k	((N_COEFF_F2K * 2 - 1) - 1)/2
#define L_FDEL		(L_FDEL_64k / 5)
#define L_FDEL_2k	(L_FDEL_64k / 32)

#define L_BSP		(2 * L_FDEL)
#define L_BSP_2k	((L_BSP*5)/32)
#define L_NEXT_ST	(L_A_MAX + L_BSP)

#define L_TOTAL_ST    (M+L_FRAME_PLUS+L_A_MAX+L_BSP)       /* total size of speech buffer in stereo mode  */
#define L_TOTAL_ST_2k (L_B_2k + L_FRAME_2k + L_A_2k + L_BSP_2k)
#define L_TOTAL_ST_hi (L_B + L_FRAME_PLUS + L_A_MAX)

#define L_FILT_2k	128
#define L_DIV_2k	((L_DIV * 5)/32)
#define L_FRAME_2k	160
#define L_NEXT_2k	80

#define L_OLD_SPEECH			(L_TOTAL_PLUS-L_FRAME_PLUS)
#define L_OLD_SPEECH_HIGH_RATE	(L_TOTAL_HIGH_RATE-L_FRAME_PLUS)

#define L_OLD_SPEECH_ST (L_TOTAL_ST-L_FRAME_PLUS)
#define L_OLD_SPEECH_2k (L_TOTAL_ST_2k-L_FRAME_2k)

#define L_OLD_SPEECH_hi (L_TOTAL_ST_hi-L_FRAME_PLUS)



#define WIENER_ORDER 20		/* Filter order for wiener filters used in parametric stereo	*/
					/* encoding. (Default value)					*/

#define WIENER_HI_ORDER WIENER_ORDER	/* Filter order for wiener filters in high band	(must be an even number) */

#define WIENER_LO_ORDER WIENER_ORDER	/* Filter order for wiener filters in low band (must be an even number)	 */


#define MAXNCHOLES 21		/* Buffer sizes in cholesky decomposition				*/
					/* Below, quantizer constants. Comments to be added.			*/

#define D_PAN_Q 0.5

#define MAX_NUMSTAGES 6 		/* Maximum number of stages in MSVQ. */

#define INTENS_LO	8
#define INTENS_HI	8
#define INTENS_MAX (INTENS_LO > INTENS_HI ? INTENS_LO : INTENS_HI)

					/* TBD */

#define MA_COEFF 0.5
#define LO_SMSVQ_CBSIZE 32	/* Codebook size  in MSVQ, low-band				*/
#define HI_SMSVQ_CBSIZE 32	/* Codebook size  in MSVQ, hi-band				*/

#define LO_SMSVQ_CBBITS 5	/* Number of bits representing index of codebook, low-band	*/
#define HI_SMSVQ_CBBITS 5	/* Number of bits representing index of codebook, high-band	*/
#define CBBITS_MAX (LO_SMSVQ_CBSIZE > HI_SMSVQ_CBSIZE ? LO_SMSVQ_CBSIZE : HI_SMSVQ_CBSIZE)

#define S_NSFR 8			/* Number of sub-frames in one 20 ms frame			*/
#define S_1_NSFR 0.125f		/* Inverse of S_NSFR						*/

#define D_BPF (DELAY_PF+20)	/* Bass postfilter delay used for stereo */
#define D_NC ((WIENER_LO_ORDER/2 * 32) / 5)		/* Non causality delay */


#define NPRM_STEREO_DIV_X		 (NPRM_DIV_TCX_STEREO + NPRM_STEREO_HI_X + 1)
#define MAX_NPRM_STEREO_DIV		(NPRM_STEREO_DIV_X)

extern const int nprm_stereo_hi_x[4];         // 2 msvq stages (filt) + 1 vq (gain)
#define NPRM_STEREO_HI_X		(2+1) // maximum: 2 msvq stages (filt) + 1 vq (gain)
//#include "cnst_tcx_stereo.h"

#define HI_FILT_ORDER	9

/* High stereo codebook */
// filter quantizers
#define NSTAGES_FILT_HI_MSVQ4 1
#define SIZE_FILT_HI_MSVQ_4A 16
#define INTENS_FILT_HI_MSVQ4  1

#define NSTAGES_FILT_HI_MSVQ7 2
#define SIZE_FILT_HI_MSVQ_7A 16
#define SIZE_FILT_HI_MSVQ_7B  8
#define INTENS_FILT_HI_MSVQ7  8

// gain quantizers
#define HI_GAIN_ORDER 2

#define NSTAGES_GAIN_HI_MSVQ2 1
#define SIZE_GAIN_HI_MSVQ_2A  4
#define INTENS_GAIN_HI_MSVQ2  1

#define NSTAGES_GAIN_HI_MSVQ5 1
#define SIZE_GAIN_HI_MSVQ_5A 32
#define INTENS_GAIN_HI_MSVQ5  1

/*FIP*/
#define LG10                24660       /*  10*log10(2)  in Q13                 */
#define LG10_28             17262       /*  28*log10(2)  in Q11                 */
#define Q_MAX               8
#define Q_MAX2              6     // 4 

#define Q_MAX3              2     // 4
#define PREEMPH_FAC_FX  22282   /* preemphasis factor                         */
#define GAMMA1_FX    30147      /* weighting factor (numerator)               */
#define TILT_FAC_FX  22282   
#define PIT_SHARP_FX 27853      /* pitch sharpening factor                    */
#define TILT_CODE_FX 9830       /* ACELP code preemphasis factor              */
#define GAMMA_HF_FX  29491      /* weighting factor for HF weghted speech     */

#define PIT_MAX      231
#define L_FRAME16k   320


#define N_MAX       1152          /* maximum fft coefficients */
#define COSOFFSET   288           /* Offset to find cosine in table */

#define QR        32768

#define QS        32         /* Initial value of averageHistTime */
#define QAVE      2048        /* Initial value of stdDevHistTime */
#define QLPH      64          /* Initial value of lphbuf */

#define FRAW  0  /* Raw File format used */
#define F3GP  1  /* 3GP File format used */


#define voSIZE         128
#define NUM_STAGE    6

#endif /* cnst_h */
