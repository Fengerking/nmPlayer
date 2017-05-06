// voAACWriter.cpp
#include "CAACWriter.h"

//VO_U32 g_dwFRModuleID = 0;		// AUDIOFW, all in one

#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}
#endif // _WIN32


#if defined __cplusplus
extern "C" {
#endif

static VO_U32 VO_API Open(VO_PTR * ppHandle, VO_FILE_SOURCE* pSource, VO_SINK_OPENPARAM * pParam)
{
	CAACWriter *p = new CAACWriter((VO_MEM_OPERATOR*)pParam->pMemOP, (VO_FILE_OPERATOR*)pParam->pSinkOP);
	if(p == NULL) return VO_ERR_OUTOF_MEMORY;

	VO_U32 nRC = p->Open(pSource, pParam);
	if(nRC != VO_ERR_NONE)
		return nRC;
	
	*ppHandle = p;
	return VO_ERR_NONE;
}

static VO_U32 VO_API Close(VO_PTR p)
{
	if(p != NULL) {
		((CAACWriter *)p)->Close();
		delete (CAACWriter *)p;
	}
	return VO_ERR_NONE;
}

static VO_U32 VO_API AddSample(VO_PTR p, VO_SINK_SAMPLE * pSample)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CAACWriter *)p)->AddSample(pSample);
}

static VO_U32 VO_API  SetParam(VO_PTR p, VO_U32 uID, VO_PTR pParam)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CAACWriter *)p)->SetParam(uID , pParam);
}

static VO_U32 VO_API GetParam(VO_PTR p, VO_U32 uID, VO_PTR pParam)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CAACWriter *)p)->GetParam(uID , pParam);
}

VO_S32 VO_API voGetAACWriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag)
{
	pReadHandle->Open		= Open;
	pReadHandle->Close		= Close;
	pReadHandle->GetParam	= GetParam;
	pReadHandle->SetParam	= SetParam;
	pReadHandle->AddSample	= AddSample;

	return VO_ERR_NONE;
}


#if defined __cplusplus
}
#endif
