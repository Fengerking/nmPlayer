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
/*  Module:     proto.h                                                 */
#ifndef  _PROTO_H_
#define  _PROTO_H_

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include	"macro.h"
#include	"d_globs.h"
#include	"e_globs.h"

/*======================================================================*/
/*         ..CELP Function definitions.                                 */
/*----------------------------------------------------------------------*/
extern void lpcanalys(
					  short *pc, 
					  short *rc, 
					  short *input, 
					  short order, 
					  short len,
					  long  *R);

extern void weight(
				   short *awght, 
				   short *a, 
				   short gammma, 
				   short order);

extern void ZeroInput(
					  ERVC_ENC_OBJ *evrc_encobj,
					  short *coef_uq, 
					  short *coef, 
					  short *in, 
					  short gamma1, 
					  short gamma2, 
					  short order, 
					  short length, 
					  short type);

extern void ImpulseRzp(
					   short *output, 
					   short *coef_uq, 
					   short *coef, 
					   short gamma1, 
					   short gamma2, 
					   short order, 
					   short length);

#if 1
extern void ImpulseRzp1(
						short *output, 
						short *coef_uq, 
						short *coef, 
						short length);

extern void ImpulseRzp2(
						short *output, 
						short *coef_uq, 
						short *coef, 
						short length);
#endif

#ifdef ASM_OPT
extern void ImpulseRzp1_asm(
						short *output, 
						short *coef_uq, 
						short *coef, 
						short length);
#endif

extern void SynthesisFilter(
							short *output, 
							short *input, 
							short *coef, 
							short *memory, 
							short order, 
							short length);

extern void autocorrelation(
							long  *r, 
							short *r_scale,
							short *winput, 
							short len, 
							short order);

extern void durbin(
				   long  *r, 
				   short *rc, 
				   short *pc, 
				   short order);

extern void a2lsp(
				  short *freq, 
				  short *a);

extern void lsp2a(
				  short *a, 
				  short *freq);

extern void Interpol(
					 short *Lar, 
					 short *last, 
					 short *current, 
					 short SubNum, 
					 short order);

extern void Interpol_delay(
						   long  *out, 
						   short *last, 
						   short *current, 
						   short SubNum);

extern void acb_excitation(
						   short *Ex1, 
						   short gain, 
						   long  *delay3, 
						   short *PitchMemory, 
						   short length);

extern void Bitpack(
					short in, 
					unsigned short *TrWords, 
					short NoOfBits, 
					short *ptr);

extern void BitUnpack(
					  short *out, 
					  unsigned short *RecWords, 
					  short NoOfBits, 
					  short *ptr);

/*======================================================================*/
/*         ..RCELP routines.                                            */
/*----------------------------------------------------------------------*/

#ifdef ASM_OPT
extern void GetResidual_asm(
						short *residual, 
						short *input, 
						short *coef, 
						short *mem, 
						short order, 
						short length);
#endif

extern void GetResidual(
						short *residual, 
						short *input, 
						short *coef, 
						short *mem, 
						short order, 
						short length);

extern void fndppf(
				   ERVC_ENC_OBJ *evrc_encobj,
				   short *delay, 
				   short *beta, 
				   short *buf, 
				   short dmin, 
				   short dmax, 
				   short length);

extern void ComputeACB(
					   ERVC_ENC_OBJ *evrc_encobj,
					   short *residualm, 
					   short *excitation, 
					   long  *delay, 
					   short *residual, 
					   short guard, 
					   short *dpm, 
					   short *accshift, 
					   short beta, 
					   short length, 
					   short rshift);

extern void putacbc(
					short *exctation, 
					short *input, 
					short dpl, 
					short subframel, 
					short extra, 
					long  *delay3, 
					short freq, 
					short prec);

extern void mod(
				ERVC_ENC_OBJ *evrc_encobj,
				short *residualm, 
				short *accshift, 
				short beta, 
				short shiftr, 
				short resolution, 
				short *exctation, 
				short *Dresidual, 
				short *residual, 
				short guard, 
				short *dpm, 
				long  delay, 
				short subframel, 
				short extra);

extern void cshiftframe(
						short *sfstart, 
						short *sfend, 
						short *maxshift2, 
						short dpm, 
						short *residual, 
						short guard, 
						short accshift, 
						short maxshift, 
						long  delay, 
						short subframel, 
						short extra);

extern void maxeloc(
					short *maxloc, 
					long  *maxener, 
					short *signal, 
					short dp, 
					short length, 
					short ewl);

extern void modifyorig(
					   ERVC_ENC_OBJ *evrc_encobj,
					   short *residualm, 
					   short *accshift, 
					   short beta, 
					   short *dpm, 
					   short shiftrange, 
					   short resolution, 
					   short *TARGET, 
					   short *residual, 
					   short dp, 
					   short sfend);
#ifdef ASM_OPT
extern void bl_intrp_asm(
					 short *output, 
					 short *input, 
					 long  delay, 
					 short factor, 
					 short fl);
#endif

extern void bl_intrp(
					 short *output, 
					 short *input, 
					 long  delay, 
					 short factor, 
					 short fl);

extern void getgain(
					short *exctation, 
					short *lambda, 
					short *H, 
					short *idxcb, 
					short *gcb, 
					short *gcb_mid, 
					short gcb_size, 
					short Quantize, 
					short *mresidual, 
					short subframel, 
					short hlength);
#ifdef ASM_OPT
extern void ConIR_asm( 
					  short *out,
					  short *in, 
					  short *H, 
					  short length);
#else
extern void ConvolveImpulseR( 
							 short *out,
							 short *in, 
							 short *H, 
							 short length);
#endif

extern void PickToAverage(
						  short *res, 
						  short *signal, 
						  short length);

extern void lspmaq_dec(
					   short ndim, 
					   short kdim, 
					   short many, 
					   short *nsub, 
					   short *nsiz, 
					   short *y, 
					   short *index, 
					   short br, 
					   short *);

extern void lspmaq(
				   short *x, 
				   short ndim, 
				   short kdim, 
				   short many, 
				   short *nsub, 
				   short *nsiz, 
				   short alp, 
				   short *y, 
				   short *index, 
				   short br, 
				   short *);

extern void lspmaq1(short *x, 
					short many, 
					short *nsub, 
					short *nsiz, 
					short *y, 
					short *index, 
					short *lsptab);

extern void Weight2Res(
					   short *, 
					   short *, 
					   short *, 
					   short *, 
					   short, 
					   short, 
					   short, 
					   short);



#ifdef ASM_OPT
extern void fir_asm(
				short *output, 
				short *input, 
				short *coef, 
				short *FIRmemory, 
				short order, 
				short length);

extern void iir_asm(
				short *output, 
				short *input, 
				short *coef, 
				short *IIRmemory, 
				short order, 
				short length);
#else
extern void fir(
				short *output, 
				short *input, 
				short *coef, 
				short *FIRmemory, 
				short order, 
				short length);

#endif

extern void iir(
				short *output, 
				short *input, 
				short *coef, 
				short *IIRmemory, 
				short order, 
				short length);

//extern void apf(
//    short *, 
//    short *, 
//    short *, 
//    long, 
//    short, 
//    short, 
//    short, 
//    short, 
//    short, 
//    short, 
//    short, 
//     short);

extern void GetExc800bps(
						 ERVC_COMPONENT *evrc_ecom,
						 short *output, 
						 short *best, 
						 short scale, 
						 short *input, 
						 short length, 
						 short flag, 
						 short n);

extern void GetExc800bps_dec(
							 ERVC_COMPONENT *evrc_ecom,
							 short *output, 
							 short length, 
							 short best, 
							 short flag, 
							 short n, 
							 short fer_flag);

extern short  ran_g(
					long *i_seed,
					long *g_seed,
					short *seed0);

extern void fcb_gainq(
					  short *index,
					  short *gain_q,
					  long l_gain_nq,
					  short *gain_tab,
					  short gain_size);


#endif
