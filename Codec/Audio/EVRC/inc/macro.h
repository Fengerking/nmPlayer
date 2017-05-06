/**********************************************************************
Each of the companies; Lucent, Motorola, Nokia, and Qualcomm (hereinafter 
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
all Normative portions of the Enhanced Variable Rate Codec (EVRC) as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
 
/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
/*     Copyright (C) 1997-1998 Telecommunications Industry Association. */
/*     All rights reserved.                                             */
/*----------------------------------------------------------------------*/
/* Note:  Reproduction and use of this software for the design and      */
/*     development of North American Wideband CDMA Digital              */
/*     Cellular Telephony Standards is authorized by the TIA.           */
/*     The TIA does not authorize the use of this software for any      */
/*     other purpose.                                                   */
/*                                                                      */
/*     The availability of this software does not provide any license   */
/*     by implication, estoppel, or otherwise under any patent rights   */
/*     of TIA member companies or others covering any use of the        */
/*     contents herein.                                                 */
/*                                                                      */
/*     Any copies of this software or derivative works must include     */
/*     this and all other proprietary notices.                          */
/*======================================================================*/
/*  Module:     macro.h                                                 */
#ifndef  _MACRO_H_
#define  _MACRO_H_

/* #define UNIX */

/* Macros */
#define  Min(a,b) (a<b ? a:b)
#define  Max(a,b) (a>b ? a:b)
#define  Sign(Z) ((Z) < 0 ? -1l : 1l)
#define  UNIX_DEBUG(x)
#define  SPACING   9

/* includes */
#include <stdlib.h>
//#include <math.h>
#include <stdio.h>
//#include <fcntl.h>
#include "voEvrcDecID.h"

/* generic definitions */
#define FALSE 0
#define TRUE 1


#define SHORT_SHIFT_ON

/* user function definition */
//#include "proto.h"

/* speech coder parameters */
#define FrameSize   160     /* CELP frame size */
#define NoOfSubFrames 3     /* Number of sub frames in one frame     */
#define SubFrameSize  54
#define HPMEMORY      FrameSize+SubFrameSize

/* Memory required for HPspeech array    */

#define ORDER         10    /* LPC order                             */
#define MIN_LSP_SEP   261       /* (64Hz=0.05/(2*PI)*32767 Min. LSP Separation */
#define GAMMA1        29491 /*0.9*/ /* Weighting filter fraction coefficient */
#define GAMMA2        16384 /*0.5*/ /* Weighting filter fraction coefficient (>.5 muffled)*/

#define maxFCBGainSize  32  /* Size of fcb scalar gain (8k,=16 for 4k*/
#define ACBGainSize   8     /* Size of acb scalar gain               */

#define Hlength       SubFrameSize  /* Length of impulse response       */
#define ACBMemSize    128   /* Size of adaptive c.b. memory          */
#define PACKWDSNUM    11    /* Without one word for frame erasure signaling */
#define PACKBYTESNUM  16

#define _Gamma_4      32571  /*0.994*/

/* Post filter definitions */
#define ALPHA         18678  /*0.57*/   /* Short term post filter parameter (whiten formants)*/
#define BETA          24576  /*0.75*/   /* Short term post filter parameter (boost formants)*/
#define U             6554 /*0.20*/ /* Spectral tilt (orig=.2, >.2 clear, <.2 muffled)*/
#define AGC           27853 /*0.85*/    /* AGC factor                       */
#define LTGAIN        16384 /*0.50*/    /* Long term post filter gain       */

/* Post filter defines for half rate */
#define HALF_ALPHA    16384 /*0.50*/    /* Short term post filter parameter (whiten formants)*/
#define HALF_U        11469 /*0.35*/    /* Spectral tilt (orig=.2, >.2 clear, <.2 muffled)*/

/* Rcelp parameters */
#define GUARD          80   /* Guard buffers for RCELP          */
#define RRESOLUTION    8    /* Jitter resolution                */
#define RSHIFT         3    /* Search boundary                  */
#define DMIN          20    /* Minimum delay                    */
#define DMAX         120    /* Maximum delay                    */
#define BLPRECISION    8    /* Interpolation filter taps        */
#define BLFREQ       29491 /*0.9*/  /* Cut-off filter frequency         */
#define EXTRA         10    /* Extra samples calc. in exc.      */

#define  BQ_N_DATA     160
#define  BQ_N_FILTERS    3
#define  BQ_N_ORDER      2
#define  BQ_N_SAVE       (BQ_N_FILTERS * BQ_N_ORDER)
#define  BQ_N_BUFFER     (BQ_N_SAVE + BQ_N_DATA)
#define  BQ_N_W          (2 * BQ_N_FILTERS * (BQ_N_ORDER + 1))

/* Defines */
#define         FRM_LEN                 80
#define         DELAY                   24
#define         FFT_LEN                 128

#define         NUM_CHAN                16
#define         LO_CHAN                 0
#define         MID_CHAN                5
#define         HI_CHAN                 15

#define         TRUE                    1
#define         FALSE                   0

#define         UPDATE_THLD             35
#define         METRIC_THLD             45
#define         INDEX_THLD              12
#define         SETBACK_THLD            12
#define         SNR_THLD                6
#define         INDEX_CNT_THLD          5
#define         HYSTER_CNT_THLD         6
#define         UPDATE_CNT_THLD         50

/* Define the following if 0db input to ns */
#define		INPUT_0_DB
#ifdef		INPUT_MINUS_6_DB

#define         NOISE_FLOOR_CHAN        64	/* 1.0/4 scaled as 23,8 */
#define         MIN_CHAN_ENRG           4	/* 0.0625/4 scaled as 23,8 */
#define         MIN_NOISE_ENRG          4	/* 0.0625/4 scaled as 23,8 */
#define         INE_CHAN                1024	/* 16.0/4 scaled as 23,8 */
#define         INE_NOISE               1024	/* 16.0/4 scaled as 23,8 */

#define         HIGH_TCE_DB             1408	/* 44.0 scaled as 10,5 */
#define         LOW_TCE_DB              768	/* 24.0 scaled as 10,5 */

#endif
#ifdef		INPUT_0_DB

#define         NOISE_FLOOR_CHAN        256	/* 1.0 scaled as 23,8 */
#define         MIN_CHAN_ENRG           16	/* 0.0625 scaled as 23,8 */
#define         MIN_NOISE_ENRG          16	/* 0.0625 scaled as 23,8 */
#define         INE_CHAN                4096	/* 16.0 scaled as 23,8 */
#define         INE_NOISE               4096	/* 16.0 scaled as 23,8 */

#define         HIGH_TCE_DB             1600	/* 50.0 scaled as 10,5 */
#define         LOW_TCE_DB              960	/* 30.0 scaled as 10,5 */

#endif



#define         TCE_RANGE               (HIGH_TCE_DB - LOW_TCE_DB)
#define         HIGH_ALPHA_S5_10        1023
#define         LOW_ALPHA_S5_10         511
#define         HIGH_ALPHA              32440	/* 0.99 scaled as 0,15 */
#define         LOW_ALPHA               16383	/* 0.50 scaled as 0,15 */

#define         ALPHA_RANGE             (HIGH_ALPHA - LOW_ALPHA)
#define         ALPHA_RAN_DIV_TCE_RAN   803		/* (0.99 - 0.50) / 20 scaled as 0,15 */
#define         DEV_THLD                896		/* 28.0 scaled as 10,5 */
#define         PRE_EMP_FAC             -26214	/* -0.8 scaled as 0,15 */
#define         CEE_SM_FAC              1181116006	/* 0.55 scaled as 0,31 */
#define         ONE_MINUS_CEE_SM_FAC    966367642	/* 0.55 scaled as 0,31 */
#define         MIN_GAIN                -27262976	/* (-13.0) scaled as 11,20 */
#define         GAIN_SLOPE              14879	/* 0.45 scaled as 0,15 */
#define         CNE_SM_FAC              3277	/* 0.1 scaled as 0,15 */

#define         ONE_MINUS_CNE_SM_FAC    (32767-CNE_SM_FAC)
#define         DE_EMP_FAC              26214	/* 0.8 scaled as 0,15 */
#define         LOG_OFFSET              626255212	/* 9.3319 scaled as 5,26 */
#define         ONE_OVER_20             1638	/* 1/20 scaled as 0,15 */
#define         TEN_S5_10               10270	/* 10.0 scaled as 5,10 */
#define         CONST_0_1875_S10_21     393216	/* 0.1875 scaled as 10,21 */
#define         CONST_2_667_S5_10       2731	/* 2.667 scaled as 5,10 */

#define         FFT_HEADROOM            2
#define         IFFT_HEADROOM           4

#endif
