
#include "VORA_sdk.h"
//#include "cmnMemory.h"
#include "voChHdle.h"
#include "voCheck.h"


#if defined (LCHECK)
VO_PTR	g_hRADecInst = NULL;
#endif

void* rm_memory_malloc(VO_MEM_OPERATOR *pMemOP, void* pUserMem, unsigned int ulSize)
{
	int ret;
	unsigned char *mem_ptr = NULL;
	VO_MEM_INFO MemInfo;

	MemInfo.Flag = 0;
	MemInfo.Size = ulSize;
	ret = pMemOP->Alloc(VO_INDEX_DEC_RA, &MemInfo);
	if(ret != 0)
		return 0;
	mem_ptr = (unsigned char *)MemInfo.VBuffer;

	pMemOP->Set(VO_INDEX_DEC_RA, mem_ptr, 0, ulSize);
	return mem_ptr;
}

UINT32 rm_memory_free(VO_MEM_OPERATOR *pMemOP, void* pUserMem, void* ptr)
{
	return pMemOP->Free(VO_INDEX_DEC_RA, ptr);
}

VO_U32 VO_API voRealAudioDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	VO_U32 nRet = 0;
#ifdef LCHECK
    VO_PTR  pTemp;
#endif
	VORA_SDK* pDec = NULL;
	VO_MEM_OPERATOR *pMemOP;
	//int interMem;

	//interMem = 0;

	if(pUserData == NULL || (pUserData->memflag&0x0f) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
//#ifndef _SYMBIAN_
//		cmnMemFillPointer(VO_INDEX_DEC_RA);
//
//		interMem = 1;
//
//		pMemOP = &g_memOP;
//#else
		return VO_ERR_INVALID_ARG;
//#endif
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}
#if defined (LCHECK)
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_RA, pUserData->memflag|1, g_hRADecInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_RA, pUserData->memflag|1, g_hRADecInst, NULL);

	//nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_RA, 1, g_hRADecInst, pDec->pLibOperator);
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

	pDec = (VORA_SDK*)rm_memory_malloc(pMemOP, NULL, sizeof(VORA_SDK));
	if(!pDec)
		return VO_ERR_OUTOF_MEMORY;

	pDec->pvoMemop = pMemOP;
	if(pUserData->memflag&0xf0)
		pDec->pLibOperator = pUserData->libOperator;
	else
		pDec->pLibOperator = NULL;

#ifdef LCHECK
    pDec->hCheck = pTemp;
#endif

	*phCodec = pDec;
	return VO_ERR_NONE;
}


VO_U32 VO_API voRealAudioDecUninit(VO_HANDLE hCodec)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	VO_MEM_OPERATOR *pMemOP;
	if(pDec) {
		if(pDec->RealDecAPI.Uninit)
			pDec->RealDecAPI.Uninit(hCodec);
	}
	pMemOP = pDec->pvoMemop;

#if defined (LCHECK)
	voCheckLibUninit(pDec->hCheck);
#endif
	rm_memory_free(pMemOP, NULL, pDec);

	return VO_ERR_NONE;
}

VO_U32 VO_API voRealAudioDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	if(pDec == NULL) {
		return VO_ERR_WRONG_STATUS;
	}

	if(pInput == NULL) {
		return VO_ERR_INVALID_ARG;
	}
	
	pDec->inbuf = pInput->Buffer;
	pDec->inlength = pInput->Length;

	return VO_ERR_NONE;	
}

VO_U32 VO_API voRealAudioDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	int ret;
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;
	if(pDec->RealDecAPI.GetOutputData == NULL)
		return VO_ERR_WRONG_STATUS;

	if(pOutBuffer == NULL || pOutBuffer->Buffer == NULL || pOutBuffer->Length <= 0)
		return VO_ERR_INVALID_ARG;

	ret = pDec->RealDecAPI.GetOutputData(hCodec, pOutBuffer, pOutInfo);

#if defined (LCHECK)
	voCheckLibCheckAudio(pDec->hCheck, pOutBuffer, &(pOutInfo->Format));
#endif

	return ret;
}

VO_U32 VO_API voRealAudioDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int ret;
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	VO_MEM_OPERATOR *pMemOP;
	if(pDec==NULL)
		return VO_ERR_WRONG_STATUS;

	if(pData == NULL)
		return VO_ERR_INVALID_ARG;

	pMemOP = pDec->pvoMemop;

	ret = VO_ERR_NONE; 
	switch(uParamID)
	{
	case VOID_PID_RA_FMT_INIT:
		{
			VO_CODECBUFFER* pDataBuffer = (VO_CODECBUFFER*)pData;
			VORA_INIT_PARAM* params	= (VORA_INIT_PARAM*)pDataBuffer->Buffer;
			pDec->rmParam = params;
			ret = voRealAudioRMFmtDecInit(&hCodec, VO_AUDIO_CodingRA, NULL);
		}
		break;
	case VOID_PID_RA_RAW_INIT:
		{

			VORA_RAW_INIT_PARAM* param = (VORA_RAW_INIT_PARAM*)pData;
			if(pData==NULL)
				return -4;
			pMemOP->Copy(VO_INDEX_DEC_RA, &(pDec->rawParam),param,sizeof(VORA_RAW_INIT_PARAM));
			
			if (param->version == RA8_LBR)
			{
				ret = voRealAudioRAWDecInit(&hCodec, VO_AUDIO_CodingRA, NULL);
			}
			else
			{
				return VO_ERR_NOT_IMPLEMENT;
			}			
		}
		break;
	case VO_PID_COMMON_HEADDATA:
	case VOID_PID_RA_BUF_INIT:
		{
			VO_CODECBUFFER *pInput = (VO_CODECBUFFER *)pData;
			pDec->bufPara = pInput->Buffer;
			pDec->inlength = pInput->Length;
			ret = voRealAudioRMBufDecInit(&hCodec, VO_AUDIO_CodingRA, NULL);
		}
		break;
	default:
		if(pDec->RealDecAPI.SetParam == NULL)
			return VO_ERR_WRONG_STATUS;
		return pDec->RealDecAPI.SetParam(pDec, uParamID, pData);
	}
	return ret;	
}

VO_U32 VO_API voRealAudioDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VORA_SDK* pDec = (VORA_SDK*)hCodec;
	if(pDec == NULL)
		return VO_ERR_WRONG_STATUS;
	if(pDec->RealDecAPI.GetParam == NULL)
		return VO_ERR_WRONG_STATUS;
	return pDec->RealDecAPI.GetParam(hCodec, uParamID, pData);
}

VO_EXPORT_FUNC VO_S32 VO_API YYGetRADecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voRealAudioDecInit;
	pDecHandle->SetInputData = voRealAudioDecSetInputData;
	pDecHandle->GetOutputData = voRealAudioDecGetOutputData;
	pDecHandle->SetParam = voRealAudioDecSetParameter;
	pDecHandle->GetParam = voRealAudioDecGetParameter;
	pDecHandle->Uninit = voRealAudioDecUninit;

	return VO_ERR_NONE;
}
