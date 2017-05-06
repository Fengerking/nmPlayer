/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voQCELPEnc.c

Contains:	QCELP Enc API Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-06		LHP			Create file

*******************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include    <stdlib.h>
#include	"celp.h"
#include	"pack.h"
#include	"mem_align.h"
#include	"cmnMemory.h"
#include    "voIndex.h"
#include    "voQCELP.h"
#include    "voCheck.h"

#ifdef G1_LOG
#define LOG_TAG "voQCELPEnc"
#include <utils/Log.h>
#define __D(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#endif

//#define DUMP

#ifdef DUMP
FILE *dumpin = NULL;
FILE *dumpout = NULL;
#endif

static void UpdataInspeech(short *inspeech, unsigned char *input)
{
	int    index;
	short  *indata;
	short  *outdata;

	indata	 = (short *)(input);
	outdata = inspeech + LPCOFFSET;
	for(index = FSIZE; index; index--)
	{
		*outdata++ = ((*indata) + 2) >> 2;     //divs 4 oprate
		indata++;
	}
}

void *g_hQCELPEncInst = NULL;

VO_U32 VO_API voQCELP_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	QCP13_ENCOBJ* qcpenc;
	FrameStream    *stream;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem = 0;
	int ret = 0;

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

	qcpenc = (QCP13_ENCOBJ*)mem_malloc(pMemOP, sizeof(QCP13_ENCOBJ), 32);
	if(NULL == qcpenc)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	qcpenc->stream= (FrameStream *)mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(qcpenc->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	qcpenc->stream->frame_ptr = (unsigned char *)mem_malloc(pMemOP, Frame_Maxsize, 32);
	if(qcpenc->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;	

	qcpenc->encoder_memory = (struct ENCODER_MEM *)mem_malloc(pMemOP, sizeof(struct ENCODER_MEM), 32);
	if(NULL == qcpenc->encoder_memory)
	{
		ret = VO_ERR_OUTOF_MEMORY;
	}

	qcpenc->packet = (struct PACKET*)mem_malloc(pMemOP, sizeof(struct PACKET), 32);
	if(NULL == qcpenc->packet)
	{
		ret = VO_ERR_OUTOF_MEMORY;
	}

	qcpenc->in_speech = (short *)mem_malloc(pMemOP, sizeof(short)*(FSIZE+LPCOFFSET), 32);
	if(NULL == qcpenc->in_speech)
	{
		ret = VO_ERR_OUTOF_MEMORY; 
	}

	qcpenc->out_speech = (short *)mem_malloc(pMemOP, sizeof(short)*FSIZE, 32);
	if(NULL == qcpenc->out_speech)
	{
		ret = VO_ERR_OUTOF_MEMORY; 
	}

	qcpenc->lpc_ir = (short *)mem_malloc(pMemOP, sizeof(short)*LENGTH_OF_IMPULSE_RESPONSE, 32);
	if(NULL == qcpenc->lpc_ir)
	{
		ret = VO_ERR_OUTOF_MEMORY;
	}

	if(ret == VO_ERR_OUTOF_MEMORY)
	{
		if(NULL != qcpenc->encoder_memory)
		{
			mem_free(pMemOP, qcpenc->encoder_memory);
			qcpenc->encoder_memory = NULL;
		}
		if(NULL != qcpenc->packet)
		{
			mem_free(pMemOP, qcpenc->packet);
			qcpenc->packet = NULL;
		}
		if(NULL != qcpenc->in_speech)
		{
			mem_free(pMemOP, qcpenc->in_speech);
			qcpenc->in_speech = NULL;
		}
		if(NULL != qcpenc->out_speech)
		{
			mem_free(pMemOP, qcpenc->out_speech);
			qcpenc->out_speech = NULL;
		}
		if(NULL != qcpenc)
		{
			mem_free(pMemOP, qcpenc);
			qcpenc = NULL;
		}
		return ret;
	}
	/* initialise values */
	stream  = qcpenc->stream;
	InitFrameBuffer(stream);

	qcpenc->hysteresis = 0;
	qcpenc->reduced_rate_flag = 0;
	qcpenc->rda_flag = 0;
	pMemOP->Set(VO_INDEX_ENC_QCELP, qcpenc->encoder_memory, 0, sizeof(struct ENCODER_MEM));
	pMemOP->Set(VO_INDEX_ENC_QCELP, qcpenc->packet, 0, sizeof(struct PACKET));
	pMemOP->Set(VO_INDEX_ENC_QCELP, qcpenc->in_speech, 0, sizeof(short)*(FSIZE+LPCOFFSET));
	pMemOP->Set(VO_INDEX_ENC_QCELP, qcpenc->out_speech, 0, sizeof(short)*FSIZE);
	pMemOP->Set(VO_INDEX_ENC_QCELP, qcpenc->lpc_ir, 0, sizeof(short)*LENGTH_OF_IMPULSE_RESPONSE);

	initialize_encoder(qcpenc->encoder_memory, pMemOP);

	if(interMem)
	{
		qcpenc->voMemoprator.Alloc = cmnMemAlloc;
		qcpenc->voMemoprator.Copy = cmnMemCopy;
		qcpenc->voMemoprator.Free = cmnMemFree;
		qcpenc->voMemoprator.Set = cmnMemSet;
		qcpenc->voMemoprator.Check = cmnMemCheck;
		pMemOP = &qcpenc->voMemoprator;
	}
	qcpenc->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)qcpenc;

    //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	    voCheckLibInit(&(qcpenc->hCheck), VO_INDEX_ENC_QCELP, 1, g_hQCELPEncInst, pUserData->libOperator);
	else
        voCheckLibInit(&(qcpenc->hCheck), VO_INDEX_ENC_QCELP, 1, g_hQCELPEncInst, NULL);
#endif
#ifdef DUMP
	dumpin = fopen("/data/local/inlog.dat", "wb+");
	dumpout = fopen("/data/local/outlog.dat", "wb+");
#endif

#ifdef G1_LOG
	__D("Succefully Init \n");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELP_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	QCP13_ENCOBJ   *qcpenc;
	FrameStream    *stream;
	VO_MEM_OPERATOR *pMemOP;
	//int  len;
	if(NULL == hCodec)
		return VO_ERR_INVALID_ARG;

	qcpenc = (QCP13_ENCOBJ *)hCodec;
	stream  = qcpenc->stream;
	pMemOP  = qcpenc->pvoMemop;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
		return VO_ERR_INVALID_ARG;

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->frame_ptr  = stream->frame_ptr_bk;
	stream->used_len   = 0;

#ifdef DUMP
	fwrite(stream->set_ptr, 1, stream->set_len,dumpin);
	fflush(dumpin);
#endif

#ifdef G1_LOG
	__D("SetInputData stream->set_ptr = 0x%p,stream->set_len = %d \n", stream->set_ptr, stream->set_len);
#endif

	//if(stream->framebuffer_len)
	//{
	//	len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
	//	pMemOP->Copy(VO_INDEX_ENC_QCELP, stream->frame_ptr + stream->framebuffer_len, stream->set_ptr, len);
	//	stream->set_len -= len;
	//	stream->set_ptr += len;
	//	stream->framebuffer_len = stream->framebuffer_len + len;
	//	stream->frame_ptr = stream->frame_ptr_bk;
	//}
	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELP_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	int i;
	short *indata, *outdata;
	QCP13_ENCOBJ   *qcpenc;
	struct PACKET  *packet;
    FrameStream    *stream;
    VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	qcpenc = (QCP13_ENCOBJ*)hCodec;
	packet = qcpenc->packet;
	stream = qcpenc->stream;
    pMemOP = qcpenc->pvoMemop;

	if ((NULL == stream))
	{
		return VO_ERR_INVALID_ARG;
	}

	if(stream->framebuffer_len  < Frame_MaxByte)
	{
		stream->frame_storelen = stream->framebuffer_len;
		if(stream->frame_storelen)
		pMemOP->Copy(VO_INDEX_ENC_QCELP, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen );
		if(stream->set_len > 0)
		{
			UpdateFrameBuffer(stream, pMemOP);
		}
		if(stream->framebuffer_len < Frame_MaxByte)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}
 
	UpdataInspeech(qcpenc->in_speech, stream->frame_ptr);

	if(NULL != pOutput->Buffer)
	{
		packet->data = (char *)pOutput->Buffer;
	}
	else
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	encoder(qcpenc);

	/***** Update in_speech buffer ***************/	
	indata = qcpenc->in_speech + FSIZE;
	outdata = qcpenc->in_speech;
	for (i=LPCSIZE-FSIZE+LPCOFFSET; i; i--)
	{
		*outdata++ = *indata++;
	}

	if(packet->mode >= NUMMODES || packet->mode < 0)
		return VO_ERR_INVALID_ARG;

	stream->used_len += 320;
	stream->frame_ptr += 320; 
	stream->framebuffer_len  -= 320;
	pOutput->Length  = qcp_byte_num_per_frame[packet->mode];

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(qcpenc->hCheck, pOutput, &(pAudioFormat->Format));
#endif
	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->InputUsed = stream->used_len;
	}

#ifdef G1_LOG
	__D("GetOutputData pOutput->Buffer = 0x%p,pOutput->Length = %d \n", pOutput->Buffer, pOutput->Length);
#endif

#ifdef DUMP
	fwrite(pOutput->Buffer, 1, pOutput->Length, dumpout);
	fflush(dumpout);
#endif

	return VO_ERR_NONE;
}


VO_U32 VO_API voQCELP_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	QCP13_ENCOBJ *qcpenc;
    FrameStream  *stream;
	int  *lValue = (int *)pData;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	qcpenc = (QCP13_ENCOBJ*)hCodec;
	stream = qcpenc->stream;
	switch(uParamID)
	{
	//case VO_PID_COMMON_INPUTTYPE:
	//	if(*pData >= 0 && *pData <= 4)
	//	{
	//		qcpenc->reduced_rate_flag = (short)(*pData);
	//		if(qcpenc->reduced_rate_flag == 3){
	//			qcpenc->encoder_memory->avg_rate = 9; 
	//			qcpenc->encoder_memory->avg_rate_thr = 9;
	//			qcpenc->encoder_memory->target_snr_thr  = 10240;
	//		}
	//		else if(qcpenc->reduced_rate_flag == 4){
	//			qcpenc->encoder_memory->avg_rate = 7; 
	//			qcpenc->encoder_memory->avg_rate_thr = 7;
	//			qcpenc->encoder_memory->target_snr_thr  = 1024*7;
	//		}
	//	}
	//	break;
	case VO_PID_COMMON_FLUSH:
		reset_encoder(qcpenc->encoder_memory);
		FlushFrameBuffer(stream);
		break;
	case VO_PID_QCELP_RDA:
		qcpenc->rda_flag = *lValue;

#ifdef G1_LOG
		__D("Bit rate module = %d\n", qcpenc->rda_flag);
#endif
        break;
	case VO_PID_COMMON_HEADDATA:
		break;
	default: 
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELP_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	switch(uParamID)
	{
	case VO_PID_QCELP_CHANNELS:
		temp = 1;
		pData = (void *)(&temp);
		break;
	case VO_PID_QCELP_SAMPLERATE: 
		temp = 8000;
		pData = (void *)(&temp);
		break;
	default: 
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELP_Uninit(VO_HANDLE hCodec)
{
	QCP13_ENCOBJ    *qcpenc;
    FrameStream     *stream;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	qcpenc = (QCP13_ENCOBJ*)hCodec;
	stream = qcpenc->stream;
	pMemOP = qcpenc->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(qcpenc->hCheck);
#endif
	if(stream)
	{
		if(stream->frame_ptr_bk)
		{
			mem_free(pMemOP, stream->frame_ptr_bk);
			stream->frame_ptr_bk = NULL;
		}
		mem_free(pMemOP, stream);
		stream = NULL;
	}

	free_encoder(qcpenc->encoder_memory, pMemOP);
	if(NULL != qcpenc->encoder_memory)
	{
		mem_free(pMemOP, qcpenc->encoder_memory);
		qcpenc->encoder_memory = NULL;
	}

	if(NULL != qcpenc->packet)
	{
		mem_free(pMemOP, qcpenc->packet);
		qcpenc->packet = NULL;
	}

	if(NULL != qcpenc->in_speech)
	{
		mem_free(pMemOP, qcpenc->in_speech);
		qcpenc->in_speech = NULL;
	}

	if(NULL != qcpenc->out_speech)
	{
		mem_free(pMemOP, qcpenc->out_speech);
		qcpenc->out_speech = NULL;
	}

	if(NULL != qcpenc->lpc_ir)
	{
		mem_free(pMemOP, qcpenc->lpc_ir);
		qcpenc->lpc_ir = NULL;
	}

	if(NULL != qcpenc)
	{
		mem_free(pMemOP, qcpenc);
		qcpenc = NULL;
	}

#ifdef DUMP
	fclose(dumpin);
	fclose(dumpout);
#endif

	return VO_ERR_NONE;
}


VO_S32 VO_API voGetQCELPEncAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voQCELP_Init;
	pDecHandle->SetInputData = voQCELP_SetInputData;
	pDecHandle->GetOutputData = voQCELP_GetOutputData;
	pDecHandle->SetParam = voQCELP_SetParam;
	pDecHandle->GetParam = voQCELP_GetParam;
	pDecHandle->Uninit = voQCELP_Uninit;

	return VO_ERR_NONE;
}
