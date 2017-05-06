
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "g711.h"
#include "voG711.h"
#include "cmnMemory.h"
#include "voCheck.h"
#include "mem_align.h"

typedef short Word16;
typedef int Word32;

VO_HANDLE g_G711dec_inst = NULL;

static int voG711DecProc(G711DecStat *st)
{
	Word16 i;
	Word16 len = (Word16)st->input_len;
	unsigned char *input = (unsigned char *)st->input_buffer;
	Word16 *output = (Word16 *)st->output_buffer;

	if(st->Workmode == VOG711_MDMU_LAW )
	{
		for(i=0; i < len; i++)
		{
			*(output+i) = voG711DecUlaw2linear((unsigned char)*(input+i));
		}
	}
	else
	{
		for(i=0; i < len; i++)
		{
			*(output+i) = voG711DecAlaw2linear((unsigned char)*(input+i));
		}
	}
	return (len<<1);
}

static VO_U32 VO_API voG711_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	G711DecStat *st;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem = 0;
  VO_U32 ret						= VO_ERR_NONE;
#if LCHECK	
  VO_PTR p_lic_handle				= NULL;
  VO_LIB_OPERATOR *p_lib_op		= NULL;
#endif
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

#ifdef LCHECK
  if ( pUserData ) {
    if ( pUserData->memflag & 0XF0 ) {//valid libOperator flag
      p_lib_op = pUserData->libOperator;
    }
  }
  ret = voCheckLibInit( &p_lic_handle, VO_INDEX_DEC_G711, pUserData ? pUserData->memflag : 0, g_G711dec_inst, p_lib_op );

  if ( ret != VO_ERR_NONE ) {
    voCheckLibUninit( p_lic_handle );
    p_lic_handle = NULL;
    return ret;
  }  
#endif

	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	if ((st = (G711DecStat *)mem_malloc(pMemOP, sizeof(G711DecStat), 32)) == NULL)
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
#ifdef LCHECK
  st->hCheck = p_lic_handle;
#endif

	*phCodec = (VO_HANDLE)st;
	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G711DecStat *gData;
	if (NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G711DecStat *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->input_buffer  = (unsigned char *)pInput->Buffer;
	gData->input_len = pInput->Length;

	return VO_ERR_NONE;
}

static VO_U32 VO_API voG711_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	G711DecStat *st = (G711DecStat *)hCodec;

	st->output_buffer = (short *)pOutput->Buffer;

	st->output_len = voG711DecProc(st);

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
	G711DecStat *st = (G711DecStat *)hCodec;
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
	G711DecStat *st = (G711DecStat*)hCodec;
	VO_MEM_OPERATOR *pMemOP;

	pMemOP = st->pvoMemop;

#ifdef LCHECK
	voCheckLibUninit(st->hCheck);
#endif

	if(hCodec)
	{
		mem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetG711DecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voG711_Init;
	pDecHandle->SetInputData = voG711_SetInputData;
	pDecHandle->GetOutputData = voG711_GetOutputData;
	pDecHandle->SetParam = voG711_SetParam;
	pDecHandle->GetParam = voG711_GetParam;
	pDecHandle->Uninit = voG711_Uninit;

	return VO_ERR_NONE;
}








