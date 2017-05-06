/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ra8lbr_decode.c,v 1.2.2.1 2005/05/04 18:21:53 hubbe Exp $
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.
 * All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the Real Format Source Code
 * Porting and Optimization License, available at
 * https://helixcommunity.org/2005/license/realformatsource (unless
 * RealNetworks otherwise expressly agrees in writing that you are
 * subject to a different license).  You may also obtain the license
 * terms directly from RealNetworks.  You may not use this file except
 * in compliance with the Real Format Source Code Porting and
 * Optimization License. There are no redistribution rights for the
 * source code of this file. Please see the Real Format Source Code
 * Porting and Optimization License for the rights, obligations and
 * limitations governing use of the contents of the file.
 * 
 * RealNetworks is the developer of the Original Code and owns the
 * copyrights in the portions it created.
 * 
 * This file, and the files included with this file, is distributed and
 * made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL
 * SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT
 * OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 * https://rarvcode-tck.helixcommunity.org
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */
#include "voRADecID.h"
#include "voMem.h"
#include "helix_types.h"
#include "helix_result.h"
#include "ra8lbr_decode.h"
#include "ra_format_info.h"
#include "gecko2codec.h"

#define GECKO_VERSION               ((1L<<24)|(0L<<16)|(0L<<8)|(3L))
#define GECKO_MC1_VERSION           ((2L<<24)|(0L<<16)|(0L<<8)|(0L))

static HX_RESULT ra8lbr_unpack_opaque_data(ra8lbr_data* pData,
                                           UINT8*       pBuf,
                                           UINT32       ulLength, 
												UINT32*      numPackets);

/*
 * ra8lbr_decode_init
 */
HX_RESULT
ra8lbr_decode_init(void*              pInitParams,
                   UINT32             ulInitParamsSize,
                   ra_format_info*    pStreamInfo,
                   void**             pDecode,
                   void*              pUserMem,
                   rm_malloc_func_ptr fpMalloc,
                   rm_free_func_ptr   fpFree,
				   VO_MEM_OPERATOR	  *pMemOP,
				   VO_LIB_OPERATOR	  *pLibOper)
{
    HX_RESULT retVal = HXR_FAIL;
    ra8lbr_decode* pDec;
    ra8lbr_data unpackedData[8];
    UINT32 nChannels, frameSizeInBits;
	UINT32 numPackets = 1;

    pDec = (ra8lbr_decode*) fpMalloc(pMemOP, pUserMem, sizeof(ra8lbr_decode));

    if (pDec)
    {
        *pDecode = (void *)pDec;

		retVal = ra8lbr_unpack_opaque_data(&(unpackedData[0]),
			pStreamInfo->pOpaqueData,
			pStreamInfo->ulOpaqueDataSize,
			&numPackets);

        if (retVal == HXR_OK)
        {
            /* save the stream info and init data we'll need later */
           pDec->ulNumChannels = (UINT32)pStreamInfo->usNumChannels;
			
			if(pDec->ulNumChannels > 2)
			{
				if(unpackedData[0].channelMask == 0x00003)
					pDec->ulNumChannels = 2;
				else if(unpackedData[0].channelMask == 0x00004)
					pDec->ulNumChannels = 1;
			}

            pDec->ulFrameSize = pStreamInfo->ulBitsPerFrame;
            if(pDec->ulFrameSize)
				pDec->ulFramesPerBlock = pStreamInfo->ulGranularity / pDec->ulFrameSize;
            pDec->ulSamplesPerFrame = unpackedData[0].nSamples;
            pDec->ulSampleRate = pStreamInfo->ulSampleRate;
			pDec->ulActualRate = pStreamInfo->ulActualRate;
			pDec->ulNumPackets = numPackets;

            /* multichannel not supported, use simple logic for channel mask */
            if (pDec->ulNumChannels == 1)
                pDec->ulChannelMask = 0x00004;
            else if (pDec->ulNumChannels == 2)
                pDec->ulChannelMask = 0x00003;
            else
                pDec->ulChannelMask = 0xFFFFF;

            nChannels = pDec->ulNumChannels;
            frameSizeInBits = pDec->ulFrameSize * 8;

            /* initialize the decoder backend and save a reference to it */
            pDec->pDecoder = Gecko2InitDecoder(pDec->ulSamplesPerFrame/nChannels,
                                               nChannels,
                                               unpackedData[0].nRegions,
                                               frameSizeInBits,
                                               pDec->ulSampleRate,
                                               unpackedData[0].cplStart,
                                               unpackedData[0].cplQBits,
                                               (int*)&pDec->ulDelayFrames,
											   pMemOP);

            /* Allocate a dummy input frame for flushing the decoder */
            pDec->pFlushData = (UCHAR *) fpMalloc(pMemOP, pUserMem, pDec->ulFrameSize);

            if (pDec->pDecoder == HXNULL || pDec->pFlushData == HXNULL)
            {
                retVal = HXR_FAIL;
            }
            else
            {
				/* Set number of delay samples to discard on decoder start */
                pDec->ulDelayRemaining = pDec->ulDelayFrames * pDec->ulSamplesPerFrame;
            }
        }
    }

    return retVal;
}

HX_RESULT 
ra8lbr_decode_reset(void*   pDecode,
                    UINT16* pSamplesOut,
                    UINT32  ulNumSamplesAvail,
                    UINT32* pNumSamplesOut)
{
    HX_RESULT retVal = HXR_FAIL;
    ra8lbr_decode* pDec = (ra8lbr_decode*) pDecode;
    UINT32 n, framesToDecode;
	UINT32 i;
	INT32 firstPacketSize,nextPacketsSize;

    *pNumSamplesOut = 0;

    if (pSamplesOut != HXNULL)
    {
        framesToDecode = pDec->ulDelayFrames;
        if (framesToDecode * pDec->ulSamplesPerFrame > ulNumSamplesAvail)
            framesToDecode = ulNumSamplesAvail / pDec->ulSamplesPerFrame;

        for(n = 0; n  < framesToDecode; n++)
        {
			firstPacketSize = pDec->ulFrameSize*8;
			if(pDec->ulNumPackets > 1)
			{
				for(i=1; i<pDec->ulNumPackets; i++){
					nextPacketsSize = 2 * pDec->pFlushData[pDec->ulFrameSize - pDec->ulNumPackets + i];
					firstPacketSize -= (nextPacketsSize + 1)*8;
					if (firstPacketSize < 0) {
						return -1;
					}
				}
			}  

			retVal = Gecko2Decode(pDec->pDecoder, pDec->pFlushData,
                                  0xFFFFFFFFUL, (short*)(pSamplesOut + *pNumSamplesOut), firstPacketSize);
            *pNumSamplesOut += pDec->ulSamplesPerFrame;
        }

        /* reset the delay compensation */
        pDec->ulDelayRemaining = pDec->ulDelayFrames * pDec->ulSamplesPerFrame;
    }

    return retVal;
}

HX_RESULT
ra8lbr_decode_conceal(void* pDecode,
                      UINT32 ulNumSamples)
{
    ra8lbr_decode* pDec = (ra8lbr_decode*) pDecode;
    pDec->ulFramesToConceal = ulNumSamples / pDec->ulSamplesPerFrame;

    return HXR_OK;
}

HX_RESULT
ra8lbr_decode_decode(void*       pDecode,
                     UINT8*      pData,
                     UINT32      ulNumBytes,
                     UINT32*     pNumBytesConsumed,
                     UINT16*     pSamplesOut,
                     UINT32      ulNumSamplesAvail,
                     UINT32*     pNumSamplesOut,
                     UINT32      ulFlags)
{
	HX_RESULT retVal = HXR_FAIL;
	ra8lbr_decode* pDec = (ra8lbr_decode*) pDecode;
	UINT32 n, framesToDecode, lostFlag;
	UINT8* inBuf = pData;
	UINT16* outBuf = pSamplesOut;
	UINT32 i;
	INT32 firstPacketSize,nextPacketsSize;

    *pNumBytesConsumed = 0;
    *pNumSamplesOut = 0;

	framesToDecode = 1;

    if (pDec->ulFramesToConceal != 0)
    {
        if (pDec->ulFramesToConceal > ulNumSamplesAvail / pDec->ulSamplesPerFrame)
        {
            framesToDecode = ulNumSamplesAvail / pDec->ulSamplesPerFrame;
            pDec->ulFramesToConceal -= framesToDecode;
        }
        else
        {
            framesToDecode = pDec->ulFramesToConceal;
            pDec->ulFramesToConceal = 0;
        }

        inBuf=pDec->pFlushData;

        for (n = 0; n < framesToDecode; n++)
        {
			firstPacketSize = pDec->ulFrameSize*8;
			if(pDec->ulNumPackets > 1)
			{
				for(i=1; i<pDec->ulNumPackets; i++){
					nextPacketsSize = 2 * inBuf[pDec->ulFrameSize - pDec->ulNumPackets + i];
					firstPacketSize -= (nextPacketsSize + 1)*8;
					if (firstPacketSize < 0) {
						return -1;
					}
				}
			}  
			
			retVal = Gecko2Decode(pDec->pDecoder, inBuf, 0xFFFFFFFFUL, (short*)outBuf, firstPacketSize);

            if (retVal != 0)
            {
                retVal = HXR_FAIL;
                break;
            }

            outBuf += pDec->ulSamplesPerFrame;
            *pNumSamplesOut += pDec->ulSamplesPerFrame;
        }
    }
    else if (ulNumBytes % pDec->ulFrameSize == 0)
    {
        framesToDecode = ulNumBytes / pDec->ulFrameSize;

        if (framesToDecode > ulNumSamplesAvail / pDec->ulSamplesPerFrame)
        {
            framesToDecode = ulNumSamplesAvail / pDec->ulSamplesPerFrame;
        }

        for (n = 0; n < framesToDecode; n++)
        {
			lostFlag = !((ulFlags>>n) & 1);

			firstPacketSize = pDec->ulFrameSize*8;
			if(pDec->ulNumPackets > 1)
			{
				for(i=1; i<pDec->ulNumPackets; i++){
					nextPacketsSize = 2 * inBuf[pDec->ulFrameSize - pDec->ulNumPackets + i];
					firstPacketSize -= (nextPacketsSize + 1)*8;
					if (firstPacketSize < 0) {
						return -1;
					}
				}
			}  
            retVal = Gecko2Decode(pDec->pDecoder, inBuf, lostFlag, (short*)outBuf, firstPacketSize);

            if (retVal != 0)
            {
                retVal = HXR_FAIL;
                break;
            }

            inBuf += pDec->ulFrameSize;
            *pNumBytesConsumed += pDec->ulFrameSize;
            outBuf += pDec->ulSamplesPerFrame;
            *pNumSamplesOut += pDec->ulSamplesPerFrame;
        }
    }

    /* Discard invalid output samples */
    if (retVal == HXR_FAIL) /* decoder error */
    {
        *pNumSamplesOut = 0;
        /* protect consumer ears by zeroing the output buffer,
           just in case the error return code is disregarded. */
		for (n = 0; n < pDec->ulSamplesPerFrame * framesToDecode; n++)
			pSamplesOut[n] = 0;
    }
    else if (pDec->ulDelayRemaining > 0) /* delay samples */
    {
        if (pDec->ulDelayRemaining >= *pNumSamplesOut)
        {
            pDec->ulDelayRemaining -= *pNumSamplesOut;
            *pNumSamplesOut = 0;
        }
        else
        {
			int samples;
			short *srcSample = (short *)(pSamplesOut + pDec->ulDelayRemaining);
            *pNumSamplesOut -= pDec->ulDelayRemaining;

			samples = *pNumSamplesOut/sizeof(short);

			for (n = 0; n < samples; n++)
				pSamplesOut[n] = srcSample[n];
			
            pDec->ulDelayRemaining = 0;
        }
    }

    return retVal;
}

HX_RESULT
ra8lbr_decode_getmaxsize(void*   pDecode,
                         UINT32* pNumSamples)
{
    ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
    *pNumSamples = pDec->ulSamplesPerFrame * pDec->ulFramesPerBlock;

    return HXR_OK;
}

HX_RESULT
ra8lbr_decode_getchannels(void*   pDecode,
                          UINT32* pNumChannels)
{
    ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
    *pNumChannels = pDec->ulNumChannels;

    return HXR_OK;
}

HX_RESULT
ra8lbr_decode_getchannelmask(void*   pDecode,
                             UINT32* pChannelMask)
{
    ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
    *pChannelMask = pDec->ulChannelMask;
    return HXR_OK;
}

HX_RESULT
ra8lbr_decode_getrate(void*   pDecode,
                      UINT32* pSampleRate)
{
    ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
	if(pDec->ulActualRate && pDec->ulActualRate != pDec->ulSampleRate)
		*pSampleRate = pDec->ulActualRate;
	else
		*pSampleRate = pDec->ulSampleRate;
    return HXR_OK;
}

HX_RESULT
ra8lbr_decode_getdelay(void*   pDecode,
                       UINT32* pNumSamples)
{
    //ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
    /* delay compensation is handled internally */
    *pNumSamples = 0;

    return HXR_OK;
}

HX_RESULT 
ra8lbr_decode_close(void* pDecode,
                    void* pUserMem,
                    rm_free_func_ptr fpFree, 
					VO_MEM_OPERATOR	  *pMemOP)
{
    ra8lbr_decode* pDec = (ra8lbr_decode *)pDecode;
    /* free the ra8lbr decoder */
    if (pDec->pDecoder)
        Gecko2FreeDecoder(pDec->pDecoder, pMemOP);
    /* free the dummy input buffer */
    if (pDec->pFlushData)
        fpFree(pMemOP, pUserMem, pDec->pFlushData);
    /* free the ra8lbr backend */
    fpFree(pMemOP, pUserMem, pDec);

    return HXR_OK;
}

HX_RESULT
ra8lbr_unpack_opaque_data(ra8lbr_data* ppData,
                          UINT8*       pBuf,
                          UINT32       ulLength,
    						 UINT32*      numPackets)
{
    HX_RESULT retVal = HXR_FAIL;
    UINT8* off = pBuf;

	ra8lbr_data* pData = ppData;
	
	*numPackets = 0;

    while (pBuf != HXNULL && ulLength != 0)
    {
        retVal = HXR_OK;

		if(ulLength >= 8) {
			pData->version = ((INT32)*off++)<<24;
			pData->version |= ((INT32)*off++)<<16;
			pData->version |= ((INT32)*off++)<<8;
			pData->version |= ((INT32)*off++);

			pData->nSamples = *off++<<8;
			pData->nSamples |= *off++;

			pData->nRegions = *off++<<8;
			pData->nRegions |= *off++;
			
			ulLength -= 8;
		}

        if (pData->version >= GECKO_VERSION && ulLength >= 8)
        {
            pData->delay = ((INT32)*off++)<<24;
            pData->delay |= ((INT32)*off++)<<16;
            pData->delay |= ((INT32)*off++)<<8;
            pData->delay |= ((INT32)*off++);

            pData->cplStart = *off++<<8;
            pData->cplStart |= *off++;

            pData->cplQBits = *off++<<8;
            pData->cplQBits |= *off++;

			 ulLength -= 8;
        }
        else
        {
            /* the fixed point ra8lbr decoder supports dual-mono decoding with
               a single decoder instance if cplQBits is set to zero. */
            pData->cplStart = 0;
            pData->cplQBits = 0;
        }

        if (pData->version == GECKO_MC1_VERSION)
        {
            //retVal = HXR_UNSUPPORTED_AUDIO;
            /* MultiChannel not supported! */
			if(ulLength >= 4) {     
				pData->channelMask = ((INT32)*off++)<<24;
				pData->channelMask |= ((INT32)*off++)<<16;
				pData->channelMask |= ((INT32)*off++)<<8;
				pData->channelMask |= ((INT32)*off++);

				ulLength -= 4;
			}            
        }

		pData++;

		*numPackets += 1;
    }

    return retVal;
}
