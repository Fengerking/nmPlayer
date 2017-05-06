/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010	    *
*									                                    *
************************************************************************/
/***********************************************************************
File:		voAPE_D_Api.c

Contains:	APE Decoder API function files

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

************************************************************************/

#include  "stdio.h"
#include  "string.h"
#include  "malloc.h"
#include  "voAPETag.h"
#include  "voAPE_Global.h"
#include  "voAPE_D_API.h"
#include  "voTypedef.h"
#include  "voAPEDec.h"
#include  "cmnMemory.h"
#include  "voIndex.h"
#include  "mem_align.h"


void *g_hAPEDecInst = NULL;

#define DUMP

#ifdef DUMP
FILE *dump_file = NULL;
#endif

VO_U32 VO_API voAPE_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	voAPE_GData    *gData;

	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem = 0;
	if(pUserData == NULL || pUserData->memflag != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
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

	gData = (voAPE_GData *)mem_malloc(pMemOP, sizeof(voAPE_GData), 32);
	gData->voAPED_State = 0;                /* 0 --- Parser APE Header info, 1 --- decoder APE raw-data*/
	gData->PredFlag = 0;                    /* 0 --- have not init Predictor structure */

	gData->ape_desc = (voAPE_DESCRIPTOR *)mem_malloc(pMemOP, sizeof(voAPE_DESCRIPTOR), 32);
	memset(gData->ape_desc, 0, sizeof(voAPE_DESCRIPTOR));

	gData->ape_hdr  = (voAPE_HEADER *)mem_malloc(pMemOP, sizeof(voAPE_HEADER), 32);
	memset(gData->ape_hdr, 0, sizeof(voAPE_HEADER));

	gData->ape_hdr_old = (voAPE_HEADER_OLD *)mem_malloc(pMemOP, sizeof(voAPE_HEADER_OLD), 32);
	memset(gData->ape_hdr_old, 0, sizeof(voAPE_HEADER_OLD));

	gData->ape_info  = (voAPE_FILE_INFO *)mem_malloc(pMemOP, sizeof(voAPE_FILE_INFO), 32);
	memset(gData->ape_info, 0, sizeof(voAPE_FILE_INFO));

	gData->Xcnnf = (voCNNFState *)mem_malloc(pMemOP, sizeof(voCNNFState), 32);
	/* init m_rbAdaptA, m_rbAdaptB */
	gData->Xcnnf->m_rbAdaptA.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Xcnnf->m_rbAdaptA.m_pCurrent = &gData->Xcnnf->m_rbAdaptA.m_pData[8];
	gData->Xcnnf->m_rbAdaptB.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Xcnnf->m_rbAdaptB.m_pCurrent = &gData->Xcnnf->m_rbAdaptB.m_pData[8]; 
	/*init m_rbPredictionA, m_rbPredictionB*/
	gData->Xcnnf->m_rbPredictionA.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Xcnnf->m_rbPredictionA.m_pCurrent = &gData->Xcnnf->m_rbPredictionA.m_pData[8];
	gData->Xcnnf->m_rbPredictionB.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Xcnnf->m_rbPredictionB.m_pCurrent = &gData->Xcnnf->m_rbPredictionB.m_pData[8]; 

	gData->Ycnnf = (voCNNFState *)mem_malloc(pMemOP, sizeof(voCNNFState), 32);
	/* init m_rbAdaptA, m_rbAdaptB */
	gData->Ycnnf->m_rbAdaptA.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Ycnnf->m_rbAdaptA.m_pCurrent = &gData->Ycnnf->m_rbAdaptA.m_pData[8];
	gData->Ycnnf->m_rbAdaptB.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Ycnnf->m_rbAdaptB.m_pCurrent = &gData->Ycnnf->m_rbAdaptB.m_pData[8]; 
	/*init m_rbPredictionA, m_rbPredictionB*/
	gData->Ycnnf->m_rbPredictionA.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Ycnnf->m_rbPredictionA.m_pCurrent = &gData->Ycnnf->m_rbPredictionA.m_pData[8];
	gData->Ycnnf->m_rbPredictionB.m_pData = (int *)mem_malloc(pMemOP, (512 + 8) * sizeof(int), 32);
	gData->Ycnnf->m_rbPredictionB.m_pCurrent = &gData->Ycnnf->m_rbPredictionB.m_pData[8]; 


	gData->DecProcess = (voDecodeBytes *)mem_malloc(pMemOP, sizeof(voDecodeBytes), 32);

	memset(gData->DecProcess, 0, sizeof(voDecodeBytes));

	gData->stream_buf = (FrameStream *)mem_malloc(pMemOP, sizeof(FrameStream), 32);

	gData->stream_buf->frame_ptr = (unsigned char *)mem_malloc(pMemOP, Frame_Maxsize, 32);


	InitFrameBuffer(gData->stream_buf);

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

	*phCodec = (VOPTR)gData;

#ifdef DUMP
	dump_file = fopen("f:\\log.txt", "wb+");
#endif

#ifdef LCHECK 
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	    voCheckLibInit(&(st->hCheck), VO_INDEX_DEC_APE, 1, g_hAPEDecInst, pUserData->libOperator);
	else
        voCheckLibInit(&(st->hCheck), VO_INDEX_DEC_APE, 1, g_hAPEDecInst, NULL);
#endif

	return  VO_ERR_NONE;
}

VO_U32 VO_API voAPE_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	voAPE_GData   *gData;
	FrameStream   *stream;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (voAPE_GData *)hCodec;
	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}
	stream = gData->stream_buf;

	stream->set_ptr   = pInput->Buffer;
	stream->set_len   = pInput->Length;
	stream->frame_ptr = stream->set_ptr;
	stream->used_len  = 0;

#ifdef DUMP
	fwrite(pInput->Buffer, 1, pInput->Length, dump_file);
	fflush(dump_file);
#endif

	return VO_ERR_NONE;

}

VO_U32 VO_API voAPE_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	voAPE_GData     *gData;
	FrameStream     *stream;
	VO_MEM_OPERATOR *pMemOP;
	unsigned char   Magic[4];
	int             state = 0;
	int             nBlocksLeft = 0;

	if(NULL == hCodec)
	{
		return -1;
	}

	gData = (voAPE_GData *)hCodec;
	stream = (FrameStream *)gData->stream_buf;
	pMemOP = gData->pvoMemop;

	stream->outbuf = (unsigned char *)pOutput->Buffer;

	if (stream->set_len <= 10)
	{
		stream->frame_ptr = stream->frame_ptr_bk;
		stream->set_len = 0;
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	voAPEDecodeStep(hCodec);

	pOutput->Length = BLOCKS_NUMBERS * gData->ape_info->nChannels * 2 ;                     /* one frame output length BLOCKS_NUMBERS * 4K Bytes */

#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, pAudioFormat);
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = gData->ape_info->nChannels;
		pAudioFormat->Format.SampleRate = gData->ape_info->nSampleRate;
		pAudioFormat->Format.SampleBits = gData->ape_info->nBitsPerSample;	
		pAudioFormat->InputUsed = (stream->used_len - stream->over_byte);
	}

	return  VO_ERR_NONE;
}

VO_U32 VO_API voAPE_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int ret;
	voAPE_GData        *gData;
	VO_CODECBUFFER     *inData;
	VO_AUDIO_FORMAT    *pFormat;
	voAPE_FILE_INFO    *pApeInfo;
	voAPEBlocks        *pApeBlk;
	
	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	gData = (voAPE_GData *)hCodec;
	pApeInfo = (voAPE_FILE_INFO *)gData->ape_info;
    pApeBlk  = &(gData->DecProcess->Blk_State);

	switch(uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
		inData = (VO_CODECBUFFER *)pData;
		ret = voAPEMetaInfo(gData, inData->Buffer , inData->Length);
		break;

	case VO_PID_COMMON_FLUSH:
		if(*(int *)pData)
		{
			FlushFrameBuffer(gData->stream_buf);
		}
        pApeBlk->m_nCurrentFrameBufferBlock = 0;
		memset(gData->DecProcess, 0, sizeof(voDecodeBytes));
		break;
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = pApeInfo->nChannels;
		pFormat->SampleBits = pApeInfo->nBitsPerSample;
		pFormat->SampleRate = pApeInfo->nSampleRate;
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return  VO_ERR_NONE;
}

VO_U32 VO_API voAPE_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{

	return  VO_ERR_NONE;
}

VO_U32 voAPE_Uninit(VO_HANDLE hCodec)
{
	VO_MEM_OPERATOR *pMemOP;
	voAPE_GData     *gData = (voAPE_GData *)hCodec;
	int             nComplevel = gData->ape_info->nCompressionLevel;
	pMemOP = gData->pvoMemop;

	if(gData->ape_desc != NULL)
		mem_free(pMemOP, gData->ape_desc);
	if(gData->ape_hdr != NULL)
		mem_free(pMemOP, gData->ape_hdr);
	if(gData->ape_hdr_old != NULL)
		mem_free(pMemOP, gData->ape_hdr_old);
	if(gData->ape_info != NULL)
		mem_free(pMemOP, gData->ape_info);

	/* Release gData->Xcnnf */
	if(gData->Xcnnf != NULL)
	{
		if(gData->Xcnnf->m_rbAdaptA.m_pData != NULL)
			mem_free(pMemOP, gData->Xcnnf->m_rbAdaptA.m_pData);
		if(gData->Xcnnf->m_rbAdaptB.m_pData != NULL)
			mem_free(pMemOP, gData->Xcnnf->m_rbAdaptB.m_pData);
		if(gData->Xcnnf->m_rbPredictionA.m_pData != NULL)
			mem_free(pMemOP, gData->Xcnnf->m_rbPredictionA.m_pData);
		if(gData->Xcnnf->m_rbPredictionB.m_pData != NULL)
			mem_free(pMemOP, gData->Xcnnf->m_rbPredictionB.m_pData);
		voAPE_PreUnInit(gData->Xcnnf, nComplevel);
	}
	mem_free(pMemOP, gData->Xcnnf);

#ifdef DUMP
	fclose(dump_file);
#endif

#ifdef LCHECK
	    voCheckLibUninit(gData->hCheck);
#endif

	/* Release gData->Ycnnf */
	if(gData->Ycnnf != NULL)
	{
		if(gData->Ycnnf->m_rbAdaptA.m_pData != NULL)
			mem_free(pMemOP, gData->Ycnnf->m_rbAdaptA.m_pData);
		if(gData->Ycnnf->m_rbAdaptB.m_pData != NULL)
			mem_free(pMemOP, gData->Ycnnf->m_rbAdaptB.m_pData);
		if(gData->Ycnnf->m_rbPredictionA.m_pData != NULL)
			mem_free(pMemOP, gData->Ycnnf->m_rbPredictionA.m_pData);
		if(gData->Ycnnf->m_rbPredictionB.m_pData != NULL)
			mem_free(pMemOP, gData->Ycnnf->m_rbPredictionB.m_pData);

		voAPE_PreUnInit(gData->Ycnnf, nComplevel);
	}
	mem_free(pMemOP, gData->Ycnnf);

	if(gData->DecProcess != NULL)
		mem_free(pMemOP, gData->DecProcess);
	if(gData->stream_buf->frame_ptr_bk != NULL)
		mem_free(pMemOP, gData->stream_buf->frame_ptr_bk);
    
	mem_free(pMemOP, gData->stream_buf);

	mem_free(pMemOP, gData);

	return  VO_ERR_NONE;
}

VO_S32 VO_API voGetAPEDecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voAPE_Init;
	pDecHandle->SetInputData = voAPE_SetInputData;
	pDecHandle->GetOutputData = voAPE_GetOutputData;
	pDecHandle->SetParam = voAPE_SetParam;
	pDecHandle->GetParam = voAPE_GetParam;
	pDecHandle->Uninit = voAPE_Uninit;

	return VO_ERR_NONE;
}









