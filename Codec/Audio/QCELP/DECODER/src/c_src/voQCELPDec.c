/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voQCELPDec.c

Contains:	QCELP Dec API Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-06		LHP			Create file

*******************************************************************************/
#include     <stdlib.h>
#include     "voCheck.h"
#include     "qcelp.h"
#include     "qcelp13.h"
#include	 "mem_align.h"
//#include	 "cmnMemory.h"
#include     "voIndex.h"
#include     "voQCELP.h"


#define      QCELP_SAMPLE_NUM    160
void         *g_hQCELPDecInst = NULL;

VO_U32 VO_API voQCELPDec_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32 nRet = 0;
#ifdef LCHECK
	VO_PTR pTemp;
#endif

	Int16 i;
	Int32 ar;
	QCP13_DecObj   *pdecobj;
	QC13_DObj      *Decode;
	QC13_PACKET    *qcelp13_packet;	
	FrameStream    *stream;
	VO_U32 result = 0;
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
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_QCELP, pUserData->memflag|1, g_hQCELPDecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_QCELP, pUserData->memflag|1, g_hQCELPDecInst, NULL);
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

	if(NULL == (pdecobj = (QCP13_DecObj*)voQCELPDec_mem_malloc(pMemOP, sizeof(QCP13_DecObj), 32)))
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	pdecobj->VoKey = 0;                     /* Enable Key protection */
	Decode = &pdecobj->Decoder;
	qcelp13_packet = &pdecobj->q13_packet;

    pMemOP->Set(VO_INDEX_DEC_QCELP, qcelp13_packet, 0, sizeof(QC13_PACKET));

	pdecobj->stream= (FrameStream *)voQCELPDec_mem_malloc(pMemOP, sizeof(FrameStream), 32);
	if(pdecobj->stream == NULL)
		return VO_ERR_OUTOF_MEMORY;

	pdecobj->stream->frame_ptr = (unsigned char *)voQCELPDec_mem_malloc(pMemOP, Frame_Maxsize, 32);
	if(pdecobj->stream->frame_ptr == NULL)
		return VO_ERR_OUTOF_MEMORY;

	if (NULL == (Decode->lsp_pred_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->G_pred = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * QC13_GPRED_ORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->qlsp_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->pf_hist_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->bright_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * MAX_PITCH_SF, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->last_qlsp_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->ring_pole_hist_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->ring_pole_hist_32 = (Int32 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int32) * LPCORDER, 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->adptv_cbk_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * (MAXLAG +1), 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->prefilt_acbk_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * (MAXLAG +1), 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->last_G_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * (QC13_GORDER), 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}
	if (NULL == (Decode->bpf_hist_16 = (Int16 *)voQCELPDec_mem_malloc(pMemOP, sizeof(Int16) * (BPFORDER), 32)))
	{
		result = VO_ERR_OUTOF_MEMORY;
	}

	if (VO_ERR_OUTOF_MEMORY == result)
	{
		if (NULL != Decode->lsp_pred_16) voQCELPDec_mem_free(pMemOP, Decode->lsp_pred_16);
		if (NULL != Decode->G_pred) voQCELPDec_mem_free(pMemOP, Decode->G_pred);
		if (NULL != Decode->qlsp_16) voQCELPDec_mem_free(pMemOP, Decode->qlsp_16);
		if (NULL != Decode->pf_hist_16) voQCELPDec_mem_free(pMemOP, Decode->pf_hist_16);
		if (NULL != Decode->bright_16) voQCELPDec_mem_free(pMemOP, Decode->bright_16);
		if (NULL != Decode->last_qlsp_16) voQCELPDec_mem_free(pMemOP, Decode->last_qlsp_16);
		if (NULL != Decode->ring_pole_hist_16) voQCELPDec_mem_free(pMemOP, Decode->ring_pole_hist_16);
		if (NULL != Decode->ring_pole_hist_32)  voQCELPDec_mem_free(pMemOP, Decode->ring_pole_hist_32);
		if (NULL != Decode->adptv_cbk_16) voQCELPDec_mem_free(pMemOP, Decode->adptv_cbk_16);
		if (NULL != Decode->prefilt_acbk_16) voQCELPDec_mem_free(pMemOP, Decode->prefilt_acbk_16);
		if (NULL != Decode->last_G_16) voQCELPDec_mem_free(pMemOP, Decode->last_G_16);
		if (NULL != Decode->bpf_hist_16) voQCELPDec_mem_free(pMemOP, Decode->bpf_hist_16);

		*phCodec = 0;
		return result;
	}
	/* initialise values */
	stream  = pdecobj->stream;
	InitFrameBuffer(stream);
	Decode->seed = 0;
	Decode->err_cnt = (Int16)0;
	Decode->low_rate_cnt = (Int16)0;

	ar = 0l;
	for (i = 0; i < LPCORDER; i++)
	{
		ar +=(Int32)INV_HALF_LPCORDER << 12;
		Decode->last_qlsp_16[i] = (Int16)(ar >> 16);
		Decode->lsp_pred_16[i] = 0;
	}

	Decode->last_b_16 = 0;
	Decode->last_lag = 0;
	for (i=0; i < QC13_GORDER; i++)
	{
		Decode->last_G_16[i] = 0;
	}

	Decode->last_G_8th_rate_16 = 0;
	Decode->agc_factor_16 = 8192; // 1.0 in Q13 format

	for (i = 0; i < MAXLAG+1; i++)
	{
		Decode->prefilt_acbk_16[i]= 0;
		Decode->adptv_cbk_16[i]= 0;
	}

	for (i=0; i < BPFORDER; i++)
	{
		Decode->bpf_hist_16[i] = 0;
	}

	for (i = 0; i < LPCORDER; i++)
	{
		Decode->pf_hist_16[i] = 0;
		Decode->ring_pole_hist_16[i] = 0;
		Decode->ring_pole_hist_32[i] = 0;
		Decode->qlsp_16[i] = 0;
	}
	//    for (i = 0; i < QC13_GORDER; i++)
	for (i = 0; i < QC13_GPRED_ORDER; i++)
	{
		Decode->G_pred[i] = 0;
	}

	for (i = 0; i < MAX_PITCH_SF; i++)
	{
		Decode->bright_16[i] = 0;
	}

	Decode->pf_flag = PF_ON;
	Decode->pitch_post_flag = YES;
	Decode->bright_hist_16 = 0;

	//initialize qcelp13_packet
	//for (i = 0; i < LPCORDER; i++)
	//{
	//	qcelp13_packet->lpc[i] = 0;
	//}
	//qcelp13_packet->min_lag = 0;
	//qcelp13_packet->sd_dec = 0;
	//qcelp13_packet->sd_enc = 0;
	//for (i = 0; i < QC13_MAX_SF; i++)
	//{
	//	qcelp13_packet->b[i] = 0;
	//	qcelp13_packet->lag[i] = 0;
	//	qcelp13_packet->G[i] = 0;
	//	qcelp13_packet->i[i] = 0;
	//	qcelp13_packet->Gsign[i] = 0;
	//	qcelp13_packet->frac[i] = 0;
	//}

	//if(interMem)
	//{
	//	pdecobj->voMemoprator.Alloc = cmnMemAlloc;
	//	pdecobj->voMemoprator.Copy = cmnMemCopy;
	//	pdecobj->voMemoprator.Free = cmnMemFree;
	//	pdecobj->voMemoprator.Set = cmnMemSet;
	//	pdecobj->voMemoprator.Check = cmnMemCheck;
	//	pMemOP = &pdecobj->voMemoprator;
	//}
	pdecobj->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)pdecobj;

    //VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst);
#ifdef  LCHECK
	pdecobj->hCheck = pTemp;	
#endif

	return VO_ERR_NONE;	
}

VO_U32 VO_API voQCELPDec_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	QCP13_DecObj   *pdecobj;
	FrameStream    *stream;
	VO_MEM_OPERATOR *pMemOP;
	int  len;
	if(NULL == hCodec)
		return VO_ERR_INVALID_ARG;

	pdecobj = (QCP13_DecObj *)hCodec;
	stream  = pdecobj->stream;
	pMemOP  = pdecobj->pvoMemop; 

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
		return VO_ERR_INVALID_ARG;

	stream->set_ptr    = pInput->Buffer;
	stream->set_len    = pInput->Length;
	stream->frame_ptr  = stream->frame_ptr_bk;
	stream->used_len   = 0;
	if(stream->framebuffer_len)
	{
		len = MIN(Frame_Maxsize - stream->framebuffer_len, stream->set_len);
		pMemOP->Copy(VO_INDEX_DEC_QCELP, stream->frame_ptr + stream->framebuffer_len, stream->set_ptr, len);
		stream->set_len -= len;
		stream->set_ptr += len;
		stream->framebuffer_len = stream->framebuffer_len + len;
		stream->frame_ptr = stream->frame_ptr_bk;
	}
	return VO_ERR_NONE;
}

static int frame_counter=0;

VO_U32 VO_API voQCELPDec_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	int   len;
	Int16 rate;
	Int16 *outspeech;
	QCP13_DecObj  *pdecobj;
	QC13_PACKET   *qcelp13_packet;	
	FrameStream   *stream; 
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	pdecobj = (QCP13_DecObj*)hCodec;
	qcelp13_packet = &pdecobj->q13_packet;
	stream = pdecobj->stream;
	pMemOP = pdecobj->pvoMemop;

#ifdef NXP   /* NXP special license check */
	if((frame_counter >= 9000) && (pdecobj->VoKey != 1))
		exit(0);
#endif

	if ((NULL == stream))
	{
		return VO_ERR_INVALID_ARG;
	}

	if(stream->framebuffer_len  < Frame_MaxByte)
	{
		stream->frame_storelen = stream->framebuffer_len;
		if(stream->frame_storelen)
			pMemOP->Copy(VO_INDEX_DEC_QCELP, stream->frame_ptr_bk , stream->frame_ptr , stream->frame_storelen);
		if(stream->set_len > 0)
		{
			UpdateFrameBuffer(stream,  pMemOP);
		}
		if(stream->framebuffer_len < Frame_MaxByte)
		{
			if(pAudioFormat)
				pAudioFormat->InputUsed = stream->used_len;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	//while(stream->frame_ptr[0] < 0 || stream->frame_ptr[0] > 5)
	//{
	//	stream->frame_ptr++;
	//	stream->framebuffer_len--;
	//}

	
	rate = stream->frame_ptr[0];
	if(rate < 0 || rate > 5)
	{
	   //stream->frame_ptr += stream->used_len;
	   //stream->framebuffer_len  -= stream->used_len;
    //   pAudioFormat->InputUsed = stream->used_len;
       return VO_ERR_WRONG_STATUS;
	}

	if ((NULL == pOutput->Buffer) || (pOutput->Length < QCELP_SAMPLE_NUM*sizeof(short)))
	{
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	qcelp13_packet->data = (Int8 *)stream->frame_ptr;
	outspeech = (Int16 *)pOutput->Buffer;
	len = QC13_byte_num_per_frame[rate];

	if (VO_ERR_NONE !=  QC13_Decode(pdecobj, outspeech))
	{
		return VO_ERR_WRONG_STATUS;
	}

	pOutput->Length = QCELP_SAMPLE_NUM * sizeof(Int16);	
	stream->frame_ptr += len;
	stream->framebuffer_len  -= len;

	//VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);
#ifdef LCHECK
	voCheckLibCheckAudio(pdecobj->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	frame_counter++;
	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->InputUsed = stream->used_len;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELPDec_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	QCP13_DecObj    *pdecobj;
	FrameStream     *stream;
	int *lValue =   (int*)pData;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	pdecobj = (QCP13_DecObj *)hCodec;
	stream  = pdecobj->stream;

	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		FlushFrameBuffer(stream);
		break;
	case 0x4000FFFF:
		pdecobj->VoKey = *lValue;
		break;
	case VO_PID_COMMON_HEADDATA:
		break;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voQCELPDec_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int  temp;
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

VO_U32 VO_API voQCELPDec_Uninit(VO_HANDLE hCodec)
{
	QCP13_DecObj    *pdecobj;
	QC13_DObj       *Decode;
	FrameStream     *stream;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	pdecobj =  (QCP13_DecObj*)hCodec;
	Decode  =  &pdecobj->Decoder;
	stream  =  pdecobj->stream;
	pMemOP  =  pdecobj->pvoMemop;

	//VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);
#ifdef LCHECK
	voCheckLibUninit(pdecobj->hCheck);
#endif

	if(stream)
	{
		if(stream->frame_ptr_bk)
		{
			voQCELPDec_mem_free(pMemOP, stream->frame_ptr_bk);
			stream->frame_ptr_bk = NULL;
		}
		voQCELPDec_mem_free(pMemOP, stream);
		stream = NULL;
	}

	if (NULL != Decode->lsp_pred_16) voQCELPDec_mem_free(pMemOP, Decode->lsp_pred_16);
	if (NULL != Decode->G_pred) voQCELPDec_mem_free(pMemOP, Decode->G_pred);
	if (NULL != Decode->qlsp_16) voQCELPDec_mem_free(pMemOP, Decode->qlsp_16);
	if (NULL != Decode->pf_hist_16) voQCELPDec_mem_free(pMemOP, Decode->pf_hist_16);
	if (NULL != Decode->bright_16) voQCELPDec_mem_free(pMemOP, Decode->bright_16);
	if (NULL != Decode->last_qlsp_16) voQCELPDec_mem_free(pMemOP, Decode->last_qlsp_16);
	if (NULL != Decode->ring_pole_hist_16) voQCELPDec_mem_free(pMemOP, Decode->ring_pole_hist_16);
	if (NULL != Decode->ring_pole_hist_32)  voQCELPDec_mem_free(pMemOP, Decode->ring_pole_hist_32);
	if (NULL != Decode->adptv_cbk_16) voQCELPDec_mem_free(pMemOP, Decode->adptv_cbk_16);
	if (NULL != Decode->prefilt_acbk_16) voQCELPDec_mem_free(pMemOP, Decode->prefilt_acbk_16);
	if (NULL != Decode->last_G_16) voQCELPDec_mem_free(pMemOP, Decode->last_G_16);
	if (NULL != Decode->bpf_hist_16) voQCELPDec_mem_free(pMemOP, Decode->bpf_hist_16);

	voQCELPDec_mem_free(pMemOP, pdecobj);

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetQCELPDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voQCELPDec_Init;
	pDecHandle->SetInputData = voQCELPDec_SetInputData;
	pDecHandle->GetOutputData = voQCELPDec_GetOutputData;
	pDecHandle->SetParam = voQCELPDec_SetParam;
	pDecHandle->GetParam = voQCELPDec_GetParam;
	pDecHandle->Uninit = voQCELPDec_Uninit;

	return VO_ERR_NONE;
}






