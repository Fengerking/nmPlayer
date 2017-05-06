#include "voWMSP2.h"
#include "WMSP2Manager.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif


VO_U32 voWMSP2Init(VO_HANDLE* phHandle, VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM* pParam)
{
	CWMSP2Manager *pManager = new CWMSP2Manager();
	if (!pManager)
		return VO_ERR_OUTOF_MEMORY;

	if( pManager->Init(pSource, nFlag, pParam) )
	{
		delete pManager;
		*phHandle = 0;
		return VO_ERR_SOURCE_OPENFAIL;
	}

	*phHandle = pManager;

	return VO_ERR_SOURCE_OK;
}

VO_U32 voWMSP2Uninit(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (NULL == pManager) {
		VOLOGE("Out of memory");
		return VO_RET_SOURCE2_FAIL;
	}

	VO_U32 uiRet = pManager->Uninit();
	delete pManager;

	return uiRet;
}

VO_U32 voWMSP2Open(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;

	return pManager->Open();
}

VO_U32 voWMSP2Close(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->Close();
}

VO_U32 voWMSP2Start(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->Start();
}

VO_U32 voWMSP2Pause(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->Pause();
}

VO_U32 voWMSP2Stop(VO_HANDLE hHandle)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->Stop();
}

VO_U32 voWMSP2Seek(VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->Seek(pTimeStamp);
}

VO_U32 voWMSP2GetDuration(VO_PTR hHandle, VO_U64* pDuration)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetDuration(pDuration);
}

VO_U32 voWMSP2GetSample(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PTR pSample)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetSample(nOutPutType, pSample);
}

VO_U32 voWMSP2GetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetProgramCount(pProgramCount);
}

VO_U32 voWMSP2GetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetProgramInfo(nProgram, pProgramInfo);
}

VO_U32 voWMSP2GetCurTrackInfo(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType, VO_SOURCE2_TRACK_INFO** ppTrackInfo)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetCurTrackInfo(nTrackType, ppTrackInfo);
}

VO_U32 voWMSP2SelectProgram(VO_HANDLE hHandle, VO_U32 nProgram)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->SelectProgram(nProgram);
}

VO_U32 voWMSP2SelectStream(VO_HANDLE hHandle, VO_U32 nStream)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->SelectStream(nStream);
}

VO_U32 voWMSP2SelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->SelectTrack(nTrack);
}

VO_U32 voWMSP2GetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetDRMInfo(ppDRMInfo);
}

VO_U32 voWMSP2SendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->SendBuffer(buffer);
}

VO_U32 voWMSP2GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	if (pManager->HaveLicense() == VO_FALSE)
		return VO_RET_SOURCE2_FAIL;

	return pManager->GetParam(nParamID, pParam);
}

VO_U32 voWMSP2SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	if(!hHandle)
		return VO_ERR_BASE;

	CWMSP2Manager * pManager = (CWMSP2Manager*)hHandle;
	return pManager->SetParam(nParamID, pParam);
}

VO_S32 VO_API voGetWMSP2ReadAPI (VO_SOURCE2_API * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Init				= voWMSP2Init;
		pReadHandle->Uninit				= voWMSP2Uninit;
		pReadHandle->Open				= voWMSP2Open;
		pReadHandle->Close				= voWMSP2Close;
		pReadHandle->Start				= voWMSP2Start;
		pReadHandle->Pause				= voWMSP2Pause;
		pReadHandle->Stop				= voWMSP2Stop;
		pReadHandle->Seek				= voWMSP2Seek;
		pReadHandle->GetDuration		= voWMSP2GetDuration;
		pReadHandle->GetSample			= voWMSP2GetSample;
		pReadHandle->GetProgramCount	= voWMSP2GetProgramCount;
		pReadHandle->GetProgramInfo		= voWMSP2GetProgramInfo;
		pReadHandle->GetCurTrackInfo	= voWMSP2GetCurTrackInfo;
		pReadHandle->SelectProgram		= voWMSP2SelectProgram;
		pReadHandle->SelectStream		= voWMSP2SelectStream;
		pReadHandle->SelectTrack		= voWMSP2SelectTrack;
		pReadHandle->GetDRMInfo			= voWMSP2GetDRMInfo;
		pReadHandle->SendBuffer			= voWMSP2SendBuffer;
		pReadHandle->GetParam			= voWMSP2GetParam;
		pReadHandle->SetParam			= voWMSP2SetParam;
	}

	return VO_ERR_SOURCE_OK;
}

#ifndef _VONAMESPACE
#if defined __cplusplus
}
#endif
#endif