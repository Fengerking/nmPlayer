/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point RealAudio 8 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * October 2003
 *
 * coder.h - private, implementation-specific header file
 **************************************************************************************/

#ifndef _CODER_H
#define _CODER_H
#include "voRADecID.h"
#include "voMem.h"
#include "gecko2codec.h"	/* contains public API */
#include "statname.h"		/* do name-mangling for static linking */
#include "assembly.h"

#if defined _WIN32 && defined _DEBUG && defined FORTIFY
#include "fortify.h"
#endif

#define CODINGDELAY		2					/* frames of coding delay */

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

/* do y <<= n, clipping to range [-2^30, 2^30 - 1] (i.e. output has one guard bit) */
#define CLIP_2N_SHIFT(y, n) { \
	int sign = (y) >> 31;  \
	if (sign != (y) >> (30 - (n)))  { \
		(y) = sign ^ (0x3fffffff); \
	} else { \
		(y) = (y) << (n); \
	} \
}

#define FBITS_OUT_DQ	12		/* number of fraction bits in output of dequant */
#define FBITS_LOST_IMLT	7		/* number of fraction bits lost (>> out) in IMLT */
#define GBITS_IN_IMLT	4		/* min guard bits in for IMLT */

/* coder */
#define NBINS			20					/* transform bins per region */
#define MAXCATZNS		(1 << 7)
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

typedef struct _HuffInfo {
	int maxBits;
    unsigned char count[16];	/* number of codes at this length */
	int offset;
} HuffInfo;

/* bitstream info */
typedef struct _BitStreamInfo {
	unsigned char *buf;
	int off;
	int key;
} BitStreamInfo;

/* gain control info */
typedef	struct _GAINC {
	int	nats;				/* number of attacks */
	int	loc[MAXNATS];		/* location of attack */
	int	gain[MAXNATS];		/* gain code */
	int maxExGain;			/* max gain after expansion */
} GAINC;

/* buffers for decoding and reconstructing transform coefficients */
typedef struct _DecBufs {
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

} DecBufs;

typedef struct _Gecko2Info {
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

	/* dither for dequant */
	int lfsr[MAXNCHAN];

	/* transform info */
	int rateCode;
	int xformIdx;
	int rmsMax[MAXNCHAN];
	int xbits[MAXNCHAN][2];

	/* bitstream info */
	BitStreamInfo bsi;
	
	/* gain control info */
	GAINC dgainc[MAXNCHAN][CODINGDELAY];

	/* data buffers */
	DecBufs db;
	
} Gecko2Info;

/* memory allocation */
Gecko2Info *AllocateBuffers(VO_MEM_OPERATOR	  *pMemOP);
void FreeBuffers(Gecko2Info *gi, VO_MEM_OPERATOR	  *pMemOP);

/* bitstream decoding */
int DecodeSideInfo(Gecko2Info *gi, unsigned char *buf, int availbits, int ch);
unsigned int GetBits(BitStreamInfo *bsi, int nBits, int advanceFlag);
void AdvanceBitstream(BitStreamInfo *bsi, int nBits);

/* huffman decoding */
int DecodeHuffmanScalar(const unsigned short *huffTab, const HuffInfo *huffTabInfo, int bitBuf, int *val);	

/* gain control parameter decoding */
int DecodeGainInfo(Gecko2Info *gi, GAINC *gainc, int availbits);
void CopyGainInfo(GAINC *gaincDest, GAINC *gaincSource);

/* joint stereo parameter decoding */
void JointDecodeMLT(Gecko2Info *gi, int *mltleft, int *mltrght);
int DecodeCoupleInfo(Gecko2Info *gi, int availbits);

/* transform coefficient decoding */
int DecodeEnvelope(Gecko2Info *gi, int availbits, int ch);
void CategorizeAndExpand(Gecko2Info *gi, int availbits);
int DecodeTransform(Gecko2Info *gi, int *mlt, int availbits, int *lfsrInit, int ch);

/* inverse transform */
void IMLTNoWindow(int tabidx, int *mlt, int gb);
void R4FFT(int tabidx, int *x);

/* synthesis window, gain control, overlap-add */
void DecWindowWithAttacks(int tabidx, int *buf1, short *pcm1, int nChans, GAINC *gainc0, GAINC *gainc1, int fbits[2]);
#ifndef ARM
void DecWindowNoAttacks(int tabidx, int *buf1, short *pcm1, int nChans);
#else
void DecWindowNoAttacks(int tabidx, int *buf1, short *pcm1, int nChans,const int *table1,const int *table2,const int *table3);
#endif
/* bitpack.c */
extern const unsigned char pkkey[4];
	
/* hufftabs.c */
#define HUFFTAB_COUPLE_OFFSET		2

extern const HuffInfo huffTabCoupleInfo[5];
extern const unsigned short huffTabCouple[119];
extern const HuffInfo huffTabPowerInfo[13];
extern const unsigned short huffTabPower[312];
extern const HuffInfo huffTabVectorInfo[7];
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

#endif	/* _CODER_H */
