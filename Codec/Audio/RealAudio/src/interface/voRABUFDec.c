
#include		"ra_decode.h"
#include		"ra_depack.h"
#include        "VORA_sdk.h"
#include        <string.h>
#ifndef NULL
#define NULL 0
#endif

typedef struct  
{
	ra_decode*		pDecoder;
	ra_format_info*	ra_format;
	int*			pattern;
	unsigned char*	superBlock;
	unsigned char*	frameBlock;
	int		superid;
	int		interleave_block_size;
	int		interleave_factor;
	int		interleave_id;
	int		interleave_flag;
	int		stream_type;
	int		ulFourCC;
	int		frameNum;
	int		maxoutSize;
	
}VORA_RMBufParam;

#define READ32(data32, buf)		\
	(data32) = ((INT32)*(buf)++)<<24;	\
    (data32) |= ((INT32)*(buf)++)<<16;	\
    (data32) |= ((INT32)*(buf)++)<<8;	\
    (data32) |= ((INT32)*(buf)++);	

#define READ16(data16, buf)		\
    (data16) = ((INT32)*(buf)++)<<8;	\
    (data16) |= ((INT32)*(buf)++);

#define READ8(data8, buf)		\
    (data8) = ((INT32)*(buf)++);

#define SKIP(buf, step)			\
	(buf) += (step);


HX_RESULT	format4_header(unsigned char* bufs, VORA_RMBufParam* param)
{
	HX_RESULT hr;
	ra_format_info *format;
	int data32;
	short data16;

	hr = HXR_OK;
	format = param->ra_format;

	SKIP(bufs, 10);									//reserved
	SKIP(bufs, 4);									//version, revision
	
	READ16(data16, bufs);
	READ16(format->usFlavorIndex, bufs);
	READ32(format->ulGranularity, bufs);
	READ32(data32, bufs);							//total_bytes
	READ32(data32, bufs);							//bytes_per_minute
	READ32(data32, bufs);							//bytes_per_minute2
		
	READ16(param->interleave_factor, bufs);
	READ16(param->interleave_block_size, bufs);
	READ32(data32, bufs);							//user_data
	
	READ32(format->ulSampleRate , bufs);
	format->ulSampleRate >>= 16;	
	format->ulActualRate = format->ulSampleRate;

	READ16(format->usBitsPerSample, bufs);
	READ16(format->usNumChannels, bufs);
	
	READ32(param->interleave_id, bufs);
	READ32(param->ulFourCC, bufs);

	READ8(param->interleave_flag, bufs);	
	READ8(data32, bufs);
	READ8(param->stream_type, bufs);
	
	format->ulOpaqueDataSize = 0;		

	return hr;
}

HX_RESULT	format5_header(unsigned char* bufs, VORA_RMBufParam* param)
{
	HX_RESULT hr;
	ra_format_info *format;
	int data32;
	short data16;

	hr = HXR_OK;
	format = param->ra_format;

	SKIP(bufs, 10);									//reserved
	SKIP(bufs, 4);									//version, revision
	
	READ16(data16, bufs);							//header_bytes
	READ16(format->usFlavorIndex, bufs);
	READ32(format->ulGranularity, bufs);
	READ32(data32, bufs);							//total_bytes
	READ32(data32, bufs);							//bytes_per_minute
	READ32(data32, bufs);							//bytes_per_minute2
		
	READ16(param->interleave_factor, bufs);
	READ16(param->interleave_block_size, bufs);
	READ16(format->ulBitsPerFrame, bufs);

	READ32(data32, bufs);							//user_data
	
	READ32(format->ulSampleRate , bufs);
	READ32(format->ulActualRate , bufs);
	format->ulSampleRate >>= 16;	
	format->ulActualRate >>= 16;

	READ16(format->usBitsPerSample, bufs);
	READ16(format->usNumChannels, bufs);
	
	READ32(param->interleave_id, bufs);					//codec_id_length
	READ32(param->ulFourCC, bufs);
	
	READ8(param->interleave_flag, bufs);
	READ8(data32, bufs);
	READ8(param->stream_type, bufs);
	READ8(data16, bufs);								// has_interleave_pattern_flag

	data32	= (param->interleave_factor *
			param->interleave_block_size) / format->ulBitsPerFrame;
		
	param->pattern = (int *)malloc(data32*4);
	if(param->pattern == NULL)
		return HXR_FAIL;

	if(data16)
	{
		for(data16 = 0; data16 < data32; data16++){
			READ16(param->pattern[data16], bufs);
		}
	}
	else if(param->interleave_id == 0x67656E72)
	{
		int i = 0; /* Frame index within superblock */
		int f = 0; /* Frame index within block */
		int b = 0; /* Block index within superblock */
		int even = 1;
		int frames_per_superblock = data32;
		int frames_per_block = param->interleave_block_size / format->ulBitsPerFrame;
		if (param->interleave_factor == 1)
		{
			for (i = 0; i < frames_per_superblock; i++)
			{
				param->pattern[i] = i;
			}
		}		
		
		else
		{
			while (i < frames_per_superblock)
			{
				param->pattern[i] = b * frames_per_block + f;
				i++;
				b += 2;
				if (b >= param->interleave_factor)
				{
					if (even)
					{
						even = 0;
						b = 1;
					}
					else
					{
						even = 1;
						b = 0;
						f++;
					}
				}
			}
		}
	}

	READ32(format->ulOpaqueDataSize, bufs);
	
	format->pOpaqueData = bufs;

	
	return hr;
}

HX_RESULT	ra_decoder_unpackerHeader(unsigned char* bufs, VORA_RMBufParam* param)
{
	HX_RESULT hr;
	int data32;
	short data16;

	hr = HXR_OK;

	
	READ32(data32, bufs);
	if(data32 != 0x2E7261FD)
		return HXR_FAIL;

	READ16(data16, bufs);
	if(data16 == 4)
	{
		hr = format4_header(bufs, param);
	}
	else if(data16 == 5)
	{
		hr = format5_header(bufs, param);
	}
	else
	{
		return HXR_FAIL;
	}
	
	return hr;
}

VO_U32 VO_API voRealAudioRMBufDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_AUDIO_CODECAPI	*DecAPI;
	//ra_format_info	ra_format;
	VORA_SDK* pDec =(VORA_SDK*)(*phCodec);
	VO_MEM_OPERATOR	*pMemOP;

	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	DecAPI = &(pDec->RealDecAPI);
	pMemOP = pDec->pvoMemop;

	DecAPI->Init			= voRealAudioRMBufDecInit;
	DecAPI->Uninit			= voRealAudioRMBufDecUninit;
	DecAPI->SetInputData	= NULL;
	DecAPI->GetOutputData	= voRealAudioRMBufDecGetOutputData;
	DecAPI->SetParam		= voRealAudioRMBufDecSetParameter;
	DecAPI->GetParam		= voRealAudioRMBufDecGetParameter;

	pDec->customData	= (void*)malloc(sizeof(VORA_RMBufParam));
	{
		VORA_RMBufParam* param=(VORA_RMBufParam*)(pDec->customData);
		param->pDecoder = (ra_decode*)ra_decode_create(NULL,NULL, pMemOP);
		if(param->pDecoder)
		{
			HX_RESULT hr;
			unsigned char* bufparams	= pDec->bufPara;

			param->ra_format = (ra_format_info*)(rm_memory_malloc(pMemOP, NULL,sizeof(ra_format_info)));

			param->interleave_block_size = 0;
			param->interleave_factor = 0;
			
			hr = ra_decoder_unpackerHeader(bufparams, param);
			if(HXR_OK != hr)
				return hr;
			
			param->ra_format->usAudioQuality = 100;
			param->frameNum = 0;
			param->superid = 0;

			hr = ra_decode_init(param->pDecoder, param->ulFourCC, NULL, 0, param->ra_format, pMemOP, pDec->pLibOperator);
			if(HXR_OK != hr)
				return hr;

			param->superBlock = (unsigned char*)rm_memory_malloc(pMemOP, NULL,param->interleave_factor * param->interleave_block_size);
			if(param->superBlock == NULL)
				return VO_ERR_OUTOF_MEMORY;

			param->frameBlock = (unsigned char*)rm_memory_malloc(pMemOP, NULL, param->ra_format->ulGranularity);
			if(param->frameBlock == NULL)
				return VO_ERR_OUTOF_MEMORY;

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


VO_U32 VO_API voRealAudioRMBufDecUninit(VO_HANDLE hCodec)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	VO_MEM_OPERATOR	*pMemOP;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	pMemOP = pDec->pvoMemop;

	if(pDec->customData)
	{
		VORA_RMBufParam* param = (VORA_RMBufParam*)pDec->customData;

		if(param->ra_format)
		{
			rm_memory_free(pMemOP, NULL, param->ra_format);
			param->ra_format = NULL;
		}

		if(param->pattern)
		{
			rm_memory_free(pMemOP, NULL, param->pattern);
			param->pattern = NULL;
		}

		if(param->superBlock)
		{
			rm_memory_free(pMemOP, NULL, param->superBlock);
			param->superBlock = NULL;
		}

		if(param->frameBlock)
		{
			rm_memory_free(pMemOP, NULL, param->frameBlock);
			param->frameBlock = NULL;
		}


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

VO_U32 VO_API voRealAudioRMBufDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMBufParam* param;
	unsigned char		*pTemp;
	UINT32 numBytesConsumed = 0;
	UINT32 numSamplesOut = 0;
	VO_MEM_OPERATOR	*pMemOP;
	int temp;
	int i, j;
	HX_RESULT hr;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RMBufParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;

	pMemOP = pDec->pvoMemop;

	if(param->interleave_flag)
	{
		int dwFramePerBlock;
		if(param->frameNum < param->interleave_factor)
		{
			if(pDec->inbuf != NULL && pDec->inlength != 0)
			{
				pMemOP->Copy(VO_INDEX_DEC_RA, param->superBlock + param->frameNum * param->interleave_block_size, 
					pDec->inbuf,  param->interleave_block_size);
				param->frameNum++;
			}
			if(param->frameNum < param->interleave_factor)
				return VO_ERR_INPUT_BUFFER_SMALL;
		}

		temp =  param->ra_format->ulBitsPerFrame;
		pTemp = param->frameBlock;
		dwFramePerBlock = param->interleave_block_size / temp;
		j = param->superid;

		for(i = 0; i < dwFramePerBlock; i++, j++)
		{
			int dwSrcFrameNumber = param->pattern[j];
			pMemOP->Copy(VO_INDEX_DEC_RA, pTemp, param->superBlock + 
				(dwSrcFrameNumber / dwFramePerBlock) * param->interleave_block_size + (dwSrcFrameNumber % dwFramePerBlock) * temp, temp);
			pTemp += temp;
		}

		param->superid += dwFramePerBlock;
		if(param->superid == param->interleave_factor * dwFramePerBlock){
			param->superid = 0;
			param->frameNum = 0;
		}

		hr = ra_decode_decode(param->pDecoder, 
			param->frameBlock, 
			param->interleave_block_size, 
			&numBytesConsumed, 
			(UINT16*)pOutBuffer->Buffer, 
			param->maxoutSize, 
			&numSamplesOut, 
			0xFFFFFFFF);
	}
	else
	{
		hr = ra_decode_decode(param->pDecoder, 
			pDec->inbuf, 
			pDec->inlength, 
			&numBytesConsumed, 
			(UINT16*)pOutBuffer->Buffer,  
			param->maxoutSize, 
			&numSamplesOut, 
			0xFFFFFFFF);
	}

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

	pDec->inbuf += pDec->inlength;
	pDec->inlength = 0;

	return hr;
}

VO_U32 VO_API voRealAudioRMBufDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMBufParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RMBufParam*)pDec->customData;
	if(param==NULL)
		return VO_ERR_WRONG_STATUS;

	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		if(*((int *)pData))
		{
			param->superid = 0;
			param->frameNum = 0;
		}
		break;
	default:
	    return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioRMBufDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec =(VORA_SDK*)hCodec;
	VORA_RMBufParam* param;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	param = (VORA_RMBufParam*)pDec->customData;
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
	case VOID_PID_RA_BLOCKSIZE:
		*((int *)pData) = param->ra_format->ulGranularity;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

