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
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"
#include "macro.h"
#include "basic_op.h"
#include "proto.h"
#include "e_globs.h"
#include "globs.h"
#include "rom.h"
//#include "tty.h"

/*======================================================================*/
/*         ..Get beta, et al.                                           */
/*----------------------------------------------------------------------*/
short pre_encode(
					EVRC_ENC_COMPONENT *evrc_ecom,
					short *inFbuf,
					long *Rs
)
{

	/*....(local) variables.... */
	register int i, j;		/* counters */
	ERVC_COMPONENT	*pcom;
	ERVC_ENC_OBJ	*pencobj;
	long Lsum1;
	long Ltemp;
	short *HPspeech;
	short *ConstHPspeech;
	short *H;
	short temp;
	short S_fac;

	pcom = evrc_ecom->evrc_com;
	pencobj = evrc_ecom->evrc_encobj;
	HPspeech = pencobj->HPspeech;
	ConstHPspeech = pencobj->ConstHPspeech;
	H = pencobj->H;

	/*....execute.... */
	/* Make sure HPspeech -> HPspeech+2*GUARD have the right memory */
	for (j = 0; j < GUARD * 2; j++)
		HPspeech[j] = ConstHPspeech[j];

	/* Process Data */
	for (j = 0; j < FrameSize; j++)
	{
		HPspeech[j + GUARD * 2] = inFbuf[j];
		ConstHPspeech[j] = inFbuf[j];
	}

	/* Calculate prediction coefficients */
	/* reflection coef. not needed-returned in Scratch */
	lpcanalys(pcom->pci, pcom->Scratch, HPspeech + 2 * GUARD, ORDER, FrameSize, Rs);

	/* pci divided by 8 */
	/* Calculate impulse response of 1/A(z) Note: only one IIR filter */
	ImpulseRzp2(H, pcom->pci, pcom->pci, Hlength);
	//ImpulseRzp(H, pcom->pci, pcom->pci, 0x7fff, 0x7fff, ORDER, Hlength);

	/* H divided by 16 */
	/* Get energy of H */
    S_fac = 4;
	for (j = 0, Lsum1 = 0; j < Hlength; j++)
	{
		Ltemp = ((L_mult(H[j], H[j]) >> S_fac));
		Lsum1 = L_add(Lsum1, Ltemp);
	}

	/* determine spectral transistion degree (set flag if large) -- for frame erasures */
	for (j = 0, temp = round32(Lsum1); j < 10; j++)
		temp = sub(temp, pencobj->LPCgain);
	if (temp > 0)
		pcom->LPCflag = 1;
	else
		pcom->LPCflag = 0;

	pencobj->LPCgain = round32(Lsum1);

	/* Bandwidth expansion */
	weight(pcom->pci, pcom->pci, _Gamma_4, ORDER);

	/* Convert prediction coefficients to lsp */
	a2lsp(pencobj->lsp_nq, pcom->pci);

	/* Get residual signal */
	for (j = 0; j < ORDER; j++)
		pcom->Scratch[j] = 0;			/* Scratch is used as filter memory */

	lsp2a(pencobj->pci_nq, pencobj->Oldlsp_nq);

#ifdef ASM_OPT
	GetResidual_asm(pencobj->residual, HPspeech, pencobj->pci_nq, pcom->Scratch, ORDER, GUARD);
#else
	GetResidual(pencobj->residual, HPspeech, pencobj->pci_nq, pcom->Scratch, ORDER, GUARD);
#endif

	for (i = 0; i < NoOfSubFrames; i++)
	{
		Interpol(pencobj->lspi_nq, pencobj->Oldlsp_nq, pencobj->lsp_nq, i, ORDER);
		lsp2a(pencobj->pci_nq, pencobj->lspi_nq);

		if (i < 2)
#ifdef ASM_OPT
			GetResidual_asm(pencobj->residual + i * (SubFrameSize - 1) + GUARD,
						HPspeech + i * (SubFrameSize - 1) + GUARD, pencobj->pci_nq, pcom->Scratch, ORDER,
						SubFrameSize - 1);
#else
			GetResidual(pencobj->residual + i * (SubFrameSize - 1) + GUARD,
						HPspeech + i * (SubFrameSize - 1) + GUARD, pencobj->pci_nq, pcom->Scratch, ORDER,
						SubFrameSize - 1);
#endif
		else
#ifdef ASM_OPT
			GetResidual_asm(pencobj->residual + i * (SubFrameSize - 1) + GUARD,
						HPspeech + i * (SubFrameSize - 1) + GUARD, pencobj->pci_nq, pcom->Scratch, ORDER,
						SubFrameSize);
#else
			GetResidual(pencobj->residual + i * (SubFrameSize - 1) + GUARD,
						HPspeech + i * (SubFrameSize - 1) + GUARD, pencobj->pci_nq, pcom->Scratch, ORDER,
						SubFrameSize);
#endif
	}

#ifdef ASM_OPT
	GetResidual_asm(pencobj->residual + FrameSize + GUARD, HPspeech + FrameSize + GUARD, pcom->pci,
				pcom->Scratch, ORDER, GUARD);
#else
	GetResidual(pencobj->residual + FrameSize + GUARD, HPspeech + FrameSize + GUARD, pcom->pci,
				pcom->Scratch, ORDER, GUARD);
#endif

	/* Calculate pitch period at the end of the frame, use n.q. lpc coef. */
    fndppf(pencobj, &pencobj->delay1, &pencobj->beta1, pencobj->residual + GUARD, DMIN, DMAX, FrameSize);
    fndppf(pencobj, &pcom->delay, &pencobj->beta, pencobj->residual + 2 * GUARD, DMIN, DMAX, FrameSize);

	if (sub(pencobj->beta1, 13107) > pencobj->beta)

	{
		if (abs_s(sub(pcom->delay, pencobj->delay1)) > 15)
		{
			pencobj->beta = pencobj->beta1;
			pcom->delay = pencobj->delay1;
		}
		else
		{
			pencobj->beta = (add(pencobj->beta, pencobj->beta1) >> 1);
			pcom->delay = (add(pcom->delay, pencobj->delay1) >> 1);
		}
	}
    return (pencobj->beta);
}
