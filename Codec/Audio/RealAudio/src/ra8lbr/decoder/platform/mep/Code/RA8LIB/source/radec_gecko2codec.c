/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_gecko2codec.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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

/**************************************************************************** */
/*    Socrates Software Ltd : Toshiba Group Company */
/*    DESCRIPTION OF CHANGES: */
/*    
/*		1. Optimization achieved by usage of Intrinsic functions
/*		2. Data overlay performed
/*		3. Some C optimizations performed to suit platform
/*		4. Error handling and Debug trace features added
/*
 *    CONTRIBUTORS : Vinayak Bhat,Deepak,Sudeendra,Naveen
 *   */
/**************************************************************************** */

#include "radec_defines.h"

extern const char  __far D_HUFFTAB_SRC ;
extern const char  __far D_HUFFTAB_DST ;
extern const char  __far D_HUFFTAB_SIZ ;

extern const char  __far D_WINTAB_SRC ;
extern const char  __far D_WINTAB_DST ;
extern const char  __far D_WINTAB_SIZ ;

extern const char  __far D_AUDIOTAB_SRC ;
extern const char  __far D_AUDIOTAB_DST ;
extern const char  __far D_AUDIOTAB_SIZ ;

extern const char  __far D_FRAME_DATA_ADDR;

int *dmem_decmlt = (int*)0x201000;
#define  DMEM_DECTRAN_ADDR		0x201000
#define  DMEM_DECMLT_ADDR		0x201000
#define  DMEM_WINATT_ADDR       	0x201000

#define  FRAME_DATA_ADDR		0xA67000
#define  DMEM_FRAME_DATA		0x202E00
#define  MAXFRAME_SIZE			0x140

#ifdef RA8_DEBUG
#pragma section area .radebug
struct SRA8_Debug sRA8_Debug;
#pragma section area
#endif


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
ERRCODES Gecko2InitDecoder(Gecko2Info *gi, int *codingDelay)
{
	
	ERRCODES eStatus = RA8_SUCCESS;
	/*Check whether decider management data structure is NULL*/
    if (gi == NULL)
		return RA8_FATAL_ERR_DEC_PTR_NULL;
#ifdef RA8_DEBUG
	sRA8_Debug.iSamplesperframe = gi->sHdInfo.nSamples;
	sRA8_Debug.iNumchannels = gi->sHdInfo.nChannels;
	sRA8_Debug.iNumregions = gi->sHdInfo.nRegions;
	sRA8_Debug.iFramebits = gi->sHdInfo.nFrameBits;
	sRA8_Debug.iSampPersec = gi->sHdInfo.sampRate;
	sRA8_Debug.iCplStart = gi->sHdInfo.cplStart;
	sRA8_Debug.iCplQbits = gi->sHdInfo.cplQbits;
#endif

	/* check parameters */
	
	/*Check whether input buffer pointer is NULL*/ 
			
    
	/*Check for Samples per frame*/
	if ((gi->sHdInfo.nSamples != 256) && (gi->sHdInfo.nSamples != 512) 
			&& (gi->sHdInfo.nSamples != 1024))
		return RA8_FATAL_ERR_INVALID_SAMPLES_PER_FRAME;
	
	/*Check if Channels either mono or stereo or invalid*/
	if ((gi->sHdInfo.nChannels != 1) && (gi->sHdInfo.nChannels != 2))
		return RA8_FATAL_ERR_INVALID_CHANNELS;



	/*Check for Valid Frequency Regions for the Transform*/
	if ((gi->sHdInfo.nRegions >= 7) && (gi->sHdInfo.nRegions <= MAXREGNS))
	{
		if ((gi->sHdInfo.nRegions < ((560*gi->sHdInfo.nSamples)/(1024*NBINS))) 
			|| (gi->sHdInfo.nRegions > ((1024*gi->sHdInfo.nSamples)/(1024*NBINS))) )
				return RA8_FATAL_ERR_INVALID_FREQREGIONS;
	}
	else
		return RA8_FATAL_ERR_INVALID_FREQREGIONS;

	/*Check to see that Sampling rate is either 8,11.025,22.05 or 44.1 KHz*/
	if((gi->sHdInfo.sampRate != 8000) && (gi->sHdInfo.sampRate != 11025) 
			&& (gi->sHdInfo.sampRate != 22050) &&  (gi->sHdInfo.sampRate != 44100))
		return RA8_FATAL_ERR_INVALID_SAMPRATE;

	/*Check for Number of bits for Coupling Scale Fcator*/
	if ( gi->sHdInfo.cplQbits && (gi->sHdInfo.cplQbits < 2 || gi->sHdInfo.cplQbits > 6))
		return RA8_FATAL_ERR_INVALID_CPLQBITS;


	/*Check to see if Start of Coupling is between 0 and Regions */
	if((gi->sHdInfo.cplStart < 0) || (gi->sHdInfo.cplStart > gi->sHdInfo.nRegions))
		return RA8_FATAL_ERR_INVALID_CPLSTART;
	
	/*Check for Bits per Frame*/	
	if ( (gi->sHdInfo.nFrameBits < 192) || (gi->sHdInfo.nFrameBits > 2240)) 
		return RA8_FATAL_ERR_INVALID_BITS_PER_FRAME;
	
	/* if stereo, cplQbits == 0 means dual-mono, > 0 means joint stereo */
	gi->sHdInfo.jointStereo = 
        (gi->sHdInfo.nChannels == 2) && (gi->sHdInfo.cplQbits > 0);

	if (gi->sHdInfo.nChannels == 2 && !gi->sHdInfo.jointStereo)
		gi->sHdInfo.nFrameBits /= 2;
	
	if (gi->sHdInfo.jointStereo) 
    {
		/* joint stereo */
		gi->sHdInfo.rateBits = 5;
		if (gi->sHdInfo.nSamples > 256) 
			gi->sHdInfo.rateBits++;
		if (gi->sHdInfo.nSamples > 512) 
			gi->sHdInfo.rateBits++;
	} 
    else 
    {	
		/* mono or dual-mono */
		gi->sHdInfo.cplStart = 0;
		gi->sHdInfo.cplQbits = 0;
		gi->sHdInfo.rateBits = 5;
	}
	
	memset((unsigned char *)gi->sDecBuf.decmlt[0],0x0,8192);
	memset((unsigned char *)gi->sDecBuf.decmlt[1],0x0,8192);
	gi->sHdInfo.cRegions = gi->sHdInfo.nRegions + gi->sHdInfo.cplStart;

	gi->sHdInfo.nCatzns = (1 << gi->sHdInfo.rateBits);
	gi->sBufferInfo.lfsr[0] = gi->sBufferInfo.lfsr[1] = ('k' | 'e' << 8 | 'n' << 16 | 'c' << 24);		/* well-chosen seed for dither generator */

#ifdef RA8_DEBUG
	sRA8_Debug.iCregions = gi->sHdInfo.cRegions;
	sRA8_Debug.iRatebits = gi->sHdInfo.rateBits;	
	sRA8_Debug.iJointStereoFlg = gi->sHdInfo.jointStereo;	
#endif

	/* validate tranform size */
	if (gi->sHdInfo.nSamples == 256) 
    {
		gi->sBufferInfo.xformIdx = 0;
	} 
    else if (gi->sHdInfo.nSamples == 512) 
    {
		gi->sBufferInfo.xformIdx = 1;
	} 
    else if (gi->sHdInfo.nSamples == 1024) 
    {
		gi->sBufferInfo.xformIdx = 2;
	} 
    else 
    {
		return RA8_FATAL_ERR_INVALID_SAMPLES_PER_FRAME;
	}

	/* this is now 2, since lookahead MLT has been removed */
	*codingDelay = CODINGDELAY;
	return RA8_SUCCESS;
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

	if ( (!gi) || ((gi->sHdInfo.nSamples * gi->sHdInfo.nChannels) > (MAXNSAMP * MAXNCHAN)) || ( (gi->sHdInfo.nSamples * gi->sHdInfo.nChannels) < 0))
		return;

	/* clear PCM buffer */
	for (i = 0; i < (gi->sHdInfo.nSamples * gi->sHdInfo.nChannels); i++)
		outbuf[i] = 0;

	/* clear internal data buffers */
	for (ch = 0; ch < gi->sHdInfo.nChannels; ch++) 
	{
		for (i = 0; i < gi->sHdInfo.nSamples; i++) 
		{
			gi->sDecBuf.decmlt[ch][i] = 0;
			gi->sDecBuf.decmlt[ch][i + MAXNSAMP] = 0;
		}
		gi->sBufferInfo.xbits[ch][0] = gi->sBufferInfo.xbits[ch][1] = 0;
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
 * Return:      0 if frame decoded okay, error code (> 0) if error
 *
 * Notes:       to reduce memory and CPU usage, this only implements one-sided
 *                (backwards) interpolation for error concealment (no lookahead)
 **************************************************************************************/
ERRCODES Gecko2Decode(Gecko2Info *gi, int lostflag)
{
	int i, ch, availbits, gbMin[MAXNCHAN],iRetVal;
    unsigned char *codebuf; /*= gi->sHdInfo.inBuf;*/
    short *outbuf = gi->sHdInfo.outbuf;
    int * pTransfer;
	unsigned char ErrParam = 0;

	ERRCODES eStatus=RA8_SUCCESS;
	SBitStreamInfo sBitInfo = {0}; 
	SBitStreamInfo *psBufinfo = &sBitInfo;
	
	/*Check whether management data structure is NULL*/
	if (gi == NULL)
		return RA8_FATAL_ERR_DEC_PTR_NULL;

#ifdef RA8_DEBUG
	sRA8_Debug.iSamplesperframe=gi->sHdInfo.nSamples;
	sRA8_Debug.iNumchannels=gi->sHdInfo.nChannels;
	sRA8_Debug.iNumregions=gi->sHdInfo.nRegions;
	sRA8_Debug.iFramebits=gi->sHdInfo.nFrameBits;
	sRA8_Debug.iSampPersec=gi->sHdInfo.sampRate;
	sRA8_Debug.iCplStart=gi->sHdInfo.cplStart;
	sRA8_Debug.iCplQbits=gi->sHdInfo.cplQbits;
	sRA8_Debug.iCregions = gi->sHdInfo.cRegions;
	sRA8_Debug.iRatebits = gi->sHdInfo.rateBits;	
	sRA8_Debug.iJointStereoFlg = gi->sHdInfo.jointStereo;	
#endif

	
	/*Check whether input buffer pointer is NULL*/ 
	if( gi->sHdInfo.inBuf == NULL)
		return RA8_FATAL_ERR_INPUT_BUF_NULL;	

	/*Check whether output buffer pointer is NULL*/ 
	if(gi->sHdInfo.outbuf == NULL)
		return RA8_FATAL_ERR_OUTPUT_BUF_NULL;		
    
	/*Check for Samples per frame*/
	if ((gi->sHdInfo.nSamples != 256) && (gi->sHdInfo.nSamples != 512) 
			&& (gi->sHdInfo.nSamples != 1024))
		return RA8_FATAL_ERR_INVALID_SAMPLES_PER_FRAME;
	
	/*Check if Channels either mono or stereo or invalid*/
	if ((gi->sHdInfo.nChannels != 1) && (gi->sHdInfo.nChannels != 2))
		return RA8_FATAL_ERR_INVALID_CHANNELS;

	/*Check for Valid Frequency Regions for the Transform*/
	if ((gi->sHdInfo.nRegions >= 7) && (gi->sHdInfo.nRegions <= MAXREGNS))
	{
		if ((gi->sHdInfo.nRegions < ((560*gi->sHdInfo.nSamples)/(1024*NBINS))) 
			|| (gi->sHdInfo.nRegions > ((1024*gi->sHdInfo.nSamples)/(1024*NBINS))) )
				return RA8_FATAL_ERR_INVALID_FREQREGIONS;
	}
	else
		return RA8_FATAL_ERR_INVALID_FREQREGIONS;

	/*Check to see that Sampling rate is either 8,11.025,22.05 or 44.1 KHz*/
	if((gi->sHdInfo.sampRate != 8000) && (gi->sHdInfo.sampRate != 11025) 
			&& (gi->sHdInfo.sampRate != 22050) &&  (gi->sHdInfo.sampRate != 44100))
		return RA8_FATAL_ERR_INVALID_SAMPRATE;

	/*Check for Number of bits for Coupling Scale Fcator*/
	if ( gi->sHdInfo.cplQbits && (gi->sHdInfo.cplQbits < 2 || gi->sHdInfo.cplQbits > 6))
		return RA8_FATAL_ERR_INVALID_CPLQBITS;

	/*Check to see if Start of Coupling is between 0 and Regions */
	if((gi->sHdInfo.cplStart < 0) || (gi->sHdInfo.cplStart > gi->sHdInfo.nRegions))
		return RA8_FATAL_ERR_INVALID_CPLSTART;
	
	/*Check for Bits per Frame*/	
	if ( (gi->sHdInfo.nFrameBits < 192) || (gi->sHdInfo.nFrameBits > 2240)) 
		return RA8_FATAL_ERR_INVALID_BITS_PER_FRAME;
	
	/***** This is buffer is got into stack as input buffer is also inside *****/
	pTransfer = (int*)DMEM_DECTRAN_ADDR;

	
	dma_transfer_wait(gi->sHdInfo.inBuf/*&D_FRAME_DATA_ADDR*/,DMEM_FRAME_DATA,MAXFRAME_SIZE);
	codebuf	= DMEM_FRAME_DATA;

#ifdef __DMA_HUFFTAB__
        dma_transfer_wait(&D_HUFFTAB_SRC, &D_HUFFTAB_DST, &D_HUFFTAB_SIZ);
#endif
	
#ifdef __DMA_AUDIOTAB__
	dma_transfer_wait(&D_AUDIOTAB_SRC, &D_AUDIOTAB_DST, &D_AUDIOTAB_SIZ);
#endif

	if (lostflag) 
    {
		/* if current frame is lost, create one using interpolation - force one-sided interp to save memory */
		for (ch = 0; ch < gi->sHdInfo.nChannels; ch++) 
        {
			for (i = 0; i < gi->sHdInfo.nSamples; i++) 
            {
				gi->sDecBuf.decmlt[ch][i] = (gi->sDecBuf.decmlt[ch][i] >> 1) + (gi->sDecBuf.decmlt[ch][i] >> 2);
			}
		}
	} 
    else 
    {
	    /* current frame is valid, decode it */
		if (gi->sHdInfo.jointStereo) 
        {
			/* decode gain control info, coupling coefficients, and power envlope */
			availbits=DecodeSideInfo(gi,psBufinfo,codebuf, gi->sHdInfo.nFrameBits, 0);
			if (availbits < 0) 
			{
				Gecko2ClearBadFrame(gi, outbuf);
				if(availbits == -1)				
					return RA8_ERR_IN_SIDEINFO;				
				if(availbits == -2)
					return RA8_ERR_ATTACKS_EXCEED_IN_GAINCONTROL;
			}
			/* reconstruct power envelope */
			iRetVal = CategorizeAndExpand(gi, availbits);
#ifdef RA8_DEBUG
			for(i = 0 ; i < gi->sHdInfo.cRegions;i++)
				sRA8_Debug.iCatBuff[0][i] = sRA8_Debug.iCatBuff[1][i] = gi->sDecBuf.catbuf[i];
#endif
			if(iRetVal == RA8_ERR_IN_DECODING_POWER_ENVELOPE)
			{
				Gecko2ClearBadFrame(gi, outbuf);
				return RA8_ERR_IN_DECODING_POWER_ENVELOPE;
			}

			
#ifdef __DMA_DECTRAN__
			
			gbMin[0] = gbMin[1] = DecodeTransform(gi,psBufinfo,pTransfer, availbits, &gi->sBufferInfo.lfsr[0], 0,&ErrParam);
#ifdef RA8_DEBUG
			sRA8_Debug.igbMin[0] = sRA8_Debug.igbMin[1] = gbMin[0];
#endif
			if(ErrParam == 1)
			{
				Gecko2ClearBadFrame(gi, outbuf);
				return RA8_ERR_DECODING_HUFFMAN_VECTORS;
			}
			JointDecodeMLT(gi,pTransfer, (pTransfer+2048));			
		    dma_transfer_wait(pTransfer,gi->sDecBuf.decmlt[0],4096);
  			dma_transfer_wait((pTransfer+2048),gi->sDecBuf.decmlt[1],4096);
#else
			gbMin[0] = gbMin[1] = DecodeTransform(gi,psBufinfo, gi->sDecBuf.decmlt[0], availbits, &gi->sBufferInfo.lfsr[0], 0,&ErrParam);
#ifdef RA8_DEBUG
			sRA8_Debug.igbMin[0] = sRA8_Debug.igbMin[1] = gbMin[0];
#endif

			if(ErrParam == 1)
			{
				Gecko2ClearBadFrame(gi, outbuf);
				return RA8_ERR_DECODING_HUFFMAN_VECTORS;
			}
    		JointDecodeMLT(gi,gi->sDecBuf.decmlt[0], gi->sDecBuf.decmlt[1]);

#endif
			gi->sBufferInfo.xbits[1][1] = gi->sBufferInfo.xbits[0][1];					
#ifdef RA8_DEBUG
			sRA8_Debug.iRateCode[1] = sRA8_Debug.iRateCode[0];
			sRA8_Debug.iNats[1] = sRA8_Debug.iNats[0];
			for(i = 0;i < sRA8_Debug.iNats[0];i++)
			{
				sRA8_Debug.iLocNats[1][i] = sRA8_Debug.iLocNats[0][i];
				sRA8_Debug.iGainAttck[1][i] = sRA8_Debug.iGainAttck[0][i];
			}
			sRA8_Debug.iRmsMax[1] = sRA8_Debug.iRmsMax[0];
			for(i = 0 ; i < sRA8_Debug.iCregions;i++)
				sRA8_Debug.iRmsIndex[1][i] =sRA8_Debug.iRmsIndex[0][i]  ;
#endif
			
		} 
        else 
        {
			for (ch = 0; ch < gi->sHdInfo.nChannels; ch++) 
            {
				/* decode gain control info and power envlope */
				availbits= DecodeSideInfo(gi,psBufinfo, codebuf + (ch * gi->sHdInfo.nFrameBits >> 3), gi->sHdInfo.nFrameBits, ch);
				if (availbits < 0) 
				{
					Gecko2ClearBadFrame(gi, outbuf);
					if(availbits == -1)				
						return RA8_ERR_IN_SIDEINFO;				
					if(availbits == -2)
						return RA8_ERR_ATTACKS_EXCEED_IN_GAINCONTROL;
				}
				/* reconstruct power envelope */
				iRetVal=CategorizeAndExpand(gi, availbits);
#ifdef RA8_DEBUG
				for(i = 0; i < gi->sHdInfo.cRegions; i++)
					sRA8_Debug.iCatBuff[ch][i] = gi->sDecBuf.catbuf[i];
#endif
				if(iRetVal == RA8_ERR_IN_DECODING_POWER_ENVELOPE)
				{
					Gecko2ClearBadFrame(gi, outbuf);
					return RA8_ERR_IN_DECODING_POWER_ENVELOPE;
				}

#ifdef __DMA_DECTRAN__
				dma_transfer_wait(gi->sDecBuf.decmlt[ch], DMEM_DECTRAN_ADDR +ch*0x2000,4096);				
				/* reconstruct full MLT */
				gbMin[ch] = DecodeTransform(gi,psBufinfo,DMEM_DECTRAN_ADDR+ch*0x2000, availbits, &gi->sBufferInfo.lfsr[ch], ch,&ErrParam);
#ifdef RA8_DEBUG
				sRA8_Debug.igbMin[ch] = gbMin[ch];
#endif	
				if(ErrParam == 1)
				{
					Gecko2ClearBadFrame(gi, outbuf);
					return RA8_ERR_DECODING_HUFFMAN_VECTORS;
				}
				dma_transfer_wait(DMEM_DECTRAN_ADDR+ch*0x2000,gi->sDecBuf.decmlt[ch],4096);							 
#else
				 gbMin[ch] = DecodeTransform(gi,psBufinfo, gi->sDecBuf.decmlt[ch], availbits, &gi->sBufferInfo.lfsr[ch], ch,&ErrParam);
#ifdef RA8_DEBUG
				sRA8_Debug.igbMin[ch] = gbMin[ch];
#endif	
				 if(ErrParam == 1)
				 {
					Gecko2ClearBadFrame(gi, outbuf);
					return RA8_ERR_DECODING_HUFFMAN_VECTORS;
				 }
#endif
 				/* zero out non-coded regions */
				for (i = gi->sHdInfo.nRegions*NBINS; i < gi->sHdInfo.nSamples; i++)
					gi->sDecBuf.decmlt[ch][i] = 0;
			}
		}
		
		/* inverse transform, without window or overlap-add */
		for (ch = 0; ch < gi->sHdInfo.nChannels; ch++)
		{
#ifdef __DMA_DECMLT__
			dma_transfer_wait(gi->sDecBuf.decmlt[ch],DMEM_DECTRAN_ADDR +ch*0x2000,4096);
	
			IMLTNoWindow(gi->sBufferInfo.xformIdx,(int*)(DMEM_DECMLT_ADDR + ch*0x2000), gbMin[ch]);

			dma_transfer_wait(DMEM_DECMLT_ADDR+ch*0x2000,gi->sDecBuf.decmlt[ch],4096);
#else
			IMLTNoWindow(gi->sBufferInfo.xformIdx,gi->sDecBuf.decmlt[ch], gbMin[ch]);
#endif
				
			}
		}
#ifdef __DMA_WINTAB__
               dma_transfer_wait(&D_WINTAB_SRC, &D_WINTAB_DST, &D_WINTAB_SIZ);
#endif
		
	       for (ch = 0; ch < gi->sHdInfo.nChannels; ch++) 
	    {

		    /* apply synthesis window, gain window, then overlap-add (interleaves stereo PCM LRLR...) */
			if (gi->sGainC[ch][0].nats || gi->sGainC[ch][1].nats || gi->sBufferInfo.xbits[ch][0] || gi->sBufferInfo.xbits[ch][1])
			{

#ifdef __DMA_WINATT__
				dma_transfer_wait(gi->sDecBuf.decmlt[ch],DMEM_WINATT_ADDR, sizeof(gi->sDecBuf.decmlt[ch]));

				DecWindowWithAttacks(gi->sBufferInfo.xformIdx,dmem_decmlt/*DMEM_WINATT_ADDR*/,outbuf + ch, gi->sHdInfo.nChannels,&gi->sGainC[ch][0], &gi->sGainC[ch][1],gi->sBufferInfo.xbits[ch],&ErrParam);

#else
				DecWindowWithAttacks(gi->sBufferInfo.xformIdx, gi->sDecBuf.decmlt[ch], outbuf + ch, gi->sHdInfo.nChannels, &gi->sGainC[ch][0], &gi->sGainC[ch][1], gi->sBufferInfo.xbits[ch],&ErrParam);
#endif
#ifdef RA8_DEBUG
				sRA8_Debug.piOutPCMData[ch] = outbuf + ch;
#endif
				if(ErrParam == 1)
				{
					Gecko2ClearBadFrame(gi, outbuf);
					return RA8_ERR_DECODING_SYNTHESIS_WINDOW;
				}

			}
			else
			{
#ifdef __DMA_WINATT__
				dma_transfer_wait(gi->sDecBuf.decmlt[ch],DMEM_WINATT_ADDR, sizeof(gi->sDecBuf.decmlt[ch]));
						
				DecWindowNoAttacks(gi->sBufferInfo.xformIdx,/*DMEM_WINATT_ADDR*/dmem_decmlt,outbuf + ch, gi->sHdInfo.nChannels);
#else	
 				DecWindowNoAttacks(gi->sBufferInfo.xformIdx, gi->sDecBuf.decmlt[ch], outbuf + ch, gi->sHdInfo.nChannels);
#endif
#ifdef RA8_DEBUG
				sRA8_Debug.piOutPCMData[ch] = outbuf + ch;
#endif

			}


#ifdef __DMA_WINATT__
		dma_transfer_wait(DMEM_WINATT_ADDR+0x1000, gi->sDecBuf.decmlt[ch] + 1024, 0x1000);
#endif

		/* save gain settings for overlap */
		CopyGainInfo(&gi->sGainC[ch][0], &gi->sGainC[ch][1]);
		gi->sBufferInfo.xbits[ch][0] = gi->sBufferInfo.xbits[ch][1];
	}
	return RA8_SUCCESS;
}
void Gecko2FreeDecoder(Gecko2Info *gi)
  {
                  if (!gi)
   return;
  }

void ClearBuffer(void *buf, int nBytes)
{
      int i;
      unsigned char *cbuf = (unsigned char *)buf;
      for (i = 0; i < nBytes; i++)
               cbuf[i] = 0;
      return;
}

