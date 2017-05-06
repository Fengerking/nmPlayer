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
 * gecko2codec.c - public C API for Gecko2 decoder
 **************************************************************************************/

#include "coder.h"

/**************************************************************************************
 * Function:    Gecko2InitDecoder
 *
 * Description: initialize the fixed-point Gecko2 audio decoder
 *
 * Inputs:      number of samples per frame
 *              number of channels
 *              number of frequency regions coded
 *              number of encoded bits per frame
 *              number of samples per second
 *              start region for coupling (joint stereo only)
 *              number of bits for each coupling scalefactor (joint stereo only)
 *              pointer to receive number of frames of coding delay
 *
 * Outputs:     number of frames of coding delay (i.e. discard the PCM output from
 *                the first *codingDelay calls to Gecko2Decode())
 *
 * Return:      instance pointer, 0 if error (malloc fails, unsupported mode, etc.)
 *
 * Notes:       this implementation is fully reentrant and thread-safe - the 
 *                HGecko2Decoder instance pointer tracks all the state variables
 *                for each instance
 **************************************************************************************/
HGecko2Decoder Gecko2InitDecoder(int nSamples, int nChannels, int nRegions, int nFrameBits, int sampRate, 
			int cplStart, int cplQbits,	int *codingDelay, VO_MEM_OPERATOR  *pMemOP)
{
	Gecko2Info *gi;

	/* check parameters */
	if (nChannels < 0 || nChannels > 2)
		return 0;
	if (nRegions < 0 || nRegions > MAXREGNS)
		return 0;
	if (nFrameBits < 0 || cplStart < 0) 
		return 0;
	if (cplQbits && (cplQbits < 2 || cplQbits > 6))
		return 0;

	gi = AllocateBuffers(pMemOP);
	if (!gi)
		return 0;

	/* if stereo, cplQbits == 0 means dual-mono, > 0 means joint stereo */
	gi->jointStereo = (nChannels == 2) && (cplQbits > 0);

	gi->nSamples = nSamples;
	gi->nChannels = nChannels;
	gi->nRegions = nRegions;
	gi->nFrameBits = nFrameBits;
	if (gi->nChannels == 2 && !gi->jointStereo)
		gi->nFrameBits /= 2;
	gi->sampRate = sampRate;

	if (gi->jointStereo) {
		/* joint stereo */
		gi->cplStart = cplStart;
		gi->cplQbits = cplQbits;
		gi->rateBits = 5;
		if (gi->nSamples > 256) 
			gi->rateBits++;
		if (gi->nSamples > 512) 
			gi->rateBits++;
	} else {	
		/* mono or dual-mono */
		gi->cplStart = 0;
		gi->cplQbits = 0;
		gi->rateBits = 5;
	}
	gi->cRegions = gi->nRegions + gi->cplStart;
	gi->nCatzns = (1 << gi->rateBits);
	gi->lfsr[0] = gi->lfsr[1] = ('k' | 'e' << 8 | 'n' << 16 | 'c' << 24);		/* well-chosen seed for dither generator */

	/* validate tranform size */
	if (gi->nSamples == 256) {
		gi->xformIdx = 0;
	} else if (gi->nSamples == 512) {
		gi->xformIdx = 1;
	} else if (gi->nSamples == 1024) {
		gi->xformIdx = 2;
	} else {
		Gecko2FreeDecoder(gi, pMemOP);
		return 0;
	}

	/* this is now 2, since lookahead MLT has been removed */
	*codingDelay = CODINGDELAY;

	return (HGecko2Decoder)gi;
}

/**************************************************************************************
 * Function:    Gecko2FreeDecoder
 *
 * Description: free the fixed-point Gecko2 audio decoder
 *
 * Inputs:      HGecko2Decoder instance pointer returned by Gecko2InitDecoder()
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void Gecko2FreeDecoder(HGecko2Decoder hGecko2Decoder, VO_MEM_OPERATOR	  *pMemOP)
{
	Gecko2Info *gi = (Gecko2Info *)hGecko2Decoder;
	if (!gi)
		return;

	FreeBuffers(gi, pMemOP);

	return;
}

/**************************************************************************************
 * Function:    Gecko2ClearBadFrame
 *
 * Description: zero out pcm buffer if error decoding Gecko2 frame
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              pointer to pcm output buffer
 *
 * Outputs:     zeroed out pcm buffer
 *              zeroed out data buffers (as if codec had been reinitialized)
 *
 * Return:      none
 **************************************************************************************/
static void Gecko2ClearBadFrame(Gecko2Info *gi, short *outbuf)
{
	int i, ch;

	if (!gi || gi->nSamples * gi->nChannels > MAXNSAMP * MAXNCHAN || gi->nSamples * gi->nChannels < 0)
		return;

	/* clear PCM buffer */
	for (i = 0; i < gi->nSamples * gi->nChannels; i++)
		outbuf[i] = 0;

	/* clear internal data buffers */
	for (ch = 0; ch < gi->nChannels; ch++) {
		for (i = 0; i < gi->nSamples; i++) {
			gi->db.decmlt[ch][i] = 0;
			gi->db.decmlt[ch][i + MAXNSAMP] = 0;
		}
		gi->xbits[ch][0] = gi->xbits[ch][1] = 0;
	}

}

/**************************************************************************************
 * Function:    Gecko2Decode
 *
 * Description: decode one frame of audio data
 *
 * Inputs:      HGecko2Decoder instance pointer returned by Gecko2InitDecoder()
 *              pointer to one encoded frame 
 *                (nFrameBits / 8 bytes of data, byte-aligned)
 *              flag indicating lost frame (lostflag != 0 means lost)
 *              pointer to receive one decoded frame of PCM 
 *
 * Outputs:     one frame (nSamples * nChannels 16-bit samples) of decoded PCM
 *
 * Return:      0 if frame decoded okay, error code (< 0) if error
 *
 * Notes:       to reduce memory and CPU usage, this only implements one-sided
 *                (backwards) interpolation for error concealment (no lookahead)
 **************************************************************************************/
int Gecko2Decode(HGecko2Decoder hGecko2Decoder, unsigned char *codebuf, int lostflag, short *outbuf, unsigned int nFrameBits)
{
	int i, ch, availbits, gbMin[MAXNCHAN];
	Gecko2Info *gi = (Gecko2Info *)hGecko2Decoder;

	gi->nFrameBits = nFrameBits;
 
	//fixed 12015 issue
	if (gi->nChannels == 2 && !gi->jointStereo)
		gi->nFrameBits /= 2;

	if (!gi)
		return -1;

	if (lostflag) {
		/* if current frame is lost, create one using interpolation - force one-sided interp to save memory */
		for (ch = 0; ch < gi->nChannels; ch++) {
			for (i = 0; i < gi->nSamples; i++) {
				gi->db.decmlt[ch][i] = (gi->db.decmlt[ch][i] >> 1) + (gi->db.decmlt[ch][i] >> 2);
			}
		}
	} else {
		/* current frame is valid, decode it */
		if (gi->jointStereo) {
			/* decode gain control info, coupling coefficients, and power envlope */
			availbits = DecodeSideInfo(gi, codebuf, gi->nFrameBits, 0);
			if (availbits < 0) {
				Gecko2ClearBadFrame(gi, outbuf);
				return ERR_GECKO2_INVALID_SIDEINFO;
			}

			/* reconstruct power envelope */
			CategorizeAndExpand(gi, availbits);

			/* reconstruct full MLT, including stereo decoupling */
			gbMin[0] = gbMin[1] = DecodeTransform(gi, gi->db.decmlt[0], availbits, &gi->lfsr[0], 0);
			JointDecodeMLT(gi, gi->db.decmlt[0], gi->db.decmlt[1]);
			gi->xbits[1][1] = gi->xbits[0][1];
		} else {
			for (ch = 0; ch < gi->nChannels; ch++) {
				/* decode gain control info and power envlope */
				availbits = DecodeSideInfo(gi, codebuf + (ch * gi->nFrameBits >> 3), gi->nFrameBits, ch);
				if (availbits < 0) {
					Gecko2ClearBadFrame(gi, outbuf);
					return ERR_GECKO2_INVALID_SIDEINFO;
				}

				/* reconstruct power envelope */
				CategorizeAndExpand(gi, availbits);

				/* reconstruct full MLT */
				gbMin[ch] = DecodeTransform(gi, gi->db.decmlt[ch], availbits, &gi->lfsr[ch], ch);

				/* zero out non-coded regions */
				for (i = gi->nRegions*NBINS; i < gi->nSamples; i++)
					gi->db.decmlt[ch][i] = 0;
			}
		}

		/* inverse transform, without window or overlap-add */
		for (ch = 0; ch < gi->nChannels; ch++)
			IMLTNoWindow(gi->xformIdx, gi->db.decmlt[ch], gbMin[ch]);
	}

	for (ch = 0; ch < gi->nChannels; ch++) {
		/* apply synthesis window, gain window, then overlap-add (interleaves stereo PCM LRLR...) */
		if (gi->dgainc[ch][0].nats || gi->dgainc[ch][1].nats || gi->xbits[ch][0] || gi->xbits[ch][1])
			DecWindowWithAttacks(gi->xformIdx, gi->db.decmlt[ch], outbuf + ch, gi->nChannels, &gi->dgainc[ch][0], &gi->dgainc[ch][1], gi->xbits[ch]);
		else
		{
#ifndef ARM
			DecWindowNoAttacks(gi->xformIdx, gi->db.decmlt[ch], outbuf + ch, gi->nChannels);
#else
			DecWindowNoAttacks(gi->xformIdx, gi->db.decmlt[ch], outbuf + ch, gi->nChannels,&nmltTab[0],&windowOffset[0],&window[0]);
#endif
		}

		/* save gain settings for overlap */
		CopyGainInfo(&gi->dgainc[ch][0], &gi->dgainc[ch][1]);
		gi->xbits[ch][0] = gi->xbits[ch][1];
	}

	return 0;
}
