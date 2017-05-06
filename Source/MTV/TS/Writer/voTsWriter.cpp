#include "voSink.h"
#include "CTsWriter.h"

#ifdef _IOS
#define g_dwFRModuleID	VO_INDEX_SNK_TS
#else
VO_U32 g_dwFRModuleID = VO_INDEX_SNK_TS;
#endif
VO_FILE_OPERATOR *	gpFileOperator = NULL;

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}
#endif // _WIN32

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#if defined __cplusplus
extern "C" {
#endif
#endif ///<_VONAMESPACE

VO_U32 VO_API Open(VO_PTR * ppHandle, VO_FILE_SOURCE* pFileSource, VO_SINK_OPENPARAM * pParam)
{
	if(pParam->nFlag == 1)
	{
		gpFileOperator = (VO_FILE_OPERATOR *)pParam->pSinkOP;
	}


	CTsWriter *pWriter = new CTsWriter();
	if(pWriter == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	VO_U32 nRC = pWriter->Init(pParam->strWorkPath);
	if (nRC != VO_ERR_SOURCE_OK)
	{
		pWriter->Uninit();
		delete pWriter;
		return nRC;
	}
	nRC = pWriter->Open(pFileSource , pParam);
	if(nRC != VO_ERR_NONE)
		return nRC;

	*ppHandle = pWriter;
	return VO_ERR_NONE;
}

VO_U32 VO_API Close(VO_PTR pHandle)
{
	if(pHandle != NULL)
	{
		CTsWriter *pWriter = (CTsWriter *)pHandle;
		pWriter->Uninit();
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

	CTsWriter *pWriter = (CTsWriter *)pHandle;
	return pWriter->AddSample(pSample);

}


VO_U32 VO_API  SetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(pHandle == NULL)
		return VO_ERR_FAILED;

	CTsWriter *pWriter = (CTsWriter *)pHandle;
	return pWriter->SetParam(uID , pParam);
}


VO_U32 VO_API GetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(pHandle == NULL)
		return VO_ERR_FAILED;

	CTsWriter *pWriter = (CTsWriter *)pHandle;
	return pWriter->GetParam(uID , pParam);
}
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VO_S32 VO_API voGetTSWriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag)
{
	pReadHandle->Open		= Open;
	pReadHandle->Close		= Close;
	pReadHandle->AddSample	= AddSample;
	pReadHandle->GetParam	= GetParam;
	pReadHandle->SetParam	= SetParam;

	return VO_ERR_NONE;
}
#ifndef _VONAMESPACE
#if defined __cplusplus
}
#endif
#endif