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
/* EVRC Encoder */
/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include "basic_op.h"
#include "math_ext32.h"
#include "math_adv.h"
//#include "mathadv.h"
//#include "mathevrc.h"
//#include "mathdp31.h"

#include  "e_globs.h"
#include  "macro.h"
#include  "proto.h"
#include  "rom.h"
#include  "acelp_pf.h"          /* for ACELP fixed codebook */
//#include  "tty.h"

/*======================================================================*/
/*         ..Reset RCELP encode rate.                                   */
/*----------------------------------------------------------------------*/
void  SetEncodeRate(EVRC_ENC_COMPONENT* evrc_ecom, short rate)
{

	/*....execute.... */
	ERVC_COMPONENT *pcom;
	
	pcom = evrc_ecom->evrc_com;

	pcom->bit_rate = rate;

	switch (pcom->bit_rate)
	{
	case 1:
		pcom->FCBGainSize = 16;		/*...use half-rate... */
		pcom->gnvq = gnvq_4;
		break;
	case 3:
		pcom->nsub = nsub22;
		pcom->nsize = nsize22;
		pcom->lognsize = lognsize22;
		pcom->lsptab = lsptab22;
		pcom->knum = 3;
		pcom->FCBGainSize = 16;
		pcom->gnvq = gnvq_4;
		break;
	case 4:
		pcom->nsub = nsub28;
		pcom->nsize = nsize28;
		pcom->lognsize = lognsize28;
		pcom->lsptab = lsptab28;
		pcom->knum = 4;
		pcom->FCBGainSize = 32;
		pcom->gnvq = gnvq_8;
		break;
	}
}

/*======================================================================*/
/*         ..Reset Encoder parameters.                                  */
/*----------------------------------------------------------------------*/
void InitEncoder(EVRC_ENC_COMPONENT* evrc_ecom)
{
	/*....(local) variables.... */
	ERVC_ENC_OBJ *penobj;
	ERVC_COMPONENT *pcom;
	int j;
	
	penobj = evrc_ecom->evrc_encobj;
	pcom = evrc_ecom->evrc_com;
	/****************************************************/
	/*         Algorithm (one time) initializations     */
	/****************************************************/

	SetEncodeRate(evrc_ecom, 4);
	
	pcom->ran_g_iset = 0;
	pcom->ran_g_gset = 0;
	pcom->e_ran_g_iset = 0;
	pcom->e_ran_g_gset = 0;

	pcom->ext_Seed = 1234;
	pcom->ext_PrevBest = 0;

	penobj->encode_fcnt = 0;
	penobj->worigm = penobj->origm;
	penobj->ExconvH = evrc_ecom->evrc_com->Scratch;

	for (j = 0; j < ORDER; j++)
		penobj->SynMemoryM[j] = 0;

	penobj->OldlspE[0] = 1573;
	penobj->OldlspE[1] = 3146;
	penobj->OldlspE[2] = 4719;
	penobj->OldlspE[3] = 6291;
	penobj->OldlspE[4] = 7864;
	penobj->OldlspE[5] = 9437;
	penobj->OldlspE[6] = 11010;
	penobj->OldlspE[7] = 12583;
	penobj->OldlspE[8] = 14156;
	penobj->OldlspE[9] = 15729;

	penobj->Oldlsp_nq[0] = 1573;
	penobj->Oldlsp_nq[1] = 3146;
	penobj->Oldlsp_nq[2] = 4719;
	penobj->Oldlsp_nq[3] = 6291;
	penobj->Oldlsp_nq[4] = 7864;
	penobj->Oldlsp_nq[5] = 9437;
	penobj->Oldlsp_nq[6] = 11010;
	penobj->Oldlsp_nq[7] = 12583;
	penobj->Oldlsp_nq[8] = 14156;
	penobj->Oldlsp_nq[9] = 15729;

	for (j = 0; j < 10; j++)
		penobj->residual[j] = 0;

	for (j = 0; j < GUARD*2; j++)
		penobj->ConstHPspeech[j] = 0;

	for (j = 0; j < ORDER; j++)
	{
		penobj->WFmemIIR[j] = penobj->WFmemFIR[j] = 0;
		penobj->zero_memA[j] = penobj->zero_memA1[j] =
		penobj->zero_memA2[j] = 0;		
	}

	for (j = 0; j < ACBMemSize + SubFrameSize + EXTRA; j++)
	{
		penobj->Excitation[j] = 0;
		penobj->acb_buffer[j] = 0;
	}

	penobj->accshift = 0;
	penobj->shiftSTATE = 0;
	penobj->dpm = 0;
	penobj->pdelay = 40;

    penobj->LPCgain = 0x0008;

	for (j = 0; j < FrameSize >> 2; j++)
	{
		penobj->fnd_DECbuf[j] = 0;
	}
	penobj->fnd_memory[0] = penobj->fnd_memory[1]
		= penobj->fnd_memory[2] = 0;

	penobj->fnd_lastgoodpitch = 0;
	penobj->fnd_lastbeta = 0;

	penobj->mdf_a1[0] = 12288;
	penobj->mdf_a1[1] = 8448;
	penobj->mdf_a1[2] = 5120;
	penobj->mdf_a1[3] = 2304;
	penobj->mdf_a1[4] = 0;
	penobj->mdf_a1[5] = -1792;
	penobj->mdf_a1[6] = -3072;
	penobj->mdf_a1[7] = -3840;

	penobj->mdf_a2[0] = 24576;
	penobj->mdf_a2[1] = 28160;
	penobj->mdf_a2[2] = 30720;
	penobj->mdf_a2[3] = 32256;
	penobj->mdf_a2[4] = 32767;
	penobj->mdf_a2[5] = 32256;
	penobj->mdf_a2[6] = 30720;
	penobj->mdf_a2[7] = 28160;

	penobj->mdf_a3[0] = -4096;
	penobj->mdf_a3[1] = -3840;
	penobj->mdf_a3[2] = -3072;
	penobj->mdf_a3[3] = -1792;
	penobj->mdf_a3[4] = 0;
	penobj->mdf_a3[5] = 2304;
	penobj->mdf_a3[6] = 5120;
	penobj->mdf_a3[7] = 8448;

//ns127.c
	penobj->ns_ch_gain[0] = penobj->ns_ch_gain[1] = SW_MAX;
	for (j = LO_CHAN; j <= HI_CHAN; j++)
		penobj->ns_ch_enrg[j] = 0;
	for (j = 0; j < DELAY; j++)
		penobj->ns_window_overlap[j] = 0;
	for (j = 0; j < FFT_LEN - FRM_LEN; j++)
		penobj->ns_overlap[j] = 0;
	penobj->ns_pre_emp_mem = 0;
	penobj->ns_de_emp_mem = 0;
	penobj->ns_update_cnt = 0;
	penobj->ns_frame_cnt = 0;
}

/*======================================================================*/
/*         ..Encode speech data.                                        */
/*----------------------------------------------------------------------*/
void encode(
			EVRC_ENC_COMPONENT* evrc_ecom,
			short rate,
			unsigned char *codeBuf
			)
{
#if (FUNC_ENCODE_OPT)
	/*....(local) variables.... */
	register int i, j, n;
	ERVC_COMPONENT	*pcom;
	ERVC_ENC_OBJ	*pencobj;
	long delayi[3];
	long lsum1;
	long l_fcbGain;
	short *PackedWords;
	short *PackWdsPtr;
	short *SScratch, *H;
	short *Excitation, *worigm;
	short subframesize;
	short Aveidxppg;
	short sum1;	
	int   shft_fctr;
	
	pcom = evrc_ecom->evrc_com;
	pencobj = evrc_ecom->evrc_encobj;
	PackedWords = pcom->PackedWords;
	PackWdsPtr = pcom->PackWdsPtr;
	SScratch = pcom->SScratch;
	Excitation = pencobj->Excitation; 
	H = pencobj->H;
	worigm = pencobj->worigm;

	/* Re-initialize PackWdsPtr */
	PackWdsPtr[0] = 16;
	PackWdsPtr[1] = 0;
	for (i = 0; i < PACKWDSNUM; i++)
		PackedWords[i] = 0;

	/*....execute.... */
	SetEncodeRate(evrc_ecom, rate);

	/* Handle 800bps seperatly */
	if (pcom->bit_rate == 1)
	{
		/* Reset accumulated shift */
		pencobj->accshift = 0;
		pencobj->dpm = 0;
#if 1
		lspmaq1(pencobj->lsp_nq, 2, nsub8, nsize8, pcom->lsp, SScratch, lsptab8);
#else
		lspmaq(pencobj->lsp_nq, ORDER, 1, 2, nsub8, nsize8, 16384, pcom->lsp, SScratch, pcom->bit_rate, lsptab8);
#endif

		/* Bit-pack the quantization indices */
		for (i = 0; i < 2; i++)
			Bitpack(SScratch[i], (unsigned short *) PackedWords, lognsize8[i], PackWdsPtr);

		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
				subframesize = SubFrameSize - 1;
			else
				subframesize = SubFrameSize;

			/* interpolate lsp */
			Interpol(pcom->lspi, pencobj->OldlspE, pcom->lsp, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pcom->pci, pcom->lspi);

			/* Get lpc gain */
			/* Calculate impulse response of 1/A(z) */
			//ImpulseRzp(H, pcom->pci, pcom->pci, 0x7fff, 0x7fff, ORDER, Hlength);
			ImpulseRzp2(H, pcom->pci, pcom->pci, Hlength);
			/* Get energy of H */
			lsum1 = 0;
			for (j = 0; j < subframesize; j++)
			{
				lsum1 += H[j] * H[j];
			}
			lsum1 <<=1 ;

			shft_fctr = norm_l(lsum1);
			lsum1 <<= shft_fctr;
			sum1 = sqroot(lsum1);

			if (shft_fctr & 0x0001)
			{
				sum1 = (short)((long)sum1 * 23170 >> 15);
			}

			shft_fctr >>= 1;
			//following can't be modified
			sum1 = shr(sum1, (short)(shft_fctr - 1));

			if (pencobj->lastrateE != 1 && i == 0 && pencobj->encode_fcnt == 0)
				j = 0;			/* Reset seed */
			else
				j = 1;
			GetExc800bps(pcom, Excitation, &pcom->idxcbg, sum1, 
					pencobj->residual + GUARD + i * (SubFrameSize - 1), subframesize, j, i);

			/*...another puff fix... */
			ZeroInput(pencobj, pcom->pci, pcom->pci, Excitation + ACBMemSize - subframesize, 
				GAMMA1, GAMMA2, ORDER, subframesize, 1);
		}

        /* Trap for all ones output packet. */
        /* If rate 1/8 packet is all ones, flip msb of gain code */
        /* (encoder shall never generate an all ones rate 1/8 packet..*/
        if ((SScratch[0]&SScratch[1]&0xf)==0xf && (pcom->idxcbg==0xff))
		{
			/* Clear Frame Energy Gain MSB if output packet == all ones */
			pcom->idxcbg=0x7f;  
        }
        Bitpack(pcom->idxcbg, (unsigned short *) PackedWords, 8, PackWdsPtr);

	}
	else
	{							/* bit rates 4 or 8kbps */
		/* Send indication on the spectrum transition */
		if (pcom->bit_rate == 4)
			Bitpack(pcom->LPCflag, (unsigned short *) PackedWords, 1, PackWdsPtr);

		/* Quantize the LSPs */
#if 1
        lspmaq1(pencobj->lsp_nq, pcom->knum, pcom->nsub, pcom->nsize, pcom->lsp, SScratch,pcom->lsptab);
#else
		lspmaq(pencobj->lsp_nq, ORDER, 1, pcom->knum, pcom->nsub, pcom->nsize, 16384, pcom->lsp, SScratch, pcom->bit_rate, pcom->lsptab);
#endif
		/* Bit-pack the quantization indices */

		for (i = 0; i < pcom->knum; i++)
			Bitpack(SScratch[i], (unsigned short *) PackedWords,
					pcom->lognsize[i], PackWdsPtr);

		/* Update shiftSTATE with hysteresis */
		if (pencobj->beta < 3277)
		{
			pencobj->accshift = 0;
			pencobj->dpm = 0;
			pencobj->shiftSTATE = 0;
		}

		if (pencobj->accshift > 5120)
			pencobj->shiftSTATE = -1;
		if (pencobj->accshift < -5120)
			pencobj->shiftSTATE = 1;
		if (pencobj->accshift <= 2560 && pencobj->shiftSTATE == -1)
			pencobj->shiftSTATE = 0;
		if (pencobj->accshift >= -2560 && pencobj->shiftSTATE == 1)
			pencobj->shiftSTATE = 0;

		/* Control accshift */
		if (pencobj->shiftSTATE == 1 && pencobj->beta < 13107)
		{
			pcom->delay++;
		}
		else if (pencobj->shiftSTATE == -1 && pencobj->beta < 13107)
		{
			pcom->delay--;
		}
		if (pcom->delay > DMAX)
			pcom->delay = DMAX;
		if (pcom->delay < DMIN)
			pcom->delay = DMIN;

        Bitpack((short)(pcom->delay - DMIN), (unsigned short *) PackedWords, 7, PackWdsPtr);

		if (pcom->bit_rate == 4)
		{
			j = pcom->delay - pencobj->pdelay;
			if (abs(j) > 15)
				j = 0;
			else
				j += 16;

			Bitpack(j, (unsigned short *) PackedWords, 5, PackWdsPtr);
		}

		/* Smooth interpolation if the difference between delays is too big */
		if (abs_s((pcom->delay - pencobj->pdelay)) > 15)
			pencobj->pdelay = pcom->delay;

		Aveidxppg = 0;
		/*********************************
        * CELP codebook search procedure *
		*********************************/
		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
			{
				subframesize = SubFrameSize - 1;
			}
			else
			{
				subframesize = SubFrameSize;
			}

			/* interpolate lsp */
			Interpol(pcom->lspi, pencobj->OldlspE, pcom->lsp, i, ORDER);
			Interpol(pencobj->lspi_nq, pencobj->Oldlsp_nq, pencobj->lsp_nq, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pcom->pci, pcom->lspi);
			lsp2a(pencobj->pci_nq, pencobj->lspi_nq);

			/* Get zir */

			ZeroInput(pencobj, pencobj->pci_nq, pcom->pci, 
					Excitation + ACBMemSize, GAMMA1, GAMMA2, ORDER, subframesize, 0);

			/* Calculate impulse response of 1/A(z) * A(z/g1) / A(z/g2) */
#ifdef ARMV6
            ImpulseRzp1_asm(H, pencobj->pci_nq, pcom->pci, Hlength);
#else
			ImpulseRzp1(H, pencobj->pci_nq, pcom->pci, Hlength);
#endif
			//ImpulseRzp(H, pencobj->pci_nq, pcom->pci, GAMMA1, GAMMA2, ORDER, Hlength);

			/* Interpolate delay */
			Interpol_delay(delayi, &pencobj->pdelay, &pcom->delay, i);

			ComputeACB(pencobj,	pencobj->residualm, Excitation + ACBMemSize, delayi,
					   pencobj->residual + GUARD + i * (SubFrameSize - 1),
					   (short)(FrameSize + GUARD - i * (SubFrameSize - 1)), &pencobj->dpm,
					   &pencobj->accshift, pencobj->beta, subframesize, RSHIFT);

			/* Get weighted speech */
			/* ORIGM */

			SynthesisFilter(pencobj->origm, pencobj->residualm, pencobj->pci_nq, pencobj->SynMemoryM, ORDER, subframesize);

			/* Weighting filter */
			weight(pencobj->wpci, pencobj->pci_nq, GAMMA1, ORDER);

#ifdef  ASM_OPT
			fir_asm(pcom->Scratch, pencobj->origm, pencobj->wpci, pencobj->WFmemFIR, ORDER, subframesize);
#else
			fir(pcom->Scratch, pencobj->origm, pencobj->wpci, pencobj->WFmemFIR, ORDER, subframesize);
#endif

			weight(pencobj->wpci, pencobj->pci_nq, GAMMA2, ORDER);
#ifdef ASM_OPT
			iir_asm(worigm, pcom->Scratch, pencobj->wpci, pencobj->WFmemIIR, ORDER, subframesize);
#else
			iir(worigm, pcom->Scratch, pencobj->wpci, pencobj->WFmemIIR, ORDER, subframesize);
#endif

			/* Remove Zero input response from weighted speech */
			for (j = 0; j < subframesize; j++)
			{
				worigm[j] -= pencobj->zir[j];
			}

			/* Calculate closed loop gain */
			getgain(Excitation + ACBMemSize, &sum1, H, &pcom->idxppg, 
				ppvq, ppvq_mid, ACBGainSize, 1, worigm, subframesize, Hlength);

			Aveidxppg += pcom->idxppg;

			/* Get TARGET for fixed c.b. */
			/* Convolve excitation with H */
			/* ExconvH stored in Scratch memory */
#ifdef ASM_OPT
            ConIR_asm(pencobj->ExconvH, Excitation + ACBMemSize, H, subframesize);
#else
			ConvolveImpulseR(pencobj->ExconvH, Excitation + ACBMemSize, H, subframesize);
#endif

			for (j = 0; j < subframesize; j++)
			{
				pencobj->TARGETw[j] = worigm[j] - pencobj->ExconvH[j];
			}

			/* Convert TARGET from weighted domain to residual domain */
			Weight2Res(pencobj->TARGET, pencobj->TARGETw, pencobj->pci_nq, pcom->pci, GAMMA1, GAMMA2, ORDER, SubFrameSize);

			if (subframesize < SubFrameSize)
				pencobj->TARGETw[subframesize] = pencobj->TARGET[subframesize] = pcom->Scratch[subframesize] = 0;

			/* get delay for current subframe */
			n = (((delayi[1] + delayi[0]) >> 1) + 0x8000) >> 16;
			/* Compute fixed codebook contribution */
			if (n > subframesize)
				n = 200;

			/* ACELP fixed codebook search */
			if (pcom->bit_rate == 4)
			{

				ACELP_Code(pencobj->TARGETw, pencobj->TARGET, H, n, sum1, subframesize, pcom->Scratch,
						   &pencobj->fcbGain, pencobj->y2, pcom->fcbIndexVector, 1);

				/* constrain fcb gain */
				/* fcbGain *= (1.0 - ppvq[idxppg] * 0.15); */

                l_fcbGain = L_mpy_ls((2147483647 - (ppvq[pcom->idxppg] * 9830 << 1)), pencobj->fcbGain);
            }
			else
			{
                /** Processing half rate case  **/

				ACELP_Code(pencobj->TARGETw, pencobj->TARGET, H, n, sum1, subframesize, pcom->Scratch,
						   &pencobj->fcbGain, pencobj->y2, pcom->fcbIndexVector, 0);

				/* Constrain fcb gain */
				/* fcbGain *= (0.9 - ppvq[idxppg] * 0.1); */
				l_fcbGain = L_mpy_ls((1932735283 - (ppvq[pcom->idxppg] * 6554 << 1)), pencobj->fcbGain);
			}
			/* scale fcbGain +6db to account for 6db loss at input */
            l_fcbGain <<= 1;

			/* Quantize FCB Gain : */
			fcb_gainq(&pcom->idxcbg, &pencobj->fcbGain, l_fcbGain, pcom->gnvq, pcom->FCBGainSize);

			/* adjust fcbGain -6db */
            pencobj->fcbGain >>= 1;

			/* Add to total excitation */
			for (j = 0; j < subframesize; j++)
			{
				Excitation[j + ACBMemSize] += (short)((((long)pcom->Scratch[j] * pencobj->fcbGain) + 0x4000) >> 15);
			}

			/* Update filters memory */
			ZeroInput(pencobj, pencobj->pci_nq, pcom->pci, Excitation + ACBMemSize, GAMMA1, GAMMA2, ORDER, subframesize, 1);

            /* checking outputs */
			/* Update residualm */
			for (j = 0; j < pencobj->dpm; j++)
				pencobj->residualm[j] = pencobj->residualm[j + subframesize];

			/* Update excitation */
			for (j = 0; j < ACBMemSize; j++)
				Excitation[j] = Excitation[j + subframesize];

			/* Pack bits */
			/* ACB gain index */

			Bitpack(pcom->idxppg, (unsigned short *) PackedWords, 3, PackWdsPtr);

			if (pcom->bit_rate == 4)
			{
				pcom->idxcb = pcom->fcbIndexVector[0];
				Bitpack(pcom->idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->idxcb = pcom->fcbIndexVector[1];
				Bitpack(pcom->idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->idxcb = pcom->fcbIndexVector[2];
				Bitpack(pcom->idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->idxcb = pcom->fcbIndexVector[3];
				Bitpack(pcom->idxcb, (unsigned short *) PackedWords, 11, PackWdsPtr);
				/* FCB gain index */
				Bitpack(pcom->idxcbg, (unsigned short *) PackedWords, 5, PackWdsPtr);
			}
			else
			{
                /** Packing half-rate bits **/
				pcom->idxcb = pcom->fcbIndexVector[0];
				/* FCB shape index  10 bits */
				Bitpack(pcom->idxcb, (unsigned short *) PackedWords, 10, PackWdsPtr);
				/* FCB gain index */
				Bitpack(pcom->idxcbg, (unsigned short *) PackedWords, 4, PackWdsPtr);
			}

		}
		/* prevent overflow */
		if (Aveidxppg <= 1 && pcom->idxppg != 1)
		{
			pencobj->accshift = 0;
			pencobj->dpm = 0;
		}

		/* Update encoder variables */
		pencobj->pdelay = pcom->delay;

	}							/* Ends if for rate of 4 or 8kbps */

	pencobj->lastrateE = pcom->bit_rate;

    //shanrong chen modified
#ifdef  SHORT_SHIFT_ON
    for (i = 0; i < PACKWDSNUM; i++)
    {
        codeBuf[(i << 1) + 1] = (char)PackedWords[i];
        codeBuf[(i << 1)]	  = (char)(PackedWords[i] >> 8);
    }
#else
	for (i = 0; i < PACKWDSNUM; i++)
    {
        codeBuf[(i << 1)]     = (char)PackedWords[i];
        codeBuf[(i << 1) + 1] = (char)(PackedWords[i] >> 8);
    }
#endif
//	for (i = 0; i < PACKWDSNUM; i++)
//		codeBuf[i] = PackedWords[i];

	pencobj->encode_fcnt++;

#else

	/*....(local) variables.... */
	register short i, j, n;
	long delayi[3];
	short subframesize;
	short Aveidxppg;
	short sum1;
	long lsum1;
	short shft_fctr;
	long l_fcbGain;

	/* Re-initialize PackWdsPtr */
	PackWdsPtr[0] = 16;
	PackWdsPtr[1] = 0;
	for (i = 0; i < PACKWDSNUM; i++)
		PackedWords[i] = 0;

	/*....execute.... */
	SetEncodeRate(rate);

	/* Handle 800bps seperatly */
	if (bit_rate == 1)
	{
		/* Reset accumulated shift */
		accshift = 0;
		dpm = 0;

		lspmaq(lsp_nq, ORDER, 1, 2, nsub8, nsize8, 16384, lsp, SScratch, bit_rate, lsptab8);

		/* Bit-pack the quantization indices */
		for (i = 0; i < 2; i++)
			Bitpack(SScratch[i], (unsigned short *) PackedWords, lognsize8[i], PackWdsPtr);

		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
				subframesize = SubFrameSize - 1;
			else
				subframesize = SubFrameSize;

			/* interpolate lsp */
			Interpol(lspi, OldlspE, lsp, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pci, lspi);

			/* Get lpc gain */
			/* Calculate impulse response of 1/A(z) */
			ImpulseRzp(H, pci, pci, 0x7fff, 0x7fff, ORDER, Hlength);
			/* Get energy of H */
			lsum1 = 0;

			for (j = 0; j < subframesize; j++)
				lsum1 = L_mac(lsum1, H[j], H[j]);

			shft_fctr = norm_l(lsum1);
			lsum1 = L_shl(lsum1, shft_fctr);
			sum1 = sqroot(lsum1);

			if (shft_fctr & 0x0001)
				sum1 = mult(sum1, 23170);

			shft_fctr = shr(shft_fctr, 1);
			sum1 = shr(sum1, shft_fctr - 1);

			if (lastrateE != 1 && i == 0 && encode_fcnt == 0)
				j = 0;			/* Reset seed */
			else
				j = 1;
			GetExc800bps(Excitation, &idxcbg, sum1, residual + GUARD + i * (SubFrameSize - 1), subframesize, j, i);

			/*...another puff fix... */
			ZeroInput(zir, pci, pci, Excitation + ACBMemSize - subframesize, GAMMA1, GAMMA2, ORDER, subframesize, 1);
		}

        /* Trap for all ones output packet. */
        /* If rate 1/8 packet is all ones, flip msb of gain code */
        /* (encoder shall never generate an all ones rate 1/8 packet..*/
        if ((SScratch[0]&SScratch[1]&0xf)==0xf && (idxcbg==0xff))
		{
			/* Clear Frame Energy Gain MSB if output packet == all ones */
			idxcbg=0x7f;  
        }
        Bitpack(idxcbg, (unsigned short *) PackedWords, 8, PackWdsPtr);

	}
	else
	{							/* bit rates 4 or 8kbps */
		/* Send indication on the spectrum transition */
		if (bit_rate == 4)
			Bitpack(LPCflag, (unsigned short *) PackedWords, 1, PackWdsPtr);

		/* Quantize the LSPs */
		lspmaq(lsp_nq, ORDER, 1, knum, nsub, nsize, 16384, lsp, SScratch, bit_rate, lsptab);

		/* Bit-pack the quantization indices */

		for (i = 0; i < knum; i++)
			Bitpack(SScratch[i], (unsigned short *) PackedWords,
					lognsize[i], PackWdsPtr);

		/* Update shiftSTATE with hysteresis */
		if (beta < 3277)
		{
			accshift = 0;
			dpm = 0;
			shiftSTATE = 0;
		}

		if (accshift > 5120)
			shiftSTATE = -1;
		if (accshift < -5120)
			shiftSTATE = 1;
		if (accshift <= 2560 && shiftSTATE == -1)
			shiftSTATE = 0;
		if (accshift >= -2560 && shiftSTATE == 1)
			shiftSTATE = 0;

		/* Control accshift */
		if (shiftSTATE == 1 && beta < 13107)
			delay = add(delay, 1);
		else if (shiftSTATE == -1 && beta < 13107)
			delay = sub(delay, 1);
		if (delay > DMAX)
			delay = DMAX;
		if (delay < DMIN)
			delay = DMIN;

        Bitpack(sub(delay, DMIN), (unsigned short *) PackedWords, 7, PackWdsPtr);

		if (bit_rate == 4)
		{
			j = sub(delay, pdelay);
			if (abs(j) > 15)
				j = 0;
			else
				j = add(j, 16);

			Bitpack(j, (unsigned short *) PackedWords, 5, PackWdsPtr);
		}

		/* Smooth interpolation if the difference between delays is too big */
		if (abs_s(sub(delay, pdelay)) > 15)
			pdelay = delay;

		Aveidxppg = 0;
		/*********************************
        * CELP codebook search procedure *
		*********************************/
		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
				subframesize = sub(SubFrameSize, 1);
			else
				subframesize = SubFrameSize;

			/* interpolate lsp */
			Interpol(lspi, OldlspE, lsp, i, ORDER);
			Interpol(lspi_nq, Oldlsp_nq, lsp_nq, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pci, lspi);
			lsp2a(pci_nq, lspi_nq);

			/* Get zir */

			ZeroInput(zir, pci_nq, pci, Excitation + ACBMemSize, GAMMA1, GAMMA2, ORDER, subframesize, 0);

			/* Calculate impulse response of 1/A(z) * A(z/g1) / A(z/g2) */
			ImpulseRzp(H, pci_nq, pci, GAMMA1, GAMMA2, ORDER, Hlength);

			/* Interpolate delay */
			Interpol_delay(delayi, &pdelay, &delay, i);

			ComputeACB(residualm, Excitation + ACBMemSize, delayi,
					   residual + GUARD + i * (SubFrameSize - 1),
					   FrameSize + GUARD - i * (SubFrameSize - 1), &dpm,
					   &accshift, beta, subframesize, RSHIFT);

			/* Get weighted speech */
			/* ORIGM */

			SynthesisFilter(origm, residualm, pci_nq, SynMemoryM, ORDER, subframesize);

			/* Weighting filter */
			weight(wpci, pci_nq, GAMMA1, ORDER);
			fir(Scratch, origm, wpci, WFmemFIR, ORDER, subframesize);
			weight(wpci, pci_nq, GAMMA2, ORDER);
			iir(worigm, Scratch, wpci, WFmemIIR, ORDER, subframesize);

			/* Remove Zero input response from weighted speech */
			for (j = 0; j < subframesize; j++)
				worigm[j] = sub(worigm[j], zir[j]);

			/* Calculate closed loop gain */
			getgain(Excitation + ACBMemSize, &sum1, H, &idxppg, ppvq, ppvq_mid, ACBGainSize, 1, worigm, subframesize, Hlength);

			Aveidxppg = add(Aveidxppg, idxppg);

			/* Get TARGET for fixed c.b. */
			/* Convolve excitation with H */
			/* ExconvH stored in Scratch memory */

			ConvolveImpulseR(pencobj->ExconvH, Excitation + ACBMemSize, H, subframesize);

			for (j = 0; j < subframesize; j++)
				TARGETw[j] = sub(worigm[j], ExconvH[j]);

			/* Convert TARGET from weighted domain to residual domain */
			Weight2Res(TARGET, TARGETw, pci_nq, pci, GAMMA1, GAMMA2, ORDER, SubFrameSize);

			if (subframesize < SubFrameSize)
				TARGETw[subframesize] = TARGET[subframesize] = Scratch[subframesize] = 0;

			/* get delay for current subframe */
			n = extract_h(L_add(L_shr(L_add(delayi[1], delayi[0]), 1), 32768));
			/* Compute fixed codebook contribution */
			if (n > subframesize)
				n = 200;

			/* ACELP fixed codebook search */
			if (bit_rate == 4)
			{

				ACELP_Code(TARGETw, TARGET, H, n, sum1, subframesize, Scratch,
						   &fcbGain, y2, fcbIndexVector, 1);

				/* constrain fcb gain */
				/* fcbGain *= (1.0 - ppvq[idxppg] * 0.15); */

                l_fcbGain = L_mpy_ls(L_sub(2147483647, L_mult(ppvq[idxppg], 9830)), fcbGain);
            }
			else
			{
                /** Processing half rate case  **/

				ACELP_Code(TARGETw, TARGET, H, n, sum1, subframesize, Scratch,
						   &fcbGain, y2, fcbIndexVector, 0);

				/* Constrain fcb gain */
				/* fcbGain *= (0.9 - ppvq[idxppg] * 0.1); */
				l_fcbGain = L_mpy_ls(L_sub(1932735283, L_mult(ppvq[idxppg], 6554)), fcbGain);

			}
			/* scale fcbGain +6db to account for 6db loss at input */
            l_fcbGain = L_shl(l_fcbGain, 1);

			/* Quantize FCB Gain : */
			fcb_gainq(&idxcbg, &fcbGain, l_fcbGain, gnvq, FCBGainSize);

			/* adjust fcbGain -6db */
            fcbGain = shr(fcbGain, 1);

			/* Add to total excitation */
			for (j = 0; j < subframesize; j++)
				Excitation[j + ACBMemSize] = add(Excitation[j + ACBMemSize], round32(L_mult(Scratch[j], fcbGain)));

			/* Update filters memory */
			ZeroInput(zir, pci_nq, pci, Excitation + ACBMemSize, GAMMA1, GAMMA2, ORDER, subframesize, 1);

            /* checking outputs */
			/* Update residualm */
			for (j = 0; j < dpm; j++)
				residualm[j] = residualm[j + subframesize];

			/* Update excitation */
			for (j = 0; j < ACBMemSize; j++)
				Excitation[j] = Excitation[j + subframesize];

			/* Pack bits */
			/* ACB gain index */

			Bitpack(idxppg, (unsigned short *) PackedWords, 3, PackWdsPtr);

			if (bit_rate == 4)
			{
				idxcb = fcbIndexVector[0];
				Bitpack(idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				idxcb = fcbIndexVector[1];
				Bitpack(idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				idxcb = fcbIndexVector[2];
				Bitpack(idxcb, (unsigned short *) PackedWords, 8, PackWdsPtr);
				idxcb = fcbIndexVector[3];
				Bitpack(idxcb, (unsigned short *) PackedWords, 11, PackWdsPtr);
				/* FCB gain index */
				Bitpack(idxcbg, (unsigned short *) PackedWords, 5, PackWdsPtr);
			}
			else
			{
                /** Packing half-rate bits **/
				idxcb = fcbIndexVector[0];
				/* FCB shape index  10 bits */
				Bitpack(idxcb, (unsigned short *) PackedWords, 10, PackWdsPtr);
				/* FCB gain index */
				Bitpack(idxcbg, (unsigned short *) PackedWords, 4, PackWdsPtr);
			}

		}
		/* prevent overflow */
		if (Aveidxppg <= 1 && idxppg != 1)
		{
			accshift = 0;
			dpm = 0;
		}

		/* Update encoder variables */
		pdelay = delay;

	}							/* Ends if for rate of 4 or 8kbps */

	lastrateE = bit_rate;

	for (i = 0; i < PACKWDSNUM; i++)
		codeBuf[i] = PackedWords[i];

	encode_fcnt++;
#endif
}

/*======================================================================*/
/*         ..Save LSPs.                                                 */
/*----------------------------------------------------------------------*/
void post_encode(EVRC_ENC_COMPONENT* evrc_ecom)
{
	/*....(local) variables.... */
	register int j;
	ERVC_COMPONENT	*pcom = evrc_ecom->evrc_com;
	ERVC_ENC_OBJ	*pencobj = evrc_ecom->evrc_encobj;
	

	/*....execute.... */
	for (j = 0; j < ORDER; j++)
	{
		pencobj->OldlspE[j] = pcom->lsp[j];
		pencobj->Oldlsp_nq[j] = pencobj->lsp_nq[j];
	}
}
