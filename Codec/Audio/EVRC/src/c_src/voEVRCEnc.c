#include <string.h>
#include <stdlib.h>
#include "e_globs.h"
#include "encode.h"
#include "basic_op.h"
#include "bqiir.h"
#include "ns127.h"
#include "rda.h"
#include "rom.h"
#include "mem_align.h"
#include "cmnMemory.h"
#include "voIndex.h"
#include "voEVRC.h"
#include "voCheck.h"

#define  EncFrame_MaxByte  320
void  *g_hEVRCEncInst = NULL;

VO_U32 VO_API voEVRCEnc_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	EVRC_ENC_COMPONENT* evrc_ecom;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	FrameStream *stream;
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

	evrc_ecom = (EVRC_ENC_COMPONENT *)voEVRCCodec_mem_malloc(pMemOP, sizeof(EVRC_ENC_COMPONENT), 32);
	if(evrc_ecom == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_ENC_EVRC, evrc_ecom, 0, sizeof(EVRC_ENC_COMPONENT));

	evrc_ecom->stream= (FrameStream *)voEVRCCodec_mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(evrc_ecom->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	evrc_ecom->stream->frame_ptr = (unsigned char *)voEVRCCodec_mem_malloc(pMemOP, Frame_Maxsize, 32);
	if(evrc_ecom->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;

	evrc_ecom->evrc_com = (ERVC_COMPONENT *)voEVRCCodec_mem_malloc(pMemOP, sizeof(ERVC_COMPONENT), 32);
	if(evrc_ecom->evrc_com == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_ENC_EVRC, evrc_ecom->evrc_com, 0, sizeof(ERVC_COMPONENT));

	evrc_ecom->evrc_encobj = (ERVC_ENC_OBJ *)voEVRCCodec_mem_malloc(pMemOP, sizeof(ERVC_ENC_OBJ), 32);
	if(evrc_ecom->evrc_encobj == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_ENC_EVRC, evrc_ecom->evrc_encobj, 0, sizeof(ERVC_ENC_OBJ));

	evrc_ecom->evrc_emem = (ENCODER_MEM *)voEVRCCodec_mem_malloc(pMemOP, sizeof(ENCODER_MEM), 32);
	if(evrc_ecom->evrc_emem == NULL)
		return VO_ERR_OUTOF_MEMORY;
	pMemOP->Set(VO_INDEX_ENC_EVRC, evrc_ecom->evrc_emem, 0, sizeof(ENCODER_MEM));

	stream  = evrc_ecom->stream;
	InitFrameBuffer(stream);	
    InitEncoder(evrc_ecom);
	initialize_rate(evrc_ecom->evrc_emem);

	evrc_ecom->evrc_encobj->min_rate = EVRC_RATE_EIGHT;
	evrc_ecom->evrc_encobj->max_rate = EVRC_RATE_FULLRATE;
	evrc_ecom->evrc_encobj->ns_flag  = EVRC_NOISE_SUPP_ON;
	evrc_ecom->evrc_encobj->set_rate = 0;

	if(interMem)
	{
		evrc_ecom->voMemoprator.Alloc = cmnMemAlloc;
		evrc_ecom->voMemoprator.Copy = cmnMemCopy;
		evrc_ecom->voMemoprator.Free = cmnMemFree;
		evrc_ecom->voMemoprator.Set = cmnMemSet;
		evrc_ecom->voMemoprator.Check = cmnMemCheck;
		pMemOP = &evrc_ecom->voMemoprator;
	}
	evrc_ecom->pvoMemop = pMemOP;
	*phCodec = (VO_HANDLE)evrc_ecom;
      //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	    voCheckLibInit(&(evrc_ecom->hCheck), VO_INDEX_ENC_EVRC, 1, g_hEVRCEncInst, pUserData->libOperator);
	else
        voCheckLibInit(&(evrc_ecom->hCheck), VO_INDEX_ENC_EVRC, 1, g_hEVRCEncInst, NULL);
#endif

    return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCEnc_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	EVRC_ENC_COMPONENT   *evrc_ecom;
	FrameStream    *stream;
	VO_MEM_OPERATOR *pMemOP;

	int  len;
	if(NULL == hCodec)
		return VO_ERR_INVALID_ARG;

	evrc_ecom = (EVRC_ENC_COMPONENT *)hCodec;
	stream  = evrc_ecom->stream;
    pMemOP  = evrc_ecom->pvoMemop;
	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
		return VO_ERR_INVALID_ARG;

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->frame_ptr  = stream->frame_ptr_bk;
	stream->used_len   = 0;

	if(stream->framebuffer_len)
	{
		len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
		pMemOP->Copy(VO_INDEX_ENC_EVRC, stream->frame_ptr + stream->framebuffer_len, stream->set_ptr, len);
		stream->set_len -= len;
		stream->set_ptr += len;
		stream->framebuffer_len = stream->framebuffer_len + len;
		stream->frame_ptr = stream->frame_ptr_bk;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCEnc_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	EVRC_ENC_COMPONENT* evrc_ecom;
	ERVC_COMPONENT	*pcom;
	ERVC_ENC_OBJ	*pencobj;
	ENCODER_MEM		*pemem;
	FrameStream     *stream; 
	VO_MEM_OPERATOR *pMemOP;
	short           *buf_in = NULL;
	char            *buf_out = NULL;
	unsigned long i;
	short beta;
	long R[17];
	short rate;

	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_ecom = (EVRC_ENC_COMPONENT *)hCodec;
	pcom = evrc_ecom->evrc_com;
	pencobj = evrc_ecom->evrc_encobj;
	pemem = evrc_ecom->evrc_emem;
	stream = evrc_ecom->stream;
	pMemOP = evrc_ecom->pvoMemop;

	if(NULL == stream || NULL ==pOutput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}

	if(stream->framebuffer_len  < EncFrame_MaxByte)
	{
		stream->frame_storelen = stream->framebuffer_len;
		if(stream->frame_storelen)
			pMemOP->Copy(VO_INDEX_DEC_EVRC, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);
		if(stream->set_len > 0)
		{
			UpdateFrameBuffer(stream,  pMemOP);
		}
		if(stream->framebuffer_len < EncFrame_MaxByte)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	buf_in = (short *)stream->frame_ptr;
	bqiir(pencobj, buf_in);

	if(pencobj->ns_flag)
	{
		noise_suprs(pencobj, buf_in);
		noise_suprs(pencobj, buf_in + (320 >> 2));
	}
	else
	{
		i = 320 >> 1;
		while (i-- > 0)
		{
			(*buf_in) = shift_r((*buf_in), -1);
			buf_in++;
		}
	}

	buf_in = (short *)stream->frame_ptr;
	beta = pre_encode(evrc_ecom, buf_in, R);

	if (evrc_ecom->evrc_encobj->set_rate == 0)
	{
		rate = select_rate(pemem, R, pencobj->max_rate, pencobj->min_rate, beta);
	}
	else
	{
		rate = evrc_ecom->evrc_encobj->set_rate;
	}

	buf_out = pOutput->Buffer;
	*buf_out = (char)rate;
	encode(evrc_ecom, rate, (buf_out + 1));
	post_encode(evrc_ecom);

	pOutput->Length = evrc_framelen[rate];
	stream->frame_ptr += 160 * sizeof(short);
	stream->framebuffer_len  -= 160 * sizeof(short);
	stream->used_len += 160 * sizeof(short);

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(evrc_ecom->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->InputUsed = stream->used_len;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCEnc_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	EVRC_ENC_COMPONENT* evrc_ecom;
	FrameStream *stream;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	evrc_ecom = (EVRC_ENC_COMPONENT *)hCodec;
	stream = (FrameStream *)evrc_ecom->stream;
	switch(uParamID)
	{
	case VO_PID_EVRC_NOISESUPPT:
		evrc_ecom->evrc_encobj->ns_flag = (*(int *)pData);
		break;
	case VO_PID_EVRC_SET_RATE:
		evrc_ecom->evrc_encobj->set_rate = (short)(*(int *)pData);
		break;
	case VO_PID_EVRC_MAX_RATE:
		if((*(int *)pData)> evrc_ecom->evrc_encobj->min_rate && (*(int *)pData)<= EVRC_RATE_FULLRATE)
			evrc_ecom->evrc_encobj->max_rate = (short)(*(int *)pData);
		break;
	case VO_PID_EVRC_MIN_RATE:
		if((*(int *)pData) < evrc_ecom->evrc_encobj->max_rate && (*(int *)pData) >= EVRC_RATE_EIGHT)
			evrc_ecom->evrc_encobj->min_rate = (short)(*(int *)pData);
		break;
	case VO_PID_COMMON_FLUSH:
		FlushFrameBuffer(stream);
        InitEncoder(evrc_ecom);
	    initialize_rate(evrc_ecom->evrc_emem);
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCEnc_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	EVRC_ENC_COMPONENT* evrc_ecom;
	int  tmp;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_ecom = (EVRC_ENC_COMPONENT *)hCodec;
	
	switch(uParamID)
	{
	case VO_PID_EVRC_NOISESUPPT:
		tmp = evrc_ecom->evrc_encobj->ns_flag;
		pData = (void *)(&tmp);
		break;
	case VO_PID_EVRC_MAX_RATE:	
		tmp = evrc_ecom->evrc_encobj->max_rate;
		pData = (void *)(&tmp);
		break;
	case VO_PID_EVRC_MIN_RATE:
		tmp = evrc_ecom->evrc_encobj->min_rate;
		pData = (void *)(&tmp);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voEVRCEnc_Uninit(VO_HANDLE hCodec)
{
	EVRC_ENC_COMPONENT* evrc_ecom;
	FrameStream *stream;
	VO_MEM_OPERATOR *pMemOP;

	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}

	evrc_ecom = (EVRC_ENC_COMPONENT *)hCodec;
	stream = evrc_ecom->stream;
	pMemOP = evrc_ecom->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(evrc_ecom->hCheck);
#endif
	if(evrc_ecom->evrc_com)
	{
		voEVRCCodec_mem_free(pMemOP, evrc_ecom->evrc_com);
		evrc_ecom->evrc_com = NULL;
	}
	if(evrc_ecom->evrc_encobj)
	{
		voEVRCCodec_mem_free(pMemOP, evrc_ecom->evrc_encobj);
		evrc_ecom->evrc_encobj = NULL; 
	}
	if(evrc_ecom->evrc_emem)
	{
		voEVRCCodec_mem_free(pMemOP, evrc_ecom->evrc_encobj);
		evrc_ecom->evrc_encobj = NULL;
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

	voEVRCCodec_mem_free(pMemOP, evrc_ecom);  
    return VO_ERR_NONE;
}


VO_S32 VO_API voGetEVRCEncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(NULL == pEncHandle)
		return VO_ERR_INVALID_ARG;
	pEncHandle->Init = voEVRCEnc_Init;
	pEncHandle->SetInputData = voEVRCEnc_SetInputData;
	pEncHandle->GetOutputData = voEVRCEnc_GetOutputData;
	pEncHandle->SetParam = voEVRCEnc_SetParam;
	pEncHandle->GetParam = voEVRCEnc_GetParam;
	pEncHandle->Uninit = voEVRCEnc_Uninit;

	return VO_ERR_NONE;
}

