/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voStreamingDownloader.cpp

	Contains:	voStreamingDownloader class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-13		Aiven			Create file

*******************************************************************************/
#include "SDownloaderLog.h"
#include "StreamingDownloader.h"
#include "voStreamingDownloader.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif


//#define _SELFTEST
#ifdef _SELFTEST
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
#endif

VO_U32 Init(VO_HANDLE* phHandle, VO_SOURCE2_EVENTCALLBACK* pCallback, VO_SOURCE2_INITPARAM * pInitParam)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+Init");
	
#ifdef _SELFTEST
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	_CrtSetBreakAlloc(465);
#endif

	CStreamingDownloader * pSrc = new CStreamingDownloader();

	ulRet = pSrc->Init(pCallback, pInitParam);

	*phHandle = (VO_HANDLE)pSrc;

	SD_LOGI("-Init");
	return ulRet;

}

VO_U32 Uninit(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+Uninit");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Uninit();
	delete	pSrc;

#ifdef _SELFTEST
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	_CrtDumpMemoryLeaks();	
#endif

	SD_LOGI("-Uninit");

	return VO_RET_SOURCE2_OK;

}

VO_U32 Open( VO_HANDLE hHandle, VO_PTR pSource, VO_U32 uFlag, VO_PTR pLocalDir)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+Open");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = pSrc->Open(pSource, uFlag, (VO_PTCHAR)pLocalDir);
	
	SD_LOGI("-Open");

	return ulRet;
}

VO_U32 Close(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+Close");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Close();

	SD_LOGI("-Close");
	
	return ulRet;

}


VO_U32 StartDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+StartDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StartDownload();

	SD_LOGI("-StartDownload");

	return ulRet;

}

VO_U32 StopDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+StopDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StopDownload();

	SD_LOGI("-StopDownload");

	return ulRet;

}

VO_U32 PauseDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+PauseDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StartDownload();

	SD_LOGI("-PauseDownload");

	return ulRet;
}

VO_U32 ResumeDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+ResumeDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StopDownload();

	SD_LOGI("-ResumeDownload");

	return ulRet;

}


VO_U32 GetProgramInfo(VO_HANDLE hHandle, VO_SOURCE2_PROGRAM_INFO** ppProgramInfo)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+GetProgramInfo");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetProgramInfo(ppProgramInfo);

	SD_LOGI("-GetProgramInfo");

	return ulRet;

}

VO_U32 SelectStream(VO_HANDLE hHandle, VO_U32 nStreamID)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SelectStream");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SelectStream(nStreamID);

	SD_LOGI("-SelectStream");

	return ulRet;

}

VO_U32 SelectTrack(VO_HANDLE hHandle, VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SelectTrack");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SelectTrack(nTrackID, nType);

	SD_LOGI("-SelectTrack");
	
	return ulRet;

}

VO_U32 SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SetParam");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SetParam(nParamID, pParam);

	SD_LOGI("-SetParam");
	
	return ulRet;

}


VO_U32 GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetParam");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetParam(nParamID, pParam);

	SD_LOGI("-GetParam");
	
	return ulRet;

}

VO_S32 VO_API voGetStreamingDownloaderAPI(VO_ADAPTIVESTREAM_DOWNLOADER_API* pHandle)
{
	pHandle->Init       			= Init;
	pHandle->Uninit     			= Uninit;	
	pHandle->Open				= Open;
	pHandle->Close				= Close;
	pHandle->StartDownload		= StartDownload;
	pHandle->StopDownload		= StopDownload;	
	pHandle->PauseDownload		= PauseDownload;
	pHandle->ResumeDownload	= ResumeDownload;
	pHandle->GetProgramInfo		= GetProgramInfo;
	pHandle->SelectStream		= SelectStream;    
	pHandle->SelectTrack			= SelectTrack;
	pHandle->GetParam			= GetParam;
	pHandle->SetParam			= SetParam;

	return VO_RET_SOURCE2_OK;
} 

#ifdef __cplusplus
}
#endif

