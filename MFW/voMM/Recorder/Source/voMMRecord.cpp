// voMMRecord.cpp : Defines the exported functions for the DLL application.
//


#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "voMMRecord.h"
#include "CRecorderEngine.h"

#define CHECK_RECORD_POINT if (hRec == NULL)\
	return VO_ERR_INVALID_ARG;\
	CRecorderEngine * pRec = (CRecorderEngine *)hRec;

VO_PTR	g_hvommRecordInst = NULL;

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hvommRecordInst = hModule;
	return TRUE;
}
#endif // _WIN32


VO_U32 vommrInit (VO_HANDLE * phRec)
{
	CRecorderEngine* pEngine = new CRecorderEngine;

	*(CRecorderEngine**)phRec = pEngine;

	pEngine->Open();

	return VO_ERR_NONE;
}

VO_U32 vommrUninit (VO_HANDLE hRec)
{
	CHECK_RECORD_POINT

	pRec->Close();

	delete pRec;

	return VO_ERR_NONE;
}

VO_U32 vommrSetNotifyCallback (VO_HANDLE hRec, VOMMRecordCallBack pCallBack, VO_PTR pUserData)
{
	CHECK_RECORD_POINT

	pRec->SetNotifyCallback(pCallBack);

	return VO_ERR_NONE;
}

VO_U32 vommrSetFileName (VO_HANDLE hRec, VO_TCHAR* pszFileName)
{
	CHECK_RECORD_POINT
	
	pRec->SetFileName(pszFileName);

	return VO_ERR_NONE;
}

VO_U32 vommrSendAudioSrc (VO_HANDLE hRec, VO_MMR_ASBUFFER* pBuffer)
{
	CHECK_RECORD_POINT

	pRec->RecvAudioSrc(pBuffer);

	return VO_ERR_NONE;
}

VO_U32 vommrSendVideoSrc (VO_HANDLE hRec, VO_MMR_VSBUFFER* pBuffer)
{
	CHECK_RECORD_POINT

	pRec->RecvVideoSrc(pBuffer);

	return VO_ERR_NONE;
}

VO_U32 vommrStart (VO_HANDLE hRec)
{
	CHECK_RECORD_POINT

	pRec->StartRecord();

	return VO_ERR_NONE;
}

VO_U32 vommrStop (VO_HANDLE hRec)
{
	CHECK_RECORD_POINT

	pRec->StopRecord();

	return VO_ERR_NONE;
}

VO_U32 vommrSetParam (VO_HANDLE hRec, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RECORD_POINT

	pRec->SetParameter(nID, pValue);

	return VO_ERR_NONE;
}

VO_U32 vommrGetParam (VO_HANDLE hRec, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RECORD_POINT

	pRec->GetParameter(nID, pValue);

	return VO_ERR_NONE;
}

VO_S32 vommGetRecordAPI (VOMM_RECORDAPI * pRecord, VO_U32 uFlag)
{
	if (NULL == pRecord)
	{
		return VO_ERR_INVALID_ARG;
	}

	pRecord->Init				= vommrInit;
	pRecord->Uninit				= vommrUninit;
	pRecord->SetFileName		= vommrSetFileName;
	pRecord->SetNotifyCallback	= vommrSetNotifyCallback;
	pRecord->SendAudioSrc		= vommrSendAudioSrc;
	pRecord->SendVideoSrc		= vommrSendVideoSrc;
	pRecord->Start				= vommrStart;
	pRecord->Stop				= vommrStop;
	pRecord->SetParam			= vommrSetParam;
	pRecord->GetParam			= vommrGetParam;

	return VO_ERR_NONE;
}