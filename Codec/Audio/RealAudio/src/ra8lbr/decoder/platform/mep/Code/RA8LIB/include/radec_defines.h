
/**************************************************************************** */
/*    Socrates Software Ltd : Toshiba Group Company */
/*    DESCRIPTION OF CHANGES: */
/*		1. Error handling and Debug trace support added
/*		2. Reorganization of Gecko2Info structure
/*		3. Some macro definitions added to support DMA transfer
/*
 *    CONTRIBUTORS : Vinayak Bhat,Deepak,Sudeendra,Naveen
 *   */
/**************************************************************************** */

#ifndef _RADEC_DEFINES_H
#define _RADEC_DEFINES_H


#define CODINGDELAY				2	
#define FBITS_OUT_DQ			12		/* number of fraction bits in output of dequant */
#define FBITS_LOST_IMLT			7		/* number of fraction bits lost (>> out) in IMLT */
#define GBITS_IN_IMLT			4		/* min guard bits in for IMLT */
/*
#define INTERNAL_CATBUFF                        0x202200
#define INTERNAL_RMSINDEX                       0x202000
*/
	


#if defined(MEP)
/********** This is the OPTIMISATION LEVELS ***********/
#define __DMA_DECMLT__                  1
#define __DMA_DECTRAN__                 1
#define __DMA_WINATT__                  1

#define __DMA_PREMUL__                  1
#define __DMA_POSTMUL__                 1
#define __DMA_XTRAMUL__                 1
#define __DMA_HUFFTAB__                 1
#define __DMA_WINTAB__                  1
#define __DMA_AUDIOTAB__                1

#define __DMA_CATEGORY__                1
#define __DMA_ENVELOPE__                1
#define __DMA_SQVH__                    1
#define __DMA_COUPLE__                  1

/******************************************************/
#endif

/* coder */
#define NBINS			20		/* transform bins per region */
#define MAXCATZNS		128
#define NCPLBANDS		20

#define MAXNCHAN		2
#define MAXNSAMP		1024
#define MAXREGNS		(MAXNSAMP/NBINS)	

/* composite mlt */
#define MAXCSAMP		(2*MAXNSAMP)
#define MAXCREGN		(2*MAXREGNS)
#define NUM_MLT_SIZES	3
#define MAXNMLT			1024
#define MAXCPLQBITS		6

/* gain control */
#define NPARTS			8					/* parts per half-window */
#define MAXNATS			8
#define LOCBITS			3					/* must be log2(NPARTS)! */
#define GAINBITS		4					/* must match clamping limits! */
#define GAINMAX			4
#define GAINMIN			(-7)				/* clamps to GAINMIN<=gain<=GAINMAX */
#define GAINDIF			(GAINMAX - GAINMIN)
#define CODE2GAIN(g)	((g) + GAINMIN)

#define NUM_POWTABLES	13				/* number of distinct tables for power envelope coding */
#define MAX_HUFF_BITS	16				/* max bits in any Huffman codeword (important - make sure this is true!) */
#define HUFFTAB_COUPLE_OFFSET		2

/*
 * Random bit generator, using a 32-bit linear feedback shift register.
 * Primitive polynomial is x^32 + x^7 + x^5 + x^3 + x^2 + x^1 + x^0.
 * lfsr = state variable
 *
 * Update step:
 * sign = (lfsr >> 31);
 * lfsr = (lfsr << 1) ^ (sign & FEEDBACK);
 */
#define FEEDBACK ((1<<7)|(1<<5)|(1<<3)|(1<<2)|(1<<1)|(1<<0))

#ifndef ASSERT
#if defined (_WIN32) && defined (_M_IX86) && (defined (_DEBUG) || defined (REL_ENABLE_ASSERTS))
#define ASSERT(x) if (!(x)) __asm int 3;
#else
#define ASSERT(x) /* do nothing */
#endif
#endif

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef NULL
#define NULL 0
#endif

/*ERROR HANDLING ENUMS*/
typedef enum{
	RA8_SUCCESS								=0,  	/*PROCESSING DONE SUCCESSFULLY*/
	RA8_FATAL_ERR_DEC_PTR_NULL			    =1,		/*DECODER STRUCTURE POINTER IS NULL*/
	RA8_FATAL_ERR_INPUT_BUF_NULL			=2,		/*INPUT BUFFER POINTER WILL BE NULL*/
	RA8_FATAL_ERR_OUTPUT_BUF_NULL			=3,		/*OUTPUT BUFFER POINTER IS NULL*/
	RA8_FATAL_ERR_INVALID_SAMPLES_PER_FRAME	=4,	    /*INVALID SAMPLES PER FRAME*/
	RA8_FATAL_ERR_INVALID_CHANNELS			=5,		/*INVALID NUMBER OF CHANNELS*/
	RA8_FATAL_ERR_INVALID_FREQREGIONS		=6,		/*INVALID NUMBER OF REGIONS FOR THE TRANSFORM*/
	RA8_FATAL_ERR_INVALID_SAMPRATE			=7,		/*INVALID SAMPLING RATE*/ 
	RA8_FATAL_ERR_INVALID_CPLQBITS			=8,		/*INVALID NUMBER OF BITS FOR EACH COUPLING SCALE FACTOR*/
	RA8_FATAL_ERR_INVALID_CPLSTART			=9,	/*INVALID START OF COUPLING FREQUENCY REGION*/
	RA8_FATAL_ERR_INVALID_BITS_PER_FRAME	=10,	/*INVALID BITS PER FRAME*/
	RA8_ERR_IN_SIDEINFO						=11,	/*SIDE INFORMATION ERROR*/
	RA8_ERR_ATTACKS_EXCEED_IN_GAINCONTROL	=12,	/*GAIN EXCEEDS THA MAXIMMUM ATTACKS*/
	RA8_ERR_IN_DECODING_POWER_ENVELOPE		=13,	/*ERROR IN RECONSTRUCTION OF POWER ENVELOPE*/
	RA8_ERR_DECODING_HUFFMAN_VECTORS		=14,	/*ERROR IN DECODING HUFFMANN VECTORS*/
	RA8_ERR_DECODING_SYNTHESIS_WINDOW		=15		/*ERROR IN APPLYING SYNTHESIS WINDOW*/	
}ERRCODES;
#ifdef RAVDEBUG
#define RA8_DEBUG
#endif
#ifdef RA8_DEBUG
/*DEBUG TRACE STRUCTURE*/
struct SRA8_Debug{
	int iSamplesperframe;				/*Samples per frame*/
	int iNumchannels;					/*Number of channels*/
	int iNumregions;					/*Number of regions*/
	int iFramebits;						/*Bits per frame*/
	int iSampPersec;					/*Samples per seconds*/
	int iCplStart;						/*Start region of coupling*/
	int iCplQbits;						/*Coupling Scale Factor*/
	int iJointStereoFlg;				/*Flag to indicate Joint Stereo stream or not*/
	int iRatebits;						/*Number of bits required to fetch Rate codes in Side Information*/
	int iRateCode[MAXNCHAN];            /*Decoder Rate code*/
	int iNats[MAXNCHAN];				/*Number of Attacks in gain control*/
	int iLocNats[MAXNCHAN][MAXNATS];    /*Location of Attacks*/
	int iGainAttck[MAXNCHAN][MAXNATS];  /*Gain for each Attack*/
	int iHuffmode;						/*Flag indicating Direct or Huffmann coded stream*/
	int iBandStart;						/*Coupling Band Start*/
	int iBandEnd;						/*Coupling Band End*/
	int iCregions;						/*Number of regions including Coupling regions*/
	int iRmsMax[MAXNCHAN];              /*Maximmum Power Index for each channel*/
	int iCplIndex[NCPLBANDS];			/*Index Per Coupling Band*/
	int iRmsIndex[MAXNCHAN][MAXCREGN];  /*Power Index for each frequency region*/
	int iCatBuff[MAXNCHAN][MAXCREGN];   /*Quantizer Index for each region*/
	int igbMin[MAXNCHAN];               /*Minimmum number of Gaurd bits in Coefficients*/
	short int *piOutPCMData[MAXNCHAN];  /*Pointer to Decoded PCM Data for each channel*/
};
#endif
	
	
#define CLIP_2N_SHIFT(y, n) { \
	int sign = (y) >> 31;  \
	if (sign != (y) >> (30 - (n)))  { \
		(y) = sign ^ (0x3fffffff); \
	} else { \
		(y) = (y) << (n); \
	} \
}

typedef struct  
{
	int maxBits;
    unsigned char count[16];	/* number of codes at this length */
	int offset;
} SHuffInfo;

/* bitstream info */
typedef struct  
{
	unsigned char *buf;
	int off;
	int key;
} SBitStreamInfo;

/* gain control info */
typedef	struct 
{
	int	nats;				/* number of attacks */
	int	loc[MAXNATS];		/* location of attack */
	int	gain[MAXNATS];		/* gain code */
	int maxExGain;			/* max gain after expansion */
} SGainCInfo;

/* buffers for decoding and reconstructing transform coefficients */
typedef struct 
{
	int decmlt[MAXNCHAN][2*MAXNSAMP];	/* one double-sized MLT buffer per-channel, to hold overlap */

	/* Categorize() */
	int maxcat[MAXCREGN];
	int mincat[MAXCREGN];
	int changes[2*MAXCATZNS];			/* grows from middle outward */
	int maxheap[MAXCREGN+1];			/* upheap sentinel */
	int minheap[MAXCREGN+1];			/* upheap sentinel */

	/* DecodeMLT() */
	int rmsIndex[MAXCREGN];				/* RMS power quant index */
	int catbuf[MAXCREGN];

	/* JointDecodeMLT() */
	int cplindex[NCPLBANDS];

} SDecBufInfo;

typedef struct 
{
	/* general codec params */
	int nSamples;
	int nChannels;
	int nRegions;
	int nFrameBits;
	int sampRate;
	int cplStart;
	int cplQbits;
	int rateBits;
	int cRegions;
	int nCatzns;
	int jointStereo;
	int lossRate;
	int writeFlag;
	int resampRate;
	unsigned char *inBuf;
	short *outbuf;
	int geckoMode;
}SHeaderInfo;

typedef struct
{
	/* dither for dequant */
	int lfsr[MAXNCHAN];

	/* transform info */
	int rateCode;
	int xformIdx;
	int rmsMax[MAXNCHAN];
	int xbits[MAXNCHAN][2];
}SBufferInfo;

typedef struct Gecko2Info 
{
	
	/* data buffers */
	 SDecBufInfo sDecBuf;
		
	
	SHeaderInfo     sHdInfo;
	SBufferInfo     sBufferInfo;
	
	/* bitstream info */
	//SBitStreamInfo sBitInfo;
	
	/* gain control info */
	SGainCInfo sGainC[MAXNCHAN][CODINGDELAY];
	
} Gecko2Info;


/* bitstream decoding */
int DecodeSideInfo(Gecko2Info *,SBitStreamInfo *, unsigned char *, int, int);
unsigned int GetBits(SBitStreamInfo *, int, int);
void AdvanceBitstream(SBitStreamInfo *, int);

/* huffman decoding */
int DecodeHuffmanScalar(const unsigned short *, const SHuffInfo *, int, int *);	

/* gain control parameter decoding */
int DecodeGainInfo(Gecko2Info *,SBitStreamInfo *, SGainCInfo *, int);
void CopyGainInfo(SGainCInfo *, SGainCInfo *);

/* joint stereo parameter decoding */
void JointDecodeMLT(Gecko2Info *, int *, int *);
int DecodeCoupleInfo(Gecko2Info *,SBitStreamInfo *, int);

/* transform coefficient decoding */
int DecodeEnvelope(Gecko2Info *,SBitStreamInfo *, int, int);
int CategorizeAndExpand(Gecko2Info *, int);
int DecodeTransform(Gecko2Info *,SBitStreamInfo *, int *, int, int *, int,int *);

/* inverse transform */
void IMLTNoWindow(int, int *, int);
void R4FFT(int, int *);

/* synthesis window, gain control, overlap-add */
void DecWindowWithAttacks(int, int *, short *, int, SGainCInfo *, SGainCInfo *gainc1, int[],int*);
void DecWindowNoAttacks(int, int *, short *, int);




/* bitpack.c */
extern const unsigned char pkkey[4];

/* hufftabs.c */
extern const SHuffInfo huffTabCoupleInfo[5];
extern const unsigned short huffTabCouple[119];
extern const SHuffInfo huffTabPowerInfo[13];
extern const unsigned short huffTabPower[312];
extern const SHuffInfo huffTabVectorInfo[7];
extern const unsigned short huffTabVector[1276];

/* trigtabs.c */
extern const int nmltTab[3];
extern const int window[256 + 512 + 1024];
extern const int windowOffset[3];
extern const int cos4sin4tab[256 + 512 + 1024];
extern const int cos4sin4tabOffset[3];
extern const int cos1sin1tab[514];
extern const unsigned char bitrevtab[33 + 65 + 129];
extern const int bitrevtabOffset[3];
extern const int twidTabEven[4*6 + 16*6 + 64*6];
extern const int twidTabOdd[8*6 + 32*6 + 128*6];

#endif	/* _RADEC_DEFINES_H */
