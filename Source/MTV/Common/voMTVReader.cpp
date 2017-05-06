
/*
Mobile TV local file reader
*/

//add for detect the memory leak

//add for detect the memory leak

#ifdef MTV_CMMB
#include "CCmmbReader.h"
#ifdef _IOS
	#define g_dwFRModuleID		VO_INDEX_SRC_CMMB
#else
	VO_U32 g_dwFRModuleID = VO_INDEX_SRC_CMMB;
#endif
#elif defined MTV_TS
#include "CTsReader.h"
#ifdef _IOS
	#define g_dwFRModuleID		VO_INDEX_SRC_DVBT
#else
	VO_U32 g_dwFRModuleID = VO_INDEX_SRC_DVBT;
#endif
#endif

#include "voLog.h"
#include "CDumper.h"

#if defined(_WIN32) && !defined(WINCE)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
char OutputStr[256];
#endif	//_WIN32

#ifndef _IOS
VO_HANDLE g_hInst = VO_NULL;
#endif // _IOS

#if defined __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <Windows.h>
	BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
	{   
		g_hInst = hModule;
		return TRUE;
	}
#endif	//_WIN32

	VO_U32 VO_API voMTVOpen(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam)
	{


#if defined(_WIN32) && !defined(WINCE)
_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
_CrtMemCheckpoint( &s1 );
_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif	//_WIN32


		if(VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE != (pParam->nFlag & 0xFF) && 
			(VO_SOURCE_OPENPARAM_FLAG_OPENPD != (pParam->nFlag & 0xFF))
			&& (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL != (pParam->nFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)))
		{
			VOLOGE("source open flags unvalid!!");

			return VO_ERR_INVALID_ARG;
		}

		if(VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR != (pParam->nFlag & 0xFF00))
		{
			VOLOGE("source operator flags unvalid!!");

			return VO_ERR_INVALID_ARG;
		}
        
		//Test memory leak detector
		//uint16*  pWord = new uint16;

		//Test memory leak detector
		CMTVReader* pReader = VO_NULL;

#ifdef MTV_CMMB
		pReader = new CCmmbReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#elif defined MTV_TS
		pReader = new CTsReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif

		if(!pReader)
			return VO_ERR_OUTOF_MEMORY;

		VO_U32 rc = pReader->Load(pParam->nFlag, (VO_FILE_SOURCE*)pParam->pSource);
		if(VO_ERR_SOURCE_OK != rc)
		{
			VOLOGE("file parser load fail: 0x%08X!!", rc);

			delete pReader;
			return rc;
		}

		*ppHandle = pReader;
		return VO_ERR_SOURCE_OK;
	}

	VO_U32 VO_API voMTVClose(VO_PTR pHandle)
	{
		CDumper::WriteLog((char *)"+ voMTVClose");

		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		VO_U32 rc = pReader->Close();
		delete pReader;

		CDumper::WriteLog((char *)"- voMTVClose");
#if defined(_WIN32) && !defined(WINCE)
		memset(OutputStr, 0, 256);
		_CrtMemCheckpoint( &s2 );
		if ( _CrtMemDifference( &s3, &s1, &s2) )
			_CrtMemDumpStatistics( &s3 );
		sprintf(OutputStr,"ThreadID: %d\n",GetCurrentThreadId());
		OutputDebugStringA(OutputStr);
#endif

		return rc;
	}

	VO_U32 VO_API voMTVGetFileInfo(VO_PTR pHandle, VO_SOURCE_INFO* pSourceInfo)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		return pReader->GetInfo(pSourceInfo);
	}

	VO_U32 VO_API voMTVGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
		if(!pTrack)
			return VO_ERR_INVALID_ARG;

		return pTrack->GetInfo(pTrackInfo);
	}

	VO_U32 VO_API voMTVGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

// 		CDumper::WriteLog("-------------------------------%d track---------------------", nTrack);
// 		CDumper::WriteLog("request sample by time stamp = %I64d\r", pSample->Time);

		CBaseReader* pReader = (CBaseReader*)pHandle;
		CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
		if(!pTrack)
		{
			VOLOGE("error track index: %d!!", nTrack);

			return VO_ERR_INVALID_ARG;
		}

		if(pTrack->IsEndOfStream())
		{
			CDumper::WriteLog((char *)"track is end!!");

			return VO_ERR_SOURCE_END;
		}

		// 20100610
		pSample->Flag = 0;

		VO_U32 rc = pTrack->GetSample(pSample);
		if(VO_ERR_SOURCE_OK != rc)
		{
			if(VO_ERR_SOURCE_END == rc)
				pTrack->SetEndOfStream(VO_TRUE);

			CDumper::WriteLog((char *)"get sample error: 0x%08X!!", rc);

			return rc;
		}


		VO_TRACKTYPE nTrackType = pTrack->GetType();
		if(VOTT_VIDEO == nTrackType)
			pReader->CheckVideo(pSample);
		else if(VOTT_AUDIO == nTrackType)
			pReader->CheckAudio(pSample);

// 		if(pTrack->IsVideo())
// 			pReader->CheckVideo(pSample);
// 		else
// 			pReader->CheckAudio(pSample);

//		CDumper::WriteLog("request sample by time stamp = %I64d ok, key frame = %s, size = %d", pSample->Time, pSample->Size&0xF0000000?"true":"false", pSample->Size & 0x7FFFFFFF);
//		CDumper::WriteLog("-----------------------------------------------------------\r\r");

		return VO_ERR_SOURCE_OK;
	}

	VO_U32 VO_API voMTVSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64* pPos)
	{
		//add by qichaoshen  @2011-10-19
		//the number of max rollback time
		int   iRollbackTimeMax = 5;
		int   iRollbackTimeIndex = 1;

		//the number of millisecond for rollback
		int   iTimeInterval = 500; 
		//add by qichaoshen  @2011-10-19

		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CDumper::WriteLog((char *)"++++++++++++++++++++++++++++++ SetPos = %llu ++++++++++++++++++++++++++++++", *pPos);
		CDumper::WriteLog((char *)"%d track want to setpos %d!!", nTrack, VO_S32(*pPos));

		VOLOGE("enter the voMTVSetPos!");
		CBaseReader* pReader = (CBaseReader*)pHandle;
		CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
		if(!pTrack)
		{
			//VOLOGE("error track index: %d!!", nTrack);

			return VO_ERR_INVALID_ARG;
		}

		if(VO_SOURCE_PM_PLAY == pTrack->GetPlayMode())
		{
			if(0xffffffff == pReader->GetSeekTrack())	//not initialize!!
			{
				//CDumper::WriteLog("set seek track: %d!!", nTrack);

				pReader->SetSeekTrack(nTrack);
			}

			if(pReader->GetSeekTrack() == nTrack)	//the track will activate file set position
			{
				//CDumper::WriteLog("%d track activate moveto %d!!", nTrack, VO_S32(*pPos));

				pReader->MoveTo(*pPos);
			}
		}

		pTrack->SetEndOfStream(VO_FALSE);

		VO_U32 rc = pTrack->SetPos(pPos);

		//add by qichaoshen  @2011-10-19
		if(VO_SOURCE_PM_PLAY == pTrack->GetPlayMode())
		{
		    while((iRollbackTimeIndex < iRollbackTimeMax) && 
				  (VO_ERR_SOURCE_END == rc))
			{
				if((*pPos) > iTimeInterval)
				{
                    pReader->MoveTo((*pPos) - iTimeInterval);
					rc = pTrack->SetPos(pPos);
				}
				else
				{
					break;
				}
				iTimeInterval = iTimeInterval << 1;
			}
		}
		//add by qichaoshen  @2011-10-19

		if(VO_ERR_SOURCE_END == rc)
			pTrack->SetEndOfStream(VO_TRUE);

		CDumper::WriteLog((char *)"setpos %d return 0x%08X!!", VO_S32(*pPos), rc);
		CDumper::WriteLog((char *)"++++++++++++++++++++++++++++++ SetPos End = %I64d ++++++++++++++++++++++++++++++", *pPos);
		VOLOGE("leave the voMTVSetPos!");
		return rc;
	}

	VO_U32 VO_API voMTVSetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		return pReader->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voMTVGetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		return pReader->GetParameter(uID, pParam);
	}

	VO_U32 VO_API voMTVSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
		if(!pTrack)
			return VO_ERR_INVALID_ARG;

		return pTrack->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voMTVGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_INVALID_ARG;

		CBaseReader* pReader = (CBaseReader*)pHandle;
		CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
		if(!pTrack)
			return VO_ERR_INVALID_ARG;

		return pTrack->GetParameter(uID, pParam);
	}



VO_S32 VO_API voGetMTVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
	{
		pReadHandle->Open = voMTVOpen;
		pReadHandle->Close = voMTVClose;
		pReadHandle->GetSourceInfo = voMTVGetFileInfo;
		pReadHandle->GetTrackInfo = voMTVGetTrackInfo;
		pReadHandle->GetSample = voMTVGetSample;
		pReadHandle->SetPos = voMTVSetPos;
		pReadHandle->SetSourceParam = voMTVSetFileParam;
		pReadHandle->GetSourceParam = voMTVGetFileParam;
		pReadHandle->SetTrackParam = voMTVSetTrackParam;
		pReadHandle->GetTrackParam = voMTVGetTrackParam;

		return VO_ERR_SOURCE_OK;
	}
#if defined __cplusplus
}
#endif

