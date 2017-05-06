#include "voCheck.h"
#include "voChHdle.h"
#include "voAAC.h"
#include "typedef.h"
#include "aacenc_core.h"
#include "aac_rom.h"
#include "cmnMemory.h"
#include "mem_align.h"

const int	rates[][2] = {		
	{160,35},{240,60},{320,90},
	{400,120},{480,140},{560,160},
	{640,200},{0, 0}
};

#if defined (LCHECK) 
VO_PTR	g_hAACEncInst = NULL;
#endif

VO_U32 VO_API voAACEncInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData)
{
	AAC_ENCODER*hAacEnc;
	int error;

	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem;

	interMem = 0;
	error = 0;

	if(pUserData == NULL || (pUserData->memflag&0x0f) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;

		interMem = 1;

		pMemOP = &voMemoprator;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

	hAacEnc = (AAC_ENCODER*)mem_malloc(pMemOP, sizeof(AAC_ENCODER), 32);
	if(NULL == hAacEnc)
	{
		error = 1;
	}

	if(!error)
	{
		hAacEnc->intbuf = (short *)mem_malloc(pMemOP, AACENC_BLOCKSIZE*MAX_CHANNELS*sizeof(short), 32);
		if(NULL == hAacEnc->intbuf)
		{
			error = 1;
		}
	}

	if (!error) {
		error = (PsyNew(&hAacEnc->psyKernel, MAX_CHANNELS, pMemOP) ||
			PsyOutNew(&hAacEnc->psyOut, pMemOP));
	}

	if (!error) {
		error = QCOutNew(&hAacEnc->qcOut,MAX_CHANNELS, pMemOP);
	}

	if (!error) {
		error = QCNew(&hAacEnc->qcKernel, pMemOP);
	}

	if(error)
	{
		AacEncClose(hAacEnc, pMemOP);
		if(hAacEnc)
		{
			mem_free(pMemOP, hAacEnc);
			hAacEnc = NULL;
		}		
		*phCodec = NULL;
		return VO_ERR_OUTOF_MEMORY;
	}

	if(interMem)
	{
		hAacEnc->voMemoprator.Alloc = cmnMemAlloc;
		hAacEnc->voMemoprator.Copy = cmnMemCopy;
		hAacEnc->voMemoprator.Free = cmnMemFree;
		hAacEnc->voMemoprator.Set = cmnMemSet;
		hAacEnc->voMemoprator.Check = cmnMemCheck;

		pMemOP = &hAacEnc->voMemoprator;
	}

	if(hAacEnc->initOK == 0)
	{
		 AACENC_CONFIG config;
		 config.adtsUsed = 1;
		 config.bitRate = 128000;
		 config.nChannelsIn = 2;
		 config.nChannelsOut = 2;
		 config.sampleRate = 44100;
		 config.bandWidth = 20000;

		 AacEncOpen(hAacEnc, config);
	}

	hAacEnc->voMemop = pMemOP;

	*phCodec = hAacEnc;

#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		voCheckLibInit (&(hAacEnc->hCheck), VO_INDEX_ENC_AAC, 1, g_hAACEncInst,pUserData->libOperator);
	else
		voCheckLibInit (&(hAacEnc->hCheck), VO_INDEX_ENC_AAC, 1, g_hAACEncInst,NULL);
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACEncSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	AAC_ENCODER *hAacEnc;
	int  length;

	if(NULL == hCodec || NULL == pInput || NULL == pInput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	hAacEnc = (AAC_ENCODER *)hCodec;
	
	hAacEnc->inbuf = (short *)pInput->Buffer;
	hAacEnc->inlen = pInput->Length / sizeof(short);
	hAacEnc->uselength = 0;

	hAacEnc->encbuf = hAacEnc->inbuf;
	hAacEnc->enclen = hAacEnc->inlen;
	
	if(hAacEnc->intlen)
	{
		length = min(hAacEnc->config.nChannelsIn*AACENC_BLOCKSIZE - hAacEnc->intlen, hAacEnc->inlen);
		hAacEnc->voMemop->Copy(VO_INDEX_ENC_AAC, hAacEnc->intbuf + hAacEnc->intlen, 
			hAacEnc->inbuf, length*sizeof(short));

		hAacEnc->encbuf = hAacEnc->intbuf;
		hAacEnc->enclen = hAacEnc->intlen + length;

		hAacEnc->inbuf += length;
		hAacEnc->inlen -= length;
	}
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voAACEncGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	AAC_ENCODER* hAacEnc = (AAC_ENCODER*)hCodec;
	Word16 numAncDataBytes=0;
	Word32  inbuflen;
	int length;
	if(NULL == hAacEnc)
		return VO_ERR_INVALID_ARG;

	 inbuflen = AACENC_BLOCKSIZE*hAacEnc->config.nChannelsIn;

	 if(NULL == hAacEnc->encbuf || hAacEnc->enclen < inbuflen)
	 {
		length = hAacEnc->enclen;		
		if(hAacEnc->intlen == 0)
		{	
			hAacEnc->voMemop->Copy(VO_INDEX_ENC_AAC, hAacEnc->intbuf, 
				hAacEnc->encbuf, length*sizeof(short));		
			hAacEnc->uselength += length*sizeof(short);
		}
		else
		{
			hAacEnc->uselength += (length - hAacEnc->intlen)*sizeof(short);
		}

		hAacEnc->intlen = length;

		pOutput->Length = 0;
		if(pOutInfo)
			pOutInfo->InputUsed = hAacEnc->uselength;
		return VO_ERR_INPUT_BUFFER_SMALL;	
	 }

	 if(NULL == pOutput || NULL == pOutput->Buffer || pOutput->Length < (6144/8)*hAacEnc->config.nChannelsOut/(sizeof(Word32)))
		 return VO_ERR_OUTPUT_BUFFER_SMALL;

	 AacEncEncode( hAacEnc,
			(Word16*)hAacEnc->encbuf,
			NULL,
			&numAncDataBytes,
			pOutput->Buffer,
			&pOutput->Length);

	 if(hAacEnc->intlen)
	 {
		length = inbuflen - hAacEnc->intlen;		
		hAacEnc->encbuf = hAacEnc->inbuf;
		hAacEnc->enclen = hAacEnc->inlen;
		hAacEnc->uselength += length*sizeof(short);
		hAacEnc->intlen = 0;
	 }
	 else
	 {
		 hAacEnc->encbuf = hAacEnc->encbuf + inbuflen;
		 hAacEnc->enclen = hAacEnc->enclen - inbuflen;
		 hAacEnc->uselength += inbuflen*sizeof(short);
	 }	 

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = hAacEnc->config.nChannelsOut;
		pOutInfo->Format.SampleRate = hAacEnc->config.sampleRate;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->InputUsed = hAacEnc->uselength;
	}

#if defined (LCHECK)
	voCheckLibCheckAudio(hAacEnc->hCheck, pOutput, &(pOutInfo->Format));
#endif

	 return VO_ERR_NONE;
}

VO_U32 VO_API voAACEncUninit(VO_HANDLE hCodec)
{
	AAC_ENCODER* hAacEnc = (AAC_ENCODER*)hCodec;
	
	if(NULL != hAacEnc)
	{
		AacEncClose(hAacEnc, hAacEnc->voMemop);

#if defined (LCHECK)
		voCheckLibUninit(hAacEnc->hCheck);
#endif		
		mem_free(hAacEnc->voMemop, hAacEnc);
		hAacEnc = NULL;
	}	

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACEncSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	AACENC_CONFIG config;
	AACENC_PARAM* pAAC_param;
	VO_AUDIO_FORMAT *pWAV_Format;
	AAC_ENCODER* hAacEnc = (AAC_ENCODER*)hCodec;
	int ret, i, bitrate;

	if(NULL == hAacEnc)
		return VO_ERR_INVALID_ARG;
	
	switch(uParamID)
	{
	case VO_PID_AAC_ENCPARAM:
		AacInitDefaultConfig(&config);
		if(pData == NULL)
			return VO_ERR_INVALID_ARG;
		pAAC_param = (AACENC_PARAM*)pData;
		config.adtsUsed = pAAC_param->adtsUsed;
		config.bitRate = pAAC_param->bitRate;
		config.nChannelsIn = pAAC_param->nChannels;
		config.nChannelsOut = pAAC_param->nChannels;
		config.sampleRate = pAAC_param->sampleRate;

		if(config.nChannelsIn< 1  || config.nChannelsIn > MAX_CHANNELS  ||
             config.nChannelsOut < 1 || config.nChannelsOut > MAX_CHANNELS || config.nChannelsIn < config.nChannelsOut)
			 return VO_ERR_AUDIO_UNSCHANNEL;

		ret = -1;
		for(i = 0; i < NUM_SAMPLE_RATES; i++)
		{
			if(config.sampleRate == sampRateTab[i])
			{
				ret = 0;
				break;
			}
		}
		if(ret < 0)
			return VO_ERR_AUDIO_UNSSAMPLERATE;

		if(((config.bitRate!=0) && (config.bitRate/config.nChannelsOut < 8000)) ||
           (config.bitRate/config.nChannelsOut > 160000) || 
		   (config.bitRate > config.sampleRate*6*config.nChannelsOut))
			return VO_ERR_AUDIO_UNSFEATURE;


		bitrate = config.bitRate / config.nChannelsOut;
		bitrate = bitrate * 441 / config.sampleRate;

		for (i = 0; rates[i][0]; i++)
		{
			if (rates[i][0] >= bitrate)
				break;
		}

		config.bandWidth = rates[i][1];

		config.bandWidth = config.bandWidth * config.sampleRate / 441;

		if (config.bandWidth > 20000 || !config.bandWidth)
		  config.bandWidth = 20000;

		ret = AacEncOpen(hAacEnc, config);
		if(ret) 
			return VO_ERR_AUDIO_UNSFEATURE;
		break;
	case VO_PID_AUDIO_FORMAT:
		AacInitDefaultConfig(&config);
		if(pData == NULL)
			return VO_ERR_INVALID_ARG;
		pWAV_Format = (VO_AUDIO_FORMAT*)pData;
		config.adtsUsed = 1;
		config.nChannelsIn = pWAV_Format->Channels;
		config.nChannelsOut = pWAV_Format->Channels;
		config.sampleRate = pWAV_Format->SampleRate;

		if(config.nChannelsIn< 1  || config.nChannelsIn > MAX_CHANNELS  ||
             config.nChannelsOut < 1 || config.nChannelsOut > MAX_CHANNELS || config.nChannelsIn < config.nChannelsOut)
			 return VO_ERR_AUDIO_UNSCHANNEL;

		if(pWAV_Format->SampleBits != 16)
		{
			return VO_ERR_AUDIO_UNSFEATURE;
		}

		ret = -1;
		for(i = 0; i < NUM_SAMPLE_RATES; i++)
		{
			if(config.sampleRate == sampRateTab[i])
			{
				ret = 0;
				break;
			}
		}
		if(ret < 0)
			return VO_ERR_AUDIO_UNSSAMPLERATE;

		config.bitRate = 64000*(config.sampleRate/100)/441*config.nChannelsOut;

		if(((config.bitRate!=0) && (config.bitRate/config.nChannelsOut < 8000)) ||
           (config.bitRate/config.nChannelsOut > 160000) || 
		   (config.bitRate > config.sampleRate*6*config.nChannelsOut))
			return VO_ERR_AUDIO_UNSFEATURE;


		bitrate = config.bitRate / config.nChannelsOut;
		bitrate = bitrate * 441 / config.sampleRate;

		for (i = 0; rates[i][0]; i++)
		{
			if (rates[i][0] >= bitrate)
				break;
		}

		config.bandWidth = rates[i][1];

		config.bandWidth = config.bandWidth * config.sampleRate / 441;

		if (config.bandWidth > 20000 || !config.bandWidth)
		  config.bandWidth = 20000;

		ret = AacEncOpen(hAacEnc, config);
		if(ret) 
			return VO_ERR_AUDIO_UNSFEATURE;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACEncGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAACEncAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voAACEncInit;
	pDecHandle->SetInputData = voAACEncSetInputData;
	pDecHandle->GetOutputData = voAACEncGetOutputData;
	pDecHandle->SetParam = voAACEncSetParam;
	pDecHandle->GetParam = voAACEncGetParam;
	pDecHandle->Uninit = voAACEncUninit;

	return VO_ERR_NONE;
}