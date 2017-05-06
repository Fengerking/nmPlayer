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
//#include "voLog.h"
#include "StreamingDownloader.h"
#include "voStreamingDownloader.h"
#include "SDownloaderLog.h"

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

VO_U32 API3_Init(VO_HANDLE* phHandle, VO_SOURCE2_EVENTCALLBACK* pCallback, VO_SOURCE2_INITPARAM * pInitParam)
{
	VO_U32	 ulRet = 0;
	if(pInitParam){
		VOLOGINIT(pInitParam->strWorkPath);
	}
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

VO_U32 API3_Uninit(VO_HANDLE hHandle)
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
	VOLOGUNINIT();

	return VO_RET_SOURCE2_OK;

}

VO_U32 API3_Open( VO_HANDLE hHandle, VO_PTR pSource, VO_U32 uFlag, VO_PTR pLocalDir)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+Open");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = pSrc->Open(pSource, uFlag, (VO_PTCHAR)pLocalDir);
	
	SD_LOGI("-Open---ulRet=%x",ulRet);

	return ulRet;
}

VO_U32 API3_Close(VO_HANDLE hHandle)
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


VO_U32 API3_StartDownload(VO_HANDLE hHandle)
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

VO_U32 API3_StopDownload(VO_HANDLE hHandle)
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

VO_U32 API3_PauseDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+PauseDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StopDownload();

	SD_LOGI("-PauseDownload");

	return ulRet;
}

VO_U32 API3_ResumeDownload(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SD_LOGI("+ResumeDownload");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->StartDownload();

	SD_LOGI("-ResumeDownload");

	return ulRet;

}


VO_U32 API3_GetProgramInfo(VO_HANDLE hHandle, VO_SOURCE2_PROGRAM_INFO** ppProgramInfo)
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

VO_U32 API3_SelectStream(VO_HANDLE hHandle, VO_U32 nStreamID)
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

VO_U32 API3_SelectTrack(VO_HANDLE hHandle, VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType)
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

VO_U32 API3_SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
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


VO_U32 API3_GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
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

VO_U32 API3_GetVideoCount(VO_HANDLE hHandle)
{
	VO_U32	 nCount = 0;

	SD_LOGI("+GetVideoCount");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	nCount = pSrc->GetVideoCount();

	SD_LOGI("-GetVideoCount---nCount=%lu", nCount);

	return nCount;
}

VO_U32 API3_GetAudioCount(VO_HANDLE hHandle)
{
	VO_U32	 nCount = 0;

	SD_LOGI("+GetAudioCount");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	nCount = pSrc->GetAudioCount();

	SD_LOGI("-GetAudioCount---nCount=%lu", nCount);

	return nCount;
}

VO_U32 API3_GetSubtitleCount (VO_HANDLE hHandle)
{
	VO_U32	 nCount = 0;

	SD_LOGI("+GetSubtitleCount");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	nCount = pSrc->GetSubtitleCount();

	SD_LOGI("-GetSubtitleCount---nCount=%lu", nCount);

	return nCount;
}

VO_U32 API3_SelectVideo(VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SelectVideo---nIndex=%d",nIndex);
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SelectVideo(nIndex);

	SD_LOGI("-SelectVideo---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_SelectAudio(VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SelectAudio---nIndex=%d",nIndex);
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SelectAudio(nIndex);

	SD_LOGI("-SelectAudio---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_SelectSubtitle(VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+SelectSubtitle---nIndex=%d",nIndex);
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->SelectSubtitle(nIndex);

	SD_LOGI("-SelectSubtitle---ulRet=%lu", ulRet);

	return ulRet;
}

VO_BOOL API3_IsVideoAvailable(VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_BOOL	 blRet = VO_FALSE;

	SD_LOGI("+IsVideoAvailable");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return blRet;
	}
	
	blRet = pSrc->IsVideoAvailable(nIndex);

	SD_LOGI("-IsVideoAvailable---blRet=%lu", blRet);

	return blRet;
}

VO_BOOL API3_IsAudioAvailable(VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_BOOL	 blRet = VO_FALSE;

	SD_LOGI("+IsAudioAvailable");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return blRet;
	}
	
	blRet = pSrc->IsAudioAvailable(nIndex);

	SD_LOGI("-IsAudioAvailable---blRet=%lu", blRet);

	return blRet;
}

VO_BOOL API3_IsSubtitleAvailable (VO_HANDLE hHandle, VO_S32 nIndex)
{
	VO_BOOL	 blRet = VO_FALSE;

	SD_LOGI("+IsSubtitleAvailable");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return blRet;
	}
	
	blRet = pSrc->IsSubtitleAvailable(nIndex);

	SD_LOGI("-IsSubtitleAvailable---blRet=%lu", blRet);

	return blRet;
}

VO_U32 API3_CommitSelection (VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+CommitSelection");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->CommitSelection();

	SD_LOGI("-CommitSelection---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_ClearSelection (VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+ClearSelection");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->ClearSelection();

	SD_LOGI("-ClearSelection---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_GetVideoProperty (VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetVideoProperty");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetVideoProperty(nIndex, ppProperty);

	SD_LOGI("-GetVideoProperty---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_GetAudioProperty(VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetAudioProperty");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetAudioProperty(nIndex, ppProperty);

	SD_LOGI("-GetAudioProperty---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_GetSubtitleProperty(VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetSubtitleProperty");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetSubtitleProperty(nIndex, ppProperty);

	SD_LOGI("-GetSubtitleProperty---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_GetCurrPlayingTrackIndex (VO_HANDLE hHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetCurrPlayingTrackIndex");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetCurrPlayingTrackIndex(pCurrIndex);

	SD_LOGI("-GetCurrPlayingTrackIndex---ulRet=%lu", ulRet);

	return ulRet;
}

VO_U32 API3_GetCurrSelectedTrackIndex(VO_HANDLE hHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	VO_U32	 ulRet = 0;

	SD_LOGI("+GetCurrSelectedTrackIndex");
	
	CStreamingDownloader * pSrc = (CStreamingDownloader *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetCurrSelectedTrackIndex(pCurrIndex);

	SD_LOGI("-GetCurrSelectedTrackIndex---ulRet=%lu", ulRet);

	return ulRet;
}

VO_S32 VO_API voGetStreamingDownloaderAPI3(VO_ADAPTIVESTREAM_DOWNLOADER_API3* pHandle)
{
	pHandle->Init       			= API3_Init;
	pHandle->Uninit				= API3_Uninit;
	pHandle->Open				= API3_Open;
	pHandle->Close				= API3_Close;
	pHandle->StartDownload		= API3_StartDownload;
	pHandle->StopDownload		= API3_StopDownload;	
	pHandle->PauseDownload		= API3_PauseDownload;
	pHandle->ResumeDownload	= API3_ResumeDownload;
	pHandle->GetParam			= API3_GetParam;
	pHandle->SetParam			= API3_SetParam;
	pHandle->GetVideoCount		= API3_GetVideoCount;
	pHandle->GetAudioCount		= API3_GetAudioCount;
	pHandle->GetSubtitleCount		= API3_GetSubtitleCount;
	pHandle->SelectVideo			= API3_SelectVideo;
	pHandle->SelectAudio			= API3_SelectAudio;
	pHandle->SelectSubtitle		= API3_SelectSubtitle;
	pHandle->IsVideoAvailable		= API3_IsVideoAvailable;
	pHandle->IsAudioAvailable		= API3_IsAudioAvailable;
	pHandle->IsSubtitleAvailable	= API3_IsSubtitleAvailable;
	pHandle->CommitSelection		= API3_CommitSelection;
	pHandle->ClearSelection		= API3_ClearSelection;
	pHandle->GetVideoProperty	= API3_GetVideoProperty;
	pHandle->GetAudioProperty	= API3_GetAudioProperty;
	pHandle->GetSubtitleProperty	= API3_GetSubtitleProperty;
	pHandle->GetCurrPlayingTrackIndex = API3_GetCurrPlayingTrackIndex;
	pHandle->GetCurrSelectedTrackIndex = API3_GetCurrSelectedTrackIndex;

	return VO_RET_SOURCE2_OK;
} 

#ifdef __cplusplus
}
#endif

