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
/* coder.h - defines for a particular coder */
#ifndef  __CODER_H__
#define  __CODER_H__

#include "defines.h"

#define PMAX 120
#define PMIN  20
#define FREQBANDS 2
#define FILTERORDER 17


#define WORDS_PER_PACKET 18
#define BYTES_PER_PACKET 36
#define NUMMODES     5
#define FULLRATE_VOICED 4
#define HALFRATE_VOICED 3
#define QUARTERRATE_UNVOICED 2
#define QUARTERRATE_VOICED 5
#define EIGHTH 1
#define ERASURE 0xe
#define BLANK 0
#define STATWINDOW 400        /* the window over which rate control stats */
                              /* are calculated (non 1/8 rate)            */
#define FSIZE      160        /* Overall frame size                     */
#define FSIZE8TH    20
#define LPCSIZE    160        /* LPC frame size                         */
#define LPCOFFSET   60        /* Offset of LPC frame to Fsize           */
#define LPCORDER    10        /* The highest order of the LPC filter    */
#define PSMORDER    4         /* The highest order of the pitch smoothing */
                              /* FIR filter    */
#define INVFSIZE	205	      /* 1/FSIZE * (1 << 15)   */
#define	INVFSIZE100	20480     /* 100/FSIZE * (1 << 15)   */
#define LENGTH_OF_IMPULSE_RESPONSE  20
#define MEM_L                        0  /**/ /* overlap of codebook subframe */
#define AGC_FACTOR                  0.9375
#define PF_ZERO_WGHT_FACTOR         0.625
#define PF_POLE_WGHT_FACTOR         0.775
#define PERCEPT_WGHT_FACTOR         0.78
#define PERCEPT_WGHT_FACTOR_16      25559   /* PERCEPT_WGHT_FACTOR * (1 <<15) */
#define BWE_FACTOR                  0.9883
#define BWE_FACTOR_16               32385  /* BWE_FACTOR * (1 <<15) */
#define BRIGHT_COEFF                0.3
#define LSP_SPREAD_FACTOR           0.02
#define	INTERPRE06					19661  /*0.6 * (1 << 15) */
#define	INTERPRE08					26214  /*0.8 * (1 << 15) */

/* note that while pitch subframes are indicated for modes 0,3,4 they are    */
/* disabled in the pitch search, these pitch subframes are needed for proper */
/* interpolation                                                             */

#define PITCHSF8TH 8
#define PITCHSF_ERASURE 4
#define MAXPITSF     4  
#define MAXCBPERPIT  20 
#define MAXSF        20       /* the most pitch or cb subfames per frame*/

#define MINLAG      17        /* The minimum and maximum pitch lags     */
#define MAXLAG      143       /* this has been changed to match IS-96   */

#define MINB                 0.0 /* minimum PGain value */
#define MAXB                 2.0 /* maximum PGain value */
#define NUMBER_OF_B_LEVELS   9 /* number of quantization levels for PGain */


#define FULLG 0.5  
#define HALFG 0.5

#define MAX_FR_RESOLUTION           4
#define FR_INTERP_FILTER_LENGTH     8

#define LOWRATE_GAIN_FACTOR 0.5   /* used to suppress background noise*/
                          /* changed to 0.5 from 0.25 per
			     JAM's recommendation */
#define GORDER               3
#define GPRED_ORDER 2  /* EIGHTH rate prediction */

#define G_FACTOR 1.2608
#define G_FACTOR_16 20657  /* G_FACTOR * (1 << 14) */
#define MAXNUMCB  1

#define CBLENGTH   128

extern short  PITCHSF[NUMMODES];
extern short  FSIZE_DIV_PITCHSF[NUMMODES];
extern short  CBSF[NUMMODES];
extern short  FSIZE_DIV_CBSF[NUMMODES];
extern short  CBSF_DIV_PITCHSF[NUMMODES];
extern short  NUMCB[NUMMODES];

#endif //__CODER_H__
