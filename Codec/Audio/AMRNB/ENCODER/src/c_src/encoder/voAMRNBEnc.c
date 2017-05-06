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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "cnst.h"
#include "mode.h"
#include "frame.h"
#include "strfunc.h"
#include "sp_enc.h"
#include "pre_proc.h"
#include "sid_sync.h"
#include "vadname.h"
#include "e_homing.h"
#include "basic_op.h"
#include "AMRNB_E_API.h"
#include "voAudio.h"
#include "mem_align.h"
#include "voAMRNB.h"
#include "voIndex.h"
#include "voCheck.h"

#define SERIAL_FRAMESIZE (MAX_SERIAL_SIZE + 5)
#define Frame_Maxsize  320  
#define Frame_MaxByte  320 
#define MIN(a,b)	 ((a) < (b)? (a) : (b))

typedef struct{
	unsigned char *set_ptr;
	unsigned char *frame_ptr;
	unsigned char *frame_ptr_bk;
	int  set_len;
	int  framebuffer_len; 
	int  used_len;
}FrameStream;

typedef struct
{
	FrameStream      *stream;
	AMRNBELibParam   param;
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;	
	VO_HANDLE        hDec;
	VO_PTR           hCheck;
}GlobalAMRNBEData;

typedef struct _AMREINFO
{
	Speech_Encode_FrameState* speech_encoder_state;
	sid_syncState* sid_state;
	int mode;
	int speed;
} AMREINFO;

VO_HANDLE VO_API AMR_Enc_Open(int mode, int dtx, VO_MEM_OPERATOR *pMemOP)
{
	AMREINFO* info = (AMREINFO*)voAMRNBEnc_mem_malloc(pMemOP, sizeof(AMREINFO), 32);

	pMemOP->Set(VO_INDEX_ENC_AMRNB, info, 0, sizeof(AMREINFO));
	if (voAMRNBEnc_Speech_Encode_Frame_init(&info->speech_encoder_state, dtx, pMemOP) || voAMRNBEnc_sid_sync_init (&info->sid_state, pMemOP))
	{
		voAMRNBEnc_mem_free(pMemOP, info);
		return 0;
	}
	info->mode = mode;
	return (VO_HANDLE)info;
}

void VO_API AMR_Enc_Close(VO_HANDLE hEnc, VO_MEM_OPERATOR *pMemOP)
{
	AMREINFO* info = (AMREINFO*)hEnc;
	if(info)
	{
		voAMRNBEnc_Speech_Encode_Frame_exit(&info->speech_encoder_state, pMemOP);
		voAMRNBEnc_sid_sync_exit (&info->sid_state, pMemOP);
		voAMRNBEnc_mem_free(pMemOP, info);
	}
}

int VO_API AMRNB_Encode(VO_HANDLE hEnc, short* dataIn, unsigned char* dataOut,int frameType)
{
	Word16 packed_size;
	Word16 serial[SERIAL_FRAMESIZE];
	Word16 reset_flag;                          /* changed eedodr */
	enum Mode used_mode;
	enum TXFrameType tx_type;
	AMREINFO* info = (AMREINFO*)hEnc;
	int mode;
	memset(serial, 0, SERIAL_FRAMESIZE * sizeof(Word16));
	reset_flag = voAMRNBEnc_encoder_homing_frame_test(dataIn);	   /* check for homing frame */
	mode = info->mode;

	/* encode speech */
	voAMRNBEnc_Speech_Encode_Frame(info->speech_encoder_state, mode, dataIn, serial, &used_mode); 
	/* include frame type and mode information in serial bitstream */
	voAMRNBEnc_sid_sync(info->sid_state, used_mode, &tx_type);
	//hbfTODO isolate the file format code from voAMRNBEnc_PackBits
	packed_size = voAMRNBEnc_PackBits(used_mode, mode, tx_type, frameType, serial, dataOut);
	/* perform homing if homing frame was detected at encoder input */
	if (reset_flag != 0)
	{
		voAMRNBEnc_Speech_Encode_Frame_reset(info->speech_encoder_state);
		voAMRNBEnc_sid_sync_reset(info->sid_state);
	}
	return packed_size;
}
/**
* Initialize a AMRNB encoder instance use default settings.
* \param phCodec [out] Return the AMRNB encoder handle.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/
void *g_hAMRNBEncInst = NULL;

VO_U32 VO_API voAMRNBEnc_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	GlobalAMRNBEData* gData;
	AMRNBELibParam *inParam;
	FrameStream    *stream;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
  //int interMem = 0;

	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
    return VO_ERR_INVALID_ARG;
		/*voMemoprator.Alloc = cmnMemAlloc;
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

	gData = (GlobalAMRNBEData*)voAMRNBEnc_mem_malloc(pMemOP, sizeof(GlobalAMRNBEData), 32);
	if(gData==NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->stream = (FrameStream *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(gData->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->stream->frame_ptr = (unsigned char *)voAMRNBEnc_mem_malloc(pMemOP, Frame_Maxsize, 32);
	if(gData->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;

	stream = gData->stream;
	stream->set_ptr = NULL;
	stream->frame_ptr_bk = stream->frame_ptr;
	stream->framebuffer_len = 0;
	stream->set_len = 0;

	inParam = &(gData->param);

	//default AMR_NB encoder setting
	inParam->frameType = VOAMRNB_RFC3267;
        inParam->mode = VOI_MD122;
	//***********************************

	inParam->hEnc= gData->hDec = AMR_Enc_Open(inParam->mode, 1, pMemOP);

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

	*phCodec = (VO_HANDLE)gData;
    //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	{
	    voCheckLibInit(&(gData->hCheck), VO_INDEX_ENC_AMRNB, 1, g_hAMRNBEncInst, pUserData->libOperator);
	}
	else
	{
	    voCheckLibInit(&(gData->hCheck), VO_INDEX_ENC_AMRNB, 1, g_hAMRNBEncInst, NULL);
	}
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBEnc_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	int  len;
	GlobalAMRNBEData* gData;
	FrameStream *stream;
    VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (GlobalAMRNBEData *)hCodec;
	stream = gData->stream;
	pMemOP = gData->pvoMemop;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->used_len   = 0;

	if(stream->framebuffer_len)
	{
		len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
	    pMemOP->Copy(VO_INDEX_ENC_AMRNB, stream->frame_ptr_bk + stream->framebuffer_len, stream->set_ptr, len);
		stream->set_len -= len;
		stream->set_ptr += len;
		stream->used_len += len;
		stream->framebuffer_len += len;
		stream->frame_ptr = stream->frame_ptr_bk;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBEnc_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	GlobalAMRNBEData *gData = (GlobalAMRNBEData*)hCodec;
	AMRNBELibParam   *inParam;
	VO_MEM_OPERATOR  *pMemOP;
	FrameStream  *stream = (FrameStream *)gData->stream;

	inParam = &(gData->param);
	pMemOP = (VO_MEM_OPERATOR  *)gData->pvoMemop;


	if((stream->set_len < Frame_MaxByte) && (stream->framebuffer_len != 320))
	{
       pMemOP->Copy(VO_INDEX_ENC_AMRNB, stream->frame_ptr, stream->set_ptr, stream->set_len);
	   stream->framebuffer_len += stream->set_len;
	   stream->used_len += stream->set_len;
	   pAudioFormat->InputUsed = stream->used_len;
	   return  VO_ERR_INPUT_BUFFER_SMALL;
	}

	if(inParam==NULL)
		return VO_ERR_WRONG_STATUS;
	inParam->outputStream = (unsigned short*)pOutput->Buffer;

	if(stream->framebuffer_len == 320)
	    inParam->inputStream = stream->frame_ptr;
	else
		inParam->inputStream = stream->set_ptr;
#if ONLY_ENCODE_122
	inParam->mode = VOI_MD122;
#endif

	inParam->outputSize = AMRNB_Encode(gData->hDec, (Word16*)inParam->inputStream, (UWord8*)inParam->outputStream,inParam->frameType);
	pOutput->Length = inParam->outputSize;
	if(stream->framebuffer_len == 320)
	{
		stream->framebuffer_len -= 320;
	}
	else
	{
	    stream->set_ptr  += 320;
	    stream->set_len  -= 320;
		stream->used_len  += 320;
	}

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = stream->used_len;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRNBEnc_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	GlobalAMRNBEData* gData = (GlobalAMRNBEData*)hCodec;
	AMRNBELibParam *inParam = &(gData->param);
	AMREINFO* info = (AMREINFO*)(gData->hDec);
	FrameStream *stream = (FrameStream *)(gData->stream);
	int              *lValue = (int*)pData;

	if(inParam==NULL||info==NULL)
		return VO_ERR_WRONG_STATUS;

	switch(uParamID)
	{
	case VO_PID_AMRNB_FRAMETYPE:
		if(*lValue < VOAMRNB_RFC3267||*lValue > VOAMRNB_IF2)
			return VO_ERR_WRONG_PARAM_ID; 
		inParam->frameType = *lValue;
		break;
	case VO_PID_AMRNB_MODE:
		{
			if(*lValue < VOAMRNB_MD475||*lValue > VOAMRNB_MD122)
				return VO_ERR_WRONG_PARAM_ID; 
			info->mode = *lValue;
		}
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	case VO_PID_COMMON_FLUSH:
	    stream->set_ptr = NULL;
	    stream->framebuffer_len = 0;
	    stream->set_len = 0;
		break;
	case VO_PID_AMRNB_DTX:
		info->speech_encoder_state->cod_amr_state->dtx = (Word16)*lValue;	
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;	
}

VO_U32 VO_API voAMRNBEnc_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;
	GlobalAMRNBEData* gData = (GlobalAMRNBEData*)hCodec;
	AMRNBELibParam *inParam;
	AMREINFO* info = (AMREINFO*)(gData->hDec);
	if (gData==NULL||info==NULL) 
		return VO_ERR_OUTOF_MEMORY;
	inParam = &(gData->param);

	switch(uParamID)
	{
	case VO_PID_AMRNB_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;

			fmt->Channels    = 1;
			fmt->SampleRate  = 8000;
			fmt->SampleBits  = 16;
			break;
		}

	case VO_PID_AMRNB_CHANNELS:
		temp = 1;
		(*(int *)pData) = temp;
		break;
	case VO_PID_AMRNB_SAMPLERATE:
		temp = 8000;
		(*(int *)pData) = temp;
		break;
	case VO_PID_AMRNB_FRAMETYPE:
		(*(int *)pData) = inParam->frameType;
		break;
	case VO_PID_AMRNB_MODE:
		{
			AMREINFO* info = (AMREINFO*)(gData->hDec);
			temp = info->mode;
			(*(int *)pData) = temp;
		}
		break;
	case VO_PID_AMRNB_DTX:
		(*(int *)pData) = info->speech_encoder_state->cod_amr_state->dtx;	
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;

}

VO_U32 VO_API voAMRNBEnc_Uninit(VO_HANDLE hCodec)
{
	GlobalAMRNBEData* gData = (GlobalAMRNBEData*)hCodec;
	VO_MEM_OPERATOR *pMemOP;
	pMemOP = gData->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(gData->hCheck);
#endif
	if(gData->stream)
	{
		if(gData->stream->frame_ptr_bk)
		{
	     voAMRNBEnc_mem_free(pMemOP, gData->stream->frame_ptr_bk);
         gData->stream->frame_ptr_bk = NULL;
		}
	    voAMRNBEnc_mem_free(pMemOP, gData->stream);
		gData->stream = NULL;
	}
	if(hCodec)
	{
		if(gData->hDec)
			AMR_Enc_Close(gData->hDec, pMemOP);

		voAMRNBEnc_mem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAMRNBEncAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voAMRNBEnc_Init;
	pDecHandle->SetInputData = voAMRNBEnc_SetInputData;
	pDecHandle->GetOutputData = voAMRNBEnc_GetOutputData;
	pDecHandle->SetParam = voAMRNBEnc_SetParam;
	pDecHandle->GetParam = voAMRNBEnc_GetParam;
	pDecHandle->Uninit = voAMRNBEnc_Uninit;

	return VO_ERR_NONE;
}

