/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: aac_decode.c,v 1.2.2.1 2005/05/04 18:21:58 hubbe Exp $
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

//#include "helix_types.h"
//#include "helix_result.h"
#include "aac_decode.h"
#include "ra_format_info.h"
#include "voAAC.h"
#include "ga_config.h"
#include "aac_bitstream.h"


/*
 * aac_decode_init
 */
HX_RESULT
aac_decode_init(void*              pInitParams,
                UINT32             ulInitParamsSize,
                ra_format_info*    pStreamInfo,
                void**             pDecode,
                void*              pUserMem,
                rm_malloc_func_ptr fpMalloc,
                rm_free_func_ptr   fpFree,
				VO_MEM_OPERATOR	   *pMemOP,
				VO_LIB_OPERATOR	  *pLibOper)
{
    HX_RESULT result = HXR_OK;
    aac_decode* pDec;
    VO_AUDIO_CODECAPI *pAudioAPI;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
	//VO_AUDIO_FORMAT outFormat;	
    ga_config_data configData;
    UCHAR *pData;
    UCHAR *inBuf;
    INT16 *temp;
    UINT32 numBytes, numBits;
    UINT32 cfgType;
	int returnCode;
	int frameType = VOAAC_RAWDATA;

    /* for MP4 bitstream parsing */
    struct BITSTREAM *pBs = 0;

    /* allocate the aac_decode struct */
    pDec = (aac_decode*) fpMalloc(pMemOP, pUserMem, sizeof(aac_decode));
    if (pDec == HXNULL)
    {
        return HXR_OUTOFMEMORY;
    }

    *pDecode = (void *)pDec;

    /* allocate the frame info struct */
    pDec->pAudioAPI = fpMalloc(pMemOP, pUserMem, sizeof(VO_AUDIO_CODECAPI));
    pAudioAPI = (VO_AUDIO_CODECAPI *) pDec->pAudioAPI;
    /* allocate the decoder backend instance */
    returnCode  = -1;//voGetAACDecAPI(pAudioAPI);
    if (pDec->pAudioAPI == HXNULL)
    {
        return HXR_OUTOFMEMORY;
    }

    /* save the stream info and init data we'll need later */
    pDec->ulNumChannels = (UINT32)pStreamInfo->usNumChannels;
    pDec->ulBlockSize = pStreamInfo->ulGranularity;
    pDec->ulFrameSize = pStreamInfo->ulBitsPerFrame;
    pDec->ulFramesPerBlock = pDec->ulBlockSize / pDec->ulFrameSize;
    /* output frame size is doubled for safety in case of implicit SBR */
    pDec->ulSamplesPerFrame = 1024*pStreamInfo->usNumChannels*2;
    pDec->ulSampleRateCore = pStreamInfo->ulSampleRate;
    pDec->ulSampleRateOut  = pStreamInfo->ulActualRate;
    if (pStreamInfo->ulOpaqueDataSize < 1)
    {
        return HXR_FAIL; /* insufficient config data */
    }

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = pMemOP;
	if(pLibOper)
	{
		useData.memflag |= 0x10;
		useData.libOperator = pLibOper;
	}

	returnCode = pAudioAPI->Init(&(pDec->pDecoder), VO_AUDIO_CodingAAC, &useData);

	hCodec = pDec->pDecoder;

    /* get the config data */
    pData = (UCHAR *)pStreamInfo->pOpaqueData;
    cfgType = pData[0];
    inBuf = pData + 1;
    numBytes = pStreamInfo->ulOpaqueDataSize - 1;

    if (cfgType == 1) /* ADTS Frame */
    {
        /* allocate temp buffer for decoding first ADTS frame */
        frameType = VOAAC_ADTS;
		
		temp = (INT16 *)fpMalloc(pMemOP, pUserMem, sizeof(INT16) * pDec->ulSamplesPerFrame);
        if (temp == HXNULL)
        {
            return HXR_OUTOFMEMORY;
        }
        else
        {
			inData.Buffer = inBuf;
			inData.Length = numBytes;

			outData.Buffer = (UINT8 *)temp;
			outData.Length = sizeof(INT16) * pDec->ulSamplesPerFrame;

			returnCode = pAudioAPI->SetInputData(hCodec, &inData);
			if(returnCode)
				return HXR_FAIL;
			
			returnCode = pAudioAPI->GetOutputData(hCodec,&outData, &outInfo);

           /* free the temp buffer */
            fpFree(pMemOP, pUserMem, temp);
        }

        if (returnCode == 0)
        {
            
			pDec->ulNumChannels = outInfo.Format.Channels;
            pDec->ulSamplesPerFrame = outData.Length / (sizeof(short));
            pDec->ulSampleRateOut = outInfo.Format.SampleRate;	
			if(pDec->ulSamplesPerFrame/pDec->ulNumChannels == 1024)
				pDec->ulSampleRateCore = pDec->ulSampleRateOut;
			else
				pDec->ulSampleRateCore = pDec->ulSampleRateOut >> 1;
            pDec->bSBR = (pDec->ulSampleRateCore != pDec->ulSampleRateOut);
        }
    }
    else if (cfgType == 2) /* MP4 Audio Specific Config Data */
    {
		numBits = numBytes*8;

        if (newBitstream(&pBs, numBits, pUserMem, fpMalloc, pMemOP))
            return HXR_FAIL;

        feedBitstream(pBs, (const UCHAR *)inBuf, numBits);
        setAtBitstream(pBs, 0, 1);
        result = ga_config_get_data(pBs, &configData);
        deleteBitstream(pBs, pUserMem, fpFree, pMemOP);
        if (result != HXR_OK) /* config data error */
        {
            return HXR_FAIL;
        }

        pDec->ulNumChannels = configData.numChannels;
        pDec->ulSampleRateCore = configData.samplingFrequency;
        pDec->ulSampleRateOut = configData.extensionSamplingFrequency;
        pDec->bSBR = configData.bSBR;

        /*  ulSamplesPerFrame is set to the maximum possible output length.
         *  The config data has the initial output length, which might
         *  be doubled once the first frame is handed in (if AAC+ is
         *  signalled implicitly).
         */
        pDec->ulSamplesPerFrame = 2*configData.frameLength*configData.numChannels;		
    }
    else /* unsupported config type */
    {
            return HXR_FAIL;
    }

    /* make certain that all the channels can be handled */
    if (pDec->ulNumChannels > AAC_MAX_NCHANS) {
        return HXR_UNSUPPORTED_AUDIO;
    }

    /* set the channel mask - custom maps not supported */
    switch (pDec->ulNumChannels) {
    case  1: pDec->ulChannelMask = 0x00004; break;/* FC                */
    case  2: pDec->ulChannelMask = 0x00003; break;/* FL,FR             */
    case  3: pDec->ulChannelMask = 0x00007; break;/* FL,FR,FC          */
    case  4: pDec->ulChannelMask = 0x00107; break;/* FL,FR,FC,BC       */
    case  5: pDec->ulChannelMask = 0x00037; break;/* FL,FR,FC,BL,BR    */
    case  6: pDec->ulChannelMask = 0x0003F; break;/* FL,FR,FC,LF,BL,BR */
    default: pDec->ulChannelMask = 0xFFFFF; break;/* Unknown           */
    }

    /* set the delay samples */
    pDec->ulDelayRemaining = pDec->ulSamplesPerFrame;
	
	pAudioAPI->SetParam(hCodec,VO_PID_AAC_FRAMETYPE,&frameType);
	pAudioAPI->SetParam(hCodec,VO_PID_AUDIO_CHANNELS,&(pDec->ulNumChannels));
	pAudioAPI->SetParam(hCodec,VO_PID_AUDIO_SAMPLEREATE,&(pDec->ulSampleRateCore));

    return HXR_OK;
}

HX_RESULT 
aac_decode_reset(void*   pDecode,
                 UINT16* pSamplesOut,
                 UINT32  ulNumSamplesAvail,
                 UINT32* pNumSamplesOut)
{
    int reflush;
	aac_decode* pDec = (aac_decode*) pDecode;
	VO_AUDIO_CODECAPI *pAudioAPI = (VO_AUDIO_CODECAPI *) pDec->pAudioAPI;

	reflush = 1;

	pAudioAPI->SetParam(pDec->pDecoder,VO_PID_COMMON_FLUSH, &reflush);
    *pNumSamplesOut = 0;
    pDec->ulSamplesToConceal = 0;

    /* reset the delay compensation */
    pDec->ulDelayRemaining = pDec->ulSamplesPerFrame;

    return HXR_OK;
}

HX_RESULT
aac_decode_conceal(void* pDecode,
                   UINT32 ulNumSamples)
{
    aac_decode* pDec = (aac_decode*) pDecode;
    if (pDec->bSBR)
        pDec->ulSamplesToConceal = (ulNumSamples + 2*AAC_MAX_NSAMPS - 1) / (2*AAC_MAX_NSAMPS);
    else
        pDec->ulSamplesToConceal = (ulNumSamples + AAC_MAX_NSAMPS - 1) / AAC_MAX_NSAMPS;

    return HXR_OK;
}

HX_RESULT
aac_decode_decode(void*       pDecode,
                  UINT8*      pData,
                  UINT32      ulNumBytes,
                  UINT32*     pNumBytesConsumed,
                  UINT16*     pSamplesOut,
                  UINT32      ulNumSamplesAvail,
                  UINT32*     pNumSamplesOut,
                  UINT32      ulFlags)
{
    HX_RESULT retVal = HXR_FAIL;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
    aac_decode* pDec = (aac_decode*) pDecode;
    UINT32 lostFlag, maxSamplesOut, n;
	int returnCode;
    UINT32 ulNumBytesRemaining;
    UINT8* inBuf = pData;
    UINT16* outBuf = pSamplesOut;
	VO_AUDIO_CODECAPI *pAudioAPI = (VO_AUDIO_CODECAPI *) pDec->pAudioAPI;

    ulNumBytesRemaining = ulNumBytes;
    *pNumBytesConsumed = 0;

    lostFlag = !(ulFlags & 1);

    if (pDec->ulSamplesToConceal || lostFlag)
    {
        int nsample;
		if (lostFlag) /* conceal one frame */
            *pNumSamplesOut = pDec->ulSamplesPerFrame;
        else
        {
            maxSamplesOut = pDec->ulSamplesPerFrame * pDec->ulFramesPerBlock;
            *pNumSamplesOut = maxSamplesOut;
            if (pDec->ulSamplesToConceal < maxSamplesOut)
                *pNumSamplesOut = pDec->ulSamplesToConceal;
            pDec->ulSamplesToConceal -= *pNumSamplesOut;
        }
        /* just fill with silence */
		for(nsample = 0; nsample < *pNumSamplesOut; nsample++)
		{
			pSamplesOut[nsample] = 0;
		}
        
		nsample = 1;

		pAudioAPI->SetParam(pDec->pDecoder,VO_PID_COMMON_FLUSH, &nsample);

        *pNumBytesConsumed = 0;
        return HXR_OK;
    }

	inData.Buffer = inBuf;
	inData.Length = ulNumBytes;

	outData.Buffer = (VO_PBYTE)outBuf;
	outData.Length = *pNumSamplesOut;

	returnCode = pAudioAPI->SetInputData(pDec->pDecoder, &inData);
	
	returnCode = pAudioAPI->GetOutputData(pDec->pDecoder,&outData, &outInfo);

    if (returnCode == VO_ERR_NONE)
    {
        pDec->ulSampleRateCore =  outInfo.Format.SampleRate;
        pDec->ulSampleRateOut = pDec->ulSampleRateCore;
        pDec->ulNumChannels = outInfo.Format.Channels;
        *pNumSamplesOut = outData.Length/2;
        *pNumBytesConsumed = outInfo.InputUsed;
		ulNumBytesRemaining = ulNumBytes - outInfo.InputUsed;
		
        retVal = HXR_OK;
    }
    else if (retVal == VO_ERR_INPUT_BUFFER_SMALL)
    {
        *pNumBytesConsumed = ulNumBytes;
        *pNumSamplesOut = 0;
        retVal = HXR_OK;
    }
    else
    {
        *pNumSamplesOut = 0;
        retVal = HXR_FAIL;
    }
    
    /* Zero out invalid output samples */
    if (*pNumSamplesOut == 0)
    {
		for (n = 0; n < pDec->ulSamplesPerFrame; n++)
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
aac_decode_getmaxsize(void*   pDecode,
                      UINT32* pNumSamples)
{
    aac_decode* pDec = (aac_decode *)pDecode;
    *pNumSamples = pDec->ulSamplesPerFrame * pDec->ulFramesPerBlock;

    return HXR_OK;
}

HX_RESULT
aac_decode_getchannels(void*   pDecode,
                       UINT32* pNumChannels)
{
    aac_decode* pDec = (aac_decode *)pDecode;
    *pNumChannels = pDec->ulNumChannels;

    return HXR_OK;
}

HX_RESULT
aac_decode_getchannelmask(void*   pDecode,
                          UINT32* pChannelMask)
{
    aac_decode* pDec = (aac_decode *)pDecode;
    *pChannelMask = pDec->ulChannelMask;

    return HXR_OK;
}

HX_RESULT
aac_decode_getrate(void*   pDecode,
                   UINT32* pSampleRate)
{
    aac_decode* pDec = (aac_decode *)pDecode;
    *pSampleRate = pDec->ulSampleRateOut;

    return HXR_OK;
}

HX_RESULT
aac_decode_getdelay(void*   pDecode,
                    UINT32* pNumSamples)
{
    //aac_decode* pDec = (aac_decode *)pDecode;
    /* delay compensation is handled internally */
    *pNumSamples = 0;

    return HXR_OK;
}

HX_RESULT 
aac_decode_close(void* pDecode,
                 void* pUserMem,
                 rm_free_func_ptr fpFree,
				 VO_MEM_OPERATOR  *pMemOP)
{
    aac_decode* pDec = (aac_decode *)pDecode;
	VO_AUDIO_CODECAPI *pAudioAPI = (VO_AUDIO_CODECAPI *) pDec->pAudioAPI;

    /* free the aac decoder */
    if (pDec->pDecoder)
    {
        pAudioAPI->Uninit(pDec->pDecoder);
        pDec->pDecoder = HXNULL;
    }
    /* free the frame info struct */
    if (pDec->pAudioAPI)
    {
        fpFree(pMemOP, pUserMem, pDec->pAudioAPI);
        pDec->pAudioAPI = HXNULL;
    }
    /* free the aac backend */
    fpFree(pMemOP, pUserMem, pDec);
    pDec = HXNULL;

    return HXR_OK;
}
