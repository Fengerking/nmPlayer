#include	"voDRA.h"
#include	"config.h"
#include	"global.h"
#include	"cmnMemory.h"
#include	"frame.h"
#include	"voChHdle.h"
#include	"voCheck.h"

#if defined (LCHECK)
VO_PTR	g_hDRADecInst = NULL;
#endif

VO_U32 VO_API voDRADecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData )
{
	DraDecInfo		*decoder = NULL;
	FrameHeader		*header = NULL;
	FrameStream		*stream = NULL;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int				*afBinNatural = NULL;
	int				len, reti;
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

	decoder = (DraDecInfo *)mem_malloc(pMemOP, sizeof(DraDecInfo), 32);
	if(NULL == decoder)
		goto INIT_FAIL;

	len = sizeof(FrameHeader);
	header = (FrameHeader *)mem_malloc(pMemOP, sizeof(FrameHeader), 32);
	if(NULL == header)
		goto INIT_FAIL;
	
	stream = (FrameStream *)mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(NULL == stream)
		goto INIT_FAIL;
	StreamInit(stream);

	afBinNatural = (int *)mem_malloc(pMemOP, sizeof(int)*MAX_FRAMELENGTH, 32);
	if(NULL == afBinNatural)
		goto INIT_FAIL;

	decoder->decoder_buf = (unsigned char*)mem_malloc(pMemOP, BUFFER_DATA, 32);
	if(decoder->decoder_buf == 0)
		goto INIT_FAIL;	

	decoder->header = header;
	decoder->stream = stream;
	decoder->afBinNatural = afBinNatural;
	decoder->channelSpec = 0xffff;

	stream->buffer_bk = decoder->decoder_buf;

	if(interMem)
	{
		decoder->voMemoprator.Alloc = cmnMemAlloc;
		decoder->voMemoprator.Copy = cmnMemCopy;
		decoder->voMemoprator.Free = cmnMemFree;
		decoder->voMemoprator.Set = cmnMemSet;
		decoder->voMemoprator.Check = cmnMemCheck;

		pMemOP = &decoder->voMemoprator;
	}

	decoder->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)decoder;

#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		voCheckLibInit (&(decoder->hCheck), VO_INDEX_DEC_DRA, 1, g_hDRADecInst, pUserData->libOperator);
	else
		voCheckLibInit (&(decoder->hCheck), VO_INDEX_DEC_DRA, 1, g_hDRADecInst, NULL);
#endif

	return VO_ERR_NONE;
	
INIT_FAIL:
	if(header)	mem_free(pMemOP, header);
	if(stream)  mem_free(pMemOP, stream);
	if(afBinNatural) mem_free(pMemOP, afBinNatural);	
	if(decoder->decoder_buf)mem_free(pMemOP, decoder->decoder_buf);
	if(decoder)	mem_free(pMemOP, decoder);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;
}

VO_U32 VO_API voDRADecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	DraDecInfo		*decoder;
	FrameStream		*stream;
	int				len;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (DraDecInfo *)hCodec;
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

	if(stream->storelength)
	{
		len = MIN( BUFFER_DATA - stream->storelength, stream->inlen);
		decoder->pvoMemop->Copy(VO_INDEX_DEC_DRA, stream->buffer_bk  + stream->storelength, 
			stream->buffer, len);

		stream->this_frame = stream->buffer_bk;
		stream->length = stream->storelength + len;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voDRADecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	DraDecInfo		*decoder= NULL;
	FrameHeader		*header = NULL;
	FrameStream		*stream = NULL;
	FrameCHInfo		*chInfo = NULL;
	unsigned char*  start;
	int				len, i, samplerate;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (DraDecInfo *)hCodec;
	header = decoder->header;
	stream = decoder->stream;
	
	if(NULL == pOutput || NULL == pOutput->Buffer )
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}	
	
	start = stream->this_frame;
	len = HeaderDecode(header, stream);
	if(len == -2) 
	{
		if(stream->storelength) {
			len = MIN( BUFFER_DATA - stream->storelength, stream->inlen) - stream->length;
			len = MAX(len , 0);
			stream->this_frame = stream->buffer + len;
			stream->length = stream->inlen - len;
			stream->storelength = 0;
			stream->usedlength += len;
			start = stream->this_frame;

			len = HeaderDecode(header, stream);
		}
	}

	if(len < 0)
	{
		stream->usedlength += stream->this_frame - start;
		len = stream->length;		
		if(stream->storelength == 0)
		{	
			decoder->pvoMemop->Copy(VO_INDEX_DEC_DRA, stream->buffer_bk, 
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

	if(decoder->channelNum < header->nNumLfeCh + header->nNumNormalCh)
	{
		for(i = decoder->channelNum; i < header->nNumLfeCh + header->nNumNormalCh; i++)	
		{
			if(decoder->chInfo[i] == NULL)
			{
				chInfo = (FrameCHInfo *)mem_malloc(decoder->pvoMemop, sizeof(FrameCHInfo), 32);
				if(chInfo == NULL)
				{
					return VO_ERR_OUTOF_MEMORY;
				}

				decoder->pvoMemop->Set(VO_INDEX_DEC_DRA, chInfo, 0, sizeof(FrameCHInfo));
				chInfo->deinterleaved = decoder->afBinNatural;
				decoder->chInfo[i] = chInfo;
			}
		}

		decoder->channelNum = header->nNumLfeCh + header->nNumNormalCh;
		decoder->chanDecNum = decoder->channelNum;
		decoder->LefNum = header->nNumLfeCh;
	}
	else
	{
		decoder->channelNum = header->nNumLfeCh + header->nNumNormalCh;
		decoder->chanDecNum = decoder->channelNum;
	}	

	if(pOutput->Length < decoder->channelNum * sizeof(short) * MAX_FRAMELENGTH)
		return VO_ERR_OUTPUT_BUFFER_SMALL;

	samplerate = samplerateTab[header->nSamplerateRateIndex];
	
	if(samplerate != decoder->samplerate)
	{
		decoder->sfbw = &sfbwidth_table[header->nSamplerateRateIndex];
		decoder->samplerate = samplerate;
	}
	
	len = DecoderFrame(decoder, (short*)pOutput->Buffer);

	stream->this_frame += header->nNumWord;
	stream->length -= header->nNumWord;

	if(stream->storelength)
	{
		int length = stream->this_frame - start;
		length -= stream->storelength;
		stream->buffer += length;
		stream->inlen  -= length;
		stream->usedlength += length;
		stream->this_frame = stream->buffer;
		stream->length = stream->inlen;
		stream->storelength = 0;
	}
	else
	{
		stream->usedlength += stream->this_frame - start;
	}
	
	if(len < 0) 
	{
		pOutput->Length = 0;
		return VO_ERR_DRA_INVFRAME;
	}	

	pOutput->Length = decoder->chanDecNum * sizeof(short) * MAX_FRAMELENGTH;

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = decoder->chanDecNum;
		pOutInfo->Format.SampleRate =decoder->samplerate;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->InputUsed = stream->usedlength;
	}
	
#if defined (LCHECK)
	voCheckLibCheckAudio(decoder->hCheck, pOutput, pOutInfo);
#endif

	return VO_ERR_NONE;
}


VO_U32 VO_API voDRADecUninit(VO_HANDLE hCodec)
{
	DraDecInfo		*decoder= NULL;
	VO_MEM_OPERATOR *pMemOP;
	int i;	
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (DraDecInfo *)hCodec;
	pMemOP = decoder->pvoMemop;
	
	for(i = 0; i < MAX_CHANNEL; i++)
	{
		if(decoder->chInfo[i])
		{
			if(decoder->chInfo[i]->fJicScale)
			{
				mem_free(pMemOP, decoder->chInfo[i]->fJicScale);
				decoder->chInfo[i]->fJicScale = 0;
			}
			
			if(decoder->chInfo[i]->fSumDff)
			{
				mem_free(pMemOP, decoder->chInfo[i]->fSumDff);
				decoder->chInfo[i]->fSumDff = 0;
			}

			mem_free(pMemOP, decoder->chInfo[i]);
			decoder->chInfo[i] = NULL;
		}
	}

	if(decoder->stream)
	{
		mem_free(pMemOP, decoder->stream);
		decoder->stream = NULL;
	}

	if(decoder->header)
	{
		mem_free(pMemOP, decoder->header);
		decoder->header= NULL;
	}

	if(decoder->afBinNatural)
	{
		mem_free(pMemOP, decoder->afBinNatural);
		decoder->afBinNatural = NULL;
	}

	if(decoder->decoder_buf){
		mem_free(pMemOP, decoder->decoder_buf);
		decoder->decoder_buf = NULL;
	}

	mem_free(pMemOP,decoder);

#if defined (LCHECK)
	voCheckLibUninit(decoder->hCheck);
#endif

	return VO_ERR_NONE;	
}

VO_U32 VO_API voDRADecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	DraDecInfo		*decoder= NULL;
	FrameStream		*stream = NULL;
	FrameCHInfo		*pchInfo= NULL;
	int ret, nch;
	if(NULL == hCodec || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (DraDecInfo *)hCodec;
	stream = decoder->stream;
	
	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		if(*((int *)pData))
		{
			stream->inlen = 0;
			stream->length = 0;
			stream->storelength = 0;
			stream->usedlength = 0;
			
			for(nch = 0; nch < MAX_CHANNEL; nch++)
			{
				pchInfo = decoder->chInfo[nch];

				if(pchInfo)
				{
					decoder->pvoMemop->Set(VO_INDEX_DEC_DRA, pchInfo->overlap, 0, sizeof(int)*MAX_FRAMELENGTH);
				}
			}
		}
		break;	
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voDRADecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	DraDecInfo *decoder;
	VO_AUDIO_FORMAT *pFormat;
	if(NULL == hCodec || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	decoder = (DraDecInfo *)hCodec;

	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = decoder->channelNum;
		pFormat->SampleBits = 16;
		pFormat->SampleRate = decoder->samplerate;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetDRADecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voDRADecInit;
	pDecHandle->SetInputData = voDRADecSetInputData;
	pDecHandle->GetOutputData = voDRADecGetOutputData;
	pDecHandle->SetParam = voDRADecSetParam;
	pDecHandle->GetParam = voDRADecGetParam;
	pDecHandle->Uninit = voDRADecUninit;

	return VO_ERR_NONE;
}