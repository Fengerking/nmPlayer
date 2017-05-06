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
/*  Module:     e_globs.c                                               */
/*----------------------------------------------------------------------*/
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  1202                                        */
/*      Stack/Local RAM:    0                                           */
/*----------------------------------------------------------------------*/
#ifndef _E_GLOBS_H__
#define _E_GLOBS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include  "macro.h"
#include  "globs.h"
#include  "rda.h"
#include  "stream.h"

extern void  *g_hEVRCEncInst;
/*======================================================================*/
/*         ..Globals (encoder).                                         */
/*----------------------------------------------------------------------*/

typedef struct ERVC_ENC_OBJ_TAG{
	short   HPspeech[FrameSize+GUARD*2];     /* orig. speech signal - memory overlap with residual */
	short   ConstHPspeech[GUARD*2];  /* temporary buffer to store HPspech  */

	short   OldlspE[ORDER];      /* Last frame quantized lsp                */

	short   lsp_nq[ORDER];       /* Correlation coefficients                */
	short   Oldlsp_nq[ORDER];    /* Last frame quantized lsp                */
	short   lspi_nq[ORDER];      /* Interpolation of correlation coeff      */
	short   pci_nq[ORDER];       /* Interpolated prediction coefficients    */
	short   wpci[ORDER];     /* Interpolated weighted prediction coefficients */
	short	zero_memA[ORDER];
	short	zero_memA1[ORDER];
	short	zero_memA2[ORDER];	/* filter memory */
	
	short	mdf_a1[RRESOLUTION];
	short	mdf_a2[RRESOLUTION];
	short	mdf_a3[RRESOLUTION];

	short   Excitation[ACBMemSize + SubFrameSize + EXTRA];
	short	acb_buffer[ACBMemSize + SubFrameSize + EXTRA];

	short   H[Hlength+1];        /* Impulse response [Hlength]              */
	short   HtH[Hlength+1];      /* Impulse response ^2 [Hlength]           */
	short   SynMemoryM[ORDER];   /* weighted speech synthesis filter memory */

	short   TARGET[SubFrameSize+1];  /* Residual - Zero input response       */
	short   TARGETw[SubFrameSize+1];

	short   WFmemFIR[ORDER]; /* Weighting filter memory                 */
	short   WFmemIIR[ORDER]; /* Weighting filter memory                 */

	short   zir[SubFrameSize];   /* Zero Input Response (can share memory w/ HtH)*/

	short   residual[2 * GUARD + FrameSize + 10];
	short   residualm[SubFrameSize + EXTRA];

	short  bq_xsave[BQ_N_FILTERS*BQ_N_SAVE];
	long   bq_ysave[BQ_N_FILTERS*BQ_N_SAVE];

	short   origm[SubFrameSize];
	short   *worigm;      /* shared weighted original memory */

	short  *ExconvH; /* Excitation of adaptive codebook convolved with H (uses Scratch)*/
	long    encode_fcnt;     /* Frame counter */

	short	fnd_DECbuf[FrameSize >> 2];
	short   y2[55];                 /* Filtered innovative vector (debug only) */
	short	fnd_memory[3];
	short	fnd_lastgoodpitch;
	short	fnd_lastbeta;	

	short	ns_first;
	short	ns_pre_emp_mem;
	short	ns_de_emp_mem;
	short	ns_overlap[FFT_LEN - FRM_LEN];
	short	ns_ch_gain[FFT_LEN >> 1];	/* scaled as 0,15 */
	short	ns_update_cnt;
	short	ns_window_overlap[DELAY];
	short	ns_hyster_cnt;
	short	ns_last_update_cnt;
	short	ns_ch_enrg_long_db[NUM_CHAN];		/* scaled as 10,5  */

	long	ns_ch_enrg[NUM_CHAN];	/* scaled as 23,8 */
	long	ns_ch_noise[NUM_CHAN];		/* scaled as 15,16 (change to 23,8) */
	long	ns_frame_cnt;

	short	ns_last_normb_shift;		/* last block norm shift count */

	short   accshift;
	short   delay1;
	short   pdelay;
	short   beta, beta1;
	short   dpm;

	short   LPCgain;     /* used for frame erasures */
	short   shiftSTATE;
	short   lastrateE;       /* last bitrate used for encoder*/

	short	min_rate;
    short	max_rate;
	short	ns_flag;

//extern short   fcbIndexVector[10];     /* ACELP fixed codebook index vector */
	short   fcbGain;                /* ACELP fixed codebook gain */
	short   set_rate;
}ERVC_ENC_OBJ;

typedef struct EVRC_ENC_COMPONENT_TAG {
	ERVC_COMPONENT	  *evrc_com;
	ERVC_ENC_OBJ	  *evrc_encobj;
	ENCODER_MEM		  *evrc_emem;
	FrameStream       *stream;
	VO_MEM_OPERATOR   *pvoMemop;
	VO_MEM_OPERATOR   voMemoprator;
	VO_PTR            hCheck;
}EVRC_ENC_COMPONENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  //_E_GLOBS_H__

