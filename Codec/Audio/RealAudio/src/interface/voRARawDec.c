
#include		"coder.h"
#include        "VORA_sdk.h"

#ifndef _IOS
#include        <malloc.h>
#endif

typedef struct  
{
	HGecko2Decoder lbrDec;
	int codingDelay;
}VORA_RAWParam;

VO_U32 VO_API voRealAudioRAWDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_AUDIO_CODECAPI	*DecAPI;
	VORA_SDK* pDec =(VORA_SDK*)(*phCodec);
	VO_MEM_OPERATOR	  *pMemOP;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	DecAPI = &(pDec->RealDecAPI);
	pMemOP = pDec->pvoMemop;

	DecAPI->Init			= voRealAudioRAWDecInit;
	DecAPI->Uninit			= voRealAudioRAWDecUninit;
	DecAPI->SetInputData	= NULL;
	DecAPI->GetOutputData	= voRealAudioRAWDecGetOutputData;
	DecAPI->SetParam		= voRealAudioRAWDecSetParameter;
	DecAPI->GetParam		= voRealAudioRAWDecGetParameter;
	pDec->customData	= (void*)malloc(sizeof(VORA_RAWParam));
	{
		//int ra_version = pDec->rawParam.version;
		VORA_RAWParam* param = (VORA_RAWParam*)pDec->customData;
		int nSamples	= pDec->rawParam.samplesPerFrame;
		int nChannels	= pDec->rawParam.channelNum;
		int sampRate	= pDec->rawParam.sample_rate;
		int	nFrameBits	= pDec->rawParam.frameSizeInBits;
		int nRegions	= pDec->rawParam.region;
		//int	codingDelay = 0;
		int	cplStart	= pDec->rawParam.cplStart;
		int	cplQbits	= pDec->rawParam.cplQbits;
		param->lbrDec=Gecko2InitDecoder(nSamples/nChannels, 
				nChannels, nRegions, nFrameBits, sampRate, 
				cplStart, cplQbits,	(int *)&(param->codingDelay), pMemOP);
		
		if(param->lbrDec==0)
		return VO_ERR_OUTOF_MEMORY;
	}
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRAWDecUninit(VO_HANDLE hCodec)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	VO_MEM_OPERATOR	  *pMemOP;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	
	pMemOP = pDec->pvoMemop;

	if(pDec->customData)
	{
		VORA_RAWParam* param = (VORA_RAWParam*)pDec->customData;
		Gecko2FreeDecoder(param->lbrDec, pMemOP);
		rm_memory_free(pMemOP, NULL, pDec->customData);
		pDec->customData = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRAWDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RAWParam* param;
	int returnCode;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RAWParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;

	if(pDec->inbuf == NULL || pDec->inlength <= 0)
		return -3;

	returnCode = Gecko2Decode(param->lbrDec, pDec->inbuf, 0, (short*)(pOutBuffer->Buffer), pDec->inlength*8);

	if(returnCode<0)
	{
		return returnCode;
	}

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = pDec->rawParam.channelNum;
		pOutInfo->Format.SampleRate = pDec->rawParam.sample_rate;
		pOutInfo->Format.SampleBits = 16;

		pOutInfo->InputUsed = pDec->inlength;
	}

	pDec->inbuf += pDec->inlength;
	pDec->inlength = 0;
	if(param->codingDelay>0)
	{
		param->codingDelay--;
		return -101;
	}
	else
	{

		return VO_ERR_NONE;
	}
}

VO_U32 VO_API voRealAudioRAWDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RAWParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RAWParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;
	//TODO:
	switch(uParamID)
	{
	case 1:
		break;
	default:
	    return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRAWDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RAWParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RAWParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;
	//TODO:
	switch(uParamID)
	{
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}
