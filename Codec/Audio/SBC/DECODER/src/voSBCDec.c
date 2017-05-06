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

VO_U32 VO_API voSBCDecInit(VO_HANDLE * phCodec, VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)//HVOCODEC *phCodec)
{
	sbc_t *psbcdec = NULL;
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

	if((psbcdec = (sbc_t *)voMalloc(vopMemOP, sizeof(sbc_t))) == NULL)
		return VO_ERR_OUTOF_MEMORY;

	voSBCMemset(psbcdec, 0, sizeof(sbc_t));

	psbcdec->vopMemOP = vopMemOP;
	if(SBCInit(psbcdec, 0) != 0)
		return VO_ERR_OUTOF_MEMORY;

#if ENABLECHECK
	if((pUserData->memflag & 0xF0) == 0x10 && pUserData->libOperator)
	{
		voCheckLibInit(&(psbcdec->hCheck), VO_INDEX_DEC_WMA, 1, g_hvommPlayInst, pUserData->libOperator);
//			return VO_ERR_LICENSE_ERROR;
	}
#endif

	*phCodec = (VO_HANDLE *)psbcdec;
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCDecUninit(VO_HANDLE hCodec)
{
	sbc_t *psbcdec = (sbc_t *)hCodec;	
	if(psbcdec != NULL)
	{
		VO_MEM_OPERATOR *vopMemOP = psbcdec->vopMemOP;
#if ENABLECHECK
		if(psbcdec->hCheck != NULL)
			voCheckLibUninit(psbcdec->hCheck);
#endif
		if(psbcdec->m_pFramBuf)
			voSBCFree(psbcdec->m_pFramBuf);
		SBCFinish(psbcdec);
		voFree(vopMemOP, psbcdec);
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	sbc_t *psbcdec = (sbc_t *)hCodec;
	if(NULL == psbcdec || NULL == pInput || NULL == pInput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	/* set input data */
	if(psbcdec->m_pNextBegin == psbcdec->m_pFramBuf)
	{
		/* there is no rest data at last input */
		psbcdec->m_pInput	= pInput->Buffer;
		psbcdec->m_InSize	= pInput->Length;
	}
	else
	{
		/* there is rest data at last input */
		VO_U32 temp = psbcdec->m_FrameSize - psbcdec->m_TempLen;
		if(pInput->Length >= temp)
		{
			/* the summation of the last rest data and the current input is more than or equal to a frame size */
			voSBCMemcpy(psbcdec->m_pNextBegin, pInput->Buffer, temp);
			psbcdec->m_pNextBegin += temp;
			psbcdec->m_pInput	= pInput->Buffer + temp;
			psbcdec->m_InSize	= pInput->Length - temp;
			psbcdec->m_TempLen	= psbcdec->m_FrameSize;
		}
		else
		{
			/* the summation of the last rest data and the current input is less than a frame size */
			voSBCMemcpy(psbcdec->m_pNextBegin, pInput->Buffer, pInput->Length);
			psbcdec->m_pNextBegin += pInput->Length;
			psbcdec->m_pInput	= pInput->Buffer + pInput->Length;
			psbcdec->m_InSize	= 0;
			psbcdec->m_TempLen	+= pInput->Length;
		}
	}
	psbcdec->m_Consumed	= 0;

	return VO_ERR_NONE;
}



VO_U32 VO_API voSBCDecGetOutData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	sbc_t *psbcdec = (sbc_t *)hCodec;
	int consumed = 0;

	if(psbcdec == NULL || NULL == pOutput || NULL == pOutput->Buffer)
	{
		return VO_ERR_INVALID_ARG;
	}

	/* decoding */
	if(psbcdec->m_pNextBegin != psbcdec->m_pFramBuf)
	{
		/* there is the rest data of Last input */
		consumed = SBCDecode(psbcdec, psbcdec->m_pFramBuf, psbcdec->m_TempLen, pOutput->Buffer, 
			OUTBUFFERSIZE, &pOutput->Length);
		if(consumed < 0)
			return VO_ERR_INVALID_ARG;
		else if(consumed == 0)
		{
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
		else
		{
			psbcdec->m_Consumed		+= consumed;
			psbcdec->m_pNextBegin	= psbcdec->m_pFramBuf;
			psbcdec->m_TempLen		= 0;
		}
	}
	else
	{
		/* the current input */
		consumed = SBCDecode(psbcdec, psbcdec->m_pInput, psbcdec->m_InSize, pOutput->Buffer, 
			OUTBUFFERSIZE, &pOutput->Length);
		if(consumed >=0)
		{
			psbcdec->m_pInput	+= consumed;
			psbcdec->m_InSize	-= consumed;
			psbcdec->m_Consumed	+= consumed;
		}
		else if(consumed == -1)
		{
			if(psbcdec->m_InSize != 0)
			{
				/* save the test data of the current input */
				voSBCMemcpy(psbcdec->m_pNextBegin, psbcdec->m_pInput, psbcdec->m_InSize);
				psbcdec->m_TempLen = psbcdec->m_InSize;
				psbcdec->m_pNextBegin += psbcdec->m_InSize;
			}
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
		else
			return VO_ERR_INVALID_ARG;
	}

	/* out information */
	if(pOutInfo != NULL)
	{
		pOutInfo->Format.SampleRate	= psbcdec->m_Rate;
		pOutInfo->Format.Channels	= psbcdec->m_Channels;
		pOutInfo->Format.SampleBits	= 16;
		pOutInfo->InputUsed			= psbcdec->m_Consumed;
	}
#if ENABLECHECK
	if(voCheckLibCheckAudio(psbcdec->hCheck, pOutput, &(pOutInfo->Format)) != VO_ERR_NONE)
		return VO_ERR_LICENSE_ERROR;
#endif
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCDecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voSBCDecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	sbc_t *psbcdec = (sbc_t *)hCodec;
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *sbc_format = (VO_AUDIO_FORMAT *)pData;
			sbc_format->Channels	= psbcdec->m_Channels;
			sbc_format->SampleRate	= psbcdec->m_Rate;
			sbc_format->SampleBits	= 16;
		}
		break;

	case VO_PID_AUDIO_SAMPLEREATE:
		*((int *)pData)	= psbcdec->m_Rate;
		break;

	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = psbcdec->m_Channels;
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetSBCDecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init			= voSBCDecInit;
	pDecHandle->SetInputData	= voSBCDecSetInputData;
	pDecHandle->GetOutputData	= voSBCDecGetOutData;
	pDecHandle->SetParam		= voSBCDecSetParam;
	pDecHandle->GetParam		= voSBCDecGetParam;
	pDecHandle->Uninit			= voSBCDecUninit;

	return VO_ERR_NONE;
}