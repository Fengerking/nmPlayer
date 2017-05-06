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
/* coder.rate.h - defines for ratedec.c */
#ifndef  __CODERATE_H__
#define  __CODERATE_H__

#define PITCH_NUM 2
#define INC_FACTOR 1.03
#define INC_FACTOR_16 16876  /* INC_FACTOR * (1 << 14)   */  
#define LPC_ORDER_NOISE 8
#define SNR_MAP_THRESHOLD 3
#define IS96_INC  1.00547
#define IS96_INC_16  16474 /* IS96_INC * (1 << 14)   */ 
#define VOICE_INITIAL 65 + 12
#define VOICE_INITIAL_HI 55 + 12
#define COSTABLE_SCALE 9
#define COSTABLE_LEN   512

#define GRID_POINT  60
#define STATVOICED 5
#define SCALE_DOWN_ENERGY 0.97
#define SCALE_DOWN_ENERGY_16 31785  /* SCALE_DOWN_ENERGY * (1 << 15)  */
#define FULL_THRESH 0

#define TLEVELS 8

#define THR_SIL_1_4  15 /* 14db below current energy is declared 1/4 rate silence */
#define THR_SIL_1_4_20Q  15728640 /* THR_SIL_1_4 * (1 << 20) */
#define THR_SIL_1_2  9 /* 9db below current energy is declared 1/2 rate silence */
#define THR_SIL_1_2_20Q  9437184 /* THR_SIL_1_2 * (1 << 20) */

#define THR_DIFFLSP 0.02 /* lsp must not change too quickly also for rate to
			    get bumped down                                 */
#define THR_DIFFLSP_15Q 655 /* THR_DIFFLSP * (1 << 15)    */

#define THR_DIFFLSP_15Q 655 /* THR_DIFFLSP * (1 << 15)    */
#define FRAME_ENG_SCALE	28918  /* 0.8825 * (1 << 15) */
#define SMSNR  0.6  /* leaky integration constant used smooth snr estimate */
                    /* changed on 8-Dec-94 per Sharath's recommendation */
#define SMSNR_16  19661     /* SMSNR * (1 << 15)  */       
#define RATEM 0.02         /* bounds around avg rate that are acceptable */
#define RATEM_15Q 655         /* RATEM_15Q * (1 << 15) */
#define ALPHA_STAT 0.9995   /*40second time constant   */

#define ADP 8
#define NACF_ADAP_BGN_THR  0.38 /* threshold signifying frame does            */
                               /* not have any voiced speech in it           */
                               /* so we might start to adapt thresholds      */
#define NACF_ADAP_BGN_THR_16  12452  /* NACF_ADAP_BGN_THR * (1 << 15)  */
#define NACF_SOLID_VOICED  0.5  /* threshold above which we are pretty sure   */
                               /* speech is present and thus SNRs can be     */
                               /* adjusted accordingly                       */
#define NACF_SOLID_VOICED_16  16384 
#define FULLRATE_BPS 14.4
#define HALFRATE_BPS 7.2
#define QUARTER_BPS 3.6
#define FULLRATE_BPS_16 29491   /* FULLRATE_BPS * (1 << 11) */
#define HALFRATE_BPS_16 14746    /* HALFRATE_BPS * (1 << 11) */
#define QUARTER_BPS_16 7373     /* QUARTER_BPS * (1 << 11) */
#define HIGH_BND_SNR  25		/*14.0*/  /* in db */

#define LOW_BND_SNR 6		  /* in db */
#define THR_NACF_BUMPUP 0.4   /* if nacf is lower than this and not declared*/
                             /* unvoiced then bump up rate                 */
#define THR_NACF_BUMPUP_16 13107   /*THR_NACF_BUMPUP * (1 << 15) */
#define THR_PRED_GAIN 5     /* in db, cc:08-23-94 */
#define THR_PRED_GAIN_6Q 320	/* 5 * (1 << 6) */

#define HIGH_THRESH_LIM  5059644*16

#define DECIMATE_F 2
#define DEC_ORDER 17
#define FIR_UNV_LEN 21
#define FSIZE_DIV_DECIMATE_F  80
#define PMAX_DIV_DECIMATE_F   60
#define PMIN_DIV_DECIMATE_F   10

extern int   hangover[TLEVELS]; 
extern short dis_coef[5]; 
extern short THRESH_SNR[FREQBANDS][TLEVELS][2];
extern short LOWEST_LEVEL[FREQBANDS];
extern short rate_filt[FREQBANDS][FILTERORDER];
extern short decimate_filter[DEC_ORDER];
extern short unv_filter[FIR_UNV_LEN];
extern short initlsp[LPCORDER];
extern short grid[GRID_POINT + 1];
extern short CosineTable_16[COSTABLE_LEN];
extern short SinineTable_16[COSTABLE_LEN];
extern short AntiCosTable_16[COSTABLE_LEN + 1];

#endif //__CODERATE_H__

