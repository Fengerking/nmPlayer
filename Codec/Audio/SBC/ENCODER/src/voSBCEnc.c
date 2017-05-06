//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    voSBCEnc.c

Abstract:

    Interface for VisualOn SBC encoder.

Author:

    Witten Wen 29-December-2009

Revision History:

*************************************************************************/

#include "voCheck.h"
#include "voSBC.h"
#include "cmnMemory.h"
#include "sbc.h"

#define OUTBUFFERSIZE	(1<<12)
#define ENABLECHECK		0

VO_PTR	g_hvommPlayInst = NULL;
VO_U32 VO_API voSBCInit(VO_HANDLE * phCodec, VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)//HVOCODEC *phCodec)
{
	sbc_t *psbc = NULL;
	VO_MEM_OPERATOR *vopMemOP;

	if(vType != VO_AUDIO_CodingSBC)
		return VO_ERR_INVALID_ARG;

	if(pUserData ==NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		cmnMemFillPointer(VO_AUDIO_CodingSBC);
		vopMemOP = &g_memOP;
	}
	else
	{
		vopMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

	if((psbc = (sbc_t *)voMalloc(vopMemOP, sizeof(sbc_t))) == NULL)
		return VO_ERR_OUTOF_MEMORY;

	voSBCMemset(psbc, 0, sizeof(sbc_t));

	psbc->vopMemOP = vopMemOP;
	if(SBCInit(psbc, 0) != 0)
		return VO_ERR_OUTOF_MEMORY;

#if ENABLECHECK
	if((pUserData->memflag & 0xF0) == 0x10 && pUserData->libOperator)
	{
		voCheckLibInit(&(psbc->hCheck), VO_INDEX_DEC_WMA, 1, g_hvommPlayInst, pUserData->libOperator);
//			return VO_ERR_LICENSE_ERROR;
	}
#endif

	*phCodec = (VO_HANDLE *)psbc;
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCEncUninit(VO_HANDLE hCodec)
{
	sbc_t *psbc = (sbc_t *)hCodec;
	
	if(psbc != NULL)
	{
		VO_MEM_OPERATOR *vopMemOP = psbc->vopMemOP;
#if ENABLECHECK
		if(psbc->hCheck != NULL)
			voCheckLibUninit(psbc->hCheck);
#endif
		if(psbc->m_pFramBuf)
			voSBCFree(psbc->m_pFramBuf);
		SBCFinish(psbc);
		voFree(vopMemOP, psbc);
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCEncSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	sbc_t *psbcenc = (sbc_t *)hCodec;
	if(NULL == psbcenc || NULL == pInput || NULL == pInput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	/* set input data */
	if(psbcenc->m_pNextBegin == psbcenc->m_pFramBuf)
	{
		/* there is no rest data at last input */
		psbcenc->m_pInput	= pInput->Buffer;
		psbcenc->m_InSize	= pInput->Length;
	}
	else
	{
		/* there is rest data at last input */
		VO_U32 temp = psbcenc->m_FrameSize - psbcenc->m_TempLen;
		if(pInput->Length >= temp)
		{
			/* the summation of the last rest data and the current input is more than or equal to a frame size */
			voSBCMemcpy(psbcenc->m_pNextBegin, pInput->Buffer, temp);
			psbcenc->m_pNextBegin += temp;
			psbcenc->m_pInput	= pInput->Buffer + temp;
			psbcenc->m_InSize	= pInput->Length - temp;
			psbcenc->m_TempLen	= psbcenc->m_FrameSize;
		}
		else
		{
			/* the summation of the last rest data and the current input is less than a frame size */
			voSBCMemcpy(psbcenc->m_pNextBegin, pInput->Buffer, pInput->Length);
			psbcenc->m_pNextBegin += pInput->Length;
			psbcenc->m_pInput	= pInput->Buffer + pInput->Length;
			psbcenc->m_InSize	= 0;
			psbcenc->m_TempLen	+= pInput->Length;
		}
	}
	psbcenc->m_Consumed	= 0;

	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCEncGetOutData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	sbc_t *psbcenc = (sbc_t *)hCodec;
	int consumed = 0;

	if(psbcenc == NULL || NULL == pOutput || NULL == pOutput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}

	/* encoding */
	if(psbcenc->m_pNextBegin != psbcenc->m_pFramBuf)
	{
		/* there is the rest data of Last input */
		consumed = SBCEncode(psbcenc, psbcenc->m_pFramBuf, psbcenc->m_TempLen, pOutput->Buffer, 
			OUTBUFFERSIZE, &pOutput->Length);
		if(consumed < 0)
			return VO_ERR_INVALID_ARG;
		else if(consumed == 0)
		{
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
		else
		{
			psbcenc->m_Consumed		+= consumed;
			psbcenc->m_pNextBegin	= psbcenc->m_pFramBuf;
			psbcenc->m_TempLen		= 0;
		}
	}
	else
	{
		/* the current input */
		consumed = SBCEncode(psbcenc, psbcenc->m_pInput, psbcenc->m_InSize, pOutput->Buffer, 
			OUTBUFFERSIZE, &pOutput->Length);
		if(consumed < 0)
			return VO_ERR_INVALID_ARG;
		else if(consumed == 0)
		{
			if(psbcenc->m_InSize != 0)
			{
				/* save the test data of the current input */
				voSBCMemcpy(psbcenc->m_pNextBegin, psbcenc->m_pInput, psbcenc->m_InSize);
				psbcenc->m_TempLen = psbcenc->m_InSize;
				psbcenc->m_pNextBegin += psbcenc->m_InSize;
			}
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
		else
		{
			psbcenc->m_pInput	+= consumed;
			psbcenc->m_InSize	-= consumed;
			psbcenc->m_Consumed	+= consumed;
		}
	}

	/* out information */
	if(pOutInfo != NULL)
	{
		pOutInfo->Format.SampleRate	= psbcenc->m_Rate;
		pOutInfo->Format.Channels	= psbcenc->m_Channels;
		pOutInfo->Format.SampleBits	= 16;
		pOutInfo->InputUsed			= psbcenc->m_Consumed;
	}
#if ENABLECHECK
	if(voCheckLibCheckAudio(psbcenc->hCheck, pOutput, &(pOutInfo->Format)) != VO_ERR_NONE)
		return VO_ERR_LICENSE_ERROR;
#endif
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCEncSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	sbc_t *psbcenc = (sbc_t *)hCodec;
	if(psbcenc == NULL || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		psbcenc->m_Rate = *((int *)pData);
		break;

	case VO_PID_AUDIO_CHANNELS:
		psbcenc->m_Channels = *((int *)pData);
		break;

	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *pWAV_Format = (VO_AUDIO_FORMAT *)pData;

			psbcenc->m_Rate = pWAV_Format->SampleRate;
			psbcenc->m_Channels = pWAV_Format->Channels;
			if(pWAV_Format->SampleBits!=16)
				return VO_ERR_NOT_IMPLEMENT;
			psbcenc->m_FrameSize = SBCGetCodesize(psbcenc);//psbcenc->m_Blocks * psbcenc->m_Subbands * psbcenc->channels * 2;
			voSBCSetInit(psbcenc);
			break;
		}
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCEncGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	sbc_t *psbcenc = (sbc_t *)hCodec;
	if(psbcenc == NULL || pData == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		*((int *)pData) = psbcenc->m_Rate;
		break;

	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = psbcenc->m_Channels;
		break;

	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *pFormat = (VO_AUDIO_FORMAT *)pData;
			pFormat->SampleRate	= psbcenc->m_Rate;
			pFormat->Channels	= psbcenc->m_Channels;
			pFormat->SampleBits	= 16;
		}
		break;

	case VO_PID_SBC_INPUTSIZE:
		*((int *)pData) = psbcenc->m_FrameSize;
		break;

	case VO_PID_SBC_MAXOUTSIZE:
		*((int *)pData) = OUTBUFFERSIZE;
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetSBCEncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(pEncHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pEncHandle->Init			= voSBCInit;
	pEncHandle->SetInputData	= voSBCEncSetInputData;
	pEncHandle->GetOutputData	= voSBCEncGetOutData;
	pEncHandle->SetParam		= voSBCEncSetParam;
	pEncHandle->GetParam		= voSBCEncGetParam;
	pEncHandle->Uninit			= voSBCEncUninit;

	return VO_ERR_NONE;
}

