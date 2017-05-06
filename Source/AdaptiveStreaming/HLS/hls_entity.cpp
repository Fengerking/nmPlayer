#include "string.h"
#include "hls_entity.h"
#include "voLog.h"
#include "voHLSDRM.h"
#include "voOSFunc.h"
#include "CDataBox.h"
#include "voCheck.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

C_HLS_Entity::C_HLS_Entity()
{
    m_pEventCallbackFunc = NULL;    
    memset(&m_sChunkContainer, 0, sizeof(S_CHUNK_CONTAINER));    
    m_pLogParam = NULL;
    m_pHLicCheck = NULL;
    m_bUpdateRunning = VO_FALSE;
    memset(&m_sCurrentAdaptiveStreamItemForPlayList, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
	m_eProgramType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
    m_pProgFunc = NULL;
}

C_HLS_Entity::~C_HLS_Entity()
{
    ResetAllContext();
}

VO_U32     C_HLS_Entity::Init_HLS(VO_ADAPTIVESTREAM_PLAYLISTDATA* pData, VO_SOURCE2_EVENTCALLBACK*   pEventCallback, VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
#ifndef _WITHOUT_LIC_CHECK_
    if(pInitParam == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

#ifndef	_WITHOUT_NEW_LOG_
    VOLOGINIT(pInitParam->strWorkPath);
#endif
	ulRet = voCheckLibInit (&m_pHLicCheck, VO_INDEX_SRC_HLS, VO_LCS_WORKPATH_FLAG, 0, pInitParam->strWorkPath); 
    if(ulRet != VO_ERR_NONE )
    {
        if(m_pHLicCheck != NULL)
        {
            voCheckLibUninit(m_pHLicCheck);
        }
        m_pHLicCheck = NULL;
        return ulRet;
    }
#endif    
    if(pEventCallback == NULL || pData == NULL || pData->pProgFunc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    m_pEventCallbackFunc = pEventCallback;
    ulRet = CommitPlayListData(pData);
    return ulRet;
}
    
VO_U32     C_HLS_Entity::Uninit_HLS()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    ResetAllContext();
#ifndef _WITHOUT_LIC_CHECK_
    if(m_pHLicCheck != NULL)
    {
        voCheckLibUninit(m_pHLicCheck);
        m_pHLicCheck = NULL;
    }
#endif

#ifndef	_WITHOUT_NEW_LOG_
	VOLOGUNINIT();
#endif
    return ulRet;
}
    
VO_U32     C_HLS_Entity::Close_HLS()
{
    VO_U32    ulRet = VO_RET_SOURCE2_OK;
    ResetAllContext();
    StopPlaylistUpdate();
    return ulRet;
}

VO_U32     C_HLS_Entity::Start_HLS()
{
    VO_U32   ulRet = 0;
    S_PLAY_SESSION*    pPlaySession = NULL;
    _STREAM_INFO*   pStreamInfo = NULL;
    VO_U32   ulIndex = 0;

    if(m_sM3UManager.IsPlaySessionReady() != VO_TRUE)
	{
        ulRet = PreparePlaySession();
        if(ulRet != 0)
        {
            VOLOGI("Can't start current play session!");
            return VO_RET_SOURCE2_FAIL;
        }
	}

	ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
	if(ulRet == 0)
	{
		switch(pPlaySession->pStreamPlayListNode->eChuckPlayListType)
		{
		    case M3U_LIVE:
			{
				m_eProgramType = VO_SOURCE2_STREAM_TYPE_LIVE;
				m_pProgFunc->SetProgType(m_pProgFunc->hHandle, VO_SOURCE2_STREAM_TYPE_LIVE);
                m_sM3UManager.SetStartPosForLiveStream();
                VOLOGI("Current Program is LIVE!");
				break;
			}
		    case M3U_VOD:
			{
				m_eProgramType = VO_SOURCE2_STREAM_TYPE_VOD;
				m_pProgFunc->SetProgType(m_pProgFunc->hHandle, VO_SOURCE2_STREAM_TYPE_VOD);
                VOLOGI("Current Program is VOD!");
				break;
			}
		    default:
			{
				m_eProgramType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
				break;
			}
		}
	}

    if(m_eProgramType == VO_SOURCE2_STREAM_TYPE_LIVE)
    {
        m_sM3UManager.SetStartPosForLiveStream();
    }

	ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
	if(ulRet == 0)
	{    
		if(pPlaySession!= NULL && pPlaySession->pStreamPlayListNode != NULL && pPlaySession->pStreamPlayListNode->eChuckPlayListType == M3U_LIVE)
		{
			begin();
		}
	}

    VOLOGI("Set the Program Changed!");
    return ulRet;
}

VO_U32     C_HLS_Entity::Stop_HLS()
{
    VO_U32   ulRet = 0;
    StopPlaylistUpdate();
    return ulRet;
}

void    C_HLS_Entity::begin()
{
    m_bUpdateRunning = VO_TRUE;
    vo_thread::begin();
}

void   C_HLS_Entity::thread_function()
{
	set_threadname((char*) "Playlist Update" );
    PlayListUpdateForLive();
	VOLOGI( "Update Thread Exit!" );
}

VO_U32     C_HLS_Entity::Open_HLS()
{
    VO_U32   ulRet = 0;
    VO_CHAR  strURL[2048] = {0};
    S_PLAY_SESSION*     pPlaySession = NULL;
    M3U_MANIFEST_TYPE   eRootPlaylistType = M3U_UNKNOWN_PLAYLIST;
    
    ulRet = ParseHLSPlaylist(&m_sCurrentAdaptiveStreamItemForPlayList, HLS_ROOT_MANIFEST_ID);
    if(ulRet != 0)
    {
		VOLOGI("Open Fail!");
		return VO_RET_SOURCE2_OPENFAIL;
	}

    if(m_sCurrentAdaptiveStreamItemForPlayList.pData != NULL)
    {
        delete[] m_sCurrentAdaptiveStreamItemForPlayList.pData;
        m_sCurrentAdaptiveStreamItemForPlayList.pData = NULL;
    }
    
    ulRet = m_sM3UManager.GetRootManifestType(&eRootPlaylistType);
    if(ulRet != 0)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = GenerateTheProgramInfo();
    if(ulRet != 0)
    {
        VOLOGE("Generate the Program Info Failed!");
    }
    return ulRet;
}


VO_U32     C_HLS_Entity::GetChunk_HLS(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	voCAutoLock lock( &m_ListLock );
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    E_PLAYLIST_TYPE    ePlayListType = E_UNKNOWN_STREAM;
    VO_BOOL            bInAdaption = VO_FALSE;
    VO_BOOL            bDrmEnable = VO_FALSE;

    if(ppChunk == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    ulRet = GetChunckItem(uID, ppChunk);
    if(ulRet == 0)
    {
        (*ppChunk)->ullTimeScale = 1000;
    }
    
    if(ulRet == 0)
    {
        if((*ppChunk)->pChunkDRMInfo != NULL)
        {
            bDrmEnable = VO_TRUE;
        }
        
        VOLOGI("the start time:%d, the duration:%d, the drm type:%d, the uFlag:%d, the ExtInfo:%d, the deadtime:%lld, the url:%s, the root url:%s, the playlist id:%d, the chapter id:%d", (VO_U32)((*ppChunk)->ullStartTime), (VO_U32)((*ppChunk)->ullDuration), bDrmEnable, ((*ppChunk)->uFlag),
               (*ppChunk)->pStartExtInfo->uFlag, (*ppChunk)->ullChunkDeadTime, (*ppChunk)->szUrl, (*ppChunk)->szRootUrl, (*ppChunk)->sKeyID.uTrackID, (*ppChunk)->uPeriodSequenceNumber);
    }
    
    return ulRet;
}


VO_U32     C_HLS_Entity::Update_HLS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
    return 0;
}

VO_U32     C_HLS_Entity::ParseHLSPlaylist(VO_VOID*  pPlaylistData, VO_U32 ulPlayListId)
{   
    voCAutoLock lock( &m_ListLock );
    VO_U32   ulRet = 0;
    VO_ADAPTIVESTREAM_PLAYLISTDATA*   pPlaylistItem = NULL;

    if(pPlaylistData == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    pPlaylistItem = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)pPlaylistData;
    VOLOGI("NewURL:%s", pPlaylistItem->szNewUrl);
    VOLOGI("URL:%s", pPlaylistItem->szUrl);
    ulRet = m_sM3UManager.ParseManifest(pPlaylistItem->pData, pPlaylistItem->uDataSize, (VO_CHAR*)pPlaylistItem->szNewUrl, ulPlayListId);            
	return  ulRet;
}

VO_U32     C_HLS_Entity::PreparePlaySession()
{
    VO_U32   ulRet = 0;
    VO_CHAR  strURL[2048] = {0};
	VO_CHAR  strRoot[2048] = {0};
    M3U_MANIFEST_TYPE   ePlaylistType = M3U_UNKNOWN_PLAYLIST;
    S_PLAY_SESSION*     pPlaySession = NULL;
    VO_U32              ulIndex = 0;
    VO_U32              ulCurrentParseTime = 0;
    VO_U32              ulParseRet = 0;
    VO_U32              ulPlayListId = 0xffffffff;
    S_PLAYLIST_NODE*    pPlayListNode = NULL;

    ulRet = m_sM3UManager.GetRootManifestType(&ePlaylistType);
    if(ulRet != 0)
    {
        return VO_RET_SOURCE2_FAIL;
    }

    while(m_sM3UManager.IsPlaySessionReady() == VO_FALSE && ulCurrentParseTime<(HLS_MAX_MANIFEST_RETRY_COUNT))
    {
        switch(ePlaylistType)
        {
            case M3U_CHUNK_PLAYLIST:
            {
                break;
            }
            case M3U_STREAM_PLAYLIST:
            {
                pPlayListNode = m_sM3UManager.GetPlayListNeedParseForSessionReady();
                if(pPlayListNode != NULL)
                {
                    ulParseRet = NotifyToParse(pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
                    ulCurrentParseTime++;
                }
                break;
            }
        }
    }

    return 0;
}

VO_U32    C_HLS_Entity::CommitPlayListData(VO_VOID*  pPlaylistData)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
    VO_ADAPTIVESTREAM_PLAYLISTDATA*   pPlaylistItem = NULL;
    VO_PBYTE    pData = NULL;

    if(pPlaylistData == NULL)
    {
        VOLOGI("The pPlaylistData is NULL!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(&m_sCurrentAdaptiveStreamItemForPlayList, 0, sizeof(m_sCurrentAdaptiveStreamItemForPlayList));
    pPlaylistItem = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)pPlaylistData;
    pData = new VO_BYTE[pPlaylistItem->uDataSize+1];
    if(pData == NULL)
    {
        VOLOGI("Lack of memory!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(pData, 0, pPlaylistItem->uDataSize+1);
    memcpy(pData, pPlaylistItem->pData, pPlaylistItem->uDataSize);
    m_sCurrentAdaptiveStreamItemForPlayList.pData = pData;
    m_sCurrentAdaptiveStreamItemForPlayList.uDataSize = pPlaylistItem->uDataSize;
    memcpy(m_sCurrentAdaptiveStreamItemForPlayList.szRootUrl, pPlaylistItem->szRootUrl, strlen(pPlaylistItem->szRootUrl));
    memcpy(m_sCurrentAdaptiveStreamItemForPlayList.szUrl, pPlaylistItem->szNewUrl, strlen(pPlaylistItem->szNewUrl));
    memcpy(m_sCurrentAdaptiveStreamItemForPlayList.szNewUrl, pPlaylistItem->szNewUrl, strlen(pPlaylistItem->szNewUrl));
	m_pProgFunc = pPlaylistItem->pProgFunc;
    return   ulRet;
}

VO_U32     C_HLS_Entity::Seek_HLS(VO_U64*  pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
	voCAutoLock lock( &m_ListLock );
    
    VO_U32     ulRet = 0;
    VO_BOOL    bNeedResetParser = VO_FALSE;
    VO_U32     ulTimeInput = (VO_U32)(*pTimeStamp);
    VO_U32     ulTimeChunkOffset = 0;

    if(pTimeStamp == NULL)
    {
        VOLOGI("Input Param Invalid!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    
    ulRet = m_sM3UManager.SetThePos(ulTimeInput, &bNeedResetParser, &ulTimeChunkOffset, sSeekMode);
    if(ulRet == 0)
    {
        VOLOGI("Seek time:%d  OK! New Chunk Offset:%d", ulTimeInput, ulTimeChunkOffset);
        *pTimeStamp = ulTimeChunkOffset;
    }
    else
    {
        VOLOGI("Seek time:%d  Failed!", ulTimeInput);
    }

    VOLOGI("return value for seek:%d", ulRet);
    if(ulRet != VO_RET_SOURCE2_SEEKMODEUNSUPPORTED)
    {
        return 0;
    }
    else
    {
        return ulRet;
    }    
}
    
VO_U32     C_HLS_Entity::GetDuration_HLS(VO_U64 * pDuration)
{
    VO_U32    ulRet = 0;
    VO_U32    ulDuration = 0;

    if(pDuration == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    
    ulRet = m_sM3UManager.GetTheDuration(&ulDuration);
    if(ulRet != 0)
    {
        return VO_RET_SOURCE2_NEEDRETRY;
    }

    *pDuration = ulDuration;
    return 0;
}
    
VO_U32     C_HLS_Entity::GetProgramCounts_HLS(VO_U32*  pProgramCounts)
{
    VO_U32    ulRet = 0;
    
    if(pProgramCounts == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    *pProgramCounts = 1;
    return 0;
}
    
VO_U32     C_HLS_Entity::GetProgramInfo_HLS(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo)
{
    VO_U32   ulRet= 0;
    VO_U32   ulIndex = 0;
    VO_U32   ulIndexForTrack = 0;

    if(ppProgramInfo == NULL)
    {
        VOLOGI("Input Param Invalid!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    
    return ulRet;
}

VO_U32     C_HLS_Entity::GetCurTrackInfo_HLS(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo)
{
    return 0;
}
VO_U32     C_HLS_Entity::SelectProgram_HLS (VO_U32 uProgramId)
{
    return 0;
}

VO_U32     C_HLS_Entity::SelectStream_HLS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
    VO_U32   ulRet = 0;
    S_PLAY_SESSION*    pPlaySession = NULL;
    S_PLAYLIST_NODE*   pPlayListNode = NULL;

	VOLOGI("The New Stream Id:%d", uStreamId);
    pPlayListNode = m_sM3UManager.FindPlayListById(uStreamId);
    if(pPlayListNode == NULL)
    {
		VOLOGI("Stream:%d doesn't exist!", uStreamId);
        return VO_RET_SOURCE2_FAIL;
    }

    ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet == 0)
    {
		if(pPlaySession->pStreamPlayListNode != NULL && pPlaySession->pStreamPlayListNode->ulPlayListId == uStreamId)
        {
			VOLOGI("The Stream:%d already selected!", uStreamId);
			return 0;
        }
    }

    ulRet = NotifyToParse(pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
    if(ulRet != 0)
    {
		VOLOGI("Parse the PlayList:%d fail!", pPlayListNode->ulPlayListId);
        return VO_RET_SOURCE2_FAIL;
    }

    m_sM3UManager.AdjustChunkPosInListForBA(sPrepareChunkPos);
    m_sM3UManager.SetPlayListToSession(uStreamId);
    m_sM3UManager.AdjustXMedia();    
    m_sM3UManager.AdjustSequenceIdInSession();

    return 0;
}

VO_U32     C_HLS_Entity::SelectTrack_HLS(VO_SOURCE2_TRACK_TYPE nType, VO_U32 nTrackID)
{
	VO_U32   ulRet = 0;
	S_PLAY_SESSION*    pPlaySession = NULL;
	S_PLAYLIST_NODE*   pPlayListNode = NULL;
    VO_U32             ulStreamIndex = 0xffffffff;    
    VO_U32             ulStreamId = 0xffffffff;
    VO_U32             ulTrackIndex = 0;
    VO_U32             ulLoop = 0;
    VO_U32             ulConvertTrackId = nTrackID;
    S_PLAYLIST_NODE*   pCurPlayListWithNewType = NULL;
    S_PLAYLIST_NODE*   pCurMainStreamPlayList = NULL;
    S_PLAYLIST_NODE*   pNewTargetPlayList = NULL;

    ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet != 0)
    {
        return VO_RET_SOURCE2_FAIL;
    }

	switch(nType)
	{
	    case VO_SOURCE2_TT_AUDIO:
		{
            pCurPlayListWithNewType = pPlaySession->pAlterAudioPlayListNode;
			break;
		}
		case VO_SOURCE2_TT_VIDEO:
		{
			pCurPlayListWithNewType = pPlaySession->pAlterVideoPlayListNode;
			break;
		}
		case VO_SOURCE2_TT_SUBTITLE:
		{
			pCurPlayListWithNewType = pPlaySession->pAlterSubTitlePlayListNode;
			break;
		}
	}

    VOLOGI("new Type:%d, new Id:%d", nType, nTrackID);
    pCurMainStreamPlayList = pPlaySession->pStreamPlayListNode;
    
    if(pCurPlayListWithNewType == NULL)
    {
        VOLOGI("no %d type XMedia Track", nType);
        return 0;
    }

    pNewTargetPlayList = m_sM3UManager.FindTargetPlayListWithTrackTypeAndId(nType, nTrackID);

    if(pNewTargetPlayList == NULL)
    {
        VOLOGI("No target Track!");
        return 0;
    }

    if(pNewTargetPlayList->ulPlayListId == pCurPlayListWithNewType->ulPlayListId)
    {
        VOLOGI("already selected!");
        return 0;
    }
    
    ulRet = NotifyToParse(pNewTargetPlayList->strRootURL, pNewTargetPlayList->strShortURL, pNewTargetPlayList->ulPlayListId);
	if(ulRet != 0)
    {
			VOLOGI("nTrackID:%d parse fail!", pNewTargetPlayList->ulPlayListId);
			return VO_RET_SOURCE2_FAIL;
	}

	m_sM3UManager.SetPlayListToSession(pNewTargetPlayList->ulPlayListId);
    m_sM3UManager.AdjustSequenceIdInSession();
    return 0;
}

VO_U32     C_HLS_Entity::GetDRMInfo_HLS(VO_SOURCE2_DRM_INFO** ppDRMInfo)
{
    return 0;
}

VO_U32     C_HLS_Entity::GetParam_HLS(VO_U32 nParamID, VO_PTR pParam )
{
    VO_U64*    pullTimeValue = NULL;
    VO_U32     ulRet = 0;

    if(pParam == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    switch(nParamID)
	{
        case  VO_PID_ADAPTIVESTREAMING_DVR_ENDTIME:
        {
            pullTimeValue = (VO_U64*)pParam;
            *pullTimeValue = (VO_U64)(m_sM3UManager.GetTheEndTimeForLiveStream());
            break;
        }
    
        case  VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH:
        {            
            pullTimeValue = (VO_U64*)pParam;
            *pullTimeValue = (VO_U64)(m_sM3UManager.GetTheDvrDurationForLiveStream());            
            break;
        }
    
        case  VO_PID_ADAPTIVESTREAMING_DVR_LIVETIME:            
        {            
            pullTimeValue = (VO_U64*)pParam;
            *pullTimeValue = (VO_U64)(m_sM3UManager.GetTheLiveTimeForLiveStream());            
            break;
        }

        case VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH:
        {
            pullTimeValue = (VO_U64*)pParam;
            m_sM3UManager.GetTheDvrEndLengthForLiveStream(pullTimeValue);            
            break;
        }

        case VO_PID_ADAPTIVESTREAMING_DVR_CHUNKWINDOWPOS:
        {
            VO_U32   ulTimeOffset = 0;
            VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS*   pChunkPosInfo = (VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS*)pParam;
            ulRet = m_sM3UManager.GetChunkOffsetValueBySequenceId((VO_U32)pChunkPosInfo->uChunkID, &ulTimeOffset);
            if(ulRet != 0)
            {
                return ulRet;
            }
            else
            {
                pChunkPosInfo->ullWindowPosition = (VO_U64)ulTimeOffset;
            }
            break;
        }
        
        case VO_PID_ADAPTIVESTREAMING_DVR_PROGRAMSTREAMTYPE:
        {
            VO_SOURCE2_PROGRAM_TYPE  eProgramType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
            VO_SOURCE2_PROGRAM_TYPE*  peProgramType = (VO_SOURCE2_PROGRAM_TYPE*)pParam;
            m_sM3UManager.GetCurrentProgreamStreamType(&eProgramType);            
            *peProgramType = eProgramType;
            break;
        }
        
	}

	return 0;
}

VO_U32     C_HLS_Entity::SetParam_HLS(VO_U32 nParamID, VO_PTR pParam )
{
    VO_U32     ulRet = 0;
    VO_U64*    pullTimeValue = NULL;
    
    if(pParam == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    
    switch(nParamID)
    {
        case VO_PID_SOURCE2_EVENTCALLBACK:
        {
            VO_SOURCE2_EVENTCALLBACK*   psCallbackEvent = NULL;
            psCallbackEvent = (VO_SOURCE2_EVENTCALLBACK*)(pParam);
            m_pEventCallbackFunc = psCallbackEvent;
            break;
        }
        case VO_PID_SOURCE2_LIBOP:
        {            
            VOLOGI( "set_libop");
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_UTC:
        {
            VOLOGI("set the UTC Time!");
            m_sM3UManager.SetUTCTime((VO_U64 *) pParam);
            break;
        }
        case VO_PID_COMMON_LOGFUNC:
        {
            if(pParam != NULL)
            {
            //    VO_LOG_PRINT_CB*  pVologCB = (VO_LOG_PRINT_CB *)pParam;
            //    vologInit (pVologCB->pUserData, pVologCB->fCallBack);
            }
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_LIVELATENCY:
        {
            VOLOGI("Set the Live Latency value!");
            m_sM3UManager.SetLiveLatencyValue((VO_U32*)pParam);
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_PERIOD2TIME:
        {
            VO_U32   ulTimeOffset = 0;
            VO_SOURCE2_PERIODTIMEINFO*  pPeriodTimeInfo = (VO_SOURCE2_PERIODTIMEINFO*)pParam;
            ulRet = m_sM3UManager.GetCurrentSessionDurationByChapterId(pPeriodTimeInfo->uPeriodSequenceNumber, &ulTimeOffset);
            if(ulRet == 0)
            {
                pPeriodTimeInfo->ullTimeStamp = (VO_U64)ulTimeOffset;
            }
            
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_SWITCH_REF_ADUIO_THREAD:
        {
            VOLOGI("Set the Audio Ref!");
            AdjustTheSequenceIDForMainStream();
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_UPDATEURL:
        {
            VOLOGI("Update URL!");
            ulRet = UpdateURL(pParam);
            break;
        }
        case VO_PID_ADAPTIVESTREAMING_CHUNK_INFO:
        {
            m_sM3UManager.SetCurrentSequenceIdForPlayList(pParam);
            break;
        }
		case VO_ADAPTIVESTREAMPARSER_SEEKMODE_TRACK_ONLY:
        {
			VO_U32 ulNewOffset = 0;
			VO_ADAPTIVESTREAMPARSER_TRACK_SEEK_PARAM*   pTrackSeekParam = (VO_ADAPTIVESTREAMPARSER_TRACK_SEEK_PARAM*)(pParam);
			ulRet = m_sM3UManager.SeekForOneTrackOnly(pTrackSeekParam->uASTrackID, pTrackSeekParam->ullOffsetTime, &ulNewOffset);
			break;
		}
        default:
        {
            break;
        }
    }

    return ulRet;
}


VO_VOID    C_HLS_Entity::ResetAllContext()
{
    m_pEventCallbackFunc = NULL;
    m_pLogParam = NULL;    
    m_bUpdateRunning = VO_FALSE;    
    DeleteAllProgramInfo();
    m_sM3UManager.ReleaseAllPlayList();
}


VO_U32  C_HLS_Entity::GenerateTheProgramInfo()
{
	S_PLAYLIST_NODE* pPlayListArray[HLS_MAX_STREAM_COUNT_IN_MASTER] = {0};
	VO_U32           ulStreamMAXCount = HLS_MAX_STREAM_COUNT_IN_MASTER;
    VO_U32           ulStreamId = 0;
	VO_SOURCE2_STREAM_INFO  sStreamInfo = {0};
	VO_U32           ulStreamCount = 0;
	VO_U32           ulRet = 0;
    VO_U32           ulLoop = 0;
    VO_U32           ulTrackCount = 0;

    ulRet = m_sM3UManager.GetMainStreamArray(pPlayListArray, ulStreamMAXCount, &ulStreamCount);
    if(ulRet != 0)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    for(ulLoop=0; ulLoop<ulStreamCount; ulLoop++)
    {
        memset(&sStreamInfo, 0, sizeof(VO_SOURCE2_STREAM_INFO));
		sStreamInfo.uBitrate = pPlayListArray[ulLoop]->sVarMainStreamAttr.ulBitrate;
        sStreamInfo.uStreamID = pPlayListArray[ulLoop]->ulPlayListId;
        sStreamInfo.uSelInfo = 0;

		if(ulLoop == 0)
		{
			sStreamInfo.uSelInfo |= VO_SOURCE2_SELECT_RECOMMEND;
		}

		if(ulStreamCount == 1)
		{
			sStreamInfo.uSelInfo |= VO_SOURCE2_SELECT_SELECTED;
		}
		m_pProgFunc->SetStream(m_pProgFunc->hHandle, &sStreamInfo, VOS2_PROGINFO_NEW);
		FillTrackInfoForStream(&sStreamInfo, pPlayListArray[ulLoop]);
	}

    return 0;
}

VO_U32  C_HLS_Entity::SetSelInfoInForStream(_STREAM_INFO* pStreamInfo)
{
    VO_U32   ulRet = 0;
    return 0;
}

VO_U32    C_HLS_Entity::NotifyToParse(VO_CHAR*  pURLRoot, VO_CHAR*   pURL, VO_U32 ulPlayListId)
{
    VO_U32                            ulEventCallbackCount = 0;
    VO_ADAPTIVESTREAM_PLAYLISTDATA    sVarPlayListData = {0};
    VO_U32                            ulRet = 0;

    if(strlen(pURL) == 0)
    {
        VOLOGI("empty URL, return ok!")
        return 0;
    }
   
    if(m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
    {

        while(ulEventCallbackCount < HLS_MAX_MANIFEST_RETRY_COUNT)
        {
            ulRet = RequestManfestAndParsing(&sVarPlayListData, pURLRoot, pURL, ulPlayListId);
            if(ulRet == 0)
            {
                VOLOGI("RequestManfestAndParsing ok!");
                break;
            }
            else
            {
                ulEventCallbackCount++;
            }
        }
    }
    
    return ulRet;
}

VO_U32     C_HLS_Entity::PlayListUpdateForLive()
{
    S_PLAY_SESSION*     pPlaySession = NULL;
    VO_U32              ulRet = 0;
    VO_U32              ulTimeInterval = 0;
    VO_U64              ullStartTime = 0;
    VO_CHAR             strRoot[1024] = {0};
    VO_BOOL             bStop = m_bUpdateRunning;

    ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet != 0)
    {
        m_bUpdateRunning = VO_FALSE;
        return VO_RET_SOURCE2_OK;
    }

    if(pPlaySession->pStreamPlayListNode != NULL && pPlaySession->pStreamPlayListNode->eChuckPlayListType == M3U_LIVE)
    {
        while(m_bUpdateRunning == VO_TRUE)
        {
            ullStartTime = voOS_GetSysTime();
            ulTimeInterval = m_sM3UManager.GetChunckItemIntervalTime();            
            VOLOGI("The update interval:%d", ulTimeInterval);

            UpdateThePlayListForLive();
            m_sM3UManager.AdjustSequenceIdInSession();

            while(m_bUpdateRunning == VO_TRUE && ( (voOS_GetSysTime() - ullStartTime) <= ulTimeInterval ) )
            {
                ulTimeInterval = m_sM3UManager.GetChunckItemIntervalTime();
				bStop = (m_bUpdateRunning?VO_FALSE:VO_TRUE);
                voOS_SleepExitable(100, &bStop);
            }
            
			bStop = (m_bUpdateRunning?VO_FALSE:VO_TRUE);
            voOS_SleepExitable(50, &bStop);
        }
    }

    VOLOGI("Update Thread End!");
	return 0;
}

VO_VOID    C_HLS_Entity::StopPlaylistUpdate()
{   
	VOLOGI( "+stop_updatethread" );
    m_bUpdateRunning = VO_FALSE;
	vo_thread::stop();
	VOLOGI( "-stop_updatethread" );
}

VO_U32  C_HLS_Entity::GetChunckItem(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID,VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk)
{
    VO_U32 ulRet = 0;
	S_PLAY_SESSION*   pPlaySession = NULL;
	VO_ADAPTIVESTREAMPARSER_CHUNK*    pAdaptiveItem = NULL;
	VO_DRM2_HSL_PROCESS_INFO*         pDrmItem = NULL;
	S_CHUNCK_ITEM                     sChunkItem = {0};
	E_PLAYLIST_TYPE                   ePlayListTypeForChunk = E_UNKNOWN_STREAM;

	ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet != 0)
    {
		VOLOGI("The PlaySession is not ready now!");
		return ulRet;
	}

	switch(uID)
	{
	    case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
		{
			if(pPlaySession->pStreamPlayListNode != NULL)
			{
				if(strlen(pPlaySession->pStreamPlayListNode->strShortURL) != 0)
				{
					InitChunkNode(&(m_sChunkContainer.sMainStreamChunk));
					pAdaptiveItem = &(m_sChunkContainer.sMainStreamChunk.sCurrentAdaptiveStreamItem);
					pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO;
					ePlayListTypeForChunk = E_MAIN_STREAM;
				}
				else
				{
					return VO_RET_SOURCE2_ERRORDATA;
				}
			}
			else
			{
				return VO_RET_SOURCE2_END;
			}
			break;
		}
	    case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		{
			if(pPlaySession->pAlterAudioPlayListNode != NULL)
			{
				if(strlen(pPlaySession->pAlterAudioPlayListNode->strShortURL) != 0)
				{
					InitChunkNode(&(m_sChunkContainer.sAltrerAudioChunk));
					pAdaptiveItem = &(m_sChunkContainer.sAltrerAudioChunk.sCurrentAdaptiveStreamItem);
					pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO;
					ePlayListTypeForChunk = E_X_MEDIA_AUDIO_STREAM;
				}
				else
				{
					return VO_RET_SOURCE2_ADAPTIVESTREAMING_DATA_IN_MUXED_SEGMENT;
				}
			}
			else
			{
				return VO_RET_SOURCE2_END;
			}
			break;
		}
	    case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		{
			if(pPlaySession->pAlterVideoPlayListNode != NULL)
			{
				if(strlen(pPlaySession->pAlterVideoPlayListNode->strShortURL) != 0)
				{
					InitChunkNode(&(m_sChunkContainer.sAltrerVideoChunk));
					pAdaptiveItem = &(m_sChunkContainer.sAltrerVideoChunk.sCurrentAdaptiveStreamItem);
					pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_VIDEO;
					ePlayListTypeForChunk = E_X_MEDIA_VIDEO_STREAM;
				}
				else
				{
					return VO_RET_SOURCE2_END;
				}
			}
			else
			{
				if(pPlaySession->pStreamPlayListNode != NULL && strlen(pPlaySession->pStreamPlayListNode->strShortURL) != 0 )
				{
					InitChunkNode(&(m_sChunkContainer.sMainStreamChunk));
					pAdaptiveItem = &(m_sChunkContainer.sMainStreamChunk.sCurrentAdaptiveStreamItem);                
					if(pPlaySession->pAlterAudioPlayListNode != NULL && strlen(pPlaySession->pAlterAudioPlayListNode->strShortURL) != 0)
					{
						pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_VIDEO; 
						ePlayListTypeForChunk = E_MAIN_STREAM;
					}
					else
					{
						pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO;
						ePlayListTypeForChunk = E_MAIN_STREAM;
					}
				}
				else
				{
					return VO_RET_SOURCE2_END;                    
				}
			}
			break;
		}

	    case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		{
			if(pPlaySession->pAlterSubTitlePlayListNode != NULL)
			{
				if(strlen(pPlaySession->pAlterSubTitlePlayListNode->strShortURL) != 0)
				{
					InitChunkNode(&(m_sChunkContainer.sAltrerSubTitleChunk));
					pAdaptiveItem = &(m_sChunkContainer.sAltrerSubTitleChunk.sCurrentAdaptiveStreamItem);
					pAdaptiveItem->Type = VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE;
					ePlayListTypeForChunk = E_X_MEDIA_SUBTITLE_STREAM;
				}
				else
				{
					return VO_RET_SOURCE2_END;
				}
			}
			else
			{
				return VO_RET_SOURCE2_END;
			}
			break;
		}

	    default:
		{
			return VO_RET_SOURCE2_END;
		}
	}

	memset(&sChunkItem, 0, sizeof(S_CHUNCK_ITEM));
	ulRet = m_sM3UManager.GetCurrentChunk(ePlayListTypeForChunk, &sChunkItem);
	if(ulRet != 0)
	{
		ulRet = ConvertErrorCodeToSource2(ulRet);
		return ulRet;
	}

	pDrmItem = (VO_DRM2_HSL_PROCESS_INFO*)(pAdaptiveItem->pChunkDRMInfo);
	memset(pAdaptiveItem->pChunkDRMInfo, 0, sizeof(VO_DRM2_HSL_PROCESS_INFO));
	memcpy(pDrmItem->szCurURL, sChunkItem.strChunkParentURL, strlen(sChunkItem.strChunkParentURL));
	memcpy(pDrmItem->szKeyString, sChunkItem.strEXTKEYLine, strlen(sChunkItem.strEXTKEYLine));
	pDrmItem->uSequenceNum = sChunkItem.ulSequenceIDForKey;
	VOLOGI("Key sequence Id value:%d", sChunkItem.ulSequenceIDForKey);

	pAdaptiveItem->uChunkID = sChunkItem.ulSequenceIDForKey;
	memset(pAdaptiveItem->szUrl, 0, MAXURLLEN);
	memcpy(pAdaptiveItem->szUrl, sChunkItem.strChunckItemURL, strlen(sChunkItem.strChunckItemURL));
	memset(pAdaptiveItem->szRootUrl, 0, MAXURLLEN);
	memcpy(pAdaptiveItem->szRootUrl, sChunkItem.strChunkParentURL, strlen(sChunkItem.strChunkParentURL));
	if(sChunkItem.ullChunckOffset != INAVALIBLEU64 && sChunkItem.ullChunckLen != INAVALIBLEU64)
	{
	    VOLOGI("chunk offset:%d", (VO_U32)sChunkItem.ullChunckOffset);        
	    VOLOGI("chunk length:%d", (VO_U32)sChunkItem.ullChunckLen);
		pAdaptiveItem->ullChunkOffset = sChunkItem.ullChunckOffset;
		pAdaptiveItem->ullChunkSize = sChunkItem.ullChunckLen;
	}
	else
	{
		pAdaptiveItem->ullChunkOffset = INAVALIBLEU64;
		pAdaptiveItem->ullChunkSize = INAVALIBLEU64;
	}
	pAdaptiveItem->ullChunkDeadTime = sChunkItem.ullEndTime;
	pAdaptiveItem->ullChunkLiveTime = sChunkItem.ullBeginTime;
	pAdaptiveItem->ullStartTime = sChunkItem.ullTimeStampOffset;
	pAdaptiveItem->ullDuration = sChunkItem.ulDurationInMsec;
	pAdaptiveItem->sKeyID.uTrackID = sChunkItem.ulPlayListId;
	pAdaptiveItem->uPeriodSequenceNumber = sChunkItem.ulDisSequenceId;

	if(sChunkItem.bDisOccur == VO_TRUE)
	{
		pAdaptiveItem->pStartExtInfo->uFlag = VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE;
		pAdaptiveItem->uFlag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE | VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE | VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_DISCONTINUE;
	}

	switch(sChunkItem.eChunkState)
	{
	    case E_CHUNCK_NORMAL:
		{
			break;
		}
	    case E_CHUNCK_NEW_STREAM:
		{
			pAdaptiveItem->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
			break;
		}
	    case E_CHUNCK_SMOOTH_ADAPTION:
		{
			pAdaptiveItem->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION ;
			break;
		}
	    case E_CHUNCK_FORCE_NEW_STREAM:
		{
			pAdaptiveItem->pStartExtInfo->uFlag = VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE;
			pAdaptiveItem->uFlag |= (VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE | VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE);
			break;
		}
	    case E_CHUNCK_SMOOTH_ADAPTION_EX:
		{
			pAdaptiveItem->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE ;
			pAdaptiveItem->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX ;            
			break;
		}
	}

	*ppChunk = pAdaptiveItem;
	return ulRet;
}

VO_U32  C_HLS_Entity::GetTrackCountByMainPlayList(S_PLAYLIST_NODE* pPlayListInfo, VO_U32*  pulCount)
{
    VO_CHAR*   pGroupId = NULL;
    VO_U32   ulMuxCount = 1;
    VO_U32   ulAlterVideoCount = 0;
    VO_U32   ulAlterAudioCount = 0;
    VO_U32   ulAlterSubTitleCount = 0;
    VO_U32   ulAlterClosedCaptionCount = 0;
    VO_U32   ulTrackCount = 0;
    VO_U32   ulRet = 0;


    if(pPlayListInfo == NULL || pulCount == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	//Get Video Count
	if(strlen(pPlayListInfo->sVarMainStreamAttr.strVideoAlterGroup) != 0)
	{
		pGroupId = pPlayListInfo->sVarMainStreamAttr.strVideoAlterGroup;
		ulRet = m_sM3UManager.GetXMediaStreamCountWithGroupAndType(pGroupId, E_X_MEDIA_VIDEO_STREAM, &ulAlterVideoCount);
	}

	//Get Audio Count
	if(strlen(pPlayListInfo->sVarMainStreamAttr.strAudioAlterGroup) != 0)
	{
		pGroupId = pPlayListInfo->sVarMainStreamAttr.strAudioAlterGroup;
		ulRet = m_sM3UManager.GetXMediaStreamCountWithGroupAndType(pGroupId, E_X_MEDIA_AUDIO_STREAM, &ulAlterAudioCount);
	}

	//Get SubTitle Count
	if(strlen(pPlayListInfo->sVarMainStreamAttr.strSubTitleAlterGroup) != 0)
	{
		pGroupId = pPlayListInfo->sVarMainStreamAttr.strSubTitleAlterGroup;
		ulRet = m_sM3UManager.GetXMediaStreamCountWithGroupAndType(pGroupId, E_X_MEDIA_SUBTITLE_STREAM, &ulAlterSubTitleCount);
	}

	//Get ClosedCaption Count
	if(strlen(pPlayListInfo->sVarMainStreamAttr.strClosedCaptionGroup) != 0)
	{
		pGroupId = pPlayListInfo->sVarMainStreamAttr.strClosedCaptionGroup;
		ulRet = m_sM3UManager.GetXMediaStreamCountWithGroupAndType(pGroupId, E_X_MEDIA_CAPTION_STREAM, &ulAlterClosedCaptionCount);
	}

	ulTrackCount = ulMuxCount + ulAlterVideoCount + ulAlterAudioCount + ulAlterSubTitleCount + ulAlterClosedCaptionCount;
    *pulCount = ulTrackCount;
    return 0;
}


VO_U32  C_HLS_Entity::FillTrackInfoForStream(VO_SOURCE2_STREAM_INFO* pStreamInfo, S_PLAYLIST_NODE* pPlayListInfo)
{
    VO_U32   ulTrackCount = 0;
	S_PLAYLIST_NODE*    pPlayListArray[HLS_MAX_X_MEDIA_COUNT_IN_GROUP]  = {0};
	VO_U32   ulMuxCount = 1;
	VO_U32   ulAlterVideoCount = 0;
	VO_U32   ulAlterAudioCount = 0;
	VO_U32   ulAlterSubTitleCount = 0;
	VO_U32   ulAlterClosedCaptionCount = 0;
    VO_CHAR*   pGroupId = NULL;
    VO_U32   ulRet = 0;
    VO_U32   ulMaxTrackCountWithInType = 0;
	VO_U32   ulTrackIndex = 0;
    VO_U32   ulLoop = 0;
	VO_CHAR   strDesc[256] = {0};
    VO_U32    ulLen = 0;
    VO_U32    ulMuxTrackId = 0;
    VO_U32    ulTrackId = 0;
	VO_SOURCE2_TRACK_INFO   sTrackInfo = {0};

	if(pStreamInfo == NULL || pPlayListInfo == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	//Add the Mux Type
	memset(&sTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
	ulRet = m_pProgFunc->GetParam(m_pProgFunc->hHandle, VOID_PROGINFO_TRACK_ID, &ulTrackId);
    if(ulRet == 0)
	{
		sTrackInfo.uTrackID = pStreamInfo->uStreamID;
        sTrackInfo.uTrackType = VO_SOURCE2_TT_STREAM;
        sTrackInfo.nMuxTrackID = 0xFFFFFFFF;
		ulRet = m_pProgFunc->SetTrack(m_pProgFunc->hHandle, pStreamInfo->uStreamID, &sTrackInfo, VOS2_PROGINFO_NEW);
	}

	memset(pPlayListArray, 0, sizeof(S_PLAYLIST_NODE*)*HLS_MAX_X_MEDIA_COUNT_IN_GROUP);
	m_sM3UManager.GetXMediaStreamArrayWithGroupAndType(pPlayListArray, pPlayListInfo->sVarMainStreamAttr.strVideoAlterGroup, E_X_MEDIA_VIDEO_STREAM, HLS_MAX_X_MEDIA_COUNT_IN_GROUP, &ulAlterVideoCount);
    for(ulLoop=0; ulLoop<ulAlterVideoCount; ulLoop++)
    {
		memset(&sTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
		sTrackInfo.uTrackID = pPlayListArray[ulLoop]->ulPlayListId;
		sTrackInfo.uTrackType = VO_SOURCE2_TT_VIDEO;

		if(pPlayListArray[ulLoop] != NULL && strlen(pPlayListArray[ulLoop]->strShortURL) != 0 )
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_INVALID;
		}
		else
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_STREAM;
		}
		ulRet = m_pProgFunc->SetTrack(m_pProgFunc->hHandle, pStreamInfo->uStreamID, &sTrackInfo, VOS2_PROGINFO_NEW);
    }

    memset(pPlayListArray, 0, sizeof(S_PLAYLIST_NODE*)*HLS_MAX_X_MEDIA_COUNT_IN_GROUP);
    m_sM3UManager.GetXMediaStreamArrayWithGroupAndType(pPlayListArray, pPlayListInfo->sVarMainStreamAttr.strAudioAlterGroup, E_X_MEDIA_AUDIO_STREAM, HLS_MAX_X_MEDIA_COUNT_IN_GROUP, &ulAlterAudioCount);
	for(ulLoop=0; ulLoop<ulAlterAudioCount; ulLoop++)
	{
		memset(&sTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
		memset(strDesc, 0, 256*sizeof(VO_CHAR));
		sprintf(strDesc, "%s-%s", pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strLanguage, pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strName);
		sTrackInfo.uTrackID = pPlayListArray[ulLoop]->ulPlayListId;
		sTrackInfo.uTrackType = VO_SOURCE2_TT_AUDIO;
		memcpy(sTrackInfo.sAudioInfo.chLanguage, strDesc, strlen(strDesc));
		if(pPlayListArray[ulLoop] != NULL && strlen(pPlayListArray[ulLoop]->strShortURL) != 0 )
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_INVALID;
		}
		else
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_STREAM;
		}
        ulRet = m_pProgFunc->SetTrack(m_pProgFunc->hHandle, pStreamInfo->uStreamID, &sTrackInfo, VOS2_PROGINFO_NEW);
	}

	memset(pPlayListArray, 0, sizeof(S_PLAYLIST_NODE*)*HLS_MAX_X_MEDIA_COUNT_IN_GROUP);
	m_sM3UManager.GetXMediaStreamArrayWithGroupAndType(pPlayListArray, pPlayListInfo->sVarMainStreamAttr.strSubTitleAlterGroup, E_X_MEDIA_SUBTITLE_STREAM, HLS_MAX_X_MEDIA_COUNT_IN_GROUP, &ulAlterSubTitleCount);
	for(ulLoop=0; ulLoop<ulAlterSubTitleCount; ulLoop++)
	{
		memset(&sTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
		memset(strDesc, 0, 256*sizeof(VO_CHAR));
		sprintf(strDesc, "%s-%s", pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strLanguage, pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strName);
        sTrackInfo.uTrackID = pPlayListArray[ulLoop]->ulPlayListId;
		sTrackInfo.uTrackType = VO_SOURCE2_TT_SUBTITLE;
		memcpy(sTrackInfo.sSubtitleInfo.chLanguage, strDesc, strlen(strDesc));
		if(pPlayListArray[ulLoop] != NULL && strlen(pPlayListArray[ulLoop]->strShortURL) != 0 )
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_INVALID;
		}
		else
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_STREAM;
		}
        ulRet = m_pProgFunc->SetTrack(m_pProgFunc->hHandle, pStreamInfo->uStreamID, &sTrackInfo, VOS2_PROGINFO_NEW);
	}

    memset(pPlayListArray, 0, sizeof(S_PLAYLIST_NODE*)*HLS_MAX_X_MEDIA_COUNT_IN_GROUP);
	m_sM3UManager.GetXMediaStreamArrayWithGroupAndType(pPlayListArray, pPlayListInfo->sVarMainStreamAttr.strClosedCaptionGroup, E_X_MEDIA_CAPTION_STREAM, HLS_MAX_X_MEDIA_COUNT_IN_GROUP, &ulAlterClosedCaptionCount);
	for(ulLoop=0; ulLoop<ulAlterClosedCaptionCount; ulLoop++)
	{
		memset(&sTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
		memset(strDesc, 0, 256*sizeof(VO_CHAR));
		sprintf(strDesc, "%s-%s", pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strLanguage, pPlayListArray[ulLoop]->sVarXMediaStreamAttr.strName);
        sTrackInfo.uTrackID = pPlayListArray[ulLoop]->ulPlayListId;
		sTrackInfo.uTrackType = VO_SOURCE2_TT_SUBTITLE;
		memcpy(sTrackInfo.sSubtitleInfo.chLanguage, strDesc, strlen(strDesc));
		if(pPlayListArray[ulLoop] != NULL && strlen(pPlayListArray[ulLoop]->strShortURL) != 0 )
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_INVALID;
		}
		else
		{
			sTrackInfo.nMuxTrackID = VODEF_PROGINFO_MUXID_STREAM;
		}
        ulRet = m_pProgFunc->SetTrack(m_pProgFunc->hHandle, pStreamInfo->uStreamID, &sTrackInfo, VOS2_PROGINFO_NEW);
	}

    return 0;
}


VO_U32       C_HLS_Entity::ConvertErrorCodeToSource2(VO_U32   ulErrorCodeInHLS)
{
    switch(ulErrorCodeInHLS)
    {
        case HLS_ERR_NONE:
        {
            return VO_RET_SOURCE2_OK;
        }
        
        case HLS_ERR_VOD_END:
        {
		    VOLOGI("VOD End!");
            return VO_RET_SOURCE2_END;
        }
		case HLS_PLAYLIST_END:
	    {
			VOLOGI("live Playlist End!");
            return VO_RET_SOURCE2_NEEDRETRY;
		}
        case HLS_ERR_EMPTY_POINTER:
        case HLS_ERR_WRONG_MANIFEST_FORMAT:
        case HLS_ERR_LACK_MEMORY:
        case HLS_UN_IMPLEMENT:
        case HLS_ERR_NOT_ENOUGH_BUFFER:
        case HLS_ERR_NOT_EXIST:
        case HLS_ERR_NOT_ENOUGH_PLAYLIST_PARSED:
        case HLS_ERR_NEED_DOWNLOAD:
        case HLS_ERR_ALREADY_EXIST:
        {
            return VO_RET_SOURCE2_ERRORDATA;  
        }
        default:
        {
            return VO_RET_SOURCE2_NOIMPLEMENT;
        }
    }
}

VO_VOID C_HLS_Entity::SetEventCallbackFunc(VO_PTR   pCallbackFunc)
{
    if(pCallbackFunc != NULL)
    {
        m_pEventCallbackFunc = (VO_SOURCE2_EVENTCALLBACK*)pCallbackFunc;
    }
}

VO_VOID    C_HLS_Entity::DeleteAllProgramInfo()
{
    return;
}

VO_VOID    C_HLS_Entity::DeleteStreamInfo(_STREAM_INFO*   pStreamInfo)
{
    return;
}

VO_U32    C_HLS_Entity::UpdateThePlayListForLive()
{    
    S_PLAY_SESSION*     pPlaySession = NULL;
    VO_U32              ulRet = 0;
    VO_CHAR             strRoot[1024] = {0};
    VO_ADAPTIVESTREAM_PLAYLISTDATA    sVarPlayListData = {0};
    VO_CHAR*            pURL = NULL;
    S_PLAYLIST_NODE*    pPlayListNode = NULL;

    ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet != 0)
    {
        return VO_RET_SOURCE2_OK;
    }

    pPlayListNode = pPlaySession->pStreamPlayListNode;
    if(pPlayListNode != NULL &&  strlen(pPlayListNode->strShortURL) != 0 &&
	   pPlayListNode->eChuckPlayListType != M3U_VOD)
    {
        memset(&sVarPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
        RequestManfestAndParsing(&sVarPlayListData, pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
    }
    
	pPlayListNode = pPlaySession->pAlterAudioPlayListNode;
	if(pPlayListNode != NULL &&  strlen(pPlayListNode->strShortURL) != 0 &&
		pPlayListNode->eChuckPlayListType != M3U_VOD)
	{
		memset(&sVarPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
		RequestManfestAndParsing(&sVarPlayListData, pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
	}
    
	pPlayListNode = pPlaySession->pAlterVideoPlayListNode;
	if(pPlayListNode != NULL &&  strlen(pPlayListNode->strShortURL) != 0 &&
		pPlayListNode->eChuckPlayListType != M3U_VOD)
	{
		memset(&sVarPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
		RequestManfestAndParsing(&sVarPlayListData, pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
	}

	pPlayListNode = pPlaySession->pAlterSubTitlePlayListNode;
	if(pPlayListNode != NULL &&  strlen(pPlayListNode->strShortURL) != 0 &&
		pPlayListNode->eChuckPlayListType != M3U_VOD)
	{
		memset(&sVarPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
		RequestManfestAndParsing(&sVarPlayListData, pPlayListNode->strRootURL, pPlayListNode->strShortURL, pPlayListNode->ulPlayListId);
	}

    return 0;
}


VO_U32      C_HLS_Entity::ResetSelInfoInForStream(_STREAM_INFO* pStreamInfo)
{
    VO_U32  ulIndex = 0;
    if(pStreamInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    for(ulIndex=0; ulIndex<pStreamInfo->uTrackCount; ulIndex++)
    {
        if(pStreamInfo->ppTrackInfo != NULL && pStreamInfo->ppTrackInfo[ulIndex] != NULL)
        {
            pStreamInfo->ppTrackInfo[ulIndex]->uSelInfo &= (~VO_SOURCE2_SELECT_SELECTED);
        }
    }

    return 0;
}

VO_U32    C_HLS_Entity::RequestManfestAndParsing(VO_ADAPTIVESTREAM_PLAYLISTDATA*  pPlayListData, VO_CHAR*  pRootPath, VO_CHAR* pManifestURL, VO_U32 ulPlayListId)
{
    VO_U32     ulRet = 0;
    VO_DATABOX_CALLBACK DataBox_CallBack;
    CDataBox databox;
    if(pPlayListData == NULL || pRootPath == NULL || pManifestURL == NULL)
    {
        VOLOGI("some input parameter point is null!");
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    DataBox_CallBack.MallocData = databox.MallocData;
    DataBox_CallBack.pUserData = (VO_PTR)&databox;
    
    memset(pPlayListData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
    memcpy(pPlayListData->szRootUrl, pRootPath, strlen(pRootPath));
    VOLOGI("event callback the root url:%s", pPlayListData->szRootUrl);
    memcpy(pPlayListData->szUrl, pManifestURL, strlen(pManifestURL));
    VOLOGI("event callback the url:%s", pPlayListData->szUrl);
    pPlayListData->pReserve = (VO_PTR)(&DataBox_CallBack);

    if(m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
    {
        ulRet = m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM, (VO_U32)(pPlayListData), 0);
        if(ulRet == 0)
        {
            if(strlen(pPlayListData->szNewUrl) != 0 && pPlayListData->pData != NULL)
            {
                ulRet = ParseHLSPlaylist(pPlayListData, ulPlayListId);
                if(ulRet != 0)
                {
                    VOLOGE("The PlayList Content error! Parse Playlist Error!");
                    return HLS_ERR_WRONG_MANIFEST_FORMAT;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return HLS_ERR_WRONG_MANIFEST_FORMAT;
            }
        }
        else
        {
            VOLOGI("DownLoad File Error!");
            return VO_RET_SOURCE2_NEEDRETRY;
        }
    }
    else
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }    
}

VO_BOOL        C_HLS_Entity::GetLicState()
{
#ifndef _WITHOUT_LIC_CHECK_
	if(m_pHLicCheck != NULL)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
#endif
	return VO_TRUE;
}

VO_U32        C_HLS_Entity::ResetContextForUpdateURL()
{
    StopPlaylistUpdate();
    DeleteAllProgramInfo();
    m_sM3UManager.ResetContextForUpdateURL();
    return 0;
}


VO_U32        C_HLS_Entity::UpdateURL(void*  pPlayListData)
{
/*
    VO_U32   ulRet = 0;
    VO_ADAPTIVESTREAM_PLAYLISTDATA*   pRootPlayList = NULL;
    E_PLAYLIST_TYPE     ePlaylistType = E_UNKNOWN_STREAM;
    S_PLAYLIST_NODE*    pPlayList = NULL;
    S_SESSION_CONTEXT*  pSessionContext = NULL;
    S_PLAY_SESSION*     pPlaySession = NULL;
    VO_U32              ulIndex = 0;
    VO_U32              ulSelectedStreamIndex = 0xffffffff;
    

    pRootPlayList = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)pPlayListData;
    if(pRootPlayList == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;    
    }

    ResetContextForUpdateURL();
    m_pProgramInfo = new _PROGRAM_INFO;
    if(m_pProgramInfo == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(m_pProgramInfo, 0, sizeof(VO_SOURCE2_PROGRAM_INFO));
    m_pProgramInfo->ppStreamInfo = NULL;
    m_pProgramInfo->sProgramType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
    ulRet = ParseHLSPlaylist(pRootPlayList, HLS_ROOT_MANIFEST_ID);
    if(ulRet != 0)
    {
        VOLOGI("Open Fail!");
        return VO_RET_SOURCE2_OPENFAIL;
    }

    ulRet = GenerateTheProgramInfo();
    if(ulRet != 0)
    {
        VOLOGE("Generate the Program Info Failed!");
        return VO_RET_SOURCE2_FAIL;
    }

    pSessionContext = m_sM3UManager.GetSessionContext();
    ulRet = PreparePlayFromSessionContext(pSessionContext);
    if(ulRet != 0)
    {
        VOLOGI("Prepare failed!");
        return ulRet;
    }

*/
    return 0;
}


VO_U32        C_HLS_Entity::PreparePlayFromSessionContext(S_SESSION_CONTEXT* pSessionContext)
{
    S_PLAY_SESSION*    pPlaySession = NULL;
    S_PLAYLIST_NODE*   pPlayList = NULL;
    VO_U32 ulRet = 0;
    if(pSessionContext == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    
    if(pSessionContext->ulCurrentMainStreamPlayListId != INVALID_PLALIST_ID)
    {
        pPlayList = m_sM3UManager.FindPlayListById(pSessionContext->ulCurrentMainStreamPlayListId);
        if(pPlayList != NULL)
        {
            ulRet = NotifyToParse(pPlayList->strRootURL, pPlayList->strShortURL, pPlayList->ulPlayListId);
        }
    }

    if(pSessionContext->ulCurrentAlterVideoStreamPlayListId != INVALID_PLALIST_ID)
    {
        pPlayList = m_sM3UManager.FindPlayListById(pSessionContext->ulCurrentAlterVideoStreamPlayListId);
        if(pPlayList != NULL)
        {
            ulRet = NotifyToParse(pPlayList->strRootURL, pPlayList->strShortURL, pPlayList->ulPlayListId);
        }
    }

    if(pSessionContext->ulCurrentAlterAudioStreamPlayListId != INVALID_PLALIST_ID)
    {
        pPlayList = m_sM3UManager.FindPlayListById(pSessionContext->ulCurrentAlterAudioStreamPlayListId);
        if(pPlayList != NULL)
        {
            ulRet = NotifyToParse(pPlayList->strRootURL, pPlayList->strShortURL, pPlayList->ulPlayListId);
        }
    }

    if(pSessionContext->ulCurrentAlterSubTitleStreamPlayListId != INVALID_PLALIST_ID)
    {
        pPlayList = m_sM3UManager.FindPlayListById(pSessionContext->ulCurrentAlterSubTitleStreamPlayListId);
        if(pPlayList != NULL)
        {
            ulRet = NotifyToParse(pPlayList->strRootURL, pPlayList->strShortURL, pPlayList->ulPlayListId);
        }
    }

    m_sM3UManager.RestoreCurrentContext();
    return 0;
}

VO_U32        C_HLS_Entity::AdjustTheSequenceIDForMainStream()
{
    VO_U32   ulRet = 0;
    S_PLAY_SESSION*      pPlaySession = NULL;

    ulRet = m_sM3UManager.GetCurReadyPlaySession(&pPlaySession);
    if(ulRet != 0)
    {
        VOLOGI("Play Session is not ready!");
        return VO_RET_SOURCE2_ERRORDATA;
    }

    if(pPlaySession->pAlterAudioPlayListNode != NULL && strlen(pPlaySession->pAlterAudioPlayListNode->strShortURL) != 0)
    {
        VOLOGI("The AlterAudio is available, the url is %s!", pPlaySession->pAlterAudioPlayListNode->strShortURL);
    }
    else
    {
        VOLOGI("The AlterAudio is disable, the url is %s!");
        return 0;
    }

    VOLOGI("Set the MainStream SequenceId to:%d", pPlaySession->ulAlterAudioSequenceId);
    pPlaySession->ulMainStreamSequenceId = pPlaySession->ulAlterAudioSequenceId;
    return 0;
}

VO_VOID        C_HLS_Entity::InitChunkNode(S_CHUNK_NODE*  pChunkNode)
{
	if(pChunkNode == NULL)
	{
		return;
	}

	memset(pChunkNode, 0, sizeof(S_CHUNK_NODE));
	pChunkNode->sCurrentAdaptiveStreamItem.pChunkDRMInfo = &(pChunkNode->sCurrentDrm);
	pChunkNode->sCurrentAdaptiveStreamItem.pStartExtInfo = &(pChunkNode->sCurrentStartExt);
	return;
}
