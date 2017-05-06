/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voAMRNB.c

Contains:	AMR_NB API Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-06		LHP			Create file

*******************************************************************************/
#include    "voAudio.h"
#include	"mem_align.h"

#include    "typedef.h"
#include    "cnst.h"
#include    "mode.h"
#include    "frame.h"
#include    "strfunc.h"
#include    "sp_dec.h"
#include    "d_homing.h"
#include    "basic_op.h"
#include    "AMRNB_D_API.h"
#include    "voAMRNB.h"
#include    "voIndex.h"
#include    "voCheck.h"


#define    Frame_Maxsize  1 * 1024 
#define    Frame_MaxByte  13 
#define    MIN(a,b)	 ((a) < (b)? (a) : (b))
#define    SERIAL_FRAMESIZE  (MAX_SERIAL_SIZE + 5) 

void *g_hAMRNBDecInst = NULL;

typedef struct{
	unsigned char *set_ptr;
	unsigned char *frame_ptr;
	unsigned char *frame_ptr_bk;
	int  set_len;
	int  framebuffer_len; 
	int  frame_storelen;
	int  used_len;
}FrameStream;

typedef struct
{
	FrameStream      *stream;
	AMRNBLibParam    param;
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;
	VO_HANDLE        hDec;
	VO_PTR           hCheck;
}GlobalAMRNBData;

typedef struct _AMRDINFO
{
	Speech_Decode_FrameState* speech_decoder_state;
	short reset_flag;
} AMRDINFO;

short   packedSize[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};
unsigned char IF2_packedsize[16] = {13, 14, 16, 18, 19, 21, 26, 31, 6,  6,  6,  6,  0,  0,  0,  1};

void voAMRNBDec_UpdateFrameBuffer(FrameStream *stream, VO_MEM_OPERATOR *pMemOP)
{
	int  len;
	len  = MIN(Frame_Maxsize - stream->frame_storelen, stream->set_len);
	pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk + stream->frame_storelen , stream->set_ptr, len);
	stream->set_len -= len;
	stream->set_ptr += len;
	stream->framebuffer_len = stream->frame_storelen + len;
	stream->frame_ptr = stream->frame_ptr_bk;
	stream->used_len += len;
}

VO_HANDLE  AMR_Dec_Open(VO_MEM_OPERATOR *pMemOP)
{
	AMRDINFO* info = (AMRDINFO*)voAMRNBDecmem_malloc(pMemOP, sizeof(AMRDINFO), 32);
	if (Speech_Decode_Frame_init(&info->speech_decoder_state, pMemOP))
	{
		voAMRNBDecmem_free(pMemOP, info);
		return 0;
	}
	info->reset_flag = 1;
	return (VO_HANDLE)(info);
}

void  AMR_Dec_Close(VO_HANDLE hDec,VO_MEM_OPERATOR *pMemOP)
{
	AMRDINFO* info = (AMRDINFO*)hDec;
	if(info)
	{
		Speech_Decode_Frame_exit(&info->speech_decoder_state, pMemOP);
		voAMRNBDecmem_free(pMemOP, info);
		hDec = NULL;
	}
}


static int VO_API AMR_Dec_Decode2(VO_HANDLE hDec, unsigned char q, unsigned char ft, unsigned char* dataIn, unsigned short* dataOut,AMRNBLibParam *Params)
{
	int i;
	short serial[SERIAL_FRAMESIZE];   /* coded bits */
	enum  RXFrameType rx_type = (enum RXFrameType)0;
	enum  Mode mode = (enum Mode)0;
	short reset_flag = 0;

	AMRDINFO* info = (AMRDINFO*)hDec;

	rx_type = voAMRNBDecUnpackBits(q, ft, dataIn, Params->frameType,&mode, serial);
	if (rx_type == RX_NO_DATA) 
		mode = info->speech_decoder_state->prev_mode;
	else 
		info->speech_decoder_state->prev_mode = mode;

	/* if homed: check if this frame is another homing frame */
	if (info->reset_flag == 1) /* only check until end of first subframe */
		reset_flag = voAMRNBDecHomingTestFirst(serial, mode);

	/* produce encoder homing frame if homed & input=decoder homing frame */
	if ((reset_flag != 0) && (info->reset_flag != 0))
	{
		for (i = 0; i < L_FRAME; i++)
		{
			dataOut[i] = EHF_MASK;
		}
	}
	else
	{     
		Speech_Decode_Frame(info->speech_decoder_state, mode, serial, rx_type, dataOut);
	}

	/* if not homed: check whether current frame is a homing frame */
	if (info->reset_flag == 0)
	{
		/* check whole frame */
		reset_flag = voAMRNBDecHomingTest(serial, mode);
	}

	/* reset decoder if current frame is a homing frame */
	if (reset_flag != 0)
	{
		Speech_Decode_Frame_reset(info->speech_decoder_state);
	}
	info->reset_flag = reset_flag;
	return VO_ERR_NONE;
}


VO_U32 VO_API AMRNBReceiveData2(GlobalAMRNBData* gData)
{
	AMRNBLibParam *inParam = &(gData->param);
	unsigned char* head=inParam->inputStream;
	FrameStream *stream = gData->stream;
	VO_MEM_OPERATOR *pMemOP = (VO_MEM_OPERATOR *)gData->pvoMemop;

	switch(inParam->frameType)
	{
	case VOI_RFC3267:
		inParam->mode = head[0]>>3;
		if((inParam->mode < 0) || (inParam->mode > 15)) 
		{
			stream->framebuffer_len = 0;
			return 1;
		}
		else
		{
			inParam->inputSize = packedSize[inParam->mode]+1;
			if(stream->framebuffer_len  < inParam->inputSize)
			{ 
				stream->frame_storelen = stream->framebuffer_len;
				if(stream->frame_storelen)
					pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);

				if(stream->set_len > 0)
				{
					voAMRNBDec_UpdateFrameBuffer(stream, pMemOP);
				}
				if(stream->framebuffer_len < inParam->inputSize)
				{
					return 1;
				}
			}
		}
		inParam->inputStream++;
		break;
	case VOI_IF1:
		inParam->mode = (head[0]>>4);
		if((inParam->mode < 0) || (inParam->mode > 8)) 
		{
			stream->framebuffer_len = 0;
			return 1;
		}
		else
		{
			inParam->inputSize = packedSize[inParam->mode]+3;
			if(stream->framebuffer_len  < inParam->inputSize)
			{ 
				stream->frame_storelen = stream->framebuffer_len;
				if(stream->frame_storelen)
					pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);

				if(stream->set_len > 0)
				{
					voAMRNBDec_UpdateFrameBuffer(stream, pMemOP);
				}
				if(stream->framebuffer_len < inParam->inputSize)
				{
					return 1;
				}
			}
		}
		inParam->inputStream +=3;
		break;
	case VOI_IF2:
		inParam->mode = head[0]&0x0f;
		if((inParam->mode < 0) || (inParam->mode > 8)) 
		{
			stream->framebuffer_len = 0;
			return 1;
		}
		else
		{
			inParam->inputSize = IF2_packedsize[inParam->mode];
			if(stream->framebuffer_len  < inParam->inputSize)
			{ 
				stream->frame_storelen = stream->framebuffer_len;
				if(stream->frame_storelen)
					pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);

				if(stream->set_len > 0)
				{
					voAMRNBDec_UpdateFrameBuffer(stream, pMemOP);
				}
				if(stream->framebuffer_len < inParam->inputSize)
				{
					return 1;
				}
			}
		}
		break;
	case VOI_RAWDATA:
		inParam->inputSize = packedSize[inParam->mode];
		if(stream->framebuffer_len  < inParam->inputSize)
		{ 
			stream->frame_storelen = stream->framebuffer_len;
			if(stream->frame_storelen)
				pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);

			if(stream->set_len > 0)
			{
				voAMRNBDec_UpdateFrameBuffer(stream, pMemOP);
			}
			if(stream->framebuffer_len < inParam->inputSize)
			{
				return 1;
			}
		}
		break;
	}
	AMR_Dec_Decode2(gData->hDec, 1, inParam->mode, inParam->inputStream, inParam->outputStream,inParam);
	inParam->outputSize = 320;
	return VO_ERR_NONE;
}

#ifdef DUMP
FILE *dump_log = NULL;
#endif


VO_U32 VO_API voAMRNBDec_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
#ifdef LCHECK
    VO_PTR pTemp = NULL;
#endif
	GlobalAMRNBData * gData;
	AMRNBLibParam   *inParam;
	FrameStream     *stream;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	//int interMem = 0;
	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
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
#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	{
		nRet = voCheckLibInit(&pTemp, VO_INDEX_DEC_AMRNB, pUserData->memflag|1, g_hAMRNBDecInst, pUserData->libOperator);
	}
	else
	{
		nRet = voCheckLibInit(&pTemp, VO_INDEX_DEC_AMRNB, pUserData->memflag|1, g_hAMRNBDecInst, NULL);
	}

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

	gData = (GlobalAMRNBData*)voAMRNBDecmem_malloc(pMemOP, sizeof(GlobalAMRNBData), 32);
	if(gData == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->stream = (FrameStream *)voAMRNBDecmem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(gData->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->stream->frame_ptr = (unsigned char *)voAMRNBDecmem_malloc(pMemOP, Frame_Maxsize, 32);
	if(gData->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;

	stream = gData->stream;
	stream->set_ptr = NULL;
	stream->frame_ptr_bk = stream->frame_ptr;
	stream->set_len = 0;
	stream->framebuffer_len = 0;
	stream->frame_storelen = 0;	

	inParam = &(gData->param);
	gData->hDec = AMR_Dec_Open(pMemOP);
	if(gData->hDec==NULL)
		return VO_ERR_OUTOF_MEMORY;
	/*if(interMem)
	{
		gData->voMemoprator.Alloc = cmnMemAlloc;
		gData->voMemoprator.Copy = cmnMemCopy;
		gData->voMemoprator.Free = cmnMemFree;
		gData->voMemoprator.Set = cmnMemSet;
		gData->voMemoprator.Check = cmnMemCheck;
		pMemOP = &gData->voMemoprator;
	}*/
	gData->pvoMemop = pMemOP;
	//VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
    gData->hCheck = pTemp;
#endif

	*phCodec = (VO_HANDLE)gData;

#ifdef DUMP
	dump_log = fopen("log.dat", "wb+");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBDec_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	//int  len;
	GlobalAMRNBData* gData;
	FrameStream *stream;
	//VO_MEM_OPERATOR *pMemOP;
	//__D("In SetInputData!");
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (GlobalAMRNBData *)hCodec;
	stream = gData->stream;
	//pMemOP = gData->pvoMemop;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->frame_ptr  = stream->frame_ptr_bk;
	stream->used_len    = 0;
	//__D("Out SetInputData!");
	//__D("After SetInputData stream->set_ptr = %d,stream->set_len = %d ", stream->set_ptr, stream->set_len);
	//__D("After SetInputData stream->framebuffer_len = %d", stream->framebuffer_len);

	return VO_ERR_NONE;
}



VO_U32 VO_API voAMRNBDec_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	GlobalAMRNBData *gData;
    AMRNBLibParam   *inParam;
    FrameStream     *stream;
    VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (GlobalAMRNBData*)hCodec;
	inParam = &(gData->param);
	stream = (FrameStream *)gData->stream;
	pMemOP = (VO_MEM_OPERATOR *)gData->pvoMemop;

	//__D("In GetOutputData!");
	if(stream->framebuffer_len  < Frame_MaxByte)
	{
		stream->frame_storelen = stream->framebuffer_len;
		//__D("Before pMemOP->Copy! stream->frame_storelen = %d", stream->frame_storelen);
		if(stream->frame_storelen)
			pMemOP->Copy(VO_INDEX_DEC_AMRNB, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);
		//__D("After pMemOP->Copy!!!");

		if(stream->set_len > 0)
		{
			voAMRNBDec_UpdateFrameBuffer(stream, pMemOP);
		}
		if(stream->framebuffer_len < Frame_MaxByte)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	inParam->inputStream = stream->frame_ptr;
	inParam->outputStream =(unsigned short*)pOutput->Buffer;

	//__D("In AMRNBReceiveData2!");
	if(AMRNBReceiveData2(gData))
	{
		//__D("error branch!!!");
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	//__D("Out AMRNBReceiveData2!");

	pOutput->Length = inParam->outputSize;
	stream->frame_ptr += inParam->inputSize;
	//__D("Before updata stream->framebuffer_len = %d", stream->framebuffer_len);
	stream->framebuffer_len  -= inParam->inputSize;
	//__D("After updata stream->framebuffer_len = %d", stream->framebuffer_len);

#ifdef DUMP
	fwrite(pOutput->Buffer, 1, pOutput->Length, dump_log);
#endif

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels  = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->InputUsed = stream->used_len;
	}
	//__D("end GetOutputData!");
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBDec_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	GlobalAMRNBData  *gData = (GlobalAMRNBData*)hCodec;
	AMRNBLibParam    *inParam;
	AMRDINFO         *info;
	int              *lValue;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	inParam = &(gData->param);
	info = gData->hDec;
	lValue = (int*)pData;

	switch(uParamID)
	{
	case VO_PID_AMRNB_FRAMETYPE:
		if(*lValue<VOAMRNB_RFC3267||*lValue>VOAMRNB_IF2)
			return VO_ERR_WRONG_PARAM_ID; 
		inParam->frameType = *lValue;
		break;
	case VO_PID_COMMON_FLUSH:
		if(*lValue)
		{
			//__D("have runned Flush function!!!");
			Decoder_amr_reset(info->speech_decoder_state->decoder_amrState, 0);
			gData->stream->frame_ptr = gData->stream->frame_ptr_bk;
			gData->stream->frame_storelen = 0;
			gData->stream->framebuffer_len = 0;
			gData->stream->set_len = 0;
			//__D("Finished Flush function!!!");
		}
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBDec_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;
	GlobalAMRNBData *gData = (GlobalAMRNBData*)hCodec;
	AMRNBLibParam   *inParam;
	if (gData==NULL) 
		return VO_ERR_OUTOF_MEMORY;
	inParam = &(gData->param);
	switch(uParamID)
	{
	case VO_PID_AMRNB_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			fmt->Channels = 1;
			fmt->SampleRate = 8000;
			fmt->SampleBits = 16;
			break;
		}

	case VO_PID_AMRNB_CHANNELS:
		temp = 1;
		pData = (void *)(&temp);
		break;
	case VO_PID_AMRNB_SAMPLERATE:
		temp = 8000;
		pData = (void *)(&temp);
		break;
	case VO_PID_AMRNB_FRAMETYPE:
		pData = (VO_PTR*)inParam->frameType;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBDec_Uninit(VO_HANDLE hCodec)
{
	GlobalAMRNBData* gData ;
	VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (GlobalAMRNBData*)hCodec;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(gData->hCheck);
#endif
	pMemOP = gData->pvoMemop;
	if(gData->stream)
	{
		if(gData->stream->frame_ptr_bk)
		{
			voAMRNBDecmem_free(pMemOP, gData->stream->frame_ptr_bk);
			gData->stream->frame_ptr_bk = NULL;
		}
		voAMRNBDecmem_free(pMemOP, gData->stream);
		gData->stream = NULL;
	}

	if(hCodec)
	{
		if(gData->hDec)
			AMR_Dec_Close(gData->hDec, pMemOP);
		voAMRNBDecmem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
#ifdef DUMP
	fclose(dump_log);
#endif
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAMRNBDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voAMRNBDec_Init;
	pDecHandle->SetInputData = voAMRNBDec_SetInputData;
	pDecHandle->GetOutputData = voAMRNBDec_GetOutputData;
	pDecHandle->SetParam = voAMRNBDec_SetParam;
	pDecHandle->GetParam = voAMRNBDec_GetParam;
	pDecHandle->Uninit = voAMRNBDec_Uninit;

	return VO_ERR_NONE;
}
