#include "voAudioMCDec.h"
#include "AMCWrap.h"

#define SF_CHECK_HANDLE if (hDec == NULL)\
	return VO_ERR_INVALID_ARG;\
AMCWrap * pWrap = (AMCWrap *)hDec;


VO_U32 aDecInit(VO_HANDLE * phDec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	
	AMCWrap * pWrap = new AMCWrap (vType, pUserData);
	if (pWrap == NULL)
		return VO_ERR_OUTOF_MEMORY;

	*phDec = pWrap;
	
	return VO_ERR_NONE;
}

VO_U32  aSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	SF_CHECK_HANDLE
	// filter out the header data with input method
	return pWrap->setInputData (pInput);
}

VO_U32 aGetOutputData(VO_HANDLE hDec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	SF_CHECK_HANDLE
		
	return pWrap->getOutputData (pOutBuffer, pOutInfo);
}

VO_U32 aDecUninit(VO_HANDLE hDec)
{
	SF_CHECK_HANDLE

	delete pWrap;
	pWrap = NULL;
	return VO_ERR_NONE;
}

VO_U32 aSetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	SF_CHECK_HANDLE

	return pWrap->setParam (uParamID, pData);
}

VO_U32 aGetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	SF_CHECK_HANDLE

	return pWrap->getParam (uParamID, pData);
}

VO_S32 VO_API voGetAudioMCDecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	pDecHandle->Init = aDecInit;
	pDecHandle->SetInputData = aSetInputData;
	pDecHandle->GetOutputData = aGetOutputData;
	pDecHandle->Uninit = aDecUninit;

	pDecHandle->SetParam = aSetParam;
	pDecHandle->GetParam = aGetParam;

	return VO_ERR_NONE;
}
