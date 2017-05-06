/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
ITU-T G.729A Speech Coder    ANSI-C Source Code
Version 1.1    Last modified: September 1996

Copyright (c) 1996,
AT&T, France Telecom, NTT, Universite de Sherbrooke
All rights reserved.
*/

/*-------------------------------------------------------------------*
* Main program of the ITU-T G.729A  8 kbit/s encoder.               *
*                                                                   *
*    Usage : coder speech_file  bitstream_file                      *
*-------------------------------------------------------------------*/
#include "typedef.h"
#include "ld8a.h"
#include "voAudio.h"
#include "mem_align.h"
#include "cmnMemory.h"
#include "voG729.h"
#include "voIndex.h"
#include "voCheck.h"
#include "g729_api.h"


Word16 prm[PRM_SIZE];          /* Analysis parameters.                  */
extern short *new_speech;
void *g_hG729EncInst = NULL;
/*--------------------------------------------------------------------------*
* Initialization of the coder.                                             *
*--------------------------------------------------------------------------*/
#ifdef DUMP
FILE *dump_dat;
#endif

VO_U32 VO_API voG729_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	G729GlobalData *gData;
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

	gData = (G729GlobalData *)mem_malloc(pMemOP, sizeof(G729GlobalData), 32);
	if(gData == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->inbuf_ptr = NULL;
	gData->outbuf_ptr = NULL;
	gData->PackType  = 0;                  /* 0 --- Raw data, 1 --- Add Sync word*/

	Init_Pre_Process();
	Init_Coder_ld8a();
	Set_zero(prm, PRM_SIZE);

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

	*phCodec = (VO_HANDLE)gData;

#ifdef DUMP
	dump_dat = fopen("E:\g729dump.dat", "wb");
#endif

#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
	    voCheckLibInit(&(gData->hCheck), VO_INDEX_ENC_G729, 1, g_hG729EncInst, pUserData->libOperator);
	else
        voCheckLibInit(&(gData->hCheck), VO_INDEX_ENC_G729, 1, g_hG729EncInst, NULL);
#endif
	return VO_ERR_NONE;
}


VO_U32 VO_API voG729_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G729GlobalData  *gData;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G729GlobalData *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->inbuf_ptr = pInput->Buffer;
	gData->inbuf_len = pInput->Length;	

	return VO_ERR_NONE;
}

VO_U32 VO_API voG729_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	G729GlobalData *gData;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (G729GlobalData *)hCodec;
	gData->outbuf_ptr = pOutput->Buffer;


	Pre_Process((Word16 *)gData->inbuf_ptr, L_FRAME);
	memcpy(new_speech, gData->inbuf_ptr, 160);
	Coder_ld8a(prm);
	prm2bits_ld8k(prm, (Word16 *)gData->outbuf_ptr, gData->PackType);

    if(gData->PackType == 1)
	gData->outbuf_len = 14;
    else
	gData->outbuf_len = 10;

#ifdef DUMP
	fwrite(gData->outbuf_ptr, 1, gData->outbuf_len, dump_dat);
#endif

	pOutput->Length = gData->outbuf_len;

#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif
	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = 160;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voG729_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G729GlobalData* gData = (G729GlobalData*)hCodec;
	int  *lValue = (int*)pData;

	switch(uParamID)
	{
	case VO_PID_G729_FRAMETYPE:
		if(*lValue < VOG729RAW_DATA||*lValue > VOG729PACK_DATA)
			return VO_ERR_WRONG_PARAM_ID; 
		gData->PackType = *lValue;
		break;

	case VO_PID_COMMON_HEADDATA:
		break;

	case VO_PID_COMMON_FLUSH:
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voG729_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;
	G729GlobalData* gData = (G729GlobalData*)hCodec;
	if (gData==NULL) 
		return VO_ERR_OUTOF_MEMORY;

	switch(uParamID)
	{
	case VO_PID_G729_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;

			fmt->Channels    = 1;
			fmt->SampleRate  = 8000;
			fmt->SampleBits  = 16;
			break;
		}

	case VO_PID_G729_CHANNELS:
		temp = 1;
		pData = (void *)(&temp);
		break;

	case VO_PID_G729_SAMPLERATE:
		temp = 8000;
		pData = (void *)(&temp);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


VO_U32 VO_API voG729_Uninit(VO_HANDLE hCodec)
{
	G729GlobalData *gData;	
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G729GlobalData *)hCodec;

	pMemOP = gData->pvoMemop;
#ifdef LCHECK
	voCheckLibUninit(gData->hCheck);
#endif
	mem_free(pMemOP, hCodec);

#ifdef DUMP
	fclose(dump_dat);
#endif

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetG729EncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(NULL == pEncHandle)
		return VO_ERR_INVALID_ARG;
	pEncHandle->Init = voG729_Init;
	pEncHandle->SetInputData = voG729_SetInputData;
	pEncHandle->GetOutputData = voG729_GetOutputData;
	pEncHandle->SetParam = voG729_SetParam;
	pEncHandle->GetParam = voG729_GetParam;
	pEncHandle->Uninit = voG729_Uninit;

	return VO_ERR_NONE;
}

