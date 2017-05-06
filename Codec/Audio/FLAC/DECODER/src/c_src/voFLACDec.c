/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voFLACDec.c

Contains:	Flac decode API function

Written by:	Huaping Liu

Change History (most recent first):
2009-06-17		LHP			Create file

*******************************************************************************/

#include     <stdio.h>
#include     "frame.h"
#include     "stream.h"
#include     "mem_align.h"
//#include     "cmnMemory.h"
#include     "voIndex.h"
#include     "voFLAC.h"
#include     "voCheck.h"

#define MIN(a,b)	 ((a) < (b)? (a) : (b))
void *g_hFLACDecInst = NULL;

//#define DUMP

#ifdef DUMP
FILE *dumpin = NULL;
FILE *dumpout = NULL;
#endif

VO_U32 VO_API voFLACDec_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32  nRet = 0;
#ifdef LCHECK
    VO_PTR  pTemp;
#endif
	FLACDecInfo		*decoder = NULL;
	FrameStream		*stream = NULL;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;

	//int interMem = 0;
	if(pUserData == NULL || (pUserData->memflag & 0x0f) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		return VO_ERR_INVALID_ARG;
		//voMemoprator.Alloc = cmnMemAlloc;
		//voMemoprator.Copy = cmnMemCopy;
		//voMemoprator.Free = cmnMemFree;
		//voMemoprator.Set = cmnMemSet;
		//voMemoprator.Check = cmnMemCheck;
		//voMemoprator.Move = cmnMemMove;
		//interMem = 1;
		//pMemOP = &voMemoprator;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	} 
	//VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_FLAC, pUserData->memflag|1, g_hFLACDecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_FLAC, pUserData->memflag|1, g_hFLACDecInst, NULL);

	if (nRet != VO_ERR_NONE)
	{
		if (pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}
#endif 

	decoder = (FLACDecInfo *)voFLACDecmem_malloc(pMemOP, sizeof(FLACDecInfo), 32);
	if(NULL == decoder)
		goto INIT_FAIL;
	memset(decoder, 0, sizeof(FLACDecInfo));

	stream = (FrameStream *)voFLACDecmem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(NULL == stream)
		goto INIT_FAIL;
	decoder->stream = stream;
	voFLACDecStreamInit(stream);	

	//if(interMem)
	//{
	//	decoder->voMemoprator.Alloc = cmnMemAlloc;
	//	decoder->voMemoprator.Copy = cmnMemCopy;
	//	decoder->voMemoprator.Free = cmnMemFree;
	//	decoder->voMemoprator.Set = cmnMemSet;
	//	decoder->voMemoprator.Check = cmnMemCheck;
	//	decoder->voMemoprator.Move = cmnMemMove;
	//	pMemOP = &decoder->voMemoprator;
	//}
	decoder->pvoMemop = pMemOP;
#ifdef LCHECK
     decoder->hCheck = pTemp;
#endif
	*phCodec = (void *)decoder;

#ifdef DUMP
    dumpin = fopen("/data/local/lhp/inlog.dat", "wb+");
	dumpout = fopen("/data/local/lhp/outlog.dat", "wb+");
#endif
	return VO_ERR_NONE;

INIT_FAIL:
	if(decoder)	
	voFLACDecmem_free(pMemOP, decoder);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;
}


VO_U32 VO_API voFLACDec_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	FLACDecInfo     *gData;
	FrameStream     *stream;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (FLACDecInfo *)hCodec;
	stream = gData->stream;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	stream->str_ptr    = pInput->Buffer;
	stream->str_len    = pInput->Length;
#ifdef DUMP
    fwrite(stream->str_ptr, 1, stream->str_len,dumpin);
	fflush(dumpin);
#endif
	stream->used_len   = 0;
	return VO_ERR_NONE;
}

VO_U32 VO_API voFLACDec_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	FLACDecInfo     *decoder;
	FrameStream	    *stream;
	FrameHeader	    *fHeader;
	Bitstream       *bitptr;
	VO_MEM_OPERATOR *pMemOP;
	int			 ret;
	int          temp_len = 0;
	int          len;
	int          nch;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	decoder = (FLACDecInfo *)hCodec;
	stream  = decoder->stream;
	fHeader = &decoder->Fheader;
	bitptr  = &stream->bitptr;
	pMemOP  = decoder->pvoMemop;

	if(NULL == pOutput->Buffer || 0 > pOutput->Length)
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}
	
	//pInData->length = StreamRefillBuffer(stream, pInData->buffer, pInData->length);
	
	len = (int)(stream->bufend - stream->buffer);
	//if(len < decoder->MetaInfo.MinFrameSize)
	{
		//if(stream->buffer - stream->buffer_bk >= decoder->MetaInfo.MaxFrameSize)
		//{
  //          pMemOP->Move(VO_INDEX_DEC_FLAC, stream->buffer_bk, stream->buffer, len);
		//	stream->bufend = stream->buffer_bk + len;
		//	stream->buffer = stream->buffer_bk;
		//}

		if(stream->str_len > 0)
		{
			temp_len = MIN(stream->maxframesize - len, stream->str_len);
			pMemOP->Copy(VO_INDEX_DEC_FLAC, stream->bufend, stream->str_ptr, temp_len);
			stream->str_ptr  += temp_len;
			stream->bufend   += temp_len;
			stream->str_len  -= temp_len;
			stream->used_len += temp_len;
		}
		if(stream->bufend - stream->buffer < decoder->MetaInfo.MinFrameSize)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	ret = voFLACDecHeaderDecode(decoder);
	if(ret < 0)
	{
        len = stream->bufend - stream->buffer;
        if ( len >=0)
        {
			pMemOP->Move(VO_INDEX_DEC_FLAC, stream->buffer_bk, stream->buffer, len);
			stream->bufend = stream->buffer_bk + len;
			stream->buffer = stream->buffer_bk;
        }
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	decoder->channel = fHeader->Channels;
	decoder->samplerate = fHeader->SampleRate;

	if(pOutput->Length  < fHeader->BlockSize * fHeader->Channels * sizeof(short))
		return VO_ERR_OUTPUT_BUFFER_SMALL;

	pOutput->Length = fHeader->BlockSize * fHeader->Channels * sizeof(short);

	for(nch  = 0; nch < fHeader->Channels; nch++)
	{
		ret = voFLACDecUpdateFrameBuffer(&decoder->Fsubframe[nch], fHeader->BlockSize, pMemOP);
		if(ret < 0)
			return VO_ERR_OUTOF_MEMORY;
	}
	
	for(nch = 0; nch < fHeader->Channels; nch++)
	{
		ret = voFLACDecodeSubFrame(decoder, nch);
		if(ret)
		{
			voFLACDecStreamFlush(decoder->stream);
			return VO_ERR_INPUT_BUFFER_SMALL;
			//StreamBufferUpdata(stream, 10);
			//return VO_ERR_FAILED;
		}
	}

	voFLACDecAlignBits(bitptr);	
	decoder->footerCrc = voFLACDecGetBits(bitptr, 16);

	len = voFLACDecCalcBytesUsed(bitptr, stream->buffer);

	voFLACDecStreamBufferUpdata(stream, len);
	
	ret = voFLACDecChanAssignMix(decoder, (short *)pOutput->Buffer);

	if(ret < 0)
		return VO_ERR_FAILED;

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(decoder->hCheck, pOutput, &(pAudioFormat->Format));
#endif 

#ifdef DUMP
    fwrite(pOutput->Buffer, 1, pOutput->Length, dumpout);
	fflush(dumpout);
#endif
	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels   = decoder->channel;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = decoder->samplerate;
		pAudioFormat->InputUsed         = stream->used_len;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voFLACDec_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	FLACDecInfo    *decoder;
	VO_CODECBUFFER *inData;
	int ret;
	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	decoder = (FLACDecInfo *)hCodec;

	switch(uParamID)
	{
	case VO_PID_FLAC_HEADER_PARAMETER:
		inData = (VO_CODECBUFFER *)pData;
		if(inData->Length > 8 && inData->Buffer[0] == 'f' && inData->Buffer[1] == 'L' &&inData->Buffer[2] == 'a' &&inData->Buffer[3] == 'C')
			ret = voFLACDecReadMetaInfo(decoder, inData->Buffer + 8 , inData->Length - 8);
		else
		    ret = voFLACDecReadMetaInfo(decoder, inData->Buffer , inData->Length);
		break;
	case VO_PID_COMMON_FLUSH:
		if(*(int *)pData)
		{
			voFLACDecStreamFlush(decoder->stream);
		}
		break;
	case VO_PID_COMMON_HEADDATA:
		inData = (VO_CODECBUFFER *)pData;
		if(inData->Length > 8 && inData->Buffer[0] == 'f' && inData->Buffer[1] == 'L' &&inData->Buffer[2] == 'a' &&inData->Buffer[3] == 'C')
			ret = voFLACDecReadMetaInfo(decoder, inData->Buffer + 8 , inData->Length - 8);
		else
			ret = voFLACDecReadMetaInfo(decoder, inData->Buffer , inData->Length);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voFLACDec_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	FLACDecInfo *decoder;
	VO_AUDIO_OUTPUTINFO *m_OutputInfo;

	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	decoder = (FLACDecInfo *)hCodec;
	m_OutputInfo = (VO_AUDIO_OUTPUTINFO *)pData;

	switch(uParamID)
	{
	case VO_PID_FLAC_GETCHANNEL:
		pData = (void *)(&decoder->channel);
		break;
	case VO_PID_FLAC_GETBLOCKSIZE:
		pData = (void *)(&decoder->MetaInfo.MaxBlockSize);	
		break;
	case VO_PID_AUDIO_FORMAT:
		m_OutputInfo->Format.Channels = decoder->channel;
		m_OutputInfo->Format.SampleRate = decoder->samplerate;
		m_OutputInfo->Format.SampleBits = decoder->MetaInfo.BitsPerSample;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voFLACDec_Uninit(VO_HANDLE hCodec)
{
	FLACDecInfo     *decoder;
	VO_MEM_OPERATOR *pMemOP;
	int nch;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (FLACDecInfo *)hCodec;
	pMemOP  = (VO_MEM_OPERATOR *)decoder->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(decoder->hCheck);
#endif 

	voFLACDecStreamFinish(decoder->stream, pMemOP);

	for(nch = 0; nch < MAX_CHANNEL; nch++)
	{
		if(decoder->Fsubframe[nch].sample)
		{
			voFLACDecmem_free(pMemOP, decoder->Fsubframe[nch].sample);
			decoder->Fsubframe[nch].sample = NULL;
		}
	}
	if(decoder->stream){
		voFLACDecmem_free(pMemOP, decoder->stream);
		decoder->stream = 0;
	}
	voFLACDecmem_free(pMemOP, decoder);
#ifdef DUMP
    fclose(dumpin);
	fclose(dumpout);
#endif
	return VO_ERR_NONE;	
}

VO_S32 VO_API voGetFLACDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voFLACDec_Init;
	pDecHandle->SetInputData = voFLACDec_SetInputData;
	pDecHandle->GetOutputData = voFLACDec_GetOutputData;
	pDecHandle->SetParam = voFLACDec_SetParam;
	pDecHandle->GetParam = voFLACDec_GetParam;
	pDecHandle->Uninit = voFLACDec_Uninit;

	return VO_ERR_NONE;
}
