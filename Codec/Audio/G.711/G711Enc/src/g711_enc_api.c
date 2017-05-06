
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "g711.h"
#include "voG711.h"
#include "cmnMemory.h"
#include "voCheck.h"
#include "mem_align.h"


static short voG711EncProc(G711EncStat *st)
{
	int i;
	int len = (unsigned long)st->input_len;
	short *input = (short *)st->input_buffer;
	unsigned char *output = (unsigned char *)st->output_buffer;

	if(st->Workmode == VOG711_MDMU_LAW )
	{
		for(i=0; i < len; i++)
		{
			*(output + i) = voG711EncLinear2ulaw((short)*(input + i));
		}
	}
	else
	{
		for(i=0; i < len; i++)
		{
			*(output + i) = voG711EncLinear2alaw((short)*(input + i));
		}
	}
	return (len>>1);
}

static VO_U32 VO_API voG711_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	G711EncStat *st;
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

	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	if ((st = (G711EncStat *)voG711EncMem_malloc(pMemOP, sizeof(G711EncStat), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	} 

	st->input_buffer  = NULL;
	st->output_buffer = NULL;
	st->input_len  = 0;
	st->output_len = 0;
	st->Workmode = 0;

	if(interMem)
	{
		st->voMemoprator.Alloc = cmnMemAlloc;
		st->voMemoprator.Copy = cmnMemCopy;
		st->voMemoprator.Free = cmnMemFree;
		st->voMemoprator.Set = cmnMemSet;
		st->voMemoprator.Check = cmnMemCheck;
		pMemOP = &st->voMemoprator;
	}
	st->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)st;

#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_G711, 1, g_hG711EncInst, pUserData->libOperator);
	else
		voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_G711, 1, g_hG711EncInst, NULL);
#endif

	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G711EncStat *gData;
	if (NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G711EncStat *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->input_buffer  = (short *)pInput->Buffer;
	gData->input_len = pInput->Length;

	return VO_ERR_NONE;
}


static VO_U32 VO_API voG711_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	G711EncStat *st = (G711EncStat *)hCodec;

	st->output_buffer = pOutput->Buffer;

	st->output_len = voG711EncProc(st);

	pOutput->Length = st->output_len;

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = st->input_len;	
	}
	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G711EncStat *st = (G711EncStat *)hCodec;
	int  *lValue = (int*)pData;

	if(st == NULL)
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_G711_MODE:
		{
			if(*lValue < VOG711_MDMU_LAW||*lValue > VOG711_MDA_LAW)
				return VO_ERR_WRONG_PARAM_ID; 
			st->Workmode = (short)*lValue;
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{

	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_Uninit(VO_HANDLE hCodec)
{
	G711EncStat *st = (G711EncStat*)hCodec;
	VO_MEM_OPERATOR *pMemOP;

	pMemOP = st->pvoMemop;

#ifdef LCHECK
	voCheckLibUninit(st->hCheck);
#endif

	if(hCodec)
	{
		voG711EncMem_free(pMemOP, hCodec);
		hCodec = NULL;
	}

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetG711EncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(NULL == pEncHandle)
		return VO_ERR_INVALID_ARG;
	pEncHandle->Init = voG711_Init;
	pEncHandle->SetInputData = voG711_SetInputData;
	pEncHandle->GetOutputData = voG711_GetOutputData;
	pEncHandle->SetParam = voG711_SetParam;
	pEncHandle->GetParam = voG711_GetParam;
	pEncHandle->Uninit = voG711_Uninit;

	return VO_ERR_NONE;
}










