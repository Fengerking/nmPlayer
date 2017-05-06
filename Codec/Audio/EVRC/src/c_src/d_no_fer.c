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
/*  Memory Usage:                           				*/
/*      ROM:                            				*/
/*      Static/Global RAM:                      			*/
/*      Stack/Local RAM:                    				*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  EVRC Decoder -- (called when no frame errors have occured.)         */
/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include "basic_op.h"
//#include "mathadv.h"
//#include "mathevrc.h"
//#include "mathdp31.h"

#include  "d_globs.h"
#include  "globs.h"
#include  "macro.h"
#include  "proto.h"
#include  "apf.h"
#include  "rom.h"
#include  "acelp_pf.h"          /* for ACELP */

//extern void decode_rate_1(short*, short, short*);
extern void decode_rate_1(EVRC_DEC_COMPONENT *, unsigned char *,short,short*);
void pit_shrp(short *, short, short, short);
void Fix_delay_contour(EVRC_DEC_COMPONENT *, long *);


/*======================================================================*/
/*         ..Decode bitstream data.                                     */
/*----------------------------------------------------------------------*/
void decode_no_fer(
//				   short *codeBuf,
				   EVRC_DEC_COMPONENT *evrc_dcom,
				   unsigned char *codeBuf,
				   short post_filter,
				   short *outFbuf
				   )
{
#if (FUNC_DECODE_NO_FER_OPT)
	/*....(local) variables.... */
	register short i, j, n;
	register short *foutP;
	long delayi[3];
	long counter_temp = 0;
	ERVC_COMPONENT	*pcom;
	ERVC_DEC_OBJ	*pdecobj;
	short *PackedWords, *PackWdsPtr;
	short *lsp, *OldlspD;
	short *SScratch;
	short *PitchMemoryD;
	short *DECspeech, *DECspeechPF;
	short subframesize;
	short sum1;
	short sum_acb;

#define P75	24576
#define P20	6554

	pcom    = evrc_dcom->evrc_com;
	pdecobj = evrc_dcom->evrc_decobj;
	PackedWords = pcom->PackedWords;
	PackWdsPtr = pcom->PackWdsPtr;
	SScratch = pcom->SScratch;
	lsp = pcom->lsp;
	OldlspD = pdecobj->OldlspD;
	PitchMemoryD = pdecobj->PitchMemoryD;
	DECspeech = pdecobj->DECspeech;
	DECspeechPF = pdecobj->DECspeechPF;

	/*....execute.... */
//	for (i = 0; i < PACKWDSNUM; i++)
//	{
//		PackedWords[i] = codeBuf[i];
//	}
	switch (pcom->bit_rate)
	{
	    case 4:
	        counter_temp = 11;
	        break;
	    case 3:
	        counter_temp = 5;
	        break;
	    case 1:
	        counter_temp = 1;
	        break;
	    default:
	        break;
	}
#ifdef  SHORT_SHIFT_ON
	for (i = 0; i < counter_temp; i++)
	{
		PackedWords[i] = (short)((unsigned short)codeBuf[(i << 1) + 1] + ((unsigned short)codeBuf[i << 1] << 8));
	}
	for (; i < PACKWDSNUM; i++)
	{
		PackedWords[i] = 0;
	}
#else
	for (i = 0; i < counter_temp; i++)
	{
		PackedWords[i] = (short)((unsigned short)codeBuf[i << 1] + ((unsigned short)codeBuf[(i << 1) + 1] << 8));
	}
	for (; i < PACKWDSNUM; i++)
	{
		PackedWords[i] = 0;
	}
#endif

	/* Re-initialize PackWdsPtr */
	PackWdsPtr[0] = 16;
	PackWdsPtr[1] = 0;

	{
		pdecobj->fer_counter -= 1;
		if (pdecobj->fer_counter < 0)
			pdecobj->fer_counter = 0;
	}


	if (pcom->bit_rate == 1)
	{
		decode_rate_1(evrc_dcom, codeBuf, post_filter, outFbuf);   //shanrong: para--codeBuf is not used in this func.
	}

	else
	{							/* If rate is 4kbps or 8kbps */
		if (pcom->bit_rate == 4)
			BitUnpack(&pcom->LPCflag, (unsigned short *) PackedWords, 1, PackWdsPtr);
		else
			pcom->LPCflag = 0;

		/* Bit-unpack the quantization indices */
		for (i = 0; i < pcom->knum; i++)
		{
			BitUnpack(&SScratch[i], (unsigned short *) PackedWords, pcom->lognsize[i], PackWdsPtr);
        }

		{
			lspmaq_dec(ORDER, 1, pcom->knum, pcom->nsub, pcom->nsize, pcom->lsp, 
						SScratch, pcom->bit_rate, pcom->lsptab);

                        /* Check for monotonic LSP */
			for (j = 1; j < ORDER; j++)
			{
                if (lsp[j] <= lsp[j - 1]) 
                {
                    pdecobj->errorFlag = 1;
                    return;
                }
            }

            /* Check for minimum separation of LSPs at the splits */
            if( pcom->bit_rate == 3 )     /* Check Half Rate splits */
            {
                if(   (lsp[3] <= (lsp[2] + MIN_LSP_SEP))
                   || (lsp[6] <= (lsp[5] + MIN_LSP_SEP)))
                {
                    pdecobj->errorFlag = 1;
                    return;
                }
            }
            else                    
            {                      /* Check Full Rate splits */
                if(   (lsp[2] <= (lsp[1] + MIN_LSP_SEP))
                   || (lsp[4] <= (lsp[3] + MIN_LSP_SEP))
                   || (lsp[7] <= (lsp[6] + MIN_LSP_SEP)))
                {
                    pdecobj->errorFlag = 1;
                    return;
                }
            }
		}

		BitUnpack(&pcom->idxppg, (unsigned short *) PackedWords, 7, PackWdsPtr);
		pcom->idxppg += DMIN;

		/* Check in delay is possible */
		if (pcom->idxppg > DMAX)
		{
			pdecobj->errorFlag = 1;
			return;
		}

		pcom->delay = pcom->idxppg;

        if (pcom->bit_rate == 4)
        {  /* check for full-rate */
            /* unpack delta_delay */
            BitUnpack(&pcom->idxppg, (unsigned short *) PackedWords, 5, PackWdsPtr);

#if ANSI_EVRC_DDELAY_CHECK
            /* check for out-of-range previous delay before Fix_delay_contour */
            if (pcom->idxppg != 0)
            {
                /* prev_delay = delay - delta_delta + 16 */
                n = pcom->delay - pcom->idxppg + 16;

                if (n < DMIN)
                {
                    pdecobj->errorFlag = 1;
                    return;
                }
                if (n > DMAX)
                {
                    pdecobj->errorFlag = 1;
                    return;
                }
            }
#endif

            /* Fix delay contour */
            if (pdecobj->fer_counter == 2 && pcom->idxppg != 0)
            {
                Fix_delay_contour(evrc_dcom, delayi);
            }
        }

		/* Smooth interpolation if the difference between delays is too big */
		if (abs(sub(pcom->delay, pdecobj->pdelayD)) > 15)
			pdecobj->pdelayD = pcom->delay;

		/* Update fer coefficients */
		pdecobj->ave_acb_gain = pdecobj->ave_fcb_gain = 0;

		foutP = outFbuf;
		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
				subframesize = SubFrameSize - 1;
			else
				subframesize = SubFrameSize;

			Interpol(pcom->lspi, OldlspD, lsp, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pcom->pci, pcom->lspi);

			/* Bandwidth expansion after frame erasure only if LPCflag is set */
			if (pcom->bit_rate == 4)
			{
				if (pcom->LPCflag && pdecobj->fer_counter == 2)
					weight(pcom->pci, pcom->pci, P75, ORDER);
			}

			Interpol_delay(delayi, &pdecobj->pdelayD, &pcom->delay, i);


			/* Un-Pack bits */
			/* ACB delay gain */
			BitUnpack(&pcom->idxppg, (unsigned short *) PackedWords, 3, PackWdsPtr);

			/* FCB shape index 1 */
			if (pcom->bit_rate == 4)
			{
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->fcbIndexVector[0] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->fcbIndexVector[1] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				pcom->fcbIndexVector[2] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 11, PackWdsPtr);
				pcom->fcbIndexVector[3] = *SScratch;
				/* FCB gain index */
				BitUnpack(&pcom->idxcbg, (unsigned short *) PackedWords, 5, PackWdsPtr);
			}
			else
			{
				BitUnpack(&pcom->idxcb, (unsigned short *) PackedWords, 10, PackWdsPtr);
				/* FCB gain index */
				BitUnpack(&pcom->idxcbg, (unsigned short *) PackedWords, 4, PackWdsPtr);
			}

			/* Compute adaptive codebook contribution */
			{
				sum_acb = ppvq[pcom->idxppg];
				pdecobj->ave_acb_gain = pdecobj->ave_acb_gain + ((sum_acb * 10923 + 0x4000) >> 15);	/* (10923=1/NoOfSubFrames) */
			}

			acb_excitation(PitchMemoryD + ACBMemSize, sum_acb, delayi,
						   PitchMemoryD, subframesize);

			/* Compute fixed codebook contribution */
			pdecobj->ave_fcb_gain = pdecobj->ave_fcb_gain + ((pcom->gnvq[pcom->idxcbg] * 1365 + 0x4000) >> 15);	/* (1365=(1/3)/8) */
			
			/* Compute fixed codebook contribution */
			if ((pcom->bit_rate == 4))	/* full rate */
                dec8_35(pcom->fcbIndexVector, pcom->Scratch, subframesize);

			if ((pcom->bit_rate == 3))	/* half rate */
                dec3_10(pcom->idxcb, pcom->Scratch, subframesize);

			{
				if (sum_acb > 14744)
					sum_acb = 14744;
				if (sum_acb < 3276)
					sum_acb = 3276;
				sum_acb <<= 1;

				/* get intrpolated delay for this subframe */
				/* n = extract_h(L_add(L_shr(L_add(delayi[1], delayi[0]), 1), 32768)); */
				/* n = extract_h(L_add(L_shr(L_add(delayi[1], delayi[0]), 1), 0x00008000L)); */
				n = (short)((((delayi[1] + delayi[0]) >> 1) + 0x8000) >> 16);
				if (n > subframesize)
					n = 200;

				/* To scale down by 8 to offset (sum1=gnvq[]),
				 * which has been scaled up by 8 */
				for (j = 0; j < SubFrameSize + 6; j++)
				{
					pcom->Scratch[j] <<= 11;
				}

				pit_shrp(pcom->Scratch, n, sum_acb, subframesize);

				sum1 = pcom->gnvq[pcom->idxcbg];
				for (j = 0; j < subframesize; j++)
				{
					PitchMemoryD[j + ACBMemSize] += (sum1 * pcom->Scratch[j] + 0x4000) >> 15;
				}
			}

			{
                if (pdecobj->FadeScale > (MAX_16 - P20))
                {
                    pdecobj->FadeScale = MAX_16;
                }
                else
                {
                    pdecobj->FadeScale += P20;
                }
			}

			for (j = 0; j < ACBMemSize; j++)
				PitchMemoryD[j] = PitchMemoryD[j + subframesize];


			/* Synthesis of decoder output signal and postfilter output signal */
			iir(DECspeech, PitchMemoryD + ACBMemSize, pcom->pci, pdecobj->SynMemory, ORDER, subframesize);

			/* Postfilter */
			if (post_filter)
			{
				if (pcom->bit_rate == 4)
				{
					apf(pdecobj, pcom->pci, ((delayi[0] + delayi[1]) >> 1), 
						ALPHA, BETA, U, AGC, LTGAIN, ORDER, subframesize, pcom->bit_rate);
				}
				else
				{
					apf(pdecobj, pcom->pci, ((delayi[0] + delayi[1]) >> 1), 
						HALF_ALPHA, BETA, HALF_U, AGC, LTGAIN, ORDER, subframesize, pcom->bit_rate);
				}
			}
			else
			{
				for (j = 0; j < subframesize; j++)
					DECspeechPF[j] = DECspeech[j];
			}

			/* Write p.f. decoder output and variables to files */
			for (j = 0; j < subframesize; j++)
			{
				short chen_temp_s = DECspeechPF[j];
				if (chen_temp_s > (short)0x3fff)
			    {
			        chen_temp_s = MAX_16;
			    }
			    else if (chen_temp_s < (short)0xc000)
			    {
			        chen_temp_s = MIN_16;
			    }
			    else
		        {
		            chen_temp_s <<= 1;
		        }
				*foutP++ = chen_temp_s; /* adjust scaling */
			}
		}
		pdecobj->pdelayD = pcom->delay;
	}							/* Ends case for rate=4kbps or 8kbps */

	/* update decoder varaibles */
	for (i = 0; i < ORDER; i++)
		OldlspD[i] = lsp[i];
	pdecobj->lastrateD = pcom->bit_rate;

	pdecobj->decode_fcnt++;
	pdecobj->last_fer_flag = pdecobj->fer_flag;

#else

	/*....(local) variables.... */
	register short i, j, n;
	register short *foutP;
	long delayi[3];
	short subframesize;
	short sum1;
	short sum_acb;

#define P75	24576
#define P20	6554

	/*....execute.... */
	for (i = 0; i < PACKWDSNUM; i++){
		PackedWords[i] = codeBuf[i];
		}

	/* Re-initialize PackWdsPtr */
	PackWdsPtr[0] = 16;
	PackWdsPtr[1] = 0;

	{
		fer_counter -= 1;
		if (fer_counter < 0)
			fer_counter = 0;
	}


	if (bit_rate == 1)
	{
		decode_rate_1(codeBuf, post_filter, outFbuf);
	}

	else
	{							/* If rate is 4kbps or 8kbps */

		if (bit_rate == 4)
			BitUnpack(&LPCflag, (unsigned short *) PackedWords, 1, PackWdsPtr);
		else
			LPCflag = 0;

		/* Bit-unpack the quantization indices */
		for (i = 0; i < knum; i++)
			BitUnpack(&SScratch[i], (unsigned short *) PackedWords,
					  lognsize[i], PackWdsPtr);

		{
			lspmaq_dec(ORDER, 1, knum, nsub, nsize, lsp, SScratch, bit_rate, lsptab);

                        /* Check for monotonic LSP */
			for (j = 1; j < ORDER; j++)
                            if (lsp[j] <= lsp[j - 1]) 
                            {
                                errorFlag = 1;
                                return;
                            }

                        /* Check for minimum separation of LSPs at the splits */
                        if( bit_rate == 3 )     /* Check Half Rate splits */
                        {
                            if(   (lsp[3] <= add(lsp[2],MIN_LSP_SEP))
                               || (lsp[6] <= add(lsp[5],MIN_LSP_SEP)))
                            {
                                errorFlag = 1;
                                return;
                            }
                        }
                        else                    
                        {                      /* Check Full Rate splits */
                            if(   (lsp[2] <= add(lsp[1],MIN_LSP_SEP))
                               || (lsp[4] <= add(lsp[3],MIN_LSP_SEP))
                               || (lsp[7] <= add(lsp[6],MIN_LSP_SEP)))
                            {
                                errorFlag = 1;
                                return;
                            }
                        }
		}


		BitUnpack(&idxppg, (unsigned short *) PackedWords, 7, PackWdsPtr);
		idxppg += DMIN;

		/* Check in delay is possible */
		if (idxppg > DMAX)
		{
			errorFlag = 1;
			return;
		}

		delay = idxppg;

        if (bit_rate == 4) {  /* check for full-rate */

            /* unpack delta_delay */
            BitUnpack(&idxppg, (unsigned short *) PackedWords, 5, PackWdsPtr);

#if ANSI_EVRC_DDELAY_CHECK
            /* check for out-of-range previous delay before Fix_delay_contour */
            if (idxppg != 0)
            {
                /* prev_delay = delay - delta_delta + 16 */
                n = sub(delay, sub(idxppg, 16));

                if (n < DMIN)
                {
                    errorFlag = 1;
                    return;
                }
                if (n > DMAX)
                {
                    errorFlag = 1;
                    return;
                }
            }
#endif

            /* Fix delay contour */
            if (fer_counter == 2 && idxppg != 0)
            {
                Fix_delay_contour(delayi);
            }
        }

		/* Smooth interpolation if the difference between delays is too big */
		if (abs(sub(delay, pdelayD)) > 15)
			pdelayD = delay;

		/* Update fer coefficients */
		ave_acb_gain = ave_fcb_gain = 0;

		foutP = outFbuf;
		for (i = 0; i < NoOfSubFrames; i++)
		{
			if (i < 2)
				subframesize = SubFrameSize - 1;
			else
				subframesize = SubFrameSize;

			Interpol(lspi, OldlspD, lsp, i, ORDER);

			/* Convert lsp to PC */
			lsp2a(pci, lspi);

			/* Bandwidth expansion after frame erasure only if LPCflag is set */
			if (bit_rate == 4)
			{
				if (LPCflag && fer_counter == 2)
					weight(pci, pci, P75, ORDER);
			}

			Interpol_delay(delayi, &pdelayD, &delay, i);


			/* Un-Pack bits */
			/* ACB delay gain */
			BitUnpack(&idxppg, (unsigned short *) PackedWords, 3, PackWdsPtr);

			/* FCB shape index 1 */
			if (bit_rate == 4)
			{
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				fcbIndexVector[0] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				fcbIndexVector[1] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 8, PackWdsPtr);
				fcbIndexVector[2] = *SScratch;
				BitUnpack(SScratch, (unsigned short *) PackedWords, 11, PackWdsPtr);
				fcbIndexVector[3] = *SScratch;
				/* FCB gain index */
				BitUnpack(&idxcbg, (unsigned short *) PackedWords, 5, PackWdsPtr);
			}
			else
			{
				BitUnpack(&idxcb, (unsigned short *) PackedWords, 10, PackWdsPtr);
				/* FCB gain index */
				BitUnpack(&idxcbg, (unsigned short *) PackedWords, 4, PackWdsPtr);
			}

			/* Compute adaptive codebook contribution */
			{
				sum_acb = ppvq[idxppg];
				ave_acb_gain = add(ave_acb_gain, mult_r(sum_acb, 10923));	/* (10923=1/NoOfSubFrames) */
			}

			acb_excitation(PitchMemoryD + ACBMemSize, sum_acb, delayi,
						   PitchMemoryD, subframesize);

			/* Compute fixed codebook contribution */
			ave_fcb_gain = add(ave_fcb_gain, mult_r(gnvq[idxcbg], 1365));	/* (1365=(1/3)/8) */

			/* Compute fixed codebook contribution */
			if ((bit_rate == 4))	/* full rate */
                dec8_35(fcbIndexVector, Scratch, subframesize);

			if ((bit_rate == 3))	/* half rate */
                dec3_10(idxcb, Scratch, subframesize);

			{
				if (sum_acb > 14744)
					sum_acb = 14744;
				if (sum_acb < 3276)
					sum_acb = 3276;
				sum_acb = shl(sum_acb, 1);


				/* get intrpolated delay for this subframe */
				/* n = extract_h(L_add(L_shr(L_add(delayi[1], delayi[0]), 1), 32768)); */
				/* n = extract_h(L_add(L_shr(L_add(delayi[1], delayi[0]), 1), 0x00008000L)); */
				n = round32(L_shr(L_add(delayi[1], delayi[0]), 1));
				if (n > subframesize)
					n = 200;

				/* To scale down by 8 to offset (sum1=gnvq[]),
				 * which has been scaled up by 8 */
				for (j = 0; j < SubFrameSize + 6; j++)
					Scratch[j] = shl(Scratch[j], 11);

				pit_shrp(Scratch, n, sum_acb, subframesize);

				sum1 = gnvq[idxcbg];
				for (j = 0; j < subframesize; j++)
					PitchMemoryD[j + ACBMemSize] = add(PitchMemoryD[j + ACBMemSize], mult_r(sum1, Scratch[j]));
			}

			{
				FadeScale = add(FadeScale, P20);
			}

			for (j = 0; j < ACBMemSize; j++)
				PitchMemoryD[j] = PitchMemoryD[j + subframesize];


			/* Synthesis of decoder output signal and postfilter output signal */
			iir(DECspeech, PitchMemoryD + ACBMemSize, pci, SynMemory, ORDER, subframesize);

			/* Postfilter */
			if (post_filter)
			{
				if (bit_rate == 4)
					apf(DECspeech, pci, DECspeechPF, L_shr(L_add(delayi[0], delayi[1]), 1), ALPHA, BETA, U, AGC, LTGAIN, ORDER, subframesize, bit_rate);
				else
					apf(DECspeech, pci, DECspeechPF, L_shr(L_add(delayi[0], delayi[1]), 1), HALF_ALPHA, BETA, HALF_U, AGC, LTGAIN, ORDER, subframesize, bit_rate);
			}
			else
			{
				for (j = 0; j < subframesize; j++)
					DECspeechPF[j] = DECspeech[j];
			}

			/* Write p.f. decoder output and variables to files */
			for (j = 0; j < subframesize; j++){
				*foutP++ = shl(DECspeechPF[j],1); /* adjust scaling */
				}
		}
		pdelayD = delay;
	}							/* Ends case for rate=4kbps or 8kbps */

	/* update decoder varaibles */
	for (i = 0; i < ORDER; i++)
		OldlspD[i] = lsp[i];
	lastrateD = bit_rate;

	decode_fcnt++;
	last_fer_flag = fer_flag;
#endif
}

void Fix_delay_contour(EVRC_DEC_COMPONENT *evrc_dcom, long *delayi)
{
	ERVC_COMPONENT	*pcom;
	ERVC_DEC_OBJ	*pdecobj;
	short *PitchMemoryD;
	short *PitchMemoryD_back;
	short i;
	short j;
	short subframesize;

	pcom = evrc_dcom->evrc_com;
	pdecobj = evrc_dcom->evrc_decobj;
	PitchMemoryD = pdecobj->PitchMemoryD;
	PitchMemoryD_back = pdecobj->PitchMemoryD_back;
	
	/* Fix delay countour of previous erased frame */
	j = sub(pcom->idxppg, 16);
	pdecobj->pdelayD_back = pdecobj->pdelayD;
	pdecobj->pdelayD = sub(pcom->delay, j);
	if (abs(sub(pdecobj->pdelayD, pdecobj->pdelayD_back)) > 15)
		pdecobj->pdelayD_back = pdecobj->pdelayD;
	
	for (i = 0; i < ACBMemSize; i++)
		PitchMemoryD[i] = PitchMemoryD_back[i];
	
#if 1
	for (i = 0; i < NoOfSubFrames; i++)
	{
		if (i < 2)
			subframesize = SubFrameSize - 1;
		else
			subframesize = SubFrameSize;
		
		/* Interpolate delay */
		Interpol_delay(delayi, &pdecobj->pdelayD_back, &pdecobj->pdelayD, i);
		
		/* Compute adaptive codebook contribution */
		acb_excitation(PitchMemoryD + ACBMemSize, pdecobj->ave_acb_gain, delayi,
			PitchMemoryD, subframesize);
		
		for (j = 0; j < ACBMemSize; j++)
			PitchMemoryD[j] = PitchMemoryD[j + subframesize];
	}
#endif
}

