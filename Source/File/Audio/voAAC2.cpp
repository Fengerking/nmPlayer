	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSource2AAC.cpp

	Contains:	Realize AAC Source2 Interface

	Written by:	Aiven

	Change History (most recent first):
	2012-04-11		Aiven			Create file

*******************************************************************************/

#include "voAAC2.h"
#include "CSource2AAC.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if 0
#define CSource2AAC CSource2AAC
	
	
#define SOUORCE_OBJ ((SOURCE_CLASS*)phHandle)
	
#ifdef _IOS
    #ifdef _ISS_SOURCE_
 	#define MODULENAME ISS
 	#elif defined _HLS_SOURCE_
 	#define MODULENAME HLS
 	#endif
#else
#define MODULENAME 
#endif
	
#define MODULEVOSRCINIT2(name) vosrcInit2##name
#define vosrcInit2(name) MODULEVOSRCINIT2(name)
	
#define MODULEVOSRCUNINIT2(name) vosrcUninit2##name
#define vosrcUninit2(name) MODULEVOSRCUNINIT2(name)
	
#define MODULEVOSRCOPEN2(name) vosrcOpen2##name
#define vosrcOpen2(name) MODULEVOSRCOPEN2(name)
	
#define MODULEVOSRCCLOSE2(name) vosrcClose2##name
#define vosrcClose2(name) MODULEVOSRCCLOSE2(name)
	
#define MODULEVOSRCSTART2(name) vosrcStart2##name
#define vosrcStart2(name) MODULEVOSRCSTART2(name)
	
#define MODULEVOSRCPAUSE2(name) vosrcPause2##name
#define vosrcPause2(name) MODULEVOSRCPAUSE2(name)
	
#define MODULEVOSRCSTOP2(name) vosrcStop2##name
#define vosrcStop2(name) MODULEVOSRCSTOP2(name)
	
#define MODULEVOSRCSEEK2(name) vosrcSeek2##name
#define vosrcSeek2(name) MODULEVOSRCSEEK2(name)
	
#define MODULEVOSRCGETDURATION2(name) vosrcGetDuration2##name
#define vosrcGetDuration2(name) MODULEVOSRCGETDURATION2(name)
	
#define MODULEVOSRCGETSAMPLE2(name) vosrcGetSample2##name
#define vosrcGetSample2(name) MODULEVOSRCGETSAMPLE2(name)
	
#define MODULEVOSRCGETSTREAMCOUNT2(name) vosrcGetStreamCount2##name
#define vosrcGetStreamCount2(name) MODULEVOSRCGETSTREAMCOUNT2(name)
	
#define MODULEVOSRCGETSTREAMINFO2(name) vosrcGetStreamInfo2##name
#define vosrcGetStreamInfo2(name) MODULEVOSRCGETSTREAMINFO2(name)
	
#define MODULEVOSRCGETCURSELTRACKINFO2(name) vosrcGetCurSelTrackInfo2##name
#define vosrcGetCurSelTrackInfo2(name) MODULEVOSRCGETCURSELTRACKINFO2(name)
	
#define MODULEVOSRCSELECTTRACK2(name) vosrcSelectTrack2##name
#define vosrcSelectTrack2(name) MODULEVOSRCSELECTTRACK2(name)
	
#define MODULEVOSRCGETDRMINFO2(name) vosrcGetDRMInfo2##name
#define vosrcGetDRMInfo2(name) MODULEVOSRCGETDRMINFO2(name)
	
#define MODULEVOSRCSENDBUFFER2(name) vosrcSendBuffer2##name
#define vosrcSendBuffer2(name) MODULEVOSRCSENDBUFFER2(name)
	
#define MODULEVOSRCGETPARAM2(name) vosrcGetParam2##name
#define vosrcGetParam2(name) MODULEVOSRCGETPARAM2(name)
	
#define MODULEVOSRCSETPARAM2(name) vosrcSetParam2##name
#define vosrcSetParam2(name) MODULEVOSRCSETPARAM2(name)
#endif

#if defined __cplusplus
	extern "C" {
#endif
	
#ifdef _SELFTEST
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
		_CrtMemState s1;
		_CrtMemState s2;
		_CrtMemState s3;
#endif

	
	VO_U32 vosrcInit2(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pCallback )
	{
		VO_U32	 ulRet = VO_RET_SOURCE2_OK;

#ifdef _SELFTEST
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF );
		_CrtMemCheckpoint( &s1 );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
		//	_CrtSetBreakAlloc(78);
#endif

		CSource2AAC * pSrc = new CSource2AAC();
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		VO_SOURCE2_SAMPLECALLBACK * pTmp = NULL;
		if(pCallback != NULL)
		{
			pTmp = (VO_SOURCE2_SAMPLECALLBACK *)pCallback->pInitParam;
		}
		VO_TCHAR * pWorkingPath = (pCallback != NULL) ? pCallback->strWorkPath : NULL; 
		ulRet = pSrc->Init(pSource, nFlag, pTmp, pWorkingPath);
		if (VO_ERR_SOURCE_OK != ulRet)
		{
			pSrc->Uninit();
			delete pSrc;
			return ulRet;
		}
		*phHandle = (VO_HANDLE)pSrc;
		return ulRet;
	}
	
	
	VO_U32 vosrcUninit2(VO_HANDLE hHandle)
	{
		VO_U32	 ulRet = VO_RET_SOURCE2_OK;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		VOLOGI( "vosrcUninit2--1");
		ulRet = pSrc->Uninit();
		
		VOLOGI( "vosrcUninit2--2");
		delete	pSrc;
		
#ifdef _SELFTEST
		_CrtMemCheckpoint( &s2 );
		if ( _CrtMemDifference( &s3, &s1, &s2) )
			_CrtMemDumpStatistics( &s3 );
		_CrtDumpMemoryLeaks();	
#endif	
		VOLOGI( "vosrcUninit2--3");
		return ulRet;
	}
	
	
	VO_U32 vosrcOpen2(VO_HANDLE hHandle)
	{	
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Open();
		return ulRet;
	}
	
	VO_U32 vosrcClose2(VO_HANDLE hHandle)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Close();
		return ulRet;
	}
	
	
	VO_U32 vosrcStart2(VO_HANDLE hHandle)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Start();
		return ulRet;
	}
	
	VO_U32 vosrcPause2(VO_HANDLE hHandle)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Pause();
		return ulRet;
	}
	
	VO_U32 vosrcStop2(VO_HANDLE hHandle)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Stop();
		return ulRet;
	}
	
	VO_U32 vosrcSeek2(VO_HANDLE hHandle, VO_U64* pTimeStamp)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->Seek(pTimeStamp);
		return ulRet;
	}
	
	VO_U32 vosrcGetDuration2(VO_HANDLE hHandle,VO_U64 * pDuration)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->GetDuration(pDuration);
		return ulRet;
	}
	
	
	VO_U32 vosrcGetSample2(VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		
		ulRet = pSrc->GetSample(nOutPutType, pSample);
		return ulRet;
	}
	
	VO_U32 vosrcGetStreamCount2(VO_HANDLE hHandle, VO_U32 *pStreamCount)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
		ulRet = pSrc->GetStreamCount(pStreamCount);
		return ulRet;
	}
	
	VO_U32 vosrcGetStreamInfo2(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
//		ulRet = pSrc->GetStreamInfo(nStream, ppStreamInfo);
		return ulRet;
	}
	
	VO_U32 vosrcGetCurSelTrackInfo2(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
//		ulRet = pSrc->GetCurSelTrackInfo(nOutPutType, ppTrackInfo);
		return ulRet;
	}
	
	VO_U32 vosrcSelectProgram2 (VO_HANDLE hHandle, VO_U32 nProgram)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VO_U32 vosrcSelectStream2 (VO_HANDLE hHandle, VO_U32 nProgram)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	VO_U32 vosrcSelectTrack2(VO_HANDLE hHandle, VO_U32 nTrack)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
//		ulRet = pSrc->SelectTrack(nStreamID, nSubStreamID, nTrackID);
		return ulRet;
	}
	
	
	VO_U32	vosrcGetDRMInfo2(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
		ulRet = pSrc->GetDRMInfo(ppDRMInfo);
		return ulRet;
	}
	
	
	VO_U32	vosrcSendBuffer2(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
		ulRet = pSrc->SendBuffer(buffer);
		return ulRet;
	}
	
	
	VO_U32 vosrcGetParam2(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
		ulRet = pSrc->GetParam(nParamID, pParam);
		return ulRet;
	}
	
	VO_U32 vosrcSetParam2(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		VO_U32	 ulRet = 0;
		CSource2AAC * pSrc = (CSource2AAC *)hHandle;
		if (pSrc == NULL)
		{
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
			
		ulRet = pSrc->SetParam(nParamID, pParam);
		return ulRet;
	
	}

	VO_S32 VO_API voGetSource2AACAPI(VO_SOURCE2_API* pHandle)
	{
		pHandle->Init		= vosrcInit2;
		pHandle->Uninit 	= vosrcUninit2;	
		pHandle->Open		= vosrcOpen2;
		pHandle->Close		= vosrcClose2;
		pHandle->Start		= vosrcStart2;
		pHandle->Pause		= vosrcPause2;
		pHandle->Stop		= vosrcStop2;
		pHandle->Seek		= vosrcSeek2;
		pHandle->GetDuration = vosrcGetDuration2;
		pHandle->GetSample	 = vosrcGetSample2;
		pHandle->GetProgramCount = vosrcGetStreamCount2;
		pHandle->GetProgramInfo	= vosrcGetStreamInfo2;
		pHandle->GetCurTrackInfo = vosrcGetCurSelTrackInfo2;
		pHandle->SelectProgram= vosrcSelectProgram2;
		pHandle->SelectStream = vosrcSelectStream2;
		pHandle->SelectTrack	= vosrcSelectTrack2;
		pHandle->GetDRMInfo = vosrcGetDRMInfo2;
		pHandle->SendBuffer = vosrcSendBuffer2;
		pHandle->GetParam	= vosrcGetParam2;
		pHandle->SetParam	= vosrcSetParam2;
	
		return VO_RET_SOURCE2_OK;
	} 
	
#if defined __cplusplus
	}
#endif

