	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#ifndef _SBR_H
#define _SBR_H

#include "global.h"
#include "bitstream.h"
#ifdef PS_DEC
#include "ps_dec.h"
#endif
#include "struct.h"

#define NUM_TIME_SLOTS	    16
#define voSAMP_PER_SLOT		2	/* RATE in spec */
#define VOSAMP_RATES_NUM 	9	/* downsampled (single-rate) mode unsupported, so only use Fs_sbr >= 16 kHz */

#define voMAX_NUM_ENV					5
#define MAX_NUM_NOISE_FLOORS		2
#define MAX_NUM_NOISE_FLOOR_BANDS	5	/* max Nq, see 4.6.18.3.6 */
#define MAX_NUM_PATCHES				5
#define MAX_NUM_SMOOTH_COEFS		5

#define SBR_HF_GEN			8
#define SBR_HF_ADJ			2

/* max QMF subbands covered by SBR (4.6.18.3.6) */
#define VOMQ_BANDS	48		

#define SBR_IN_QMFA	14
#define SBR_LOST_QMFA	(1 + 2 + 3 + 2 + 1)	/* 1 from cTab, 2 in premul, 3 in FFT, 2 in postmul, 1 for implicit scaling by 2.0 */
#define SBR_OUT_QMFA	(SBR_IN_QMFA - SBR_LOST_QMFA)//5

#define SBR_GBITS_IN_QMFS		2
#define SBR_IN_QMFS			    SBR_OUT_QMFA    //5
#define SBR_LOST_DCT4_64		(2 + 3 + 2)		//7 /* 2 in premul, 3 in FFT, 2 in postmul */

#define SBR_OUT_DQ_ENV	            29	     /* dequantized env scalefactors are Q(29 - envDataDequantScale) */
#define SBR_OUT_DQ_NOISE	        24	     /* range of Q_orig = [2^-24, 2^6] */
#define SBR_NOISE_FLOOR_OFFSET	    6

/* see comments in ApplyBoost() */
#define SBR_GLIM_BOOST	    24
#define SBR_QLIM_BOOST	    14

#define MAX_HUFF_BITS		20
#define NUM_QMF_DELAY_BUFS	10
#define DELAY_SAMPS_QMFA	(NUM_QMF_DELAY_BUFS * 32)
#define DELAY_SAMPS_QMFS	(NUM_QMF_DELAY_BUFS * 128)


/* Huffman Table Index */
#define voHuffTab_t_Env15      0
#define voHuffTab_f_Env15      1
#define voHuffTab_t_Env15b     2
#define voHuffTab_f_Env15b     3
#define voHuffTab_t_Env30      4
#define voHuffTab_f_Env30      5
#define voHuffTab_t_Env30b     6
#define voHuffTab_f_Env30b     7
#define voHuffTab_t_Noise30    8
#define voHuffTab_f_Noise30    5
#define voHuffTab_t_Noise30b   9
#define voHuffTab_f_Noise30b   7

/* Grid control */
#define voSBR_CLA_BITS  2
#define voSBR_ABS_BITS  2
#define voSBR_RES_BITS  1
#define voSBR_REL_BITS  2
#define voSBR_ENV_BITS  2
#define voSBR_NUM_BITS  2

/* four different SBR frame classes */
#define voFIXFIX  0
#define voFIXVAR  1
#define voVARFIX  2
#define voVARVAR  3

#define SBR_FREQ_SCALE_DEFAULT                  2
#define SBR_ALTER_SCALE_DEFAULT                 1
#define SBR_NOISE_BANDS_DEFAULT                 2
#define SBR_LIMITER_BANDS_DEFAULT               2
#define SBR_LIMITER_GAINS_DEFAULT               2
#define SBR_INTERPOL_FREQ_DEFAULT               1
#define SBR_SMOOTHING_LENGTH_DEFAULT            1  /* 0: on  1: off */

/* header */
#define SI_SBR_AMP_RES_BITS                     1
#define SI_SBR_START_FREQ_BITS                  4
#define SI_SBR_STOP_FREQ_BITS                   4
#define SI_SBR_XOVER_BAND_BITS                  3
#define SI_SBR_DATA_EXTRA_BITS                  1
#define SI_SBR_HEADER_EXTRA_1_BITS              1
#define SI_SBR_HEADER_EXTRA_2_BITS              1
#define SI_SBR_PSEUDO_STEREO_MODE_BITS          2
#define SI_SBR_FREQ_SCALE_BITS                  2
#define SI_SBR_ALTER_SCALE_BITS                 1
#define SI_SBR_NOISE_BANDS_BITS                 2
#define SI_SBR_LIMITER_BANDS_BITS               2
#define SI_SBR_LIMITER_GAINS_BITS               2
#define SI_SBR_INTERPOL_FREQ_BITS               1
#define SI_SBR_SMOOTHING_LENGTH_BITS            1
#define SI_SBR_RESERVED_HE2_BITS                1
#define SI_SBR_RESERVED_BITS_HDR                2
#define SI_SBR_RESERVED_BITS_DATA               4

/* need one SBRHeader per element (SCE/CPE), updated only on new header */
typedef struct _SBRHeader {
	VO_S32		  count;

	VO_U8		  ampRes;
	VO_U8         startFreq;
	VO_U8         stopFreq;
	VO_U8         crossOverBand;
	VO_U8         resBitsHdr;
	VO_U8         hdrExtra1;
	VO_U8         hdrExtra2;

	VO_U8         freqScale;
	VO_U8         alterScale;
	VO_U8         noiseBands;
	
	VO_U8         limiterBands;
	VO_U8         limiterGains;
	VO_U8         interpFreq;
	VO_U8         smoothMode;
} SBRHeader;

/* need one SBRGrid per channel, updated every frame */
typedef struct _SBRGrid {
	VO_U8         FrameType;
	VO_U8         SBR_AmpRes_30;
	VO_U8         ptr;

	VO_U8         L_E;						                 /* L_E */
	VO_U8         t_E[voMAX_NUM_ENV+1];	                     /* t_E */
	VO_U8         freqRes[voMAX_NUM_ENV];			             /* r */

	VO_U8         L_Q;							             /* L_Q */
	VO_U8         t_Q[MAX_NUM_NOISE_FLOORS+1];	             /* t_Q */

	VO_U8         numEnvPrev;
	VO_U8         numNoiseFloorsPrev;
	VO_U8         freqResPrev;
} SBRGrid;

/* need one SBRFreq per element (SCE/CPE/LFE), updated only on header reset */
typedef struct _SBRFreq {
	int                   kStart;				/* k_x */
	int                   nMaster;
	int                   nHigh;
	int                   nLow;
	int                   nLimiter;				/* N_l */
	int                   numQMFBands;			/* M */
	int                   NQ;	/* Nq */

	int                   kStartPrev;
	int                   numQMFBandsPrev;

	VO_U8				  freqBandMaster[VOMQ_BANDS + 1];	/* not necessary to save this  after derived tables are generated */
	VO_U8				  freqBandHigh[VOMQ_BANDS + 1];
	VO_U8				  freqBandLow[VOMQ_BANDS / 2 + 1];	/* nLow = nHigh - (nHigh >> 1) */
	VO_U8				  freqBandNoise[MAX_NUM_NOISE_FLOOR_BANDS+1];
	VO_U8				  freqBandLim[VOMQ_BANDS / 2 + MAX_NUM_PATCHES];		/* max (intermediate) size = nLow + numPatches - 1 */

	VO_U8				  numPatches;
	VO_U8				  patchNumSubbands[MAX_NUM_PATCHES + 1];
	VO_U8				  patchStartSubband[MAX_NUM_PATCHES + 1];
} SBRFreq;

typedef struct _SBRChan {
	int                   reset;
	VO_U8				  deltaFlagEnv[voMAX_NUM_ENV];
	VO_U8				  deltaFlagNoise[MAX_NUM_NOISE_FLOORS];

	signed char           envDataQuant[voMAX_NUM_ENV][VOMQ_BANDS];		/* range = [0, 127] */
	signed char           noiseDataQuant[MAX_NUM_NOISE_FLOORS][MAX_NUM_NOISE_FLOOR_BANDS];

	VO_U8				  invfMode[2][MAX_NUM_NOISE_FLOOR_BANDS];	/* invfMode[0/1][band] = prev/curr */
	int                   chirpFact[MAX_NUM_NOISE_FLOOR_BANDS];		/* bwArray */
	VO_U8				  addHarmonicFlag[2];						/* addHarmonicFlag[0/1] = prev/curr */
	VO_U8                 addHarmonic[2][64];						/* addHarmonic[0/1][band] = prev/curr */
	
	int                   gbMask[2];	/* gbMask[0/1] = XBuf[0-31]/XBuf[32-39] */
	signed char           laPrev;

	int                   fIndexNoise;
	int                   fIndexSine;
	int                   gIndexRing;
	int                   G_Temp[MAX_NUM_SMOOTH_COEFS][VOMQ_BANDS];
	int                   Q_Temp[MAX_NUM_SMOOTH_COEFS][VOMQ_BANDS];

} SBRChan;

#ifdef WMMX
#define ALIGN	__declspec(align(8))
#else//WMMX
#define ALIGN
#endif//WMMX
typedef struct{
	/* save for entire file */
	int                   number;
	int                   sampRateIdx;

	/* SBR bit stream header information */
	VO_U8                 bs_ampRes;
	VO_U8                 bs_startFreq;
	VO_U8                 bs_stopFreq;
	VO_U8                 bs_crossOverBand;

	/* state info that must be saved for each channel */
	SBRHeader             sbrHdr[MAX_CHANNELS];
	SBRHeader             sbrHdrPrev[MAX_CHANNELS];
	SBRHeader             sbrHdrPrevOK[MAX_CHANNELS];
	SBRGrid               sbrGrid[MAX_CHANNELS];
	SBRFreq               *sbrFreq[MAX_CHANNELS];
	SBRChan               *sbrChan[MAX_CHANNELS];

	int					  sbrError;
	/* temp variables, no need to save between blocks */
	VO_U8				  dataExtra;
	VO_U8				  resBitsData;
	VO_U8				  extendedDataPresent;
	int                   extendedDataSize;

	signed char           envDataDequantScale[MAX_SYNTAX_ELEMENTS][voMAX_NUM_ENV];
	int                   envDataDequant[MAX_SYNTAX_ELEMENTS][voMAX_NUM_ENV][VOMQ_BANDS];
	int                   noiseDataDequant[MAX_SYNTAX_ELEMENTS][MAX_NUM_NOISE_FLOORS][MAX_NUM_NOISE_FLOOR_BANDS];

	int                   eCurr[VOMQ_BANDS];
	VO_U8				  eCurrExp[VOMQ_BANDS];
	VO_U8				  eCurrExpMax;
	signed char           la;

	int                   crcCheckWord;
	int                   couplingFlag;
	int                   envBand;
	int                   eOMGainMax;
	int                   gainMax;
	int                   gainMaxFBits;
	int                   noiseFloorBand;
	int                   qp1Inv;
	int                   qqp1Inv;
	int                   sMapped;
	int                   sBand;
	int                   highBand;

	int                   sumEOrigMapped;
	int                   sumECurrGLim;
	int                   sumSM;
	int                   sumQM;
	int                   G_LimBoost[VOMQ_BANDS];
	int                   Qm_LimBoost[VOMQ_BANDS];
	int                   Sm_Boost[VOMQ_BANDS];

	int                   Sm_Buf[VOMQ_BANDS];
	int                   Qm_LimBuf[VOMQ_BANDS];
	int                   G_LimBuf[VOMQ_BANDS];
	int                   G_LimFbits[VOMQ_BANDS];

	int                   G_FiltLast[VOMQ_BANDS];
	int                   Q_FiltLast[VOMQ_BANDS];

	/* large buffers */
	int                   delayIdxQMFA[MAX_CHANNELS];
	int                   delayIdxQMFS[MAX_CHANNELS];
	
	int		              *delayQMFA[MAX_CHANNELS];  //[DELAY_SAMPS_QMFA];
	int				      *delayQMFS[MAX_CHANNELS];  //[DELAY_SAMPS_QMFS];
	int					  *XBufDelay[MAX_CHANNELS];  //[HF_GEN][64][2];
	ALIGN int             XBuf[32+8][64][2];
#ifdef PS_DEC
	ps_info				  *ps;
#endif
	int					  ps_used;
	int					  last_syn_ele;

} sbr_info;

/* voLog2[i] = ceil(log2(i)) (disregard i == 0) */
static const unsigned char voLog2[9] = {0, 0, 1, 2, 2, 3, 3, 3, 3};


#define Vo_Multi32(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 32)

static __inline int voGetBits(BitStream * const bsi,
				              const unsigned int nBits)
{
	unsigned int data, lowBits;

	data = bsi->iCache >> (32 - nBits);		
	bsi->iCache <<= nBits;					
	bsi->cachedBits -= nBits;				

	if (bsi->cachedBits < 0) {
		lowBits = -bsi->cachedBits;
		RefillBitstreamCache(bsi);
		data |= bsi->iCache >> (32 - lowBits);		
	
		bsi->cachedBits -= lowBits;			
		bsi->iCache <<= lowBits;			
	}
	return data;
}
/* sbrtabs.c */
extern const VO_U8 k0Tab[VOSAMP_RATES_NUM ][16];
extern const VO_U8 k2Tab[VOSAMP_RATES_NUM ][14];
extern const HuffInfo huffTabSBRInfo[10];
extern const signed char huffTabSBR[604];
extern const int log2Tab[65];
extern const int noiseTab[512*2];
extern const int cTabA[165];
extern const int cTabS[640];

int get_sr_index(const int sampleRate);

int voSBRDecUpdateFreqTables(AACDecoder* decoder,
							 SBRHeader *sbrHdr, 
							 SBRFreq *sbrFreq, 
							 int sampRateIdx);

int voSBR_Single_Channel_Element(AACDecoder* decoder,
						         BitStream *bs, 
						         int chBase
						         );

int voSBR_Channel_Pair_Element(AACDecoder* decoder,
							   BitStream *bs,
							   int chBase
							   );

int QMFAnalysis(int *inbuf, 
				int *delay, 
				int *XBuf, 
				int fBitsIn, 
				int *delayIdx, 
				int qmfaBands
				);


int voAACDecodePS(AACDecoder* decoder,
			      sbr_info* psi,
			      SBRGrid *sbrGrid,
			      SBRFreq *sbrFreq
				  );
void QMFSynthesisAfterPS(int *inbuf, int *delay, int *delayIdx, int qmfsBands, short *outbuf, int channelNum);
void QMFSynthesis(int *inbuf, 
				  int *delay, 
				  int *delayIdx, 
				  int qmfsBands, 
				  short *outbuf, 
				  int channelNum);


int voHFGen(sbr_info *psi, 
			SBRGrid *sbrGrid, 
			SBRFreq *sbrFreq, 
			SBRChan *sbrChan
            );

int voHFAdj(AACDecoder* decoder, 
			SBRHeader *sbrHdr, 
			SBRGrid *sbrGrid, 
			SBRFreq *sbrFreq, 
			SBRChan *sbrChan, 
			int ch);

void ReSetSBRDate(sbr_info *psi, VO_MEM_OPERATOR *voMemop);

int voSBR_Envelope(AACDecoder* decoder,
				   BitStream *bs, 
				   SBRGrid *sbrGrid, 
				   SBRFreq *sbrFreq, 
				   SBRChan *sbrChan, 
				   int ch);

int voSBR_Noise(AACDecoder* decoder,
				BitStream *bs, 
				SBRGrid *sbrGrid, 
				SBRFreq *sbrFreq, 
				SBRChan *sbrChan, 
				int ch
				);

int voAACReadPSData(AACDecoder *decoder, 
			   BitStream *bs, 
			   sbr_info *sbr, 
			   int bitsLeft);

void UncoupleSBREnvelope(sbr_info *psi, 
						 SBRGrid *sbrGrid, 
						 SBRFreq *sbrFreq, 
						 SBRChan *sbrChanR
						 );

void UncoupleSBRNoise(sbr_info *psi, 
					  SBRGrid *sbrGrid, 
					  SBRFreq *sbrFreq, 
					  SBRChan *sbrChanR);

void Radix4_FFT(int *x);
int  voInvRNormal(int r);
int  voSqrtFix(int q, int fBitsIn, int *fBitsOut);

#endif	/* _SBR_H */
