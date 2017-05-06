#include "CQCPWriter.h"

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

// OpenFile (const TCHAR * pFileName);
static VO_U32 VO_API Open(VO_PTR * ppHandle, VO_FILE_SOURCE* pSource, VO_SINK_OPENPARAM * pParam)
{
	CQCPWriter *p = new CQCPWriter((VO_MEM_OPERATOR*)pParam->pMemOP, (VO_FILE_OPERATOR*)pParam->pSinkOP);
	if(p == NULL) return VO_ERR_OUTOF_MEMORY;

	VO_U32 nRC = p->Open(pSource, pParam);
	if(nRC != VO_ERR_NONE)
		return nRC;
	
	*ppHandle = p;
	return VO_ERR_NONE;
}

// CloseFile (void);
static VO_U32 VO_API Close(VO_PTR p)
{
	if(p != NULL) {
		((CQCPWriter *)p)->Close();
		delete (CQCPWriter *)p;
	}
	return VO_ERR_NONE;
}


// AddAudioSample (IMediaSample * pSample);
static VO_U32 VO_API AddSample(VO_PTR p, VO_SINK_SAMPLE * pSample)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CQCPWriter *)p)->AddSample(pSample);
}


// set (VO_AUDIO_FORMAT*)pParam->{SampleRate, Channels, SampleBits}
static VO_U32 VO_API  SetParam(VO_PTR p, VO_U32 uID, VO_PTR pParam)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CQCPWriter *)p)->SetParam(uID , pParam);
}


// GetFileSize (void);
// GetFrameSize(BYTE nValue);
static VO_U32 VO_API GetParam(VO_PTR p, VO_U32 uID, VO_PTR pParam)
{
	if(p == NULL) return VO_ERR_FAILED;

	return ((CQCPWriter *)p)->GetParam(uID , pParam);
}

VO_S32 VO_API voGetQCPWriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag)
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
