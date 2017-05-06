/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voAMRNB.c

Contains:	AMR_NB API Operator Implement Code

Written by:	Lina Lv

Change History (most recent first):
2012-05-09		LinaLv			Create file

*******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include "cmnMemory.h"
#include "mem_align.h"
#include "voAudioSpeed.h"
#include "voRingBuffer.h"
#include "voAudioSpeedghdr.h"
#include "voCheck.h"

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
VO_PTR	g_hAudioSpeedInst = NULL;
#endif



#define LOGE 




VO_U32 VO_API voAudioSpeed_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
#ifdef LCHECK
	VO_PTR pTemp;
#endif

	voxPlayerGlobal *pGst;
    voxPlayerStrech *pTsm;

	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem = 0;

  LOGE("voAudioSpeed_Init enter \n");

	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
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
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_AUDIO_AudioSpeed, pUserData->memflag|1, g_hAudioSpeedInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_AUDIO_AudioSpeed, pUserData->memflag|1, g_hAudioSpeedInst, NULL);

	if (nRet != VO_ERR_NONE)
	{
		if (pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}
#endif

	pGst = (voxPlayerGlobal*)as_mem_malloc(pMemOP, sizeof(voxPlayerGlobal), 32);

	//LOGI("Malloc successfully\n");
	if(pGst==NULL)
	{
    LOGE("voAudioSpeed_Init leave VO_ERR_OUTOF_MEMORY\n");
		return VO_ERR_OUTOF_MEMORY;
	}

	/* Inint some parameters */
	pGst->nChs = 2;
	pGst->nSampleRate = 44100;
	pGst->nBitsSample = 16;

    pTsm = (voxPlayerStrech *)as_mem_malloc(pMemOP, sizeof(voxPlayerStrech), 32);

	if(interMem)
	{
		pGst->voMemoprator.Alloc = cmnMemAlloc;
		pGst->voMemoprator.Copy = cmnMemCopy;
		pGst->voMemoprator.Free = cmnMemFree;
		pGst->voMemoprator.Set = cmnMemSet;
		pGst->voMemoprator.Check = cmnMemCheck;
		pMemOP = &pGst->voMemoprator;
	}
	pGst->pvoMemop = pMemOP;
	pTsm->pvoMemop = pMemOP;

	if (pTsm == NULL)
	{
    LOGE("voAudioSpeed_Init leave VO_ERR_OUTOF_MEMORY\n");
		return VO_ERR_OUTOF_MEMORY;
	}
	else
	{
		pTsm->bFastMode = 1;                            //default enable fast mode
		pTsm->nChs = 2;
		pTsm->pInBuf = (voRingBuffer *)as_mem_malloc(pMemOP, sizeof(voRingBuffer), 32);
        pTsm->pOutBuf = (voRingBuffer *)as_mem_malloc(pMemOP, sizeof(voRingBuffer), 32);
		pTsm->pInBuf->nChannles = 2;
		pTsm->pOutBuf->nChannles = 2;
		pTsm->bAutoSeekFlag = (VO_BOOL)1;
		pTsm->bAutoSeqFlag = (VO_BOOL)1;
		pTsm->tempo=1;												//add by xsy
		tsmSetParameters(pTsm, 44100, 0, 0, 8);
	}
	pGst->pTsm = pTsm;	

	if ((pGst->pTsm->tempo>0.99999)&&(pGst->pTsm->tempo<1.00001))
	{
		tsmSetTempo(pGst->pTsm, 0.0);
		tsmClear(pGst->pTsm);
	}

#ifdef LCHECK
	pGst->hCheck = pTemp;
#endif
	*phCodec = (VO_HANDLE)pGst;
    //LOGI("Init End\n");
  LOGE("voAudioSpeed_Init leave VO_ERR_NONE\n");
	return VO_ERR_NONE;

}

VO_U32 VO_API voAudioSpeed_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	VO_MEM_OPERATOR *pMemOP;
	VO_S32 nIdx;
	voxPlayerGlobal *pGst;
  LOGE("voAudioSpeed_SetInputData enter\n");
	if(NULL == hCodec)
	{
    LOGE("voAudioSpeed_SetInputData leave, VO_ERR_INVALID_ARG , 1\n");
		return VO_ERR_INVALID_ARG;
	}
	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
    LOGE("voAudioSpeed_SetInputData leave, VO_ERR_INVALID_ARG , 2\n");
		return VO_ERR_INVALID_ARG;
	}

	
	pGst = (voxPlayerGlobal *)hCodec;
	pMemOP = pGst->pvoMemop;

	if (pGst->nBitsSample == 8)
	{
		if (pGst->pIn)
		{
			as_mem_free(pMemOP, pGst->pIn);
		}
		pGst->pIn = (VO_S16 *)as_mem_malloc(pMemOP, pInput->Length * 2, 32);
		for (nIdx = 0; nIdx < (VO_S32)(pInput->Length); nIdx++)
		{
			pGst->pIn[nIdx] = (VO_S16)((pInput->Buffer[nIdx] - 128)<< 8);
		}
        pGst->nInLens = pInput->Length / ( pGst->nChs);
	}else
	{
		pGst->pIn = (VO_S16 *)pInput->Buffer;
		pGst->nInLens = pInput->Length / ( pGst->nChs*sizeof(VO_S16));
	}
    
	pGst->nTemp = pGst->nInLens;

  LOGE("voAudioSpeed_SetInputData leave VO_ERR_NONE\n");
	return VO_ERR_NONE;
}

VO_U32 VO_API voAudioSpeed_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	VO_S32 nIdx;
	VO_S32 nTempLens;
	VO_S16 *pTemp;
	voxPlayerGlobal  *pGst;
	voxPlayerStrech  *pTsm;

  LOGE("voAudioSpeed_GetOutputData  enter\n");

	if (NULL == hCodec)
	{
    LOGE("voAudioSpeed_GetOutputData  leave, VO_ERR_INVALID_ARG\n");
		return VO_ERR_INVALID_ARG;
	}

	pGst = (voxPlayerGlobal *)hCodec;
	pTsm = (voxPlayerStrech *)pGst->pTsm;

	nTempLens = pGst->nTemp;

	if(!pTsm->pOutBuf->nSamplesInBuffer)
	{
	  LOGE("Chs = %d, Ra = %d\n", pGst->nChs, pGst->nSampleRate);
		if (pGst->nInLens > 0)
		{
			cmnMemCopy(VO_AUDIO_AudioSpeed, CheckBufferInsert(pGst->pvoMemop, pTsm->pInBuf, pGst->nInLens), pGst->pIn, pGst->nInLens * pGst->nChs * sizeof(VO_S16));
			pTsm->pInBuf->nSamplesInBuffer += pGst->nInLens;
			pGst->nInLens = 0;
		}  
	  if ((pTsm->tempo>0.99999)&&(pTsm->tempo<1.00001))
	  {
		  pTsm->pOutBuf->nSamplesInBuffer=pTsm->pInBuf->nSamplesInBuffer;
		  pTsm->pInBuf->nBufferPos=pTsm->pInBuf->nSamplesInBuffer;
		  pTsm->pInBuf->nSamplesInBuffer=0;
	  }
	  else
		tsmProcess(pTsm);
	}

	if (pTsm->pOutBuf->nSamplesInBuffer)
	{
        nTempLens= min(pTsm->pOutBuf->nSamplesInBuffer, nTempLens);

		if (pGst->nBitsSample == 8)
		{
			if ((pTsm->tempo>0.99999)&&(pTsm->tempo<1.00001))
			{
				pTemp = pTsm->pInBuf->pBuf;
			}
			else
				pTemp = voGetPtrBegin(pTsm->pOutBuf);
			
			for (nIdx = 0; nIdx < nTempLens  * pGst->nChs; nIdx++)
			{
				if (pTemp[nIdx] < 0)
				{
					pOutput->Buffer[nIdx] = (VO_U8)(((VO_U16)pTemp[nIdx]>>8) - 0x80);	
				}
				else
				{
                    pOutput->Buffer[nIdx] =(VO_U8)(0x80 + (pTemp[nIdx]>>8));		
				}
            }
			pOutput->Length = nTempLens * pGst->nChs;
		}else
		{
			if ((pTsm->tempo>0.99999)&&(pTsm->tempo<1.00001))
			{
				cmnMemCopy(VO_AUDIO_AudioSpeed, pOutput->Buffer, pTsm->pInBuf->pBuf, nTempLens * pGst->nChs * sizeof(VO_S16));
			}
			else
				cmnMemCopy(VO_AUDIO_AudioSpeed, pOutput->Buffer, voGetPtrBegin(pTsm->pOutBuf), nTempLens * pGst->nChs * sizeof(VO_S16));

			pOutput->Length = nTempLens * pGst->nChs * sizeof(VO_S16);
		}
		pTsm->pOutBuf->nSamplesInBuffer -= nTempLens;
		pTsm->pOutBuf->nBufferPos += nTempLens;
		pAudioFormat->Format.Channels = pGst->nChs;
		pAudioFormat->Format.SampleRate = pGst->nSampleRate;
		pAudioFormat->Format.SampleBits = pGst->nBitsSample;
	}else
	{
    LOGE("voAudioSpeed_GetOutputData  leave, VO_ERR_INPUT_BUFFER_SMALL\n");
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

#ifdef LCHECK 
	voCheckLibCheckAudio(pGst->hCheck, pOutput, &(pAudioFormat->Format));
#endif

  LOGE("voAudioSpeed_GetOutputData  leave, VO_ERR_NONE\n");
	return VO_ERR_NONE;
}

VO_U32 VO_API voAudioSpeed_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	voxPlayerGlobal *pGst;
	float  *fValue = (float*)pData;
	unsigned int  *iValue = (unsigned int*)pData;

  LOGE("voAudioSpeed_SetParam  enter\n");
	if (NULL == hCodec)
	{
    LOGE("voAudioSpeed_SetParam  leave, VO_ERR_INVALID_ARG\n");
		return VO_ERR_INVALID_ARG;
	}
	pGst = (voxPlayerGlobal *)hCodec;


  LOGE("uParamID = %x \n", uParamID);

	switch(uParamID)
	{
	case VO_PID_AUDIOSPEED_RATE:
		tsmSetTempo(pGst->pTsm, *fValue);
		tsmClear(pGst->pTsm);
		break;

	case VO_PID_AUDIO_CHANNELS:	
		pGst->nChs = (VO_S32)(*iValue);
        pGst->pTsm->nChs = (VO_S32)(*iValue);
		pGst->pTsm->pInBuf->nChannles = (VO_S32)(*iValue);
		pGst->pTsm->pOutBuf->nChannles = (VO_S32)(*iValue);
		tsmSetParameters(pGst->pTsm, 
			                        pGst->pTsm->sampleRate, 0, 0, 8);
		tsmClear(pGst->pTsm);
		break;

	case VO_PID_AUDIO_SAMPLEREATE:	
		pGst->nSampleRate = (VO_S32)(*iValue);
		pGst->pTsm->sampleRate = (VO_S32)(*iValue);

		tsmSetParameters(pGst->pTsm, 
			                        pGst->pTsm->sampleRate, 0, 0, 8);
		tsmClear(pGst->pTsm);
		break;
	case VO_PID_AUDIOSPEED_FLUSH:
		tsmClear(pGst->pTsm);
		break;

	case VO_PID_AUDIOSPEED_HIGHQUALITY:
		pGst->pTsm->bFastMode = 0;
		break;
	
	case VO_PID_AUDIOSPEED_BITS:
		pGst->nBitsSample = (VO_S32)(*iValue);
		break;
	default:
		 LOGE("voAudioSpeed_SetParam leave, VO_ERR_WRONG_PARAM_ID ");
		return VO_ERR_WRONG_PARAM_ID;
	}

  LOGE("voAudioSpeed_SetParam leave, VO_ERR_NONE ");
	return VO_ERR_NONE;	
}

VO_U32 VO_API voAudioSpeed_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	voxPlayerGlobal *pGst;
  LOGE("voAudioSpeed_GetParam enter");
	if(NULL == hCodec)
	{
    LOGE("voAudioSpeed_GetParam leave, VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
	}
	pGst = (voxPlayerGlobal *)hCodec;

  LOGE("voAudioSpeed_GetParam leave, VO_ERR_NONE");
	return VO_ERR_NONE;
}

VO_U32 VO_API voAudioSpeed_Uninit(VO_HANDLE hCodec)
{
	voxPlayerGlobal *pGst;
	VO_MEM_OPERATOR *pMemOP;
  LOGE("voAudioSpeed_Uninit enter");
	if (NULL == hCodec)
	{
    LOGE("voAudioSpeed_Uninit leave, VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
	}

	pGst = (voxPlayerGlobal *)hCodec;
	pMemOP = pGst->pvoMemop;

#ifdef LCHECK 
	voCheckLibUninit(pGst->hCheck);
#endif

	if (pGst->pTsm)
	{
		if (pGst->pTsm->pInBuf)
		{
			as_mem_free(pMemOP, pGst->pTsm->pInBuf);
            pGst->pTsm->pInBuf = NULL;
		}
		if (pGst->pTsm->pOutBuf)
		{
			as_mem_free(pMemOP, pGst->pTsm->pOutBuf);
			pGst->pTsm->pOutBuf = NULL;
		}
		if (pGst->pTsm->pMidBuffer)
		{
			as_mem_free(pMemOP, pGst->pTsm->pMidBuffer);
			pGst->pTsm->pMidBuffer = NULL;
		}
		if (pGst->pTsm->pRefMidBufTemp)
		{
			as_mem_free(pMemOP, pGst->pTsm->pRefMidBufTemp);
			pGst->pTsm->pRefMidBufTemp = NULL;
		}
		as_mem_free(pMemOP, pGst->pTsm);
		pGst->pTsm = NULL;
	}

	if (pGst->pIn && pGst->nBitsSample == 8)
	{
		as_mem_free(pMemOP, pGst->pIn);
		pGst->pIn = NULL;
	}
	as_mem_free(pMemOP, pGst);
	pGst = NULL;

  LOGE("voAudioSpeed_Uninit leave, VO_ERR_NONE");
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAudioSpeedAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
  LOGE("voGetAudioSpeedAPI enter");
  if(NULL == pDecHandle){
    LOGE("voGetAudioSpeedAPI leave, VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
  }
	pDecHandle->Init = voAudioSpeed_Init;
	pDecHandle->SetInputData = voAudioSpeed_SetInputData;
	pDecHandle->GetOutputData = voAudioSpeed_GetOutputData;
	pDecHandle->SetParam = voAudioSpeed_SetParam;
	pDecHandle->GetParam = voAudioSpeed_GetParam;
	pDecHandle->Uninit = voAudioSpeed_Uninit;

  LOGE("voGetAudioSpeedAPI leave, VO_ERR_NONE");
	return VO_ERR_NONE;
}

