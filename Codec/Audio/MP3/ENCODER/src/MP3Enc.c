#include "voMP3.h"
#include "cmnMemory.h"
#include "mem_align.h"
#include "Frame.h"
#include "voChHdle.h"
#include "voCheck.h"

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
VO_PTR	g_hMp3EncInst = NULL;
#endif

VO_U32 VO_API voMP3EncInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData)
{
	MP3EncInfo *encInfo = NULL;
	FrameInfo  *frameInfo = NULL;
	Bitstream  *stream = NULL;
	short      *encbuf = NULL;
	unsigned char   *headdata = NULL;
	unsigned char   *maindata = NULL;

	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem;

	interMem = 0;

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

	encInfo = (MP3EncInfo *)mem_malloc(pMemOP, sizeof(MP3EncInfo), 32);
	if(NULL == encInfo)
		goto INIT_FAIL;

	frameInfo = (FrameInfo *)mem_malloc(pMemOP, sizeof(FrameInfo), 32);
	if(NULL == frameInfo)
		goto INIT_FAIL;
	
	stream = (Bitstream *)mem_malloc(pMemOP, sizeof(Bitstream), 32);
	if(NULL == stream)
		goto INIT_FAIL;

	encbuf = (short *)mem_malloc(pMemOP, BUFFER_DATA*sizeof(short), 32);
	if(NULL == encbuf)
		goto INIT_FAIL;

	headdata = (unsigned char *)mem_malloc(pMemOP, BUFFER_HEAD, 32);
	if(NULL == headdata)
		goto INIT_FAIL;

	maindata = (unsigned char *)mem_malloc(pMemOP, BUFFER_MDLEN, 32);
	if(NULL == maindata)
		goto INIT_FAIL;

	frameInfo->channels = 2;
	frameInfo->bitrate = 128;
	frameInfo->samplerate = 44100;

	encInfo->encbuf = encbuf;
	encInfo->frame = frameInfo;
	encInfo->stream = stream;
	encInfo->headdata = headdata;
	encInfo->maindata = maindata;
	encInfo->initset = 1;
	
	if(interMem)
	{
		encInfo->voMemoprator.Alloc = cmnMemAlloc;
		encInfo->voMemoprator.Copy = cmnMemCopy;
		encInfo->voMemoprator.Free = cmnMemFree;
		encInfo->voMemoprator.Set = cmnMemSet;
		encInfo->voMemoprator.Check = cmnMemCheck;

		pMemOP = &encInfo->voMemoprator;
	}

	encInfo->pvoMemop = pMemOP;
	*phCodec = (VO_HANDLE)encInfo;

#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		voCheckLibInit (&(encInfo->hCheck), VO_INDEX_ENC_MP3, 1, g_hMp3EncInst, pUserData->libOperator);
	else
		voCheckLibInit (&(encInfo->hCheck), VO_INDEX_ENC_MP3, 1, g_hMp3EncInst, NULL);
#endif

	return VO_ERR_NONE;

INIT_FAIL:
	if(stream)		mem_free(pMemOP, stream);
	if(frameInfo)	mem_free(pMemOP, frameInfo);
	if(encbuf)		mem_free(pMemOP, encbuf);
	if(headdata)	mem_free(pMemOP, headdata);
	if(maindata)	mem_free(pMemOP, maindata);	
	if(encInfo)		mem_free(pMemOP, encInfo);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;		
}

VO_U32 VO_API voMP3EncSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	MP3EncInfo *encInfo;

	if(NULL == hCodec || NULL == pInput || NULL == pInput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	encInfo = (MP3EncInfo *)hCodec;
	
	encInfo->inbuf = (short *)pInput->Buffer;
	encInfo->inlen = pInput->Length / sizeof(short);
	encInfo->uselength = 0;
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3EncGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	short *input, *mfbuf;	
	MP3EncInfo *encInfo;
	FrameInfo  *frameInfo;
	int nch, ngrans, ret, inlength, enclen;
	unsigned char *outbuf;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec|| NULL == pOutput || NULL == pOutput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}

	encInfo = (MP3EncInfo *)hCodec;
	frameInfo = encInfo->frame;
	pMemOP = encInfo->pvoMemop;

	if(encInfo->initset)
	{
		ret = encInit(encInfo, frameInfo->channels, frameInfo->samplerate, frameInfo->bitrate);
		if(ret) 
			return ret;

		encInfo->initset = 0;
	}
	
	if(pOutput->Length < encInfo->byte_per_frame + 1)
	{
		if(pOutInfo)
			pOutInfo->InputUsed = encInfo->uselength;
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	mfbuf = encInfo->encbuf;
	input = encInfo->inbuf;
	inlength = encInfo->inlen;
	enclen = encInfo->enclen;
	nch = frameInfo->channels;
	ngrans = frameInfo->ngrans;
	
	if(enclen < (512 + SAMP_PER_FRAME)*nch)
	{
		inlength = MIN((512 + SAMP_PER_FRAME)*nch - enclen, inlength);
		
		pMemOP->Copy(VO_INDEX_ENC_MP3, mfbuf + enclen, input, inlength*2);

		encInfo->inlen -= inlength;
		encInfo->inbuf += inlength;
		encInfo->enclen += inlength;
		encInfo->uselength += inlength*sizeof(short);

		if(encInfo->enclen < (512 + SAMP_PER_FRAME)*nch)
		{
			pOutput->Buffer = 0;
			if(pOutInfo)
				pOutInfo->InputUsed = encInfo->uselength;
			return VO_ERR_INPUT_BUFFER_SMALL;	
		}
	}
	else
	{
		
		if(inlength >= SAMP_PER_FRAME*nch)
		{
			pMemOP->Copy(VO_INDEX_ENC_MP3, mfbuf, mfbuf + nch*ngrans*576, nch*512*2);
			/* read new samples to iram for further processing */
			pMemOP->Copy(VO_INDEX_ENC_MP3, mfbuf + nch*512, input, 2*SAMP_PER_FRAME*nch);
			
			encInfo->inbuf += SAMP_PER_FRAME*nch;
			encInfo->inlen -= SAMP_PER_FRAME*nch;
			encInfo->enclen = (512 + SAMP_PER_FRAME)*nch;
			encInfo->uselength += 2*SAMP_PER_FRAME*nch;
		}
		else
		{
			pMemOP->Copy(VO_INDEX_ENC_MP3, mfbuf, mfbuf + nch*ngrans*576, nch*512*2);
			/* read new samples to iram for further processing */
			pMemOP->Copy(VO_INDEX_ENC_MP3, mfbuf + nch*512, input, inlength*2);
			
			encInfo->inbuf += inlength;
			encInfo->inlen -= inlength;
			encInfo->enclen = nch*512 + inlength;
			encInfo->uselength += inlength*2;

			if(pOutInfo)
				pOutInfo->InputUsed = encInfo->uselength;
			
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	if((encInfo->slot_lag += encInfo->frac_per_frame) >= 64)
	{   
		encInfo->slot_lag   -= 64;
		frameInfo->header.padding = 1;
	}
	else
		frameInfo->header.padding = 0;
	
	frameInfo->mean_bits = 	(8 * encInfo->byte_per_frame + 8 * frameInfo->header.padding
		- frameInfo->si_len) / (ngrans * nch);

	ret = encMP3(encInfo);

	pOutput->Length = encInfo->byte_per_frame + frameInfo->header.padding;
	outbuf = pOutput->Buffer;
	inlength = frameInfo->si_len >> 3;
	pMemOP->Copy(VO_INDEX_ENC_MP3, outbuf, encInfo->headdata, inlength);
	pMemOP->Copy(VO_INDEX_ENC_MP3, outbuf + inlength, encInfo->maindata, pOutput->Length - inlength);

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = frameInfo->channels;
		pOutInfo->Format.SampleRate = frameInfo->samplerate;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->InputUsed = encInfo->uselength;
	}
	
#if defined (LCHECK)
	voCheckLibCheckAudio(encInfo->hCheck, pOutput, &(pOutInfo->Format));
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3EncSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	MP3EncInfo *encInfo;
	FrameInfo  *frameInfo;
	VO_AUDIO_FORMAT *pWAV_Format;

	if(NULL == hCodec || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	encInfo = (MP3EncInfo *)hCodec;
	frameInfo = encInfo->frame;

	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		frameInfo->samplerate = *((int*)pData);
		encInfo->initset = 1;
		break;
	case VO_PID_AUDIO_CHANNELS:
		frameInfo->channels = *((int *)pData);
		encInfo->initset = 1;
		break;
	case VO_PID_AUDIO_BITRATE:
		frameInfo->bitrate = *((int *)pData)/1000;
		encInfo->initset = 1;
		break;	
	case VO_PID_AUDIO_FORMAT:
		pWAV_Format = (VO_AUDIO_FORMAT *)pData;
		frameInfo->samplerate = pWAV_Format->SampleRate;
		frameInfo->channels = pWAV_Format->Channels;
		frameInfo->bitrate = 64*pWAV_Format->SampleRate/44100*pWAV_Format->Channels;
		if(pWAV_Format->SampleBits != 16)
			return VO_ERR_AUDIO_UNSFEATURE;
		encInfo->initset = 1;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}	

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3EncGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	MP3EncInfo *encInfo;
	FrameInfo  *frameInfo;
	VO_AUDIO_FORMAT *pFormat;

	if(NULL == hCodec || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	encInfo = (MP3EncInfo *)hCodec;
	frameInfo = encInfo->frame;

	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		*((int*)pData) = frameInfo->samplerate;		
		break;
	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = frameInfo->channels;
		break;
	case VO_PID_AUDIO_BITRATE:
		*((int *)pData) = frameInfo->bitrate;
		break;
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = frameInfo->channels;
		pFormat->SampleBits = 16;
		pFormat->SampleRate = frameInfo->samplerate;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}	

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3EncUninit(VO_HANDLE hCodec)
{
	MP3EncInfo *encInfo = NULL;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	encInfo = (MP3EncInfo *)hCodec;
	pMemOP = encInfo->pvoMemop;
	
	if(encInfo->stream)    mem_free(pMemOP, encInfo->stream);
	if(encInfo->frame)     mem_free(pMemOP, encInfo->frame);
	if(encInfo->encbuf)    mem_free(pMemOP, encInfo->encbuf);
	if(encInfo->headdata)  mem_free(pMemOP, encInfo->headdata);
	if(encInfo->maindata)  mem_free(pMemOP, encInfo->maindata);	

#if defined (LCHECK)
	voCheckLibUninit(encInfo->hCheck);
#endif

	if(encInfo)			   mem_free(pMemOP, encInfo);
	
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetMP3EncAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voMP3EncInit;
	pDecHandle->SetInputData = voMP3EncSetInputData;
	pDecHandle->GetOutputData = voMP3EncGetOutputData;
	pDecHandle->SetParam = voMP3EncSetParam;
	pDecHandle->GetParam = voMP3EncGetParam;
	pDecHandle->Uninit = voMP3EncUninit;

	return VO_ERR_NONE;
}
