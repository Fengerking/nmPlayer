#include	"voADPCM.h"
#include	"Frame.h"
#include	"voChHdle.h"
#include	"voCheck.h"

#ifdef G1_LOG
#include <android/log.h>
#define  LOG_TAG    "VisualOn"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

#if defined (LCHECK)
VO_PTR	g_hADPCMDecInst = NULL;
#endif

#ifdef TIME_LIMITATION
static unsigned int ADPCMFrameNum = 0;
#endif

VO_U32 VO_API voADPCMDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData )
{
	VO_U32  nRet = 0;
	ADPCMDecInfo	*decoder = NULL;
	FrameStream		*stream = NULL;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	//int interMem = 0;

	if(pUserData == NULL || (pUserData->memflag&0x0f) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
    return VO_ERR_INVALID_ARG;
	/*	voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;

		interMem = 1;

		pMemOP = &voMemoprator;*/
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

	decoder = (ADPCMDecInfo *)voADPCMDecmem_malloc(pMemOP, sizeof(ADPCMDecInfo), 32);
	if(NULL == decoder)
		goto INIT_FAIL;

	stream = (FrameStream *)voADPCMDecmem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(NULL == stream)
		goto INIT_FAIL;
	voADPCMDecStreamInit(stream);

	decoder->decoder_buf = (unsigned char*)voADPCMDecmem_malloc(pMemOP, IN_BUFFERSIZE, 32);
	if(decoder->decoder_buf == 0)
		goto INIT_FAIL;	

	stream->buffer_bk = decoder->decoder_buf;
	decoder->stream	= stream;

	/*if(interMem)
	{
		decoder->voMemoprator.Alloc = cmnMemAlloc;
		decoder->voMemoprator.Copy = cmnMemCopy;
		decoder->voMemoprator.Free = cmnMemFree;
		decoder->voMemoprator.Set = cmnMemSet;
		decoder->voMemoprator.Check = cmnMemCheck;

		pMemOP = &decoder->voMemoprator;
	}*/

#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&(decoder->hCheck), VO_INDEX_DEC_ADPCM, pUserData->memflag|1, g_hADPCMDecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&(decoder->hCheck), VO_INDEX_DEC_ADPCM, pUserData->memflag|1, g_hADPCMDecInst, NULL);

	if (nRet != VO_ERR_NONE)
	{
		if (decoder->hCheck)
		{
			voCheckLibUninit(decoder->hCheck);
			decoder->hCheck = NULL;
		}
		if(decoder->decoder_buf){
			voADPCMDecmem_free(pMemOP, decoder->decoder_buf);
			decoder->decoder_buf = NULL;
		}
		if(stream) voADPCMDecmem_free(pMemOP, stream);
		if(decoder)	voADPCMDecmem_free(pMemOP, decoder);

		return nRet;
	}
#endif

#ifdef G1_LOG
	LOGI("Init Successful");
#endif
	decoder->pvoMemop = pMemOP;
	*phCodec = (VO_HANDLE)decoder;

	return VO_ERR_NONE;
	
INIT_FAIL:
	if(decoder->decoder_buf){
		voADPCMDecmem_free(pMemOP, decoder->decoder_buf);
		decoder->decoder_buf = NULL;
	}
	
	if(stream) voADPCMDecmem_free(pMemOP, stream);
	if(decoder)	voADPCMDecmem_free(pMemOP, decoder);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;	
}

VO_U32 VO_API voADPCMDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	ADPCMDecInfo	*decoder;
	FrameStream		*stream;
	int				len;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (ADPCMDecInfo *)hCodec;
	stream = decoder->stream;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}	

	stream->buffer = pInput->Buffer;
	stream->inlen = pInput->Length;

	stream->this_frame = stream->buffer;
	stream->length = stream->inlen;
	stream->usedlength = 0;

#ifdef G1_LOG
    LOGI("Set Input len = %d", stream->length);
#endif

	if(stream->storelength)
	{
		len = MIN( IN_BUFFERSIZE - stream->storelength, stream->inlen);
		decoder->pvoMemop->Copy(VO_INDEX_DEC_ADPCM, stream->buffer_bk  + stream->storelength, 
			stream->buffer, len);

		stream->this_frame = stream->buffer_bk;
		stream->length = stream->storelength + len;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voADPCMDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	ADPCMDecInfo *decoder;
	FrameStream	 *stream;
	unsigned char *start;
	int ret, len;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (ADPCMDecInfo *)hCodec;
	stream = decoder->stream;
	if(NULL == pOutput || NULL ==  pOutput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}
		
	if(stream->length < decoder->blockalign)
	{
		len = stream->length;		
		if(stream->storelength == 0)
		{	
			decoder->pvoMemop->Copy(VO_INDEX_DEC_ADPCM, stream->buffer_bk, 
				stream->this_frame, len);			
		}
		stream->storelength = len;
		pOutput->Length = 0; 
		stream->usedlength += len;

		if(pOutInfo)
		{
			pOutInfo->InputUsed = stream->usedlength;
		}
		return VO_ERR_INPUT_BUFFER_SMALL;	
	}

	if(decoder->frametype == WAVE_FORMAT_MS_ADPCM || decoder->frametype == WAVE_FORMAT_IMA_ADPCM 
		|| decoder->frametype == WAVW_FORMAT_SWF_ADPCM)
	{
		if(pOutput->Length < decoder->framesperblock*decoder->channel*sizeof(short))
		{
			if(pOutInfo)
			{
				pOutInfo->InputUsed = stream->usedlength;
			}
			return VO_ERR_OUTPUT_BUFFER_SMALL;
		}
	}
	else if(decoder->frametype == WAVE_FORMAT_ITU_G726 || decoder->frametype == WAVE_FORMAT_ALAW
		|| decoder->frametype == WAVE_FORMAT_MULAW)
	{
		if((stream->length / decoder->BitsPerSample + 1)*2 > pOutput->Length)
		{
			if(pOutInfo)
			{
				pOutInfo->InputUsed = stream->usedlength;
			}
			return VO_ERR_OUTPUT_BUFFER_SMALL; 
		}
	}

	start = stream->this_frame;
	ret = voADPCMDecADPCM(decoder, (short *)pOutput->Buffer);
	if(ret)	{
		return ret;
	}

	if(stream->storelength)
	{
		int length = stream->this_frame - stream->buffer_bk;
		length -= stream->storelength;
		stream->buffer += length;
		stream->inlen  -= length;
		stream->this_frame = stream->buffer;
		stream->length = stream->inlen;
		stream->storelength = 0;
		stream->usedlength += length;
	}
	else
	{
		stream->usedlength += stream->this_frame - start;
	}

	pOutput->Length = decoder->framesperblock*decoder->channel*sizeof(short);

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = decoder->channel;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->Format.SampleRate = decoder->samplerate;
		pOutInfo->InputUsed = stream->usedlength;
	}

#ifdef TIME_LIMITATION
	ADPCMFrameNum += pOutput->Length/(decoder->channel*2);
	if(ADPCMFrameNum > decoder->samplerate*180)
	{
		decoder->pvoMemop->Set(VO_INDEX_DEC_ADPCM, pOutput->Buffer, 0, pOutput->Length);
		ADPCMFrameNum = decoder->samplerate*180+1;
		return VO_ERR_LICENSE_ERROR;
	}
#endif

#if defined (LCHECK)
	voCheckLibCheckAudio(decoder->hCheck, pOutput, &(pOutInfo->Format));
#endif

#ifdef G1_LOG
    LOGI("Get Output len = %d", pOutput->Length);
#endif

	return VO_ERR_NONE;	
}


VO_U32 VO_API voADPCMDecUninit(VO_HANDLE hCodec)
{
	ADPCMDecInfo *decoder;
	VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	decoder = (ADPCMDecInfo *)hCodec;
	pMemOP = decoder->pvoMemop;

	if(decoder->decoder_buf){
		voADPCMDecmem_free(pMemOP,decoder->decoder_buf);
		decoder->decoder_buf = 0;
	}	

	if(decoder->adpcm)
	{
		voADPCMDecmem_free(pMemOP,decoder->adpcm);
		decoder->adpcm = 0;
	}

	if(decoder->stream){
		voADPCMDecmem_free(pMemOP,decoder->stream);
		decoder->stream = 0;
	}

#if defined (LCHECK)
	voCheckLibUninit(decoder->hCheck);
#endif

	voADPCMDecmem_free(pMemOP,decoder);

#ifdef G1_LOG
    LOGI("Uinit successful");
#endif

	return VO_ERR_NONE;	
}

VO_U32 VO_API voADPCMDecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	ADPCMDecInfo *decoder;
	int ret;
	VO_CODECBUFFER  *indata;
	FrameStream	 *stream;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	if(pData == NULL)
		return VO_ERR_INVALID_ARG;

	decoder = (ADPCMDecInfo *)hCodec;
	stream = decoder->stream;
	
	switch(uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
	case VO_PID_ADPCM_FMTCHUNK:
		indata = (VO_CODECBUFFER  *)pData;
		ret = voADPCMDecReadFMTChunk(decoder, indata->Buffer, indata->Length);

		if(ret < 0)
			return VO_ERR_NOT_IMPLEMENT;
		break;
	case VO_PID_COMMON_FLUSH:
		if(*((int *)pData))
		{
			stream->inlen = 0;
			stream->length = 0;
			stream->storelength = 0;
			stream->usedlength = 0;
		}
		break;	
	default:
		return VO_ERR_NONE;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voADPCMDecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	ADPCMDecInfo *decoder;
	VO_AUDIO_FORMAT *pFormat;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	decoder = (ADPCMDecInfo *)hCodec;

	if(pData == NULL)
		return VO_ERR_INVALID_ARG;
	
	switch(uParamID)
	{
	case VO_PID_ADPCM_GETBLOCKSIZE:
		*((int *)pData) = decoder->blockalign;
		break;
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = decoder->channel;
		pFormat->SampleBits = 16;
		pFormat->SampleRate = decoder->samplerate;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetADPCMDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voADPCMDecInit;
	pDecHandle->SetInputData = voADPCMDecSetInputData;
	pDecHandle->GetOutputData = voADPCMDecGetOutputData;
	pDecHandle->SetParam = voADPCMDecSetParam;
	pDecHandle->GetParam = voADPCMDecGetParam;
	pDecHandle->Uninit = voADPCMDecUninit;

	return VO_ERR_NONE;
}