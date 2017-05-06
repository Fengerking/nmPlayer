/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voAMRWBDec.c

Contains:	AMR_WB Dec API Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-22		LHP			Create file

*******************************************************************************/

#ifdef EVC_AMRWBPLUS
#undef MSVC_AMRWBPLUS
#include <windows.h>
#endif
#include "typedef.h"
#include "basic_op.h"
#include "voCheck.h"
#include "amr_plus_fx.h"
#include "dec_if_fx.h"
#include "voAMRWB.h"
#include "mem_align.h"
#include "cmnMemory.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

/* One encoded frame (bytes) */
/*
* 6 bytes are used both for mode 9 and 10.
* Mode 9 is used in encoder and mode 10 in IF2
*/
#ifdef IF2
const UWord8 block_size[16]= {18, 23, 33, 37, 41, 47, 51, 59, 61, 6, 6, 0, 0, 0, 1, 1};
#else
const UWord8 block_size[16]= {18, 24, 33, 37, 41, 47, 51, 59, 61, 6, 6, 0, 0, 0, 1, 1};
#endif

#define      FRAME_SIZE    640

void *g_hAMRWBDecInst = NULL;

//#define DUMP
#ifdef  DUMP
FILE *dump_dat = NULL;
#endif

VO_U32 VO_API voAMRWB_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
	void *decode_state = NULL;
#ifdef LCHECK
    VO_PTR pTemp;
#endif
    WB_dec_if_state_fx *gData;
	FrameStream     *stream;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;

    int interMem = 0;
	if(pUserData == NULL || (pUserData->memflag & 0xF0) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
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
#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	{
		nRet = voCheckLibInit(&pTemp, VO_INDEX_DEC_AMRWB, pUserData->memflag|1, g_hAMRWBDecInst, pUserData->libOperator);
	}
	else
	{
		nRet = voCheckLibInit(&pTemp, VO_INDEX_DEC_AMRWB, pUserData->memflag|1, g_hAMRWBDecInst, NULL);
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

	gData = (WB_dec_if_state_fx *)voAMRWBDec_mem_malloc(pMemOP, sizeof(WB_dec_if_state_fx), 32);
	if(gData == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->stream = (FrameStream *)voAMRWBDec_mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(gData->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

    gData->stream->frame_ptr = (unsigned char *)voAMRWBDec_mem_malloc(pMemOP, Frame_Maxsize, 32);
    if(gData->stream->frame_ptr == NULL)
	    return VO_ERR_OUTOF_MEMORY;

    stream = gData->stream;
	stream->set_ptr = NULL;
    stream->frame_ptr_bk = stream->frame_ptr;
    stream->set_len = 0;
	stream->framebuffer_len = 0;
    stream->frame_storelen = 0;	

	Init_decoder(&(gData->decoder_state), pMemOP);
	decode_state = gData->decoder_state;
    if (gData->decoder_state == NULL)
	{ 
        voAMRWBDec_mem_free(pMemOP, decode_state);
        return VO_ERR_OUTOF_MEMORY;
     }

     voAMRWBDec_D_IF_reset_fx(gData);

 	if(interMem)
	{
		gData->voMemoprator.Alloc = cmnMemAlloc;
		gData->voMemoprator.Copy = cmnMemCopy;
		gData->voMemoprator.Free = cmnMemFree;
		gData->voMemoprator.Set = cmnMemSet;
		gData->voMemoprator.Check = cmnMemCheck;
		pMemOP = &gData->voMemoprator;
	}
	gData->pvoMemop = pMemOP;

#ifdef LCHECK
    gData->hCheck = pTemp;
#endif

	*phCodec = gData;

#ifdef DUMP
	dump_dat = fopen("/data/local/lhp/amrwb11.dat", "wb+");
#endif

    //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);

	return VO_ERR_NONE;
}


VO_U32 VO_API voAMRWB_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
    WB_dec_if_state_fx *gData;
	FrameStream     *stream;
	VO_MEM_OPERATOR *pMemOP;
	int  len;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (WB_dec_if_state_fx *)hCodec;
	stream = gData->stream;
    pMemOP = (VO_MEM_OPERATOR *)gData->pvoMemop;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

    stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
#ifdef DUMP
	fwrite(pInput->Buffer, 1, pInput->Length, dump_dat);
	fflush(dump_dat);    
#endif

	stream->frame_ptr  = stream->frame_ptr_bk;
    stream->used_len   = 0;

	if(stream->framebuffer_len)
	{
    len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
	pMemOP->Copy(VO_INDEX_DEC_AMRWB, stream->frame_ptr + stream->framebuffer_len, stream->set_ptr, len);
	stream->set_len -= len;
	stream->set_ptr += len;
    stream->framebuffer_len = stream->framebuffer_len + len;
	stream->frame_ptr = stream->frame_ptr_bk;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWB_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
    WB_dec_if_state_fx *gData;
	FrameStream     *stream;
	VO_MEM_OPERATOR *pMemOP;
	int    mode, frame_len;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (WB_dec_if_state_fx *)hCodec;
	stream = gData->stream;
	pMemOP = gData->pvoMemop;

	if(stream->framebuffer_len  < Frame_MaxByte)
	{
	   stream->frame_storelen = stream->framebuffer_len;

	   if(stream->frame_storelen)
	   pMemOP->Copy(VO_INDEX_DEC_AMRWB, stream->frame_ptr_bk, stream->frame_ptr, stream->frame_storelen);

	   if(stream->set_len > 0)
	   {
	      voAMRWBDecUpdateFrameBuffer(stream, pMemOP);
	   }

	   if(stream->framebuffer_len < Frame_MaxByte)
	   {
	      if(pAudioFormat)
			  pAudioFormat->InputUsed = stream->used_len;
          return VO_ERR_INPUT_BUFFER_SMALL;
	   }
	}

	mode = ((*stream->frame_ptr) >> 3) & 0x0f;
	if ((0 > mode) || (10 < mode))
	{
		voAMRWBDecFlushFrameBuffer(stream);
		return VO_ERR_WRONG_STATUS;
	}
	else
	{
		frame_len = block_size[mode];
	}

	if (mode != ((*(stream->frame_ptr + frame_len) >> 3) & 0x0f))
	{
		voAMRWBDecFlushFrameBuffer(stream);
		return VO_ERR_WRONG_STATUS;
	}
	voAMRWBDec_D_IF_decode_fx(gData,stream->frame_ptr, (Word16 *)pOutput->Buffer, 0); 

	pOutput->Length = FRAME_SIZE;
	stream->frame_ptr += frame_len;
	stream->framebuffer_len  -= frame_len;

#ifdef DUMP
	//fwrite(pOutput->Buffer, 1, pOutput->Length, dump_dat);
	//fflush(dump_dat);
#endif


	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels 	= 1;
		pAudioFormat->Format.SampleRate = 16000;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->InputUsed  = stream->used_len;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWB_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
    WB_dec_if_state_fx *gData;
	FrameStream     *stream;  
	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;
	gData = (WB_dec_if_state_fx *)hCodec;
	stream = gData->stream;
	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		voAMRWBDecFlushFrameBuffer(stream);
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWB_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;

	if (hCodec==NULL) 
		return VO_ERR_INVALID_ARG;
	switch(uParamID)
	{
		case VO_PID_AMRWB_FORMAT:
			{
				VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
				fmt->Channels   = 1;
				fmt->SampleRate = 16000;
				fmt->SampleBits = 16;
				break;
			}
		case VO_PID_AMRWB_CHANNELS:
			temp = 1;
			pData = (void *)(&temp);
			break;
		case VO_PID_AMRWB_SAMPLERATE:
			temp = 16000;
			pData = (void *)(&temp);
			break;
		default:
			return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWB_Uninit(VO_HANDLE hCodec)
{
	void *s = NULL;
    WB_dec_if_state_fx  *gData;
	VO_MEM_OPERATOR     *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

    gData = (WB_dec_if_state_fx *)hCodec;
	pMemOP = gData->pvoMemop;
    if(gData->stream)
	{
		if(gData->stream->frame_ptr_bk)
		{
	     voAMRWBDec_mem_free(pMemOP, gData->stream->frame_ptr_bk);
         gData->stream->frame_ptr_bk = NULL;
		}
	    voAMRWBDec_mem_free(pMemOP, gData->stream);
		gData->stream = NULL;
	}

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(gData->hCheck);
#endif

    s = gData->decoder_state;
	Close_decoder(s, pMemOP);
	voAMRWBDec_mem_free(pMemOP, hCodec);
	hCodec = NULL;

#ifdef DUMP
	fclose(dump_dat);
#endif

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAMRWBDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voAMRWB_Init;
	pDecHandle->SetInputData = voAMRWB_SetInputData;
	pDecHandle->GetOutputData = voAMRWB_GetOutputData;
	pDecHandle->SetParam = voAMRWB_SetParam;
	pDecHandle->GetParam = voAMRWB_GetParam;
	pDecHandle->Uninit = voAMRWB_Uninit;

	return VO_ERR_NONE;
}

