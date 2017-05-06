#include	"config.h"
#include	"global.h"
#include	"frame.h"
#include	"voMP3.h"
#include	"mem_align.h"
//#include	"cmnMemory.h"
#include    "voChHdle.h"
#include	"voCheck.h"

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
VO_PTR	g_hMP3DecInst = NULL;
#endif

#ifdef TIME_LIMITATION
static unsigned int FrameNum = 0;
#endif
//#define LOGDUMP
#ifdef LOGDUMP
static FILE* dump =NULL;
#endif

VO_U32 VO_API voMP3DecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData)
{
	VO_U32 nRet = 0;
#ifdef LCHECK
	VO_PTR pTemp;
#endif

	MP3DecInfo		*decoder = NULL;
	FrameStream		*stream = NULL;
	FrameDataInfo   *frame = NULL;
	SubbandInfo		*subband = NULL;
//#ifndef _SYMBIAN_
//	VO_MEM_OPERATOR voMemoprator;
//#endif
	VO_MEM_OPERATOR *pMemOP;
	int len;
	//int interMem;

	//interMem = 0;

	if(pUserData == NULL || (pUserData->memflag&0x0f) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
//#ifndef _SYMBIAN_
//		voMemoprator.Alloc = cmnMemAlloc;
//		voMemoprator.Copy = cmnMemCopy;
//		voMemoprator.Free = cmnMemFree;
//		voMemoprator.Set = cmnMemSet;
//		voMemoprator.Check = cmnMemCheck;
//
//		interMem = 1;
//
//		pMemOP = &voMemoprator;
//#else
		return VO_ERR_INVALID_ARG;
//#endif
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}
#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_MP3, pUserData->memflag|1, g_hMP3DecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_MP3, pUserData->memflag|1, g_hMP3DecInst, NULL);

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

	decoder = (MP3DecInfo *)voMP3DecMem_malloc(pMemOP, sizeof(MP3DecInfo), 32);
	if(NULL == decoder)
		goto INIT_FAIL;

	len = sizeof(FrameDataInfo);
	frame = (FrameDataInfo *)voMP3DecMem_malloc(pMemOP, sizeof(FrameDataInfo), 32);
	if(NULL == frame)
		goto INIT_FAIL;

	stream = (FrameStream *)voMP3DecMem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(NULL == stream)
		goto INIT_FAIL;
	voMP3DecStreamInit(stream);

	subband = (SubbandInfo *)voMP3DecMem_malloc(pMemOP, sizeof(SubbandInfo), 32);
	if(NULL == subband)
		goto INIT_FAIL;

	decoder->decoder_buf = (unsigned char*)voMP3DecMem_malloc(pMemOP, BUFFER_DATA, 32);
	if(decoder->decoder_buf == 0)
		goto INIT_FAIL;	

	stream->main_data = (unsigned char *)voMP3DecMem_malloc(pMemOP, BUFFER_MDLEN, 32);	
	if (stream->main_data == 0) 
		goto INIT_FAIL;

	stream->main_data[BUFFER_MDLEN - 4] = 
		stream->main_data[BUFFER_MDLEN - 5] = 0xFF;

	stream->buffer_bk = decoder->decoder_buf;

	decoder->stream	= stream;
	decoder->frame	= frame;
	decoder->subband = subband;
#ifdef LCHECK
    decoder->hCheck = pTemp;
#endif
//#ifndef _SYMBIAN_
//	if(interMem)
//	{
//		decoder->voMemoprator.Alloc = cmnMemAlloc;
//		decoder->voMemoprator.Copy = cmnMemCopy;
//		decoder->voMemoprator.Free = cmnMemFree;
//		decoder->voMemoprator.Set = cmnMemSet;
//		decoder->voMemoprator.Check = cmnMemCheck;
//
//		pMemOP = &decoder->voMemoprator;
//	}
//#endif

#ifdef LOGDUMP
	dump = fopen("/data/local/dump/dump.mp3", "wb");
#endif

	decoder->pvoMemop = pMemOP;
	*phCodec = (VO_HANDLE)decoder;

	return VO_ERR_NONE;
	
INIT_FAIL:
	if(stream)	{
		if (stream->main_data) {
			voMP3DecMem_free(pMemOP, stream->main_data);
			stream->main_data = 0;
		}
		voMP3DecMem_free(pMemOP, decoder->stream);
		decoder->stream = NULL;
	}
	if(decoder->decoder_buf){
		voMP3DecMem_free(pMemOP, decoder->decoder_buf);
		decoder->decoder_buf = NULL;
	}
	
	if(frame) voMP3DecMem_free(pMemOP, frame);
	if(subband) voMP3DecMem_free(pMemOP, subband);	
	if(decoder)	voMP3DecMem_free(pMemOP, decoder);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;	
}

VO_U32 VO_API voMP3DecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	MP3DecInfo		*decoder;
	FrameStream		*stream;
	int				len;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (MP3DecInfo *)hCodec;
	stream = decoder->stream;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}	

#ifdef LOGDUMP
	if(dump)
	{
		fwrite(&pInput->Length, 1, 4, dump);
		fwrite(pInput->Buffer, 1, pInput->Length, dump);
	}
#endif

	stream->buffer = pInput->Buffer;
	stream->inlen = pInput->Length;

	stream->this_frame = stream->buffer;
	stream->length = stream->inlen;
	stream->usedlength = 0;

	if(stream->storelength)
	{
		len = MIN( BUFFER_DATA - stream->storelength, stream->inlen);
		decoder->pvoMemop->Copy(VO_INDEX_DEC_MP3, stream->buffer_bk  + stream->storelength, 
			stream->buffer, len);

		stream->this_frame = stream->buffer_bk;
		stream->length = stream->storelength + len;
		stream->usedlength = -stream->storelength;
	}

	return VO_ERR_NONE;
}


VO_U32 VO_API voMP3DecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	MP3DecInfo		*decoder;
	FrameStream		*stream;
	FrameDataInfo   *frame;
	FrameHeader		*header;
	SubbandInfo		*subband;
	int				len;
	unsigned int	nch, ns;
	short*			outbuf;
	unsigned char*  start;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (MP3DecInfo *)hCodec;
	stream = decoder->stream;
	frame  = decoder->frame;
	subband = decoder->subband;
	header = &frame->header;	

	if(NULL == pOutput || NULL == pOutput->Buffer )
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}
	
	start = stream->this_frame;
	len = voMP3DecHeaderDecode(header, stream, &decoder->header_bk);
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

			len = voMP3DecHeaderDecode(header, stream, &decoder->header_bk);
		}
	}

	if(len < 0)
	{
		stream->usedlength += (stream->this_frame - start);
		len = stream->length;		
		//if(stream->storelength == 0)
		{	
			decoder->pvoMemop->Copy(VO_INDEX_DEC_MP3, stream->buffer_bk, 
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

	frame->nGrans = (header->version == MPEG1 && header->layer == MPA_LAYER_III) ? 2 : 1;
	switch(header->layer)
	{
	case 1:
		len = voMP3DecLayerI(frame ,stream);
		break;
	case 2:
		len = voMP3DecLayerII(frame ,stream);
		break;
	case 3:
		len = voMP3DecLayerIII(frame ,stream, decoder->pvoMemop);
		break;
	default:
		break;
	}

	stream->this_frame += header->framelen;
	stream->length -= header->framelen;

	if(stream->storelength)
	{
		int leftlen;
		int length = stream->this_frame - start;
		stream->usedlength += length;
		if(length > stream->storelength)
		{
			leftlen = length - stream->storelength;
			stream->buffer += leftlen;
			stream->inlen  -= leftlen;
			stream->this_frame = stream->buffer;
			stream->length = stream->inlen;
			stream->storelength = 0;
		}
		else
		{			
			decoder->pvoMemop->Copy(VO_INDEX_DEC_MP3, stream->buffer_bk, 
				stream->this_frame, stream->length);
			stream->usedlength += length;
			stream->this_frame = stream->buffer_bk;
			stream->storelength = stream->length;
		}		
	}
	else
	{
		stream->usedlength += stream->this_frame - start;
	}

	if(len < 0) 
	{
		pOutput->Length = 0;
		return VO_ERR_MP3_INVFRAME;
	}	

  if(len==VO_MP3_Mdoule_INPUT_BUFFER_SMALL){
    pOutput->Length = 0;
    return VO_ERR_INPUT_BUFFER_SMALL;
  }

	nch = header->channels;
	ns  = header->layer == MPA_LAYER_I ? 12 : 
		((header->version > MPEG1 && header->layer == MPA_LAYER_III) ? 18 : 36);
	
	len = SBLIMIT * ns * nch *sizeof(short);
	if(len > pOutput->Length)
	{
		pOutput->Length = 0;	
		if(pOutInfo)
		{
			pOutInfo->InputUsed = stream->usedlength;
		}
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	pOutput->Length = len;
	outbuf = (short *)pOutput->Buffer;

	voMP3DecSubbandFrame(frame, subband, outbuf, nch, ns);

	if(pOutInfo)
	{
		pOutInfo->Format.Channels = nch;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->Format.SampleRate = header->samplerate;
		pOutInfo->InputUsed = stream->usedlength;
	}

	decoder->pvoMemop->Copy(VO_INDEX_DEC_MP3, &decoder->header_bk, header, sizeof(FrameHeader));

#ifdef TIME_LIMITATION
	 FrameNum++;
	 if(FrameNum > 180*header->samplerate/1152)
	 {
		 decoder->pvoMemop->Set(VO_INDEX_DEC_MP3, pOutput->Buffer, 0, pOutput->Length);
		 return VO_ERR_LICENSE_ERROR;
	 }
#endif


#if defined (LCHECK)
	voCheckLibCheckAudio(decoder->hCheck, pOutput, &(pOutInfo->Format));
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3DecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	MP3DecInfo *decoder;
	FrameStream  *stream;
	SubbandInfo	*subband;
	FrameDataInfo  *frame = NULL;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	decoder = (MP3DecInfo *)hCodec;
	stream = decoder->stream;
	subband = decoder->subband;
	frame = decoder->frame;

	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		if(pData == NULL)
			return VO_ERR_INVALID_ARG;

#ifdef LOGDUMP
		if(dump)
		{
			fwrite(pData, 1, 4, dump);
		}
#endif

		if(*((int *)pData))
		{
			stream->inlen = 0;
			stream->length = 0;
			stream->storelength = 0;
			stream->md_len = 0;
			stream->usedlength = 0;

			decoder->pvoMemop->Set(VO_INDEX_DEC_MP3, subband, 0, sizeof(SubbandInfo));
			decoder->pvoMemop->Set(VO_INDEX_DEC_MP3, frame, 0, sizeof(FrameDataInfo));
		}
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3DecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	MP3DecInfo		*decoder;
	FrameHeader     *header;
	VO_AUDIO_FORMAT *pFormat;
	if(NULL == hCodec || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	decoder = (MP3DecInfo *)hCodec;
	header  = &decoder->frame->header;

	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = header->channels;
		pFormat->SampleBits = 16;
		pFormat->SampleRate = header->samplerate;
		break;
	case VO_PID_AUDIO_BITRATE:
		*((int *)pData) = header->bitrate;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voMP3DecUninit(VO_HANDLE hCodec)
{
	/* release the decoder */
	MP3DecInfo *decoder;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	decoder = (MP3DecInfo *)hCodec;
	pMemOP = decoder->pvoMemop;

	if(decoder->stream)	{
		if (decoder->stream->main_data) {
			voMP3DecMem_free(pMemOP,decoder->stream->main_data);
			decoder->stream->main_data = NULL;
		}
		voMP3DecMem_free(pMemOP,decoder->stream);
		decoder->stream = NULL;
	}

	if(decoder->decoder_buf){
		voMP3DecMem_free(pMemOP, decoder->decoder_buf);
		decoder->decoder_buf = NULL;
	}
	
	if(decoder->frame) {
		voMP3DecMem_free(pMemOP, decoder->frame);
		decoder->frame = NULL;
	}

	if(decoder->subband) {
		voMP3DecMem_free(pMemOP, decoder->subband);
		decoder->subband = NULL;
	}

#if defined (LCHECK)
	voCheckLibUninit(decoder->hCheck);
#endif
#ifdef LOGDUMP
	if(dump)
		fclose(dump);
#endif
	voMP3DecMem_free(pMemOP, decoder);

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetMP3DecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voMP3DecInit;
	pDecHandle->SetInputData = voMP3DecSetInputData;
	pDecHandle->GetOutputData = voMP3DecGetOutputData;
	pDecHandle->SetParam = voMP3DecSetParam;
	pDecHandle->GetParam = voMP3DecGetParam;
	pDecHandle->Uninit = voMP3DecUninit;

	return VO_ERR_NONE;
}
