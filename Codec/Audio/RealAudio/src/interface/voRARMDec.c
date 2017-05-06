
#include		"ra_decode.h"
#include		"ra_depack.h"
#include        "VORA_sdk.h"

VO_U32 VO_API voRealAudioRMFmtDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_AUDIO_CODECAPI	*DecAPI;
	VORA_SDK* pDec =(VORA_SDK*)(*phCodec);
	VO_MEM_OPERATOR	*pMemOP;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	DecAPI = &(pDec->RealDecAPI);
	pMemOP = pDec->pvoMemop;

	DecAPI->Init			= voRealAudioRMFmtDecInit;
	DecAPI->Uninit			= voRealAudioRMFmtDecUninit;
	DecAPI->SetInputData	= NULL;
	DecAPI->GetOutputData	= voRealAudioRMFmtDecGetOutputData;
	DecAPI->SetParam		= voRealAudioRMFmtDecSetParameter;
	DecAPI->GetParam		= voRealAudioRMFmtDecGetParameter;

	pDec->customData	= (void*)(rm_memory_malloc(pMemOP, NULL,sizeof(VORA_RMFmtParam)));
	{
		VORA_RMFmtParam* param=(VORA_RMFmtParam*)pDec->customData;
		param->pDecoder = (ra_decode*)ra_decode_create(NULL,NULL, pMemOP);
		if(param->pDecoder)
		{
			VORA_INIT_PARAM* params	= pDec->rmParam;
			HX_RESULT hr = ra_decode_init(param->pDecoder, params->ulFourCC, NULL, 0,
				(ra_format_info*)params->format, pMemOP, pDec->pLibOperator);
			if(HXR_OK != hr)
				return hr;
			ra_decode_getmaxsize(param->pDecoder, (UINT32*)(&(param->maxoutSize)));
			if(param->maxoutSize<100)
				param->maxoutSize = 16*1024; 
		}
		else
		{
			return VO_ERR_OUTOF_MEMORY;
		}		
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRMFmtDecUninit(VO_HANDLE hCodec)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	VO_MEM_OPERATOR	*pMemOP; 
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	
	pMemOP = pDec->pvoMemop;

	if(pDec->customData)
	{
		VORA_RMFmtParam* param = (VORA_RMFmtParam*)pDec->customData;
		if(param->pDecoder)
		{
			ra_decode_destroy(param->pDecoder, pMemOP);
			param->pDecoder = NULL;
		}
		rm_memory_free(pMemOP, NULL, pDec->customData);
		pDec->customData = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRMFmtDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMFmtParam* param;
	ra_block* pBlock;
	HX_RESULT hr;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	if(pDec->inbuf == NULL || pDec->inlength <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;

	param = (VORA_RMFmtParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;
	{
		UINT32 numBytesConsumed = 0;
		UINT32 numSamplesOut = pOutBuffer->Length;
		pBlock = (ra_block*)pDec->inbuf;

		if(numSamplesOut < param->maxoutSize*2)
			return VO_ERR_OUTPUT_BUFFER_SMALL;

		hr = ra_decode_decode(param->pDecoder, 
			pBlock->pData, 
			pBlock->ulDataLen, 
			&numBytesConsumed, 
			(UINT16*)pOutBuffer->Buffer, 
			param->maxoutSize, 
			&numSamplesOut, 
			pBlock->ulDataFlags);

		if(HXR_OK == hr)
		{
			UINT32 channels = 0, samplerate = 0;
			pOutBuffer->Length = numSamplesOut * 2;
			ra_decode_getchannels(param->pDecoder, &channels);
			ra_decode_getrate(param->pDecoder, &samplerate);
			pOutInfo->Format.SampleRate = samplerate;
			pOutInfo->Format.Channels = channels;
			pOutInfo->Format.SampleBits = 16;

			pOutInfo->InputUsed = pDec->inlength;
		}
	}

	pDec->inbuf += pDec->inlength;
	pDec->inlength = 0;

	return hr;
}


VO_U32 VO_API voRealAudioRMFmtDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMFmtParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RMFmtParam*)pDec->customData;
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


VO_U32 VO_API voRealAudioRMFmtDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMFmtParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RMFmtParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;
	//TODO:
	switch(uParamID)
	{
	case VOID_PID_RA_MAXOUTPUTSAMLES:
		return ra_decode_getmaxsize(param->pDecoder, (UINT32*)pData);

	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			ra_decode_getrate(param->pDecoder, (UINT32*)(&fmt->SampleRate));
			ra_decode_getchannels(param->pDecoder, (UINT32*)(&fmt->Channels));
			fmt->SampleBits = 16;
		}
		break;

	case VO_PID_AUDIO_CHANNELS:
		return ra_decode_getchannels(param->pDecoder, (UINT32*)(pData));

	case VO_PID_AUDIO_SAMPLEREATE:
		return ra_decode_getrate(param->pDecoder, (UINT32*)(pData));
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


