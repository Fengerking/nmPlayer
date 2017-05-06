#include <string.h>
#include <stdlib.h>
#include "d_globs.h"
#include "rom.h"
#include "decode.h"
#include "mem_align.h"
//#include "cmnMemory.h"
#include "voIndex.h"
#include "voEVRC.h"
#include "voCheck.h"

#define  DecFrame_MaxByte  23

void  *g_hEVRCDecInst = NULL;

//#define DUMP
#ifdef DUMP
FILE* dump = NULL;
#endif


VO_U32 VO_API voEVRCDec_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
#ifdef LCHECK
	VO_PTR pTemp;
#endif
	EVRC_DEC_COMPONENT  *evrc_dcom;
    FrameStream *stream;
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
		//interMem = 1;
		//pMemOP = &voMemoprator;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

#ifdef LCHECK	
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_EVRC, pUserData->memflag|1, g_hEVRCDecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_EVRC, pUserData->memflag|1, g_hEVRCDecInst, NULL);
	if (nRet != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}
#endif

	evrc_dcom = (EVRC_DEC_COMPONENT *)voEVRCCodec_mem_malloc(pMemOP, sizeof(EVRC_DEC_COMPONENT), 32);
	if(evrc_dcom == NULL)
		return VO_ERR_OUTOF_MEMORY;
	evrc_dcom->VoKey = 0;            /* Enable time limitation */
	pMemOP->Set(VO_INDEX_DEC_EVRC, evrc_dcom, 0, sizeof(EVRC_DEC_COMPONENT));

	evrc_dcom->evrc_com = (ERVC_COMPONENT *)voEVRCCodec_mem_malloc(pMemOP, sizeof(ERVC_COMPONENT), 32);
	if(evrc_dcom->evrc_com == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_DEC_EVRC, evrc_dcom->evrc_com, 0, sizeof(ERVC_COMPONENT));

	evrc_dcom->evrc_decobj = (ERVC_DEC_OBJ *)voEVRCCodec_mem_malloc(pMemOP, sizeof(ERVC_DEC_OBJ), 32);
	if(evrc_dcom->evrc_decobj == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_DEC_EVRC, evrc_dcom->evrc_decobj, 0, sizeof(ERVC_DEC_OBJ));

	evrc_dcom->stream= (FrameStream *)voEVRCCodec_mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(evrc_dcom->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	evrc_dcom->stream->frame_ptr = (unsigned char *)voEVRCCodec_mem_malloc(pMemOP, Frame_Maxsize, 32);
	if(evrc_dcom->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;

	stream  = evrc_dcom->stream;
	InitFrameBuffer(stream);

	InitDecoder(evrc_dcom);
	evrc_dcom->evrc_decobj->pf_flag = EVRC_POST_FILER_ON;

	//if(interMem)
	//{
	//	evrc_dcom->voMemoprator.Alloc = cmnMemAlloc;
	//	evrc_dcom->voMemoprator.Copy = cmnMemCopy;
	//	evrc_dcom->voMemoprator.Free = cmnMemFree;
	//	evrc_dcom->voMemoprator.Set = cmnMemSet;
	//	evrc_dcom->voMemoprator.Check = cmnMemCheck;
	//	pMemOP = &evrc_dcom->voMemoprator;
	//}
	evrc_dcom->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)evrc_dcom;

#ifdef DUMP
	dump = fopen ("/data/data/dump/vomeAudio.pcm", "wb");
#endif

    //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
	evrc_dcom->hCheck = pTemp;

#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCDec_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	EVRC_DEC_COMPONENT   *evrc_ecom;
	FrameStream    *stream;
	VO_MEM_OPERATOR *pMemOP;
	int  len;
	if(NULL == hCodec)
		return VO_ERR_INVALID_ARG;

	evrc_ecom = (EVRC_DEC_COMPONENT *)hCodec;
	stream  = evrc_ecom->stream;
    pMemOP  = evrc_ecom->pvoMemop;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
		return VO_ERR_INVALID_ARG;

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->frame_ptr  = stream->frame_ptr_bk;
	stream->used_len = 0;
	if(stream->framebuffer_len)
	{
		len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
		pMemOP->Copy(VO_INDEX_DEC_EVRC, stream->frame_ptr + stream->framebuffer_len, stream->set_ptr, len);
		stream->set_len -= len;
		stream->set_ptr += len;
		stream->framebuffer_len = stream->framebuffer_len + len;
		stream->frame_ptr = stream->frame_ptr_bk;
	}
	return VO_ERR_NONE;
}

static int Frame_counter = 0;

VO_U32 VO_API voEVRCDec_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	int   len = 0;
	short rate;
	EVRC_DEC_COMPONENT* evrc_dcom;
    FrameStream  *stream;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_dcom = (EVRC_DEC_COMPONENT *)hCodec;
	stream    = (FrameStream *)evrc_dcom->stream;
    pMemOP    = evrc_dcom->pvoMemop;
	if(NULL == stream)
	{
		return VO_ERR_INVALID_ARG;
	}

#ifdef NXP
	if((Frame_counter > 90) && (evrc_dcom->VoKey !=1))
		exit(0);
#endif

	if(stream->framebuffer_len  < DecFrame_MaxByte)
	{
		stream->frame_storelen = stream->framebuffer_len;
		if(stream->frame_storelen)
			pMemOP->Copy(VO_INDEX_DEC_EVRC, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen );
		if(stream->set_len > 0)
		{
			UpdateFrameBuffer(stream, pMemOP);
		}
		if(stream->framebuffer_len < DecFrame_MaxByte)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	if ((NULL == pOutput->Buffer) || (pOutput->Length < 160*sizeof(short)))
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	rate = (short)stream->frame_ptr[0];

	if(rate > 4 || rate < 0 || rate==2)
	{
		return VO_ERR_WRONG_STATUS;
	}

	len = evrc_framelen[rate];
	decode(evrc_dcom, stream->frame_ptr, (short *)pOutput->Buffer);

	stream->frame_ptr += len;
	stream->framebuffer_len  -= len;
	pOutput->Length  = 160*sizeof(short);
	stream->used_len += len;

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(evrc_dcom->hCheck, pOutput, &(pAudioFormat->Format));
#endif

#ifdef DUMP
	if(dump) 
		fwrite(pOutput->Buffer, 1, pOutput->Length, dump);
#endif	

	Frame_counter++;

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->InputUsed = stream->used_len;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCDec_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	EVRC_DEC_COMPONENT* evrc_dcom;
	FrameStream  *stream;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_dcom = (EVRC_DEC_COMPONENT *)hCodec;
	stream = evrc_dcom->stream;
	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
	     FlushFrameBuffer(stream);
	     InitDecoder(evrc_dcom);
		 break;
	case VO_PID_EVRC_POSTFILTER:
		evrc_dcom->evrc_decobj->pf_flag = (*(int *)pData);
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	case 0x4000FFFF:
		evrc_dcom->VoKey = (*(int *)pData);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;

}

VO_U32 VO_API voEVRCDec_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	EVRC_DEC_COMPONENT* evrc_dcom;
	int   tmp;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_dcom = (EVRC_DEC_COMPONENT *)hCodec;

	switch(uParamID)
	{
		case VO_PID_EVRC_CHANNELS:
			tmp = 1;
            pData = (void *)(&tmp);
			break;
		case VO_PID_EVRC_SAMPLERATE: 
			tmp = 8000;
			pData = (void *)(&tmp);
            break;
		case VO_PID_EVRC_POSTFILTER:
			tmp = evrc_dcom->evrc_decobj->pf_flag;
			pData = (void *)(&tmp); 
			break;
		default: 
			return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCDec_Uninit(VO_HANDLE hCodec)
{
	EVRC_DEC_COMPONENT* evrc_dcom;
	FrameStream *stream;
	VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_dcom = (EVRC_DEC_COMPONENT *)hCodec;
	stream = evrc_dcom->stream;
	pMemOP = evrc_dcom->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(evrc_dcom->hCheck);
#endif

	if(evrc_dcom->evrc_com)
	{
		voEVRCCodec_mem_free(pMemOP, evrc_dcom->evrc_com);
		evrc_dcom->evrc_com = NULL;
	}

	if(evrc_dcom->evrc_decobj)
	{
		voEVRCCodec_mem_free(pMemOP, evrc_dcom->evrc_decobj);
		evrc_dcom->evrc_decobj = NULL;
	}

	if(stream)
	{
		if(stream->frame_ptr_bk)
		{
			voEVRCCodec_mem_free(pMemOP, stream->frame_ptr_bk);
			stream->frame_ptr_bk = NULL;
		}
		voEVRCCodec_mem_free(pMemOP, stream);
		stream = NULL;
	}

	voEVRCCodec_mem_free(pMemOP, evrc_dcom);

#ifdef DUMP
	if(dump) 
		fclose(dump);
#endif

	return VO_ERR_NONE;

}


VO_S32 VO_API voGetEVRCDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voEVRCDec_Init;
	pDecHandle->SetInputData = voEVRCDec_SetInputData;
	pDecHandle->GetOutputData = voEVRCDec_GetOutputData;
	pDecHandle->SetParam = voEVRCDec_SetParam;
	pDecHandle->GetParam = voEVRCDec_GetParam;
	pDecHandle->Uninit = voEVRCDec_Uninit;

	return VO_ERR_NONE;
}

