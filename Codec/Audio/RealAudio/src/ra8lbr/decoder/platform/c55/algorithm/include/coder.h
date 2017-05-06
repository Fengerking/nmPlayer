/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: coder.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
 * All Rights Reserved.   
 *   
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Community 
 * Source License (the "RCSL"), including Attachment G and any 
 * applicable attachments, all available at 
 * http://www.helixcommunity.org/content/rcsl.  You may also obtain 
 * the license terms directly from RealNetworks.  You may not use this 
 * file except in compliance with the RCSL and its Attachments. There 
 * are no redistribution rights for the source code of this 
 * file. Please see the applicable RCSL for the rights, obligations 
 * and limitations governing use of the contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *   
 * Technology Compatibility Kit Test Suite(s) Location:   
 * https://rarvcode-tck.helixcommunity.org   
 *   
 * Contributor(s):   
 *   
 * ***** END LICENSE BLOCK ***** */   

/*
 *	======== coder.h ========
 *
 *  Description : Definition of datatypes required by decoder
 
 * 				  Internal vendor specific (TNI) interface header for Real Audio
 *				  algorithm. Only the implementation source files include
 *			 	  this header; this header is not shipped as part of the
 *				  algorithm.
 *
 *  			  This header contains declarations that are specific to
 *  			  this implementation and which do not need to be exposed
 *  			  in order for an application to use the Real Audio algorithm.
 *				
 *
 * Gecko2 stereo audio codec.
 * Developed by Ken Cooke (kenc@real.com)
 * August 2000
 *
 * Last modified by:	Jon Recker (jrecker@real.com)
 *						01/24/01
 */

#ifndef _RA_TNI_CODER_H_
#define _RA_TNI_CODER_H_

#include "RA_tni_priv.h"     
#include "RA_tni.h"

#ifdef  _PROFILE_TREES_
//#include "tree profile.h"//renamed by srini
#include "profile.h"
#endif // _PROFILE_TREES_
#include "profile.h"
#define ASSERT(x) /* if (!(x)) exit(1)  <-- Commented out sanity check -- Venky */



#define MAXREGNS (MAXNSAMP/NBINS) /* used only for array sizes */

/* composite mlt */
#define MAXCSAMP (2*MAXNSAMP)
#define MAXCREGN (2*MAXREGNS)

/* coder */
#define NBINS		20		/* transform bins per region */
#define MAXCATZNS	(1 << 7)
#define CODINGDELAY	03		/* frames of coding delay */

/* gain control */
#define NPARTS		8		/* parts per half-window */
#define LOCBITS		3		/* must be log2(NPARTS)! */
#define GAINBITS	4		/* must match clamping limits! */
#define GAINMAX		4
#define GAINMIN		(-7)	/* clamps to GAINMIN<=gain<=GAINMAX */
#define GAINDIF		(GAINMAX - GAINMIN)
#define	CODE2GAIN(g)	((g) + GAINMIN)
#define FASTPOW2(n)		(pow2tab[(n)+63])
#define ROOTPOW2(n)		(rootpow2tab[(n)+63])

#if !defined  DECODEVECTORS_NEW || !defined DECODEVECTORS_ASM
#define FIXRP2MAN(n)	(RA_TNI_fixrootpow2man[(((n)+63) & 0x1)])	/* JR */
#define FIXRP2EXP(n)	(RA_TNI_fixrootpow2exp[(n)+63])	/* JR */
#endif //DECODEVECTORS_NEW

/* coupling */
#define NCPLBANDS 20

#define M_PI	3.14159265358979323846

/* These constants control the tradeoff between headroom (at what level saturation occurs)
 *	and precision (how many fraction bits to keep) before and after the MLT. Before making
 *	any changes, the code should be studied and experiments run to gain a solid understanding
 *  of the impacts that changes here will have.
 * IMPORTANT: POSTBITS + NINTBITS must be <= 31
 */
#define MLTBITS				22		/* see note in ScalarDequant() */
#define POSTBITS			19		/* never set less than 17, or risk rampant saturation before gain control */
#define NINTBITS			10		/* used in gain control, shouldn't adjust unless changing gain control tables */


// literals for channels
#define CH 2	/* max channels */
#define LEFT 0
#define RGHT 1


/* huffman decoding tree */
typedef short HUFFNODE[2];

                                        
// DMA initialization parameters                                        
#if defined  CONST_TABLE_DMA || defined WORKING_BUF_DMA
extern const IDMA2_Params RA_TNI_DMA_PARAMS ;
#endif



extern const HUFFNODE RA_TNI_power_tree[MAXREGNS-1][24];	/* diff power trees */
extern const HUFFNODE *const RA_TNI_sqvh_tree[7];				/* vector code trees */
extern const HUFFNODE *const RA_TNI_cpl_tree_tab[];				/* coupling data */

extern const short RA_TNI_bitrevtab[];
extern const long *const RA_TNI_twidtabs[3];
/* extern const long *ztwidtab; */

extern const char RA_TNI_expbits_tab[8];
extern const short RA_TNI_vhsize_tab[7];

#ifndef DECODEVECTORS_ASM
/* extern const int invradix_tab[7]; */
extern const char RA_TNI_kmax_tab[7];
extern const short RA_TNI_iradix_tab[7];
extern const char RA_TNI_vd_tab[7];
extern const char RA_TNI_vpr_tab[7];
#endif //DECODEVECTORS_ASM


/* JR - new */
extern const long RA_TNI_fixdither_tab[8];
#if !defined  DECODEVECTORS_NEW || !defined DECODEVECTORS_ASM
extern const long RA_TNI_fixrootpow2man[2];
#else // DECODEVECTORS_NEW
extern const long RA_TNI_fixrootpow2man_new[2] ;
#endif //DECODEVECTORS_NEW
extern const char RA_TNI_fixrootpow2exp[127];
extern const long* const RA_TNI_fqct_man[7];
extern const char* const RA_TNI_fqct_exp[7];

/*
 * Prototypes
 */

/* bitpack.c */
short RA_TNI_Unpackbits(short nbits, USHORT *data, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);
short RA_TNI_Unpackbit(USHORT *data, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);
#ifdef NOPACK
void RA_TNI_DecodeBytes(UCHAR *codebuf, short nbits, ULONG *pkbuf);
#else
void RA_TNI_DecodeBytes(USHORT *codebuf, short nbits, ULONG *pkbuf);
#endif



/* category.c */
// Define OPT_CATEGORIZE to enable the optimized version of RA_TNI_Categorize ( merged with RA_TNI_ExpandCategory)
#ifndef OPT_CATEGORIZE
void RA_TNI_Categorize(short *rms_index, short availbits, short *catbuf, short *catlist, short ncatzns, RA_TNI_Obj *bitstrmPtr);
void RA_TNI_ExpandCategory(short *catbuf, short *catlist, short rate_code);
#else		//OPT_CATEGORIZE
// Merged version of RA_TNI_Categorize and RA_TNI_ExpandCategory
void RA_TNI_Categorize(short *rms_index, short availbits, short *catbuf,  short ncatzns,  RA_TNI_Obj *bitstrmPtr, short rate_code);
#endif		//OPT_CATEGORIZE


/* codemlt.c */
void RA_TNI_DecodeMLT(short availbits, long *mlt, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);

/* couple.c */
void RA_TNI_JointDecodeMLT(short availbits, long *mltleft, long *mltrght, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);

/* coder.c */
int InitDecoder(short nSamples, short nChannels, short nRegions, short nFrameBits, unsigned short sampRate, short cplStart, short cplQbits, short *codingDelay, RA_TNI_Obj *bitstrmPtr);
int FreeDecoder();
int Decode(UCHAR *codebuf, int nextlost, short *outbuf, RA_TNI_Obj *bitstrmPtr);
void InitDecBuffers(RA_TNI_Obj *bitstrmPtr);

/* envelope.c */
int RA_TNI_DecodeEnvelope(short *rms_index, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);

/* fft.c */
int RA_TNI_InitFFT(short n, short *nfftshifts);
void RA_TNI_FreeFFT(void);
void RA_TNI_FFT(long *fftbuf, short nsamples);


/* gainctrl.c */
int RA_TNI_InitGainControl(short nsamples, short pcmInterleaveFact);
void RA_TNI_GainCompensate(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps, short pcmInterleaveFact, long *overlap, int *outbuf);
int RA_TNI_DecodeGainInfo(GAINC *gainc, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);

/* huffman.c */
#ifdef  _PROFILE_TREES_
short RA_TNI_DecodeNextSymbol(const HUFFNODE *tree, TreeProfile *profile_tree, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);
#else
short RA_TNI_DecodeNextSymbol(const HUFFNODE *tree, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);
#endif

/* mlt.c */             
#ifdef CONST_TABLE_DMA		             
void RA_TNI_IMLTNoOverlap(long *mlt, long *pcm, short nsamples, long *buf, RA_TNI_Obj *bitstrmPtr);
#else
void RA_TNI_IMLTNoOverlap(long *mlt, long *pcm, short nsamples, long *buf);
#endif 

/* sqvh.c */
int RA_TNI_UnpackSQVH(short cat, short *k, short *s, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);

void RA_TNI_DecodeVectors(short *catbuf, short *rms_index, long *mlt, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);


#endif /* _CODER_H_ */

