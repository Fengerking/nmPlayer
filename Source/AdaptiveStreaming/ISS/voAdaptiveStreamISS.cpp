/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voAdaptiveStreamISS.cpp

	Contains:	voAdaptiveStreamISS class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#include "ISS_SmoothStreaming.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
#endif

#define SOURCE_CLASS ISS_SmoothStreaming
	
#ifdef _WIN32
#include <Windows.h>
	BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
	{
		return TRUE;
	}
#endif	//_WIN32
	

VO_U32 vosrcInit(VO_HANDLE * phHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK* pCallback, VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam)
{
	VO_U32	 ulRet = 0;
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
//	_CrtSetBreakAlloc(78);
#endif
	if(pInitParam){
		VOLOGINIT(pInitParam->strWorkPath);
	}
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Init_IIS(pData, pCallback, pInitParam);

	*phHandle = (VO_HANDLE)pSrc;
	return ulRet;
}


VO_U32 vosrcUninit(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Uninit_IIS();
	delete	pSrc;

	VOLOGUNINIT();
#ifdef _DEBUG
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	_CrtDumpMemoryLeaks();	
#endif	
	return VO_RET_SOURCE2_OK;
}


VO_U32 vosrcOpen(VO_HANDLE hHandle)
{	
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Open_IIS();
	return ulRet;
}

VO_U32 vosrcClose(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Close_IIS();
	return ulRet;
}

VO_U32 vosrcStart(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Start_IIS();
	return ulRet;
}

VO_U32 vosrcStop(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Stop_IIS();
	return ulRet;
}

VO_U32 vosrcUpdate(VO_HANDLE hHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Update_IIS(pData);
	return ulRet;
}


VO_U32 vosrcGetChunck(VO_HANDLE hHandle, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetChunk_IIS(uID, ppChunk);
	return ulRet;
}

VO_U32 vosrcSeek(VO_HANDLE hHandle, VO_U64* pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Seek_IIS(pTimeStamp, sSeekMode);
	return ulRet;
}

VO_U32 vosrcGetDuration(VO_HANDLE hHandle, VO_U64* pDuration)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetDuration_IIS(pDuration);
	return ulRet;
}

VO_U32 vosrcGetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetProgramCounts_IIS(pProgramCount);
	return ulRet;
}

VO_U32 vosrcGetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetProgramInfo_IIS(nProgramId, ppProgramInfo);
	return ulRet;
}

VO_U32 vosrcGetCurTrackInfo(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType , _TRACK_INFO **ppTrackInfo )
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetCurTrackInfo_IIS(nOutPutType, ppTrackInfo);
	return ulRet;
}

VO_U32 vosrcSelectProgram(VO_HANDLE hHandle, VO_U32 nProgramID)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->SelectProgram_IIS(nProgramID);
	return ulRet;
}

VO_U32 vosrcSelectStream(VO_HANDLE hHandle, VO_U32 nStreamID, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->SelectStream_IIS(nStreamID, sPrepareChunkPos);
	return ulRet;
}
#ifdef _new_programinfo
VO_U32 vosrcSelectTrack(VO_HANDLE hHandle, VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE sTrackType)
#else
VO_U32 vosrcSelectTrack(VO_HANDLE hHandle, VO_U32 nTrackID)
#endif
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->SelectTrack_IIS(nTrackID);
	return ulRet;
}


VO_U32	vosrcGetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO** ppDRMInfo)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->GetDRMInfo_IIS(ppDRMInfo);
	return ulRet;
}


VO_U32 vosrcGetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->GetParam_IIS(nParamID, pParam);
	return ulRet;
}

VO_U32 vosrcSetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 ulRet = 0;

	if(VO_PID_COMMON_LOGFUNC == nParamID)
	{
		if(pParam != NULL)
		{
			//VO_LOG_PRINT_CB*  pVologCB = (VO_LOG_PRINT_CB *)pParam;
			//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}
		return 0;
	}
	
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SetParam_IIS(nParamID, pParam);
	return ulRet;

}


VO_S32 VO_API voGetAdaptiveStreamISSAPI(VO_ADAPTIVESTREAM_PARSER_API* pHandle)
{
	pHandle->Init       = vosrcInit;
	pHandle->UnInit     = vosrcUninit;	
	pHandle->Open		= vosrcOpen;
	pHandle->Close		= vosrcClose;
	pHandle->Start		= vosrcStart;
	pHandle->Stop		= vosrcStop;	
	pHandle->Update     = vosrcUpdate;
	pHandle->GetChunk   = vosrcGetChunck;
	pHandle->Seek		= vosrcSeek;
	pHandle->GetDuration = vosrcGetDuration;
	pHandle->GetProgramCount	= vosrcGetProgramCount;
	pHandle->GetProgramInfo	= vosrcGetProgramInfo;
	pHandle->GetCurTrackInfo	= vosrcGetCurTrackInfo;
	pHandle->SelectProgram		= vosrcSelectProgram;
	pHandle->SelectStream		= vosrcSelectStream;    
	pHandle->SelectTrack		= vosrcSelectTrack;
	pHandle->GetDRMInfo = vosrcGetDRMInfo;
	pHandle->GetParam	= vosrcGetParam;
	pHandle->SetParam	= vosrcSetParam;

	return VO_RET_SOURCE2_OK;
} 

//#ifdef _VONAMESPACE
//}
//#else if defined __cplusplus
#ifdef __cplusplus
}
#endif



