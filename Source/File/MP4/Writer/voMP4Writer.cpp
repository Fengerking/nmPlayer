#include "voSink.h"
#include "CMP4Writer.h"
#include "voFile.h"
#include "voLog.h"

//VO_U32 g_dwFRModuleID = VO_INDEX_SNK_MP4;
VO_FILE_OPERATOR *	gpFileOperator = NULL;

#ifndef _LIB
#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}
#endif // _WIN32
#endif 

#if defined __cplusplus
extern "C" {
#endif

VO_U32 VO_API Open(VO_PTR * ppHandle, VO_FILE_SOURCE* pFileSource, VO_SINK_OPENPARAM * pParam)
{
	if (NULL != pParam)
	{
		VOLOGINIT(pParam->strWorkPath);
	}
	if(pParam->nFlag == 1)
	{
		gpFileOperator = (VO_FILE_OPERATOR *)pParam->pSinkOP;
	}


	CMP4Writer *pWriter = new CMP4Writer();
	if(pWriter == NULL)
	{
		VOLOGUNINIT();
		return VO_ERR_OUTOF_MEMORY;
	}

	VO_U32 nRC = pWriter->Open(pFileSource , pParam);
	if(nRC != VO_ERR_NONE)
	{
		VOLOGUNINIT();
		return nRC;
	}
	*ppHandle = pWriter;
	VOLOGUNINIT();
	return VO_ERR_NONE;
}

VO_U32 VO_API Close(VO_PTR pHandle)
{
	if(pHandle != NULL)
	{
		CMP4Writer *pWriter = (CMP4Writer *)pHandle;
		pWriter->Close();
		delete pWriter;
		pWriter = NULL;
	}

	return VO_ERR_NONE;
}


VO_U32 VO_API AddSample(VO_PTR pHandle, VO_SINK_SAMPLE * pSample)
{
	if(pHandle == NULL)
		return VO_ERR_FAILED;

	CMP4Writer *pWriter = (CMP4Writer *)pHandle;
	return pWriter->AddSample(pSample);

}


VO_U32 VO_API  SetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(pHandle == NULL)
		return VO_ERR_FAILED;

	CMP4Writer *pWriter = (CMP4Writer *)pHandle;
	return pWriter->SetParam(uID , pParam);
}


VO_U32 VO_API GetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(pHandle == NULL)
		return VO_ERR_FAILED;

	CMP4Writer *pWriter = (CMP4Writer *)pHandle;
	return pWriter->GetParam(uID , pParam);
}

VO_S32 VO_API voGetMP4WriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag)
{
	pReadHandle->Open		= Open;
	pReadHandle->Close		= Close;
	pReadHandle->AddSample	= AddSample;
	pReadHandle->GetParam	= GetParam;
	pReadHandle->SetParam	= SetParam;

	return VO_ERR_NONE;
}

#if defined __cplusplus
}
#endif