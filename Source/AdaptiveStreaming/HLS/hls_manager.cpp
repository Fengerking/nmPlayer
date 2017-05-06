#include "string.h"
#include "hls_manager.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

C_M3U_Manager::C_M3U_Manager()
{
    ResetSessionContext();
    m_pPlayListNodeHeader = NULL;
    m_pPlayListNodeTail = NULL;
    m_eRootPlayListType = M3U_UNKNOWN_PLAYLIST;
	m_ullUTCTime = 0;
    m_ulSystemtimeForUTC = 0;    
    memset(&m_sSessionContext, 0, sizeof(S_SESSION_CONTEXT));
    m_sSessionContext.ulCurrentMainStreamPlayListId = INVALID_PLALIST_ID;
    m_sSessionContext.ulCurrentAlterVideoStreamPlayListId = INVALID_PLALIST_ID;
    m_sSessionContext.ulCurrentAlterAudioStreamPlayListId = INVALID_PLALIST_ID;
    m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId = INVALID_PLALIST_ID;    
}

C_M3U_Manager::~C_M3U_Manager()
{
    ReleaseAllPlayList();
}

VO_U32    C_M3U_Manager::ParseManifest(VO_BYTE* pPlayListContent,VO_U32 ulPlayListContentLength, VO_CHAR*   pPlayListURL, VO_U32 ulPlayListId)
{
    VO_U32    ulRet = 0;
    M3U_MANIFEST_TYPE  eManifestType = M3U_UNKNOWN_PLAYLIST;
    M3U_CHUNCK_PLAYLIST_TYPE eChucklistType = M3U_INVALID_CHUNK_PLAYLIST_TYPE;
    M3U_CHUNCK_PLAYLIST_TYPE_EX  eChunklistTypeEx = M3U_INVALID_EX;
    S_PLAYLIST_NODE*             pPlayListNode = NULL;
    if(pPlayListContent == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    ulRet = m_sParser.ParseManifest(pPlayListContent, ulPlayListContentLength);
	if(ulRet != 0)
    {
        VOLOGI("Parse the playlist error");
        return VO_RET_SOURCE2_ERRORDATA;
	}

    ulRet = m_sParser.GetManifestType(&eManifestType, &eChucklistType, &eChunklistTypeEx);
    if(ulRet != 0)
    {
		VOLOGI("Parse the playlist error");
		return VO_RET_SOURCE2_ERRORDATA;
	}

    
    if(m_eRootPlayListType == M3U_UNKNOWN_PLAYLIST)
    {
		m_eRootPlayListType = eManifestType;
    }

    switch(eManifestType)
    {
        case M3U_CHUNK_PLAYLIST:
        {
			if(m_eRootPlayListType == M3U_CHUNK_PLAYLIST && m_pPlayListNodeHeader == NULL)
            {
                ulRet = CreatePlayList(&pPlayListNode);
				if(ulRet != 0 )
				{
					return ulRet;
				}
				else
				{
                    pPlayListNode->ePlayListType = E_MAIN_STREAM;
					m_pPlayListNodeHeader = m_pPlayListNodeTail = pPlayListNode;
					pPlayListNode->ulPlayListId = 0;
                    m_sPlaySession.pStreamPlayListNode = pPlayListNode;
                    memcpy(pPlayListNode->strRootURL, pPlayListURL, strlen(pPlayListURL));
                    memcpy(pPlayListNode->strShortURL, pPlayListURL, strlen(pPlayListURL));
				}
            }

            pPlayListNode = FindPlayListById(ulPlayListId);
			if(pPlayListNode == NULL)
			{
				VOLOGI("Can't find the PlayList with id:%d", ulPlayListId);
                return VO_RET_SOURCE2_ERRORDATA;
			}

            ReleasePlayList(pPlayListNode);
			pPlayListNode->eManifestType = eManifestType;
			pPlayListNode->eChuckPlayListType = eChucklistType;
            pPlayListNode->eChunkPlayListTypeEx = eChunklistTypeEx;
            BuildMediaPlayList(pPlayListURL, pPlayListNode->ulPlayListId);
            break;
        }
        case M3U_STREAM_PLAYLIST:
        {
            BuildMasterPlayList(pPlayListURL);
            break;
	    }
    }

    return 0;
}

VO_U32    C_M3U_Manager::BuildMasterPlayList(VO_CHAR*  pPlayListURL)
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    S_TAG_NODE*        pTagNode = NULL;
    VO_U32             ulStreamCount = 0;
    VO_U32             ulXVideoCount = 0;
	VO_U32             ulXAudioCount = 0;
	VO_U32             ulXSubTitleCount = 0;
	VO_U32             ulXCaptionCount = 0;
	VO_U32             ulIFrameCount = 0;
    S_PLAYLIST_NODE*   pLastStreamPlayListNode = NULL;
    VO_U32             ulRet = 0;
    E_PLAYLIST_TYPE    ePlayListType = E_UNKNOWN_STREAM;
    VO_U32             ulPlayListId = 0;
    VO_U32             ulLength = 0;

    ulRet = m_sParser.GetTagList(&pTagNode);
    if(ulRet != 0)
    {
		return VO_RET_SOURCE2_ERRORDATA;
    }

    while(pTagNode != NULL)
    {
        switch(pTagNode->ulTagIndex)
        {
            case STREAM_INF_NAME_INDEX:
			case MEDIA_NAME_INDEX:
			case I_FRAME_STREAM_INF_NAME_INDEX:
			{
				ulRet = GetMediaTypeFromTagNode(&ePlayListType, pTagNode);
                if(ulRet == 0)
                {
					ulRet = CreatePlayList(&pPlayListNode);
					if(ulRet != 0)
					{
						return VO_RET_SOURCE2_ERRORDATA;
					}

					pPlayListNode->ePlayListType = ePlayListType;
                    memcpy(pPlayListNode->strRootURL, pPlayListURL, strlen(pPlayListURL));
					FillPlayListInfo(pPlayListNode, pTagNode);
                    switch(ePlayListType)
					{
					    case E_MAIN_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_MAIN_STREAM+ulStreamCount;
							pLastStreamPlayListNode = pPlayListNode;
							ulStreamCount++;
							break;
						}
						case E_X_MEDIA_AUDIO_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_X_AUDIO+ulXAudioCount;
							ulXAudioCount++;
							break;
						}
						case E_X_MEDIA_VIDEO_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_X_VIDEO+ulXVideoCount;
							ulXVideoCount++;
							break;
						}
						case E_X_MEDIA_SUBTITLE_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_X_SUBTITLE+ulXSubTitleCount;
							ulXSubTitleCount++;
							break;
						}
						case E_X_MEDIA_CAPTION_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_X_CC+ulXCaptionCount;
							ulXCaptionCount++;
							break;
						}
						case E_I_FRAME_STREAM:
						{
							ulPlayListId =HLS_INDEX_OFFSET_I_FRAME_STREAM+ulIFrameCount;
							if(pLastStreamPlayListNode != NULL)
							{
								pLastStreamPlayListNode->ulExtraIFramePlayListId = ulPlayListId;
							}
							ulIFrameCount++;
							break;
						}
						default:
						{
							VOLOGI("M3u8 content error!");
						}
					}

					pPlayListNode->ulPlayListId = ulPlayListId;
				}
				else
				{
					VOLOGI("M3u8 content error!");
				}

				AddPlayListNode(pPlayListNode);
				break;
			}
			case NORMAL_URI_NAME_INDEX:
			{
				if(pLastStreamPlayListNode != NULL)
				{
					if(pTagNode->ppAttrArray[URI_LINE_ATTR_ID] != NULL &&
					   pTagNode->ppAttrArray[URI_LINE_ATTR_ID]->pString != NULL)
					{
						ulLength = (strlen(pTagNode->ppAttrArray[URI_LINE_ATTR_ID]->pString)>1023)?1023:(strlen(pTagNode->ppAttrArray[URI_LINE_ATTR_ID]->pString));
						memcpy(pLastStreamPlayListNode->strShortURL, pTagNode->ppAttrArray[URI_LINE_ATTR_ID]->pString, ulLength);
					}
				}
			}
		}

		pTagNode = pTagNode->pNext;
    }

    return 0;
}

VO_U32    C_M3U_Manager::BuildMediaPlayList(VO_CHAR*  pPlayListURL,VO_U32  ulPlayListId)
{
    VO_U32             ulRet = 0;
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    S_TAG_NODE*        pTagNode = NULL;

    S_TAG_NODE*        pTagNodeSequenceId = NULL;
    S_TAG_NODE*        pTagNodeURI = NULL;
    S_TAG_NODE*        pTagNodeInf = NULL;
    S_TAG_NODE*        pTagNodeByteRange = NULL;
    S_TAG_NODE*        pTagNodeKeyLine = NULL;
    S_TAG_NODE*        pTagNodeProgramDataTime = NULL;
    S_TAG_NODE*        pTagNodeXMap = NULL;
	S_TAG_NODE*        pTagNodeDis = NULL;
	S_TAG_NODE*        pTagNodeDisSequence = NULL;

    VO_U32             ulSequenceIdValue = 0;
    VO_U32             ulDisSequenceIdValue = 0;
    VO_U64             illCurrentOffset = INAVALIBLEU64;
    VO_U64             illCurrentLength = INAVALIBLEU64;
    VO_U32             ulChunkDurationInMs = 0;
	VO_S64             illProgramTime = 0;
	VO_U32             ulPlayListDurationInMs = 0;
    VO_BOOL            bFindDis = VO_FALSE;

	pPlayListNode = FindPlayListById(ulPlayListId);
    if(pPlayListNode == NULL)
    {
		return VO_RET_SOURCE2_ERRORDATA;
    }

    ulRet = m_sParser.GetTagList(&pTagNode);
    if(ulRet != 0)
    {
		return VO_RET_SOURCE2_ERRORDATA;
    }

    while(pTagNode != NULL)
    {
        switch(pTagNode->ulTagIndex)
        {
            case  MEDIA_SEQUENCE_NAME_INDEX:
            {
				if(pTagNode->ppAttrArray[MEDIA_SEQUENCE_VALUE_ATTR_ID] != NULL)
				{
					ulSequenceIdValue = (VO_U32)(pTagNode->ppAttrArray[MEDIA_SEQUENCE_VALUE_ATTR_ID]->illIntValue);
				}
				break;
			}

            case START_NAME_INDEX:
            {
                pPlayListNode->ulXStartExist = 1;
                pPlayListNode->ilXStartValue = (VO_S32)(pTagNode->ppAttrArray[X_START_TIMEOFFSET_ATTR_ID]->illIntValue)*1000;
                break;
            }

			case TARGETDURATION_NAME_INDEX:
			{
				if(pTagNode->ppAttrArray[TARGETDURATION_VALUE_ATTR_ID] != NULL)
                {
					pPlayListNode->ulTargetDuration = (VO_U32)(pTagNode->ppAttrArray[TARGETDURATION_VALUE_ATTR_ID]->illIntValue*1000);
				}
				break;
			}

			case  BYTERANGE_NAME_INDEX:
            {
				if(pTagNode->ppAttrArray[BYTERANGE_RANGE_ATTR_ID] != NULL)
				{
					if(pTagNode->ppAttrArray[BYTERANGE_RANGE_ATTR_ID]->pRangeInfo->ullOffset == INAVALIBLEU64)
					{
                        illCurrentOffset = illCurrentOffset;
					}
					else
					{
						illCurrentOffset = pTagNode->ppAttrArray[BYTERANGE_RANGE_ATTR_ID]->pRangeInfo->ullOffset;					
					}
					illCurrentLength = pTagNode->ppAttrArray[BYTERANGE_RANGE_ATTR_ID]->pRangeInfo->ullLength;
				}

				break;
            }

			case  INF_NAME_INDEX:
            {
				if(pTagNode->ppAttrArray[INF_DURATION_ATTR_ID] != NULL)
				{
					ulChunkDurationInMs = (VO_U32)(1000*(pTagNode->ppAttrArray[INF_DURATION_ATTR_ID]->fFloatValue));
				}
				pTagNodeInf = pTagNode;
				break;
			}

            case  KEY_NAME_INDEX:
		    {
				pTagNodeKeyLine = pTagNode;
				break;
		    }

			case NORMAL_URI_NAME_INDEX:
            {
				pTagNodeURI = pTagNode;
				break;
			}

			case DISCONTINUITY_SEQUENCE_NAME_INDEX:
			{
				if(pTagNode->ppAttrArray[DISCONTINUITY_SEQUENCE_NAME_INDEX] != NULL)
				{
					ulDisSequenceIdValue = (VO_U32)(pTagNode->ppAttrArray[DISCONTINUITY_SEQUENCE_NAME_INDEX]->illIntValue);
				}
				break;
			}

			case PROGRAM_DATE_TIME_NAME_INDEX:
			{
				illProgramTime = 1;
				break;
			}

			case DISCONTINUITY_NAME_INDEX:
		    {
				bFindDis = VO_TRUE;
				ulDisSequenceIdValue++;
				break;
			}

			case MAP_NAME_INDEX:
            {
				pTagNodeXMap = pTagNode;
				break;
			}
		}

        if(pTagNodeURI != NULL)
		{
			AssembleChunkItem(pTagNodeInf, pTagNodeURI, pTagNodeKeyLine, illProgramTime, 
				              illCurrentOffset, illCurrentLength, ulSequenceIdValue, ulDisSequenceIdValue, 
							  bFindDis, ulPlayListId, pPlayListURL, pTagNodeXMap);

			//Reset and Modify the value
			pTagNodeInf = NULL;
			pTagNodeURI = NULL;
			if(illProgramTime != 0)
			{
				illProgramTime += ulChunkDurationInMs;
			}

			ulPlayListDurationInMs += ulChunkDurationInMs;
			illCurrentOffset += illCurrentLength;
			ulSequenceIdValue++;
			bFindDis = VO_FALSE;
		}

		pTagNode = pTagNode->pNext;
    }

    if(pPlayListNode->pChunkItemHeader != NULL)
    {
		pPlayListNode->ulCurrentMinSequenceIdInDvrWindow = pPlayListNode->pChunkItemHeader->ulSequenceIDForKey;
    }

	if(pPlayListNode->pChunkItemTail != NULL)
	{
		pPlayListNode->ulCurrentMaxSequenceIdInDvrWindow = pPlayListNode->pChunkItemTail->ulSequenceIDForKey;
	}

	pPlayListNode->ulCurrentDvrDuration = ulPlayListDurationInMs;
    if(pPlayListNode->ulLastChunkDuration == 0)
    {
        pPlayListNode->ulLastChunkDuration = pPlayListNode->ulTargetDuration;    
    }

    VOLOGI("PlayList ID:%d, Min Seq:%d, Max Seq:%d, Dvr Duration:%d", pPlayListNode->ulPlayListId, pPlayListNode->ulCurrentMinSequenceIdInDvrWindow,
            pPlayListNode->ulCurrentMaxSequenceIdInDvrWindow, pPlayListNode->ulCurrentDvrDuration);

    AdjustLiveTimeAndDeadTimeForLive(pPlayListNode);
    return 0;
}


VO_VOID    C_M3U_Manager::AdjustLiveTimeAndDeadTimeForLive(S_PLAYLIST_NODE* pPlayList)
{
    VO_U64   ullTimeLive = 0;
    VO_U64   ullDeadLive = 0;
    VO_U64   ullStartOffset = 0;
    VO_U32   ulSystemGetManifest = voOS_GetSysTime();
    S_CHUNCK_ITEM*   pChunkItem = NULL;

    if(pPlayList == NULL)
    {
        return;
    }

    pChunkItem = pPlayList->pChunkItemHeader;

    ullTimeLive = m_ullUTCTime +(VO_U64)ulSystemGetManifest - (VO_U64)m_ulSystemtimeForUTC - (VO_U64)(pPlayList->ulCurrentDvrDuration);
    ullDeadLive = m_ullUTCTime +(VO_U64)ulSystemGetManifest - (VO_U64)m_ulSystemtimeForUTC + (VO_U64)(pPlayList->ulCurrentDvrDuration) + (VO_U64)(pPlayList->ulTargetDuration);
    
    while(pChunkItem != NULL)
    {
        switch(pPlayList->eChuckPlayListType)
        {
            case M3U_VOD:
            {
                pChunkItem->ullBeginTime = INAVALIBLEU64;
                pChunkItem->ullEndTime = INAVALIBLEU64;
                break;
            }

            case M3U_LIVE:
            case M3U_EVENT:
            {                
                pChunkItem->ullBeginTime = ullTimeLive;
                pChunkItem->ullEndTime = ullDeadLive;
                break;
            }       
        }

        pChunkItem->ullTimeStampOffset = ullStartOffset;
        ullTimeLive += pChunkItem->ulDurationInMsec;
        ullDeadLive += pChunkItem->ulDurationInMsec;
        ullStartOffset += pChunkItem->ulDurationInMsec;
        pChunkItem = pChunkItem->pNext;
    }

    return ;
}

S_PLAYLIST_NODE*    C_M3U_Manager::FindPlayListById(VO_U32  ulPlayListId)
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
	pPlayListNode = m_pPlayListNodeHeader;

	if(m_eRootPlayListType == M3U_CHUNK_PLAYLIST)
	{
		return m_pPlayListNodeHeader;
	}
	
	while(pPlayListNode != NULL)
    {
		if(pPlayListNode->ulPlayListId == ulPlayListId)
		{
			break;
		}
		pPlayListNode = pPlayListNode->pNext;
	}

    return pPlayListNode;
}

VO_VOID    C_M3U_Manager::ResetSessionContext()
{
    memset(&m_sPlaySession, 0, sizeof(S_PLAY_SESSION));
    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;    
    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
}


VO_VOID    C_M3U_Manager::ReleaseAllPlayList()
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    
    pPlayListNode = m_pPlayListNodeHeader;
    if(pPlayListNode == NULL)
    {
        return;
    }
    
    while(pPlayListNode != NULL)
    {
		m_pPlayListNodeHeader = pPlayListNode->pNext;
        ReleasePlayList(pPlayListNode);
        delete pPlayListNode;
		pPlayListNode = m_pPlayListNodeHeader;
    }    
	return;
}

VO_VOID    C_M3U_Manager::ReleasePlayList(S_PLAYLIST_NODE*   pPlayListNode)
{
    S_CHUNCK_ITEM*     pChunkItem = NULL;
    pChunkItem = pPlayListNode->pChunkItemHeader;

    if(pChunkItem == NULL)
    {
        return;
    }

    while(pChunkItem != NULL)
    {
        pPlayListNode->pChunkItemHeader = pChunkItem->pNext;
        delete pChunkItem;
        pChunkItem = pPlayListNode->pChunkItemHeader;
    }

	return;
}

VO_U32    C_M3U_Manager::CreatePlayList(S_PLAYLIST_NODE**   ppPlayListNode)
{
    S_PLAYLIST_NODE*    pPlayListNode = NULL;
    
	pPlayListNode = new S_PLAYLIST_NODE;
    if(pPlayListNode == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(pPlayListNode, 0, sizeof(S_PLAYLIST_NODE));
	pPlayListNode->eManifestType = M3U_UNKNOWN_PLAYLIST;
    pPlayListNode->eChuckPlayListType = M3U_INVALID_CHUNK_PLAYLIST_TYPE;
	pPlayListNode->eChunkPlayListTypeEx = M3U_NORMAL;
	pPlayListNode->ePlayListType = E_UNKNOWN_STREAM;

	*ppPlayListNode = pPlayListNode;
    return 0;
}

VO_U32    C_M3U_Manager::AssembleChunkItem(S_TAG_NODE* pTagInf, S_TAG_NODE* pURI, S_TAG_NODE* pTagNodeKeyLine, VO_S64  illProgramValue, 
							VO_S64 illOffset, VO_S64 illLength, VO_U32 ulSequenceId, VO_U32  ulDisSequence,
							VO_BOOL  bDisOccur, VO_U32  ulPlayListId, VO_CHAR*  pPlayListURI, S_TAG_NODE* pXMapTag)
{
    S_PLAYLIST_NODE*    pPlayListNode = NULL;
    S_CHUNCK_ITEM*    pChunkItem = NULL;
    VO_U32            ulLength = 0;

	if(pTagInf == NULL || pURI == NULL)
	{
        return VO_RET_SOURCE2_ERRORDATA;
	}

    if(pTagInf->ppAttrArray[INF_DURATION_ATTR_ID] == NULL || pURI->ppAttrArray[URI_LINE_ATTR_ID] == NULL)
    {
		return VO_RET_SOURCE2_ERRORDATA;
	}

    pPlayListNode = FindPlayListById(ulPlayListId);
    if(pPlayListNode == NULL)
    {
        return VO_RET_SOURCE2_ERRORDATA;
    }

	pChunkItem = new S_CHUNCK_ITEM;
    if(pChunkItem == NULL)
    {
		return VO_RET_SOURCE2_ERRORDATA;
    }

	memset(pChunkItem, 0, sizeof(S_CHUNCK_ITEM));

	pChunkItem->ulPlayListId = ulPlayListId;
	pChunkItem->ulSequenceIDForKey = ulSequenceId;
    pChunkItem->ullProgramDateTime = illProgramValue;
    pChunkItem->ulDisSequenceId = ulDisSequence;
    pChunkItem->ulDurationInMsec = (VO_U32)(1000*(pTagInf->ppAttrArray[INF_DURATION_ATTR_ID]->fFloatValue));

	if((pURI->ulAttrSet & (1<<URI_LINE_ATTR_ID)) != 0)
	{
		ulLength = (strlen(pURI->ppAttrArray[URI_LINE_ATTR_ID]->pString)<1024)?(strlen(pURI->ppAttrArray[URI_LINE_ATTR_ID]->pString)):1023;
		memcpy(pChunkItem->strChunckItemURL, pURI->ppAttrArray[URI_LINE_ATTR_ID]->pString, ulLength);
	}

    if((pTagInf->ulAttrSet & (1<<INF_DESC_ATTR_ID)) != 0)
	{
		ulLength = (strlen(pTagInf->ppAttrArray[INF_DESC_ATTR_ID]->pString)<64)?(strlen(pTagInf->ppAttrArray[INF_DESC_ATTR_ID]->pString)):63;
		memcpy(pChunkItem->strChunckItemTitle, pTagInf->ppAttrArray[INF_DESC_ATTR_ID]->pString, ulLength);
	}
    
	if(illLength == INAVALIBLEU64)
    {
		pChunkItem->eChunckContentType = E_NORMAL_WHOLE_CHUNCK_NODE;
    }
	else
	{
		pChunkItem->eChunckContentType = E_NORMAL_PART_CHUNCK_NODE;
	}

	if(bDisOccur == VO_TRUE)
    {
		pChunkItem->bDisOccur = VO_TRUE;
	}
	else
	{
		pChunkItem->bDisOccur = VO_FALSE;
	}
    
    pChunkItem->ullChunckLen = illLength;
    pChunkItem->ullChunckOffset = illOffset;
    
	ulLength = (strlen(pPlayListURI)<1024)?(strlen(pPlayListURI)):1023;
	memcpy(pChunkItem->strChunkParentURL, pPlayListURI, ulLength);

	if(pTagNodeKeyLine != NULL && pTagNodeKeyLine->ppAttrArray[KEY_LINE_CONTENT] != NULL)
	{
		ulLength = (strlen(pTagNodeKeyLine->ppAttrArray[KEY_LINE_CONTENT]->pString)<1024)?(strlen(pTagNodeKeyLine->ppAttrArray[KEY_LINE_CONTENT]->pString)):1023;
		memcpy(pChunkItem->strEXTKEYLine, pTagNodeKeyLine->ppAttrArray[KEY_LINE_CONTENT]->pString, ulLength);
	}

    if(pPlayListNode->pChunkItemHeader == NULL)
    {
		pPlayListNode->pChunkItemHeader = pPlayListNode->pChunkItemTail = pChunkItem;
    }
	else
	{
        pPlayListNode->pChunkItemTail->pNext = pChunkItem;
		pPlayListNode->pChunkItemTail = pChunkItem;
	}

	return 0;
}

VO_U32    C_M3U_Manager::GetMediaTypeFromTagNode(E_PLAYLIST_TYPE* pePlayListType, S_TAG_NODE*pTagNode)
{
    if(pTagNode == NULL || pePlayListType == NULL)
    {
		return VO_RET_SOURCE2_ERRORDATA;
    }

	switch(pTagNode->ulTagIndex)
    {
        case STREAM_INF_NAME_INDEX:
		{
			*pePlayListType = E_MAIN_STREAM;
			return 0;
		}
		case MEDIA_NAME_INDEX:
	    {
			if(pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID] != NULL &&
			   pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID]->pString != NULL)
			{
				if(strcmp(pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID]->pString, "AUDIO") == 0)
				{
					*pePlayListType = E_X_MEDIA_AUDIO_STREAM;
					return 0;
				}

				if(strcmp(pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID]->pString, "VIDEO") == 0)
				{
					*pePlayListType = E_X_MEDIA_VIDEO_STREAM;
					return 0;
				}

				if(strcmp(pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID]->pString, "SUBTITLES") == 0)
				{
					*pePlayListType = E_X_MEDIA_SUBTITLE_STREAM;
					return 0;
				}

				if(strcmp(pTagNode->ppAttrArray[MEDIA_TYPE_ATTR_ID]->pString, "CLOSED-CAPTIONS") == 0)
				{
					*pePlayListType = E_X_MEDIA_CAPTION_STREAM;
					return 0;
				}
			}
			break;
		}
		case I_FRAME_STREAM_INF_NAME_INDEX:
	    {
			*pePlayListType = E_I_FRAME_STREAM;
			return 0;
		}
    }

	return VO_RET_SOURCE2_ERRORDATA;
}

VO_VOID    C_M3U_Manager::AddPlayListNode(S_PLAYLIST_NODE*  pPlayList)
{
	if(pPlayList == NULL )
    {
		return;
    }

	if(m_pPlayListNodeHeader == NULL)
	{
		m_pPlayListNodeHeader = m_pPlayListNodeTail = pPlayList;
	}
	else
	{
		m_pPlayListNodeTail->pNext = pPlayList;
		m_pPlayListNodeTail = pPlayList;
	}
}

VO_VOID    C_M3U_Manager::FillPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode)
{
    VO_U32   ulLength = 0;


	switch(pPlayList->ePlayListType)
    {
	    case E_MAIN_STREAM:
		{
            FillMainStreamPlayListInfo(pPlayList, pTagNode);
			break;
		}
		case E_I_FRAME_STREAM:
		{
			FillIFramePlayListInfo(pPlayList, pTagNode);
			break;
		}

        case E_X_MEDIA_AUDIO_STREAM:
        case E_X_MEDIA_VIDEO_STREAM:
		case E_X_MEDIA_SUBTITLE_STREAM:
		case E_X_MEDIA_CAPTION_STREAM:
		{
			FillXMediaPlayListInfo(pPlayList, pTagNode);
			break;
		}
    }

	return;
}

VO_VOID    C_M3U_Manager::FillIFramePlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode)
{
	VO_U32  ulLength = 0;
    VO_CHAR*   pDesc = NULL;

	if(pTagNode->ppAttrArray[IFRAME_STREAM_URI_ATTR_ID] != NULL &&
       pTagNode->ppAttrArray[IFRAME_STREAM_URI_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[IFRAME_STREAM_URI_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<1023)?(strlen(pDesc)):1023;
		memcpy(pPlayList->strShortURL, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[IFRAME_STREAM_BANDWIDTH_ATTR_ID] != NULL )
	{
		pPlayList->sVarIFrameSteamAttr.ulBitrate = (VO_U32)(pTagNode->ppAttrArray[IFRAME_STREAM_BANDWIDTH_ATTR_ID]->illIntValue);
	}

	if(pTagNode->ppAttrArray[IFRAME_STREAM_CODECS_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[IFRAME_STREAM_CODECS_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[IFRAME_STREAM_CODECS_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<1023)?(strlen(pDesc)):1023;
		memcpy(pPlayList->sVarIFrameSteamAttr.strCodecDesc, pDesc, ulLength);
	}

	return;
}

VO_VOID    C_M3U_Manager::FillMainStreamPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode)
{
    VO_U32  ulLength = 0;
    VO_CHAR*   pDesc = NULL;
    
	if(pTagNode->ppAttrArray[STREAM_INF_BANDWIDTH_ATTR_ID] != NULL)
    {
		pPlayList->sVarMainStreamAttr.ulBitrate = (VO_U32)(pTagNode->ppAttrArray[STREAM_INF_BANDWIDTH_ATTR_ID]->illIntValue);
    }

	if(pTagNode->ppAttrArray[STREAM_INF_CODECS_ATTR_ID] != NULL &&
	   pTagNode->ppAttrArray[STREAM_INF_CODECS_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[STREAM_INF_CODECS_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarMainStreamAttr.strCodecDesc, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[STREAM_INF_VIDEO_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[STREAM_INF_VIDEO_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[STREAM_INF_VIDEO_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarMainStreamAttr.strVideoAlterGroup, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[STREAM_INF_AUDIO_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[STREAM_INF_AUDIO_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[STREAM_INF_AUDIO_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarMainStreamAttr.strAudioAlterGroup, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[STREAM_INF_SUBTITLE_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[STREAM_INF_SUBTITLE_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[STREAM_INF_SUBTITLE_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarMainStreamAttr.strSubTitleAlterGroup, pDesc, ulLength);
	}
	
	if(pTagNode->ppAttrArray[STREAM_INF_CLOSED_CAPTIONS_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[STREAM_INF_CLOSED_CAPTIONS_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[STREAM_INF_CLOSED_CAPTIONS_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarMainStreamAttr.strClosedCaptionGroup, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[STREAM_INF_RESOLUTION_ATTR_ID] != NULL &&
	   pTagNode->ppAttrArray[STREAM_INF_RESOLUTION_ATTR_ID]->pResolution != NULL	)
	{
		pPlayList->sVarMainStreamAttr.sResolution.ulWidth = pTagNode->ppAttrArray[STREAM_INF_RESOLUTION_ATTR_ID]->pResolution->ulWidth;
		pPlayList->sVarMainStreamAttr.sResolution.ulHeight = pTagNode->ppAttrArray[STREAM_INF_RESOLUTION_ATTR_ID]->pResolution->ulHeight;
	}
}

VO_VOID    C_M3U_Manager::FillXMediaPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode)
{
	VO_U32  ulLength = 0;
	VO_CHAR*   pDesc = NULL;
    
    pPlayList->sVarXMediaStreamAttr.eStreamType = pPlayList->ePlayListType;

	if(pTagNode->ppAttrArray[MEDIA_GROUP_ID_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_GROUP_ID_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_GROUP_ID_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarXMediaStreamAttr.strGroupId, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_NAME_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_NAME_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_NAME_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarXMediaStreamAttr.strName, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_LANGUAGE_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_LANGUAGE_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_LANGUAGE_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarXMediaStreamAttr.strLanguage, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_ASSOC_LANGUAGE_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_ASSOC_LANGUAGE_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_ASSOC_LANGUAGE_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<63)?(strlen(pDesc)):63;
		memcpy(pPlayList->sVarXMediaStreamAttr.strAssocLanguage, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_DEFAULT_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_DEFAULT_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_DEFAULT_ATTR_ID]->pString;
		if(strcmp(pDesc, "YES") == 0)
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 1;
		}
		else
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 0;
		}
	}

	if(pTagNode->ppAttrArray[MEDIA_AUTOSELECT_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_AUTOSELECT_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_AUTOSELECT_ATTR_ID]->pString;
		if(strcmp(pDesc, "YES") == 0)
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 1;
		}
		else
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 0;
		}
	}

	if(pTagNode->ppAttrArray[MEDIA_FORCED_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_FORCED_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_FORCED_ATTR_ID]->pString;
		if(strcmp(pDesc, "YES") == 0)
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 1;
		}
		else
		{
			pPlayList->sVarXMediaStreamAttr.ulDefault = 0;
		}
	}


	if(pTagNode->ppAttrArray[MEDIA_URI_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_URI_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_URI_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<1023)?(strlen(pDesc)):1023;
		memcpy(pPlayList->strShortURL, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_CHARACTERISTICS_ATTR_ID] != NULL &&
		pTagNode->ppAttrArray[MEDIA_CHARACTERISTICS_ATTR_ID]->pString != NULL)
	{
		pDesc = pTagNode->ppAttrArray[MEDIA_CHARACTERISTICS_ATTR_ID]->pString;
		ulLength = (strlen(pDesc)<1023)?(strlen(pDesc)):1023;
		memcpy(pPlayList->sVarXMediaStreamAttr.strCharacteristics, pDesc, ulLength);
	}

	if(pTagNode->ppAttrArray[MEDIA_INSTREAM_ATTR_ID] != NULL)
	{
		pPlayList->sVarXMediaStreamAttr.ulInStreamId = (VO_U32)(pTagNode->ppAttrArray[MEDIA_INSTREAM_ATTR_ID]->illIntValue);
	}
}


VO_BOOL    C_M3U_Manager::IsPlaySessionReady()
{
	if(m_sPlaySession.pStreamPlayListNode != NULL && m_sPlaySession.pStreamPlayListNode->pChunkItemHeader != NULL)
	{
		if( m_sPlaySession.pAlterAudioPlayListNode != NULL && 
			strlen(m_sPlaySession.pAlterAudioPlayListNode->strShortURL) != 0)
		{
			if(m_sPlaySession.pAlterAudioPlayListNode->pChunkItemHeader == NULL )
			{
				return VO_FALSE;
			}
		}

		if( m_sPlaySession.pAlterVideoPlayListNode != NULL && 
			strlen(m_sPlaySession.pAlterVideoPlayListNode->strShortURL) != 0)
		{
			if(m_sPlaySession.pAlterVideoPlayListNode->pChunkItemHeader == NULL )
			{
				return VO_FALSE;
			}
		}

		if( m_sPlaySession.pAlterSubTitlePlayListNode != NULL && 
			strlen(m_sPlaySession.pAlterSubTitlePlayListNode->strShortURL) != 0)
		{
			if(m_sPlaySession.pAlterSubTitlePlayListNode->pChunkItemHeader == NULL )
			{
				return VO_FALSE;
			}
		}


		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_U32    C_M3U_Manager::GetCurReadyPlaySession(S_PLAY_SESSION**  ppPlaySession)
{
    if(ppPlaySession == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    if(IsPlaySessionReady() == VO_FALSE)
    {
		return VO_RET_SOURCE2_FAIL;
    }

	*ppPlaySession = &m_sPlaySession;
    return 0;
}

VO_U32    C_M3U_Manager::SetStartPosForLiveStream()
{
    S_PLAYLIST_NODE*    pPlayList = NULL;
    VO_U32              ulOffset = 0;
    VO_U32              ulNewSequenceId = 0;
    VO_U32              ulRet = 0;
    VO_U32              ulNewOffset = 0;
    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
        return VO_RET_SOURCE2_FAIL;
    }

    pPlayList = m_sPlaySession.pStreamPlayListNode;
    ulOffset = GetPlayListStartOffset(pPlayList);

    ulRet = FindPosInPlayList(ulOffset, pPlayList, &ulNewSequenceId, &ulNewOffset);
    m_sPlaySession.ulMainStreamSequenceId = ulNewSequenceId;

    m_sPlaySession.ulAlterAudioSequenceId = ulNewSequenceId;
    m_sPlaySession.ulAlterVideoSequenceId = ulNewSequenceId;
    m_sPlaySession.ulAlterSubTitleSequenceId = ulNewSequenceId;

    VOLOGI("Set the New SequenceId:%d at start!");
    return 0;
}

VO_U32    C_M3U_Manager::GetRootManifestType(M3U_MANIFEST_TYPE*  pRootManfestType)
{
    if(pRootManfestType == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    *pRootManfestType = m_eRootPlayListType;
    return 0;
}

S_PLAYLIST_NODE*    C_M3U_Manager::GetPlayListNeedParseForSessionReady()
{

    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
        m_sPlaySession.pStreamPlayListNode = FindTheFirstMainStream();
		PrepareSessionByMainStreamDefaultSetting(m_sPlaySession.pStreamPlayListNode);
        return m_sPlaySession.pStreamPlayListNode;
    }

    if(m_sPlaySession.pStreamPlayListNode != NULL && m_sPlaySession.pStreamPlayListNode->pChunkItemHeader == NULL)
    {
		return m_sPlaySession.pStreamPlayListNode; 
    }

	if(m_sPlaySession.pAlterAudioPlayListNode != NULL && (strlen(m_sPlaySession.pAlterAudioPlayListNode->strShortURL) != 0)  
	   && m_sPlaySession.pAlterAudioPlayListNode->pChunkItemHeader == NULL)
	{
		return m_sPlaySession.pAlterAudioPlayListNode; 
	}

	if(m_sPlaySession.pAlterVideoPlayListNode != NULL && (strlen(m_sPlaySession.pAlterVideoPlayListNode->strShortURL) != 0)  
		&& m_sPlaySession.pAlterVideoPlayListNode->pChunkItemHeader == NULL)
	{
		return m_sPlaySession.pAlterVideoPlayListNode; 
	}

	if(m_sPlaySession.pAlterSubTitlePlayListNode != NULL && (strlen(m_sPlaySession.pAlterSubTitlePlayListNode->strShortURL) != 0)  
		&& m_sPlaySession.pAlterSubTitlePlayListNode->pChunkItemHeader == NULL)
	{
		return m_sPlaySession.pAlterSubTitlePlayListNode; 
	}
    
	return NULL;
}

VO_U32    C_M3U_Manager::SetThePos(VO_U32   ulTime, VO_BOOL*   pbNeedResetParser,  VO_U32* pulTimeChunkOffset,  VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
    VO_U32   ulRet = 0;
    VO_U32   ulNewSequenceId = 0;
    VO_U32   ulNewOffset = 0;
    S_PLAYLIST_NODE*  pPlayListNode = NULL;

    if(pulTimeChunkOffset == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    pPlayListNode = m_sPlaySession.pStreamPlayListNode;
    ulRet = FindPosInPlayList(ulTime, pPlayListNode, &ulNewSequenceId, &ulNewOffset);
    if(ulRet == 0)
    {
        m_sPlaySession.ulMainStreamSequenceId = ulNewSequenceId;
        m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
        *pulTimeChunkOffset = ulNewOffset;
    }
    else
    {
        return VO_RET_SOURCE2_FAIL;
    }

    pPlayListNode = m_sPlaySession.pAlterAudioPlayListNode;
    if(pPlayListNode != NULL && strlen(pPlayListNode->strShortURL) != 0)
    {
        ulRet = FindPosInPlayList(ulTime, pPlayListNode, &ulNewSequenceId, &ulNewOffset);
        if(ulRet == 0)
        {
            m_sPlaySession.ulAlterAudioSequenceId = ulNewSequenceId;
        }
    }
    m_sPlaySession.eAlterAudioInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
    

    pPlayListNode = m_sPlaySession.pAlterVideoPlayListNode;
    if(pPlayListNode != NULL && strlen(pPlayListNode->strShortURL) != 0)
    {
        ulRet = FindPosInPlayList(ulTime, pPlayListNode, &ulNewSequenceId, &ulNewOffset);
        if(ulRet == 0)
        {
            m_sPlaySession.ulAlterVideoSequenceId = ulNewSequenceId;
        }
    }
    m_sPlaySession.eAlterVideoInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;

    pPlayListNode = m_sPlaySession.pAlterSubTitlePlayListNode;
    if(pPlayListNode != NULL && strlen(pPlayListNode->strShortURL) != 0)
    {
        ulRet = FindPosInPlayList(ulTime, pPlayListNode, &ulNewSequenceId, &ulNewOffset);
        if(ulRet == 0)
        {
            m_sPlaySession.ulAlterSubTitleSequenceId = ulNewSequenceId;
        }
    }
    m_sPlaySession.eAlterSubTitleInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
    
    return 0;
}

VO_U32    C_M3U_Manager::GetTheDuration(VO_U32* pTimeDuration)
{
    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
		return VO_RET_SOURCE2_FAIL;
	}

	if(pTimeDuration == NULL)
	{
		return VO_RET_SOURCE2_ERRORDATA;
	}

    if(m_sPlaySession.pStreamPlayListNode->eChuckPlayListType == M3U_VOD)
    {
	    *pTimeDuration = m_sPlaySession.pStreamPlayListNode->ulCurrentDvrDuration;
    }
    else
    {
        *pTimeDuration = 0;
    }
    return 0;
}

VO_U32    C_M3U_Manager::GetTheEndTimeForLiveStream()
{
    return 0;
}

VO_U32    C_M3U_Manager::GetTheDvrDurationForLiveStream()
{
    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
	    return 0;
    }
    else
    {
        return m_sPlaySession.pStreamPlayListNode->ulCurrentDvrDuration;
    }
}

VO_U32    C_M3U_Manager::GetTheLiveTimeForLiveStream()
{
	return 0;
}

VO_U32    C_M3U_Manager::GetChunkOffsetValueBySequenceId(VO_U32  ulSequenceId, VO_U32* pTimeOffset)
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    VO_U32   ulRet= 0;
    if(pTimeOffset == NULL)
    {
        return HLS_ERR_EMPTY_POINTER;
    }

    pPlayListNode = m_sPlaySession.pStreamPlayListNode;
    return GetPlayListChunkOffsetValueBySequenceId(pPlayListNode, ulSequenceId, pTimeOffset);
}

VO_U32    C_M3U_Manager::GetPlayListChunkOffsetValueBySequenceId(S_PLAYLIST_NODE*   pPlayListNode, VO_U32  ulSequenceId, VO_U32* pTimeOffset)
{
    S_CHUNCK_ITEM*   pChunkItem = NULL;
    VO_U32   ulOffset = 0;
    if(pPlayListNode == NULL || pTimeOffset == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    if((ulSequenceId < pPlayListNode->ulCurrentMinSequenceIdInDvrWindow) ||
       (ulSequenceId > pPlayListNode->ulCurrentMaxSequenceIdInDvrWindow) )
    {
        VOLOGI("Invalid sequence id value:%d, not in the Window %d to %d", ulSequenceId, pPlayListNode->ulCurrentMinSequenceIdInDvrWindow,
                pPlayListNode->ulCurrentMaxSequenceIdInDvrWindow);
    }

    pChunkItem = pPlayListNode->pChunkItemHeader;
    while(pChunkItem != NULL)
    {
    
        if(pChunkItem->ulSequenceIDForKey == ulSequenceId)
        {
            *pTimeOffset = ulOffset;
            return 0;
        }
        else
        {
            ulOffset += pChunkItem->ulDurationInMsec; 
        }
        
        pChunkItem = pChunkItem->pNext;
    }

    return 0;
}

VO_U32    C_M3U_Manager::GetTheDvrEndLengthForLiveStream(VO_U64*   pEndLength)
{
    S_CHUNCK_ITEM*      pChunkItem = NULL;
    S_PLAYLIST_NODE*    pPlayList = NULL;
    VO_U32              ulTotalTime = 0;
    VO_U32              ulCurrentSequenceId = m_sPlaySession.ulMainStreamSequenceId;
    VO_U32        ulRet = 0;
    
    pPlayList = m_sPlaySession.pStreamPlayListNode;
    if(pPlayList == NULL || pEndLength == NULL)
    {
        return HLS_ERR_EMPTY_POINTER;
    }

    pChunkItem = pPlayList->pChunkItemHeader;
    while(pChunkItem != NULL)
    {
        if(pChunkItem->ulSequenceIDForKey >= ulCurrentSequenceId)
        {
            ulTotalTime += pChunkItem->ulDurationInMsec;
        }

        pChunkItem = pChunkItem->pNext;
    }

    VOLOGI("The Current SequenceId:%d, the End Length:%d", ulCurrentSequenceId, ulTotalTime);
    *pEndLength = (VO_U64)ulTotalTime;
    return 0;
}

VO_U32    C_M3U_Manager::GetCurrentProgreamStreamType(VO_SOURCE2_PROGRAM_TYPE*   peProgramType)
{
	if(m_sPlaySession.pStreamPlayListNode != NULL)
	{
		if(m_sPlaySession.pStreamPlayListNode->eChuckPlayListType == M3U_VOD)
		{
			VOLOGI("Program Type VOD Stream!")
				*peProgramType = VO_SOURCE2_STREAM_TYPE_VOD;
		}

		if(m_sPlaySession.pStreamPlayListNode->eChuckPlayListType == M3U_LIVE)
		{        
			VOLOGI("Program Type Live Stream!")
				*peProgramType = VO_SOURCE2_STREAM_TYPE_LIVE;
		}
		return 0;
	}
	else
	{
		return HLS_ERR_NOT_ENOUGH_PLAYLIST_PARSED;
	}
}

VO_VOID       C_M3U_Manager::SetUTCTime(VO_U64*   pUTCTime)
{
	VOLOGI("the UTC Time:%lld", *pUTCTime);
	m_ullUTCTime = (*pUTCTime);
	m_ulSystemtimeForUTC = voOS_GetSysTime();
	VOLOGI("the System for UTC:%d", m_ulSystemtimeForUTC);
}

VO_VOID       C_M3U_Manager::SetLiveLatencyValue(VO_U32*  pLiveLatencyValue)
{
    return;
}

VO_U32       C_M3U_Manager::GetChunckItemIntervalTime()
{
    if(m_sPlaySession.pStreamPlayListNode != NULL)
    {
        return  m_sPlaySession.pStreamPlayListNode->ulLastChunkDuration;
    }
    else
    {
        VOLOGI("session is not ready, return 10 second!");
        return 10000;
    }
}

VO_U32       C_M3U_Manager::SetPlayListToSession(VO_U32 ulPlayListId)
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;

    pPlayListNode = FindPlayListById(ulPlayListId);
    if(pPlayListNode == NULL)
    {
		return VO_RET_SOURCE2_ERRORDATA;
	}

    switch(pPlayListNode->ePlayListType)
    {
        case E_MAIN_STREAM:
        {
            if(m_sPlaySession.pStreamPlayListNode == NULL)
            {            
			    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
            }
            else
            {                
                m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_SMOOTH_ADAPTION_EX;
            }
			m_sPlaySession.pStreamPlayListNode = pPlayListNode;
            break;
        }
		case E_X_MEDIA_AUDIO_STREAM:
        {
            m_sPlaySession.pAlterAudioPlayListNode = pPlayListNode;
            m_sPlaySession.eAlterAudioInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
			break;
        }
		case E_X_MEDIA_VIDEO_STREAM:
		{
			m_sPlaySession.pAlterVideoPlayListNode = pPlayListNode;
			m_sPlaySession.eAlterVideoInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
            break;
		}
		case E_X_MEDIA_SUBTITLE_STREAM:
        {
			m_sPlaySession.pAlterSubTitlePlayListNode = pPlayListNode;
			m_sPlaySession.eAlterSubTitleInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
			break;
		}
		case E_I_FRAME_STREAM:
		{
            m_sPlaySession.pIFramePlayListNode = pPlayListNode;
            break;
		}
		default:
        {
			return VO_RET_SOURCE2_ERRORDATA;
		}
	}
	
    return 0;
}

VO_U32       C_M3U_Manager::AdjustXMedia()
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    VO_U32             ulRet = 0;
    VO_CHAR*           pGroupInStream = NULL;
	VO_CHAR*           pGroupInXMedia = NULL;

    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
        return VO_RET_SOURCE2_ERRORDATA;
    }

    pGroupInStream = m_sPlaySession.pStreamPlayListNode->sVarMainStreamAttr.strAudioAlterGroup;
    if(strlen(pGroupInStream) != 0 )
    {
		if(m_sPlaySession.pAlterAudioPlayListNode == NULL || strcmp(pGroupInStream, m_sPlaySession.pAlterAudioPlayListNode->sVarXMediaStreamAttr.strGroupId) != 0)
        {
            m_sPlaySession.pAlterAudioPlayListNode = FindPreferXMediaPlayListInGroup(pGroupInStream, E_X_MEDIA_AUDIO_STREAM);
		}
    }

	pGroupInStream = m_sPlaySession.pStreamPlayListNode->sVarMainStreamAttr.strVideoAlterGroup;
	if(strlen(pGroupInStream) != 0)
	{
		if(m_sPlaySession.pAlterVideoPlayListNode == NULL || strcmp(pGroupInStream, m_sPlaySession.pAlterVideoPlayListNode->sVarXMediaStreamAttr.strGroupId) != 0)
		{
			m_sPlaySession.pAlterVideoPlayListNode = FindPreferXMediaPlayListInGroup(pGroupInStream, E_X_MEDIA_VIDEO_STREAM);
		}
	}

	pGroupInStream = m_sPlaySession.pStreamPlayListNode->sVarMainStreamAttr.strSubTitleAlterGroup;
	if(strlen(pGroupInStream) != 0)
	{
		if(m_sPlaySession.pAlterSubTitlePlayListNode == NULL  || strcmp(pGroupInStream, m_sPlaySession.pAlterSubTitlePlayListNode->sVarXMediaStreamAttr.strGroupId) != 0)
		{
			m_sPlaySession.pAlterSubTitlePlayListNode = FindPreferXMediaPlayListInGroup(pGroupInStream, E_X_MEDIA_SUBTITLE_STREAM);
		}
	}
	
    return 0;
}

VO_U32       C_M3U_Manager::AdjustChunkPosInListForBA(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
    switch(sPrepareChunkPos)
    {
        case VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT:
        {            
            if(m_sPlaySession.pStreamPlayListNode != NULL)
            {
                if(m_sPlaySession.ulMainStreamSequenceId != 0)
                {
                    m_sPlaySession.ulMainStreamSequenceId = m_sPlaySession.ulMainStreamSequenceId-1;
                }
                else
                {
                    m_sPlaySession.ulMainStreamSequenceId = 0;                
                }
            }
            break;
        }
    }


    return 0;
}

S_PLAYLIST_NODE*       C_M3U_Manager::FindPreferXMediaPlayListInGroup(VO_CHAR*  pGroupId, E_PLAYLIST_TYPE ePlayListType)
{
	S_PLAYLIST_NODE*   pPlayList = NULL;
    pPlayList = m_pPlayListNodeHeader;
	S_PLAYLIST_NODE*   pPreferPlayList = NULL;

    while(pPlayList != NULL)
	{
		if(pPlayList->ePlayListType == ePlayListType && strcmp(pPlayList->sVarXMediaStreamAttr.strGroupId, pGroupId) == 0)
		{
			if(pPreferPlayList == NULL)
			{
				pPreferPlayList = pPlayList;
			}
			else
			{
				if(GetPreferValueForPlayList(pPreferPlayList) < GetPreferValueForPlayList(pPlayList))
				{
					pPreferPlayList = pPlayList;
				}
			}
		}

		pPlayList = pPlayList->pNext;
	}

	return pPreferPlayList;
}

VO_U32       C_M3U_Manager::GetPreferValueForPlayList(S_PLAYLIST_NODE*   pPlayList)
{
    if(pPlayList == NULL)
    {
		return 0;
    }

	switch(pPlayList->ePlayListType)
    {
        case E_X_MEDIA_AUDIO_STREAM:
        case E_X_MEDIA_SUBTITLE_STREAM:
        case E_X_MEDIA_VIDEO_STREAM:
        case E_X_MEDIA_CAPTION_STREAM:
        {
			return pPlayList->sVarXMediaStreamAttr.ulDefault*100 + pPlayList->sVarXMediaStreamAttr.ulAutoSelect*10 + pPlayList->sVarXMediaStreamAttr.ulForced;
		}
    }
	return 0;
}

VO_U32       C_M3U_Manager::GetMainStreamArray(S_PLAYLIST_NODE**  pPlayListNodeArray, VO_U32 ulArrayMaxSize, VO_U32*   pulArraySize)
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
	VO_U32             ulPlayListCount = 0;
    if(pPlayListNodeArray == NULL || pulArraySize == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

	pPlayListNode = m_pPlayListNodeHeader;
	while(pPlayListNode != NULL)
	{
		if(pPlayListNode->ePlayListType == E_MAIN_STREAM)
        {
			if(ulPlayListCount<ulArrayMaxSize)
			{
				pPlayListNodeArray[ulPlayListCount] = pPlayListNode;
				ulPlayListCount++;
			}
			else
			{
				VOLOGE("The MainStream Count is larger than he Array Size!");
				ulPlayListCount++;
			}
		}

		pPlayListNode = pPlayListNode->pNext;
	}

    *pulArraySize = ulPlayListCount;
    return 0;
}

VO_U32       C_M3U_Manager::GetMainStreamCount(VO_U32*   pulArraySize)
{
	S_PLAYLIST_NODE*   pPlayListNode = NULL;
	VO_U32             ulPlayListCount = 0;
	if(pulArraySize == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	pPlayListNode = m_pPlayListNodeHeader;
	while(pPlayListNode != NULL)
	{
		if(pPlayListNode->ePlayListType == E_MAIN_STREAM)
		{
			ulPlayListCount++;
		}

		pPlayListNode = pPlayListNode->pNext;
	}

	*pulArraySize = ulPlayListCount;
	return 0;
}

VO_U32       C_M3U_Manager::GetXMediaStreamArrayWithGroupAndType(S_PLAYLIST_NODE**  pPlayListNodeArray, VO_CHAR* pGroupId, E_PLAYLIST_TYPE  ePlayListType, VO_U32 ulArrayMaxSize, VO_U32*   pulArraySize)
{
	S_PLAYLIST_NODE*   pPlayListNode = NULL;
	VO_U32             ulPlayListCount = 0;
	if(pulArraySize == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	pPlayListNode = m_pPlayListNodeHeader;
	while(pPlayListNode != NULL)
	{
		if(pPlayListNode->ePlayListType == ePlayListType && memcmp(pPlayListNode->sVarXMediaStreamAttr.strGroupId, pGroupId, strlen(pGroupId)) == 0)
		{
			if(ulPlayListCount<ulArrayMaxSize)
			{
				pPlayListNodeArray[ulPlayListCount] = pPlayListNode;
			}
			ulPlayListCount++;
		}

		pPlayListNode = pPlayListNode->pNext;
	}

	*pulArraySize = ulPlayListCount;
	return 0;
}

VO_U32       C_M3U_Manager::GetXMediaStreamCountWithGroupAndType(VO_CHAR* pGroupId, E_PLAYLIST_TYPE  ePlayListType, VO_U32*   pulArraySize)
{
	S_PLAYLIST_NODE*   pPlayListNode = NULL;
	VO_U32             ulPlayListCount = 0;
	if(pulArraySize == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	pPlayListNode = m_pPlayListNodeHeader;
	while(pPlayListNode != NULL)
	{
		if(pPlayListNode->ePlayListType == ePlayListType && memcmp(pPlayListNode->sVarXMediaStreamAttr.strGroupId, pGroupId, strlen(pGroupId)) == 0)
		{
			ulPlayListCount++;
		}

		pPlayListNode = pPlayListNode->pNext;
	}

	*pulArraySize = ulPlayListCount;
	return 0;
}

VO_U32       C_M3U_Manager::GetCurrentChunk(E_PLAYLIST_TYPE  ePlayListType, S_CHUNCK_ITEM*   pChunkItems)
{
    VO_U32   ulRet = 0;
	VO_U32   ulSequenceId = 0;
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
	switch(ePlayListType)
    {
	    case E_MAIN_STREAM:
		{
			pPlayListNode = m_sPlaySession.pStreamPlayListNode;
			ulSequenceId = m_sPlaySession.ulMainStreamSequenceId;
			break;
		}
		case E_X_MEDIA_AUDIO_STREAM:
		{
            pPlayListNode = m_sPlaySession.pAlterAudioPlayListNode;
            ulSequenceId = m_sPlaySession.ulAlterAudioSequenceId;
            break;
		}
		case E_X_MEDIA_VIDEO_STREAM:
		{
            pPlayListNode = m_sPlaySession.pAlterVideoPlayListNode;
            ulSequenceId = m_sPlaySession.ulAlterVideoSequenceId;
            break;
		}
		case E_X_MEDIA_SUBTITLE_STREAM:
		{
            pPlayListNode = m_sPlaySession.pAlterSubTitlePlayListNode;
            ulSequenceId = m_sPlaySession.ulAlterSubTitleSequenceId;
            break;
		}
    }

    ulRet = GetChunkItem(pPlayListNode, pChunkItems, ulSequenceId);
    if(ulRet == 0)
	{
		switch(ePlayListType)
		{
		    case E_MAIN_STREAM:
			{
				pChunkItems->eChunkState = m_sPlaySession.eMainStreamInAdaptionStreamState;
				if(m_sPlaySession.eMainStreamInAdaptionStreamState != E_CHUNCK_NORMAL)
				{
                    m_sPlaySession.eMainStreamInAdaptionStreamState = E_CHUNCK_NORMAL;
				}
				m_sPlaySession.ulMainStreamSequenceId = pChunkItems->ulSequenceIDForKey+1;
				break;
			}
			case E_X_MEDIA_AUDIO_STREAM:
            {
				pChunkItems->eChunkState = m_sPlaySession.eAlterAudioInAdaptionStreamState;
				if(m_sPlaySession.eAlterAudioInAdaptionStreamState != E_CHUNCK_NORMAL)
				{
					m_sPlaySession.eAlterAudioInAdaptionStreamState = E_CHUNCK_NORMAL;
				}
				m_sPlaySession.ulAlterAudioSequenceId = pChunkItems->ulSequenceIDForKey+1;
				break;
			}
			case E_X_MEDIA_VIDEO_STREAM:
			{
				pChunkItems->eChunkState = m_sPlaySession.eAlterVideoInAdaptionStreamState;
				if(m_sPlaySession.eAlterVideoInAdaptionStreamState != E_CHUNCK_NORMAL)
				{
					m_sPlaySession.eAlterVideoInAdaptionStreamState = E_CHUNCK_NORMAL;
				}
				m_sPlaySession.ulAlterVideoSequenceId = pChunkItems->ulSequenceIDForKey+1;
				break;
			}
			case E_X_MEDIA_SUBTITLE_STREAM:
			{
				pChunkItems->eChunkState = m_sPlaySession.eAlterSubTitleInAdaptionStreamState;
				if(m_sPlaySession.eAlterSubTitleInAdaptionStreamState != E_CHUNCK_NORMAL)
				{
					m_sPlaySession.eAlterSubTitleInAdaptionStreamState = E_CHUNCK_NORMAL;
				}
				m_sPlaySession.ulAlterSubTitleSequenceId = pChunkItems->ulSequenceIDForKey+1;
				break;
			}
		}
	}

	return ulRet;
}

VO_U32       C_M3U_Manager::GetChunkItem(S_PLAYLIST_NODE*  pPlayListNode, S_CHUNCK_ITEM*   pChunkItem, VO_U32 ulSeqenceId)
{
	VO_U32           ulRet = 0;
    S_CHUNCK_ITEM*   pChuckItemInList = NULL;
	if(pPlayListNode == NULL || pChunkItem == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    VOLOGI("the PlayList Id:%d, the sequenceId:%d", pPlayListNode->ulPlayListId, ulSeqenceId);
    pChuckItemInList = pPlayListNode->pChunkItemHeader;
    while(pChuckItemInList != NULL)
    {
		if(pChuckItemInList->ulSequenceIDForKey >= ulSeqenceId)
		{
			break;
		}

		pChuckItemInList = pChuckItemInList->pNext;
    }

	if(pChuckItemInList == NULL)
    {
		VOLOGI("Can't get the sequence id:%d in playlist:%d", ulSeqenceId, pPlayListNode->ulPlayListId);
		switch(pPlayListNode->eChuckPlayListType)
		{
		    case M3U_VOD:
			{
                ulRet = HLS_ERR_VOD_END;
				break;
			}

			case M3U_LIVE:
			case M3U_EVENT:
			{
				ulRet = HLS_PLAYLIST_END;
				break;
			}
		}
		return ulRet;
    }

    memset(pChunkItem, 0, sizeof(S_CHUNCK_ITEM));
    memcpy(pChunkItem, pChuckItemInList, sizeof(S_CHUNCK_ITEM));
    return 0;
}

VO_U32       C_M3U_Manager::FindPosInPlayList(VO_U32  ulTimeOffset, S_PLAYLIST_NODE*   pPlayList, VO_U32*  pulNewSequenceId, VO_U32* pNewOffset)
{
    VO_U32   ulTotalTime = 0;
    VO_U32   ulSequenceNum = 0;
    S_CHUNCK_ITEM*    pChunkItem = NULL;
    VO_BOOL           bFindPos = VO_FALSE;
    
    if(pPlayList == NULL || pulNewSequenceId == NULL || pNewOffset == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    pChunkItem = pPlayList->pChunkItemHeader;
    ulSequenceNum = pChunkItem->ulSequenceIDForKey;
    while(pChunkItem != NULL)
    {
        if((ulTotalTime+pChunkItem->ulDurationInMsec)>ulTimeOffset)
        {
            bFindPos = VO_TRUE;            
            ulSequenceNum = pChunkItem->ulSequenceIDForKey;
            break;
        }

        ulTotalTime += pChunkItem->ulDurationInMsec;
        pChunkItem = pChunkItem->pNext;
    }

    if(bFindPos == VO_TRUE)
    {
        *pulNewSequenceId = ulSequenceNum;
        *pNewOffset = ulTotalTime;
        VOLOGI("Set PlayList:%d to the SequenceId:%d", pPlayList->ulPlayListId, ulSequenceNum);
        return 0;
    }
    else
    {
        VOLOGI("TimeOffset:%d beyond the PlayList Duration:", ulTimeOffset, pPlayList->ulCurrentDvrDuration);
        return VO_RET_SOURCE2_FAIL;
    }
}

S_PLAYLIST_NODE*       C_M3U_Manager::FindTargetPlayListWithTrackTypeAndId(VO_SOURCE2_TRACK_TYPE nType, VO_U32 ulTrackId)
{
    VO_CHAR*   pDescGroup = NULL;
    E_PLAYLIST_TYPE  ePlayListType = E_MAIN_STREAM;
    
    S_PLAYLIST_NODE*   pStreamPlayList = NULL;    
    S_PLAYLIST_NODE*   pXMediaPlayList = NULL;

    pStreamPlayList = m_sPlaySession.pStreamPlayListNode;
    if(pStreamPlayList->ulPlayListId == ulTrackId)
    {
        switch(nType)
        {
            case VO_SOURCE2_TT_AUDIOGROUP:
            case VO_SOURCE2_TT_AUDIO:
            {
                pDescGroup = pStreamPlayList->sVarMainStreamAttr.strAudioAlterGroup;
                ePlayListType = E_X_MEDIA_AUDIO_STREAM;
                break;
            }
            case VO_SOURCE2_TT_VIDEOGROUP:
            case VO_SOURCE2_TT_VIDEO:
            {            
                pDescGroup = pStreamPlayList->sVarMainStreamAttr.strVideoAlterGroup;
                ePlayListType = E_X_MEDIA_VIDEO_STREAM;
                break;
            }
            case VO_SOURCE2_TT_SUBTITLEGROUP:
            case VO_SOURCE2_TT_SUBTITLE: 
            {
                pDescGroup = pStreamPlayList->sVarMainStreamAttr.strSubTitleAlterGroup;                
                ePlayListType = E_X_MEDIA_SUBTITLE_STREAM;
                break;
            }
            default:
            {
                return NULL;
            }
        }

        pXMediaPlayList = m_pPlayListNodeHeader;
        while(pXMediaPlayList != NULL)
        {
            if(pXMediaPlayList->ePlayListType == ePlayListType &&
               memcmp(pDescGroup, pXMediaPlayList->sVarXMediaStreamAttr.strGroupId, strlen(pDescGroup))== 0 &&
               strlen(pXMediaPlayList->strShortURL) == 0)
            {
                return pXMediaPlayList;
            }

            pXMediaPlayList = pXMediaPlayList->pNext;
        }

        return pXMediaPlayList;
    }
    else
    {
        pXMediaPlayList = FindPlayListById(ulTrackId);
        return pXMediaPlayList;
    }
}

S_PLAYLIST_NODE*       C_M3U_Manager::FindTheFirstMainStream()
{
    S_PLAYLIST_NODE*   pPlayList = NULL;
    pPlayList = m_pPlayListNodeHeader;

    while(pPlayList != NULL)
    {
        if(pPlayList->ePlayListType == E_MAIN_STREAM)
        {
            return pPlayList;
        }

        pPlayList = pPlayList->pNext;
    }

    return NULL;
}

VO_VOID       C_M3U_Manager::ResetPlayListContentForLiveUpdate(S_PLAYLIST_NODE* pPlayList)
{
    if(pPlayList == NULL)
    {
        return;
    }

    pPlayList->ulItemCount = 0;
    pPlayList->ulCurrentMinSequenceIdInDvrWindow = 0;
    pPlayList->ulCurrentMaxSequenceIdInDvrWindow = 0;
    pPlayList->ulCurrentDvrDuration = 0;
    pPlayList->ulTargetDuration = 0;
    pPlayList->ulXStartExist = 0;
    pPlayList->ilXStartValue = 0;	
}

VO_U32       C_M3U_Manager::GetPlayListStartOffset(S_PLAYLIST_NODE* pPlayList)
{
    VO_U32  ulOffset = 0;
    if(pPlayList == NULL)
    {
        return 0;
    }
    
    if(pPlayList->ulXStartExist > 0)
    {
        if(pPlayList->ilXStartValue > 0)
        {
            ulOffset = pPlayList->ilXStartValue;
        }
        else
        {
            if((VO_U32)(-pPlayList->ilXStartValue)>pPlayList->ulCurrentDvrDuration)
            {
                ulOffset = 0;
            }
            else
            {
                ulOffset = pPlayList->ulCurrentDvrDuration+pPlayList->ilXStartValue;         
            }
        }
    }
    else
    {
        if(pPlayList->ulCurrentDvrDuration > (3*pPlayList->ulTargetDuration))
        {
            ulOffset = pPlayList->ulCurrentDvrDuration-(3*pPlayList->ulTargetDuration);
        }
        else
        {
            ulOffset = 0;
        }
    }

    return ulOffset;
}

VO_U32       C_M3U_Manager::GetCurrentSessionDurationByChapterId(VO_U32 uChapterId, VO_U32*   pTimeOutput)
{
    if(m_sPlaySession.pStreamPlayListNode == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }
    else
    {
        return GetPlayListDurationByChapterId(m_sPlaySession.pStreamPlayListNode, uChapterId, pTimeOutput);
    }
}

VO_U32       C_M3U_Manager::GetPlayListDurationByChapterId(S_PLAYLIST_NODE* pPlayList, VO_U32 uChapterId, VO_U32*   pTimeOutput)
{
    VO_U32          ulTotalTime = 0;
    S_CHUNCK_ITEM*  pChunkItem = NULL; 
    if(pPlayList == NULL || pTimeOutput == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    VOLOGI("PlayList Id:%d, ChapterId:%d", pPlayList->ulPlayListId, uChapterId);
    pChunkItem = pPlayList->pChunkItemHeader;
    while(pChunkItem != NULL)
    {
        if(pChunkItem->ulDisSequenceId <= uChapterId)
        {
            ulTotalTime +=  pChunkItem->ulDurationInMsec;
        }
        else
        {
            break;
        }
        pChunkItem = pChunkItem->pNext;
    }

    *pTimeOutput = ulTotalTime;
    return 0;
}

VO_VOID       C_M3U_Manager::BackupCurrentContext()
{
    m_sSessionContext.ulAvailable = 1;
    if(m_sPlaySession.pStreamPlayListNode != NULL)
    {
        m_sSessionContext.ulCurrentMainStreamPlayListId = m_sPlaySession.pStreamPlayListNode->ulPlayListId;
        m_sSessionContext.ulMainStreamSequenceId = m_sPlaySession.ulMainStreamSequenceId; 
        m_sSessionContext.eMainStreamInAdaptionStreamState = m_sPlaySession.eMainStreamInAdaptionStreamState;
    }
    else
    {
        m_sSessionContext.ulCurrentMainStreamPlayListId = INVALID_PLALIST_ID;
    }    
    VOLOGI("The current mainstream playlist id:%d, sequence id:%d, state value:%d", m_sSessionContext.ulCurrentMainStreamPlayListId, 
           m_sSessionContext.ulMainStreamSequenceId, (VO_U32)m_sPlaySession.eMainStreamInAdaptionStreamState);

    if(m_sPlaySession.pAlterVideoPlayListNode != NULL)
    {
        m_sSessionContext.ulCurrentAlterVideoStreamPlayListId = m_sPlaySession.pAlterVideoPlayListNode->ulPlayListId;
        m_sSessionContext.ulAlterVideoStreamSequenceId = m_sPlaySession.ulAlterVideoSequenceId; 
        m_sSessionContext.eAlterVideoStreamInAdaptionStreamState = m_sPlaySession.eAlterVideoInAdaptionStreamState;
    }
    else
    {
        m_sSessionContext.ulCurrentAlterVideoStreamPlayListId = INVALID_PLALIST_ID;
    }
    VOLOGI("The current AlterVideo playlist id:%d, sequence id:%d, state value:%d", m_sSessionContext.ulCurrentAlterVideoStreamPlayListId, 
           m_sSessionContext.ulAlterVideoStreamSequenceId, (VO_U32)m_sPlaySession.eAlterVideoInAdaptionStreamState);

    if(m_sPlaySession.pAlterAudioPlayListNode != NULL)
    {
        m_sSessionContext.ulCurrentAlterAudioStreamPlayListId = m_sPlaySession.pAlterAudioPlayListNode->ulPlayListId;
        m_sSessionContext.ulAlterAudioStreamSequenceId = m_sPlaySession.ulAlterAudioSequenceId; 
        m_sSessionContext.eAlterAudioStreamInAdaptionStreamState = m_sPlaySession.eAlterAudioInAdaptionStreamState;
    }
    else
    {
        m_sSessionContext.ulCurrentAlterAudioStreamPlayListId = INVALID_PLALIST_ID;
    }
    VOLOGI("The current AlterAudio playlist id:%d, sequence id:%d, state value:%d", m_sSessionContext.ulCurrentAlterAudioStreamPlayListId, 
           m_sSessionContext.ulAlterAudioStreamSequenceId, (VO_U32)m_sPlaySession.eAlterAudioInAdaptionStreamState);

    if(m_sPlaySession.pAlterSubTitlePlayListNode != NULL)
    {
        m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId = m_sPlaySession.pAlterSubTitlePlayListNode->ulPlayListId;
        m_sSessionContext.ulAlterSubTitleStreamSequenceId = m_sPlaySession.ulAlterSubTitleSequenceId; 
        m_sSessionContext.eAlterSubTitleStreamInAdaptionStreamState = m_sPlaySession.eAlterSubTitleInAdaptionStreamState;
    }
    else
    {
        m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId = INVALID_PLALIST_ID;
    }
    
    VOLOGI("The current AlterSub playlist id:%d, sequence id:%d, state value:%d", m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId, 
           m_sSessionContext.ulAlterSubTitleStreamSequenceId, (VO_U32)m_sPlaySession.eAlterSubTitleInAdaptionStreamState);
}

VO_VOID       C_M3U_Manager::ResetContextForUpdateURL()
{
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    ReleaseAllPlayList();
    m_pPlayListNodeHeader = NULL;
    m_pPlayListNodeTail = NULL;
    m_eRootPlayListType = M3U_UNKNOWN_PLAYLIST;
	memset(&m_sPlaySession, 0, sizeof(S_PLAY_SESSION));
}

S_SESSION_CONTEXT*       C_M3U_Manager::GetSessionContext()
{
    return &m_sSessionContext;
}

VO_VOID       C_M3U_Manager::RestoreCurrentContext()
{
    VO_U32 ulRet = 0;
    VO_U32   ulPlayListId = 0;

    ulPlayListId = m_sSessionContext.ulCurrentMainStreamPlayListId;
    if(ulPlayListId != INVALID_PLALIST_ID)
    {
        SetPlayListToSession(ulPlayListId);
    }

    ulPlayListId = m_sSessionContext.ulCurrentAlterAudioStreamPlayListId;
    if(ulPlayListId != INVALID_PLALIST_ID)
    {
        SetPlayListToSession(ulPlayListId);
    }

    ulPlayListId = m_sSessionContext.ulCurrentAlterVideoStreamPlayListId;
    if(ulPlayListId != INVALID_PLALIST_ID)
    {
        SetPlayListToSession(ulPlayListId);
    }

    ulPlayListId = m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId;
    if(ulPlayListId != INVALID_PLALIST_ID)
    {
        SetPlayListToSession(ulPlayListId);
    }

	VOLOGI("Set Current Session Success!");

    if(m_sPlaySession.pStreamPlayListNode != NULL && m_sPlaySession.pStreamPlayListNode->ulPlayListId == m_sSessionContext.ulCurrentMainStreamPlayListId)
    {
        m_sPlaySession.ulMainStreamSequenceId = m_sSessionContext.ulMainStreamSequenceId; 
        m_sPlaySession.eMainStreamInAdaptionStreamState = m_sSessionContext.eMainStreamInAdaptionStreamState;
        VOLOGI("Set mainstream playlist id:%d, sequence id:%d", m_sSessionContext.ulCurrentMainStreamPlayListId, m_sSessionContext.ulMainStreamSequenceId);
    }

    if(m_sPlaySession.pAlterVideoPlayListNode != NULL  && m_sPlaySession.pAlterVideoPlayListNode->ulPlayListId == m_sSessionContext.ulCurrentAlterVideoStreamPlayListId)
    {
        m_sPlaySession.ulAlterVideoSequenceId = m_sSessionContext.ulAlterVideoStreamSequenceId; 
        m_sPlaySession.eAlterVideoInAdaptionStreamState = m_sSessionContext.eAlterVideoStreamInAdaptionStreamState;
        VOLOGI("Set altervideo playlist id:%d, sequence id:%d", m_sSessionContext.ulAlterVideoStreamSequenceId, m_sSessionContext.eAlterVideoStreamInAdaptionStreamState);
    }

    if(m_sPlaySession.pAlterAudioPlayListNode != NULL  && m_sPlaySession.pAlterAudioPlayListNode->ulPlayListId == m_sSessionContext.ulCurrentAlterAudioStreamPlayListId)
    {
        m_sPlaySession.ulAlterAudioSequenceId = m_sSessionContext.ulAlterAudioStreamSequenceId; 
        m_sPlaySession.eAlterAudioInAdaptionStreamState = m_sSessionContext.eAlterAudioStreamInAdaptionStreamState;
        VOLOGI("Set alterAudio playlist id:%d, sequence id:%d", m_sSessionContext.ulAlterAudioStreamSequenceId, m_sSessionContext.eAlterAudioStreamInAdaptionStreamState);
    }

    if(m_sPlaySession.pAlterSubTitlePlayListNode != NULL  && m_sPlaySession.pAlterSubTitlePlayListNode->ulPlayListId == m_sSessionContext.ulCurrentAlterSubTitleStreamPlayListId)
    {
        m_sPlaySession.ulAlterSubTitleSequenceId = m_sSessionContext.ulAlterSubTitleStreamSequenceId; 
        m_sPlaySession.eAlterSubTitleInAdaptionStreamState = m_sSessionContext.eAlterSubTitleStreamInAdaptionStreamState;
        VOLOGI("Set altersubtitle playlist id:%d, sequence id:%d", m_sSessionContext.ulAlterSubTitleStreamSequenceId, m_sSessionContext.eAlterSubTitleStreamInAdaptionStreamState);
    }

    return ;
}

VO_VOID       C_M3U_Manager::PrepareSessionByMainStreamDefaultSetting(S_PLAYLIST_NODE* pPlayList)
{
	if(pPlayList == NULL ||  pPlayList->ePlayListType != E_MAIN_STREAM)
	{
		return;
	}

	if(strlen(pPlayList->sVarMainStreamAttr.strAudioAlterGroup) != 0)
	{
		m_sPlaySession.pAlterAudioPlayListNode = FindPreferXMediaPlayListInGroup(pPlayList->sVarMainStreamAttr.strAudioAlterGroup, E_X_MEDIA_AUDIO_STREAM);
	}

	if(strlen(pPlayList->sVarMainStreamAttr.strVideoAlterGroup) != 0)
	{
		m_sPlaySession.pAlterVideoPlayListNode = FindPreferXMediaPlayListInGroup(pPlayList->sVarMainStreamAttr.strVideoAlterGroup, E_X_MEDIA_VIDEO_STREAM);
	}

	if(strlen(pPlayList->sVarMainStreamAttr.strSubTitleAlterGroup) != 0)
	{
		m_sPlaySession.pAlterSubTitlePlayListNode = FindPreferXMediaPlayListInGroup(pPlayList->sVarMainStreamAttr.strSubTitleAlterGroup, E_X_MEDIA_SUBTITLE_STREAM);
	}
}

VO_U32       C_M3U_Manager::AdjustSequenceIdInSession()
{
    if(m_sPlaySession.pStreamPlayListNode != NULL)
    {
        AdjustSequenceIdByPlayListContext(E_MAIN_STREAM, m_sPlaySession.pStreamPlayListNode);
    }
    
    if(m_sPlaySession.pAlterAudioPlayListNode!= NULL)
    {
        AdjustSequenceIdByPlayListContext(E_X_MEDIA_AUDIO_STREAM, m_sPlaySession.pAlterAudioPlayListNode);
    }
    
    if(m_sPlaySession.pAlterVideoPlayListNode!= NULL)
    {
        AdjustSequenceIdByPlayListContext(E_X_MEDIA_VIDEO_STREAM, m_sPlaySession.pAlterVideoPlayListNode);
    }
    
    if(m_sPlaySession.pAlterSubTitlePlayListNode!= NULL)
    {
        AdjustSequenceIdByPlayListContext(E_X_MEDIA_SUBTITLE_STREAM, m_sPlaySession.pAlterSubTitlePlayListNode);
    }
    
    return 0;
}

VO_U32       C_M3U_Manager::AdjustSequenceIdByPlayListContext(E_PLAYLIST_TYPE  ePlayListType, S_PLAYLIST_NODE*  pPlayList)
{
    VO_U32*   pulCurrentSequenceId = NULL;

    if(pPlayList == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    if(ePlayListType != pPlayList->ePlayListType || strlen(pPlayList->strShortURL) == 0)
    {
        return VO_RET_SOURCE2_FAIL;
    }
    switch(ePlayListType)
    {
        case E_MAIN_STREAM:
        {
            pulCurrentSequenceId = &(m_sPlaySession.ulMainStreamSequenceId);
            break;
        }
        case E_X_MEDIA_AUDIO_STREAM:
        {
            pulCurrentSequenceId = &(m_sPlaySession.ulAlterAudioSequenceId);
            break;
        }
        case E_X_MEDIA_VIDEO_STREAM:
        {
            pulCurrentSequenceId = &(m_sPlaySession.ulAlterVideoSequenceId);
            break;
        }
        case E_X_MEDIA_SUBTITLE_STREAM:
        {
            pulCurrentSequenceId = &(m_sPlaySession.ulAlterSubTitleSequenceId);            
            break;
        }
        default:
        {
            return VO_RET_SOURCE2_FAIL;
        }
    }

    VOLOGI("PlayList Id:%d, Current Sequence Id:%d, Current PlayList Min Sequence:%d, Current PlayList Max Sequence:%d", 
           pPlayList->ulPlayListId, *pulCurrentSequenceId, pPlayList->ulCurrentMinSequenceIdInDvrWindow, pPlayList->ulCurrentMaxSequenceIdInDvrWindow);
    if((*pulCurrentSequenceId) < pPlayList->ulCurrentMinSequenceIdInDvrWindow)
    {
        *pulCurrentSequenceId = pPlayList->ulCurrentMinSequenceIdInDvrWindow;
    }

    //Max SequenceId + PlayList Item Count
    if((*pulCurrentSequenceId) > (2*pPlayList->ulCurrentMaxSequenceIdInDvrWindow-pPlayList->ulCurrentMinSequenceIdInDvrWindow))
    {
        *pulCurrentSequenceId = pPlayList->ulCurrentMaxSequenceIdInDvrWindow-1;
    }

    return 0;
}

VO_VOID    C_M3U_Manager::SetCurrentSequenceIdForPlayList(VO_VOID*  pChunkInfo)
{
    VO_ADAPTIVESTREAMPARSER_CHUNK_INFO*   pChunkInfoDetail = NULL;;    
    S_PLAYLIST_NODE*   pPlayListNode = NULL;
    VO_U32             ulRet = 0;
    
    pChunkInfoDetail = (VO_ADAPTIVESTREAMPARSER_CHUNK_INFO*)pChunkInfo;
    if(pChunkInfoDetail == NULL)
    {
        return;
    }

    VOLOGI("Set ChunkInfo: playlistid:%d, sequence id:%d", pChunkInfoDetail->uTrackID, pChunkInfoDetail->uChunkID);
    pPlayListNode = FindPlayListById(pChunkInfoDetail->uTrackID);
    
    if(pPlayListNode == m_sPlaySession.pStreamPlayListNode)
    {
        VOLOGI("Set the Seq For MainStream!");
        m_sPlaySession.ulMainStreamSequenceId = (VO_U32)pChunkInfoDetail->uChunkID;
        return;
    }

    if(pPlayListNode == m_sPlaySession.pAlterAudioPlayListNode)
    {
        VOLOGI("Set the Seq For AlterAudio!");
        m_sPlaySession.ulAlterAudioSequenceId = (VO_U32)pChunkInfoDetail->uChunkID;
        return;
    }

    if(pPlayListNode == m_sPlaySession.pAlterVideoPlayListNode)
    {
        VOLOGI("Set the Seq For AlterVideo!");
        m_sPlaySession.ulAlterVideoSequenceId = (VO_U32)pChunkInfoDetail->uChunkID;
        return; 
    }

    if(pPlayListNode == m_sPlaySession.pAlterSubTitlePlayListNode)
    {
        VOLOGI("Set the Seq For AlterSubTitle!");
        m_sPlaySession.ulAlterSubTitleSequenceId = (VO_U32)pChunkInfoDetail->uChunkID;
        return; 
    }
    
    return;
}

VO_U32    C_M3U_Manager::SeekForOneTrackOnly(VO_U32  ulPlaylistId, VO_U64 ullTimeOffset, VO_U32* pNewOffset)
{
    S_PLAYLIST_NODE*   pPlaylistNode = NULL;
	S_PLAYLIST_NODE*   pCurWorkPlaylistNode = NULL;
	VO_U32             ulOffset = 0;
    VO_U32             ulNewSequenceId = 0;
    VO_U32             ulNewOffset = 0;
    VO_U32             ulRet = 0;
    
    if(pNewOffset == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

	pPlaylistNode = FindPlayListById(ulPlaylistId);
    if(pPlaylistNode)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	switch(pPlaylistNode->ePlayListType)
    {
        case E_X_MEDIA_AUDIO_STREAM:
		{
			pCurWorkPlaylistNode = m_sPlaySession.pAlterAudioPlayListNode;
			break;
		}
		case E_X_MEDIA_SUBTITLE_STREAM:
	    {
			pCurWorkPlaylistNode = m_sPlaySession.pAlterVideoPlayListNode;
			break;
		}
		default:
	    {
			VOLOGI("Invalid PlayList type value:%d", pPlaylistNode->ePlayListType);
			return VO_RET_SOURCE2_FAIL;
		}
    }

	if(pPlaylistNode != pCurWorkPlaylistNode)
    {
		VOLOGI("PlayList:%d doesn't work now", pPlaylistNode->ulPlayListId);
		return VO_RET_SOURCE2_FAIL;
	}

    if(strlen(pPlaylistNode->strShortURL) == 0)
    {
		VOLOGI("The PlayList:%d media data in Muxed Stream!");
		return 0;
    }

	ulOffset = (VO_U32)ullTimeOffset;
	ulRet = FindPosInPlayList(ulOffset, pPlaylistNode, &ulNewSequenceId, &ulNewOffset);
    if(ulRet != 0)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	switch(pPlaylistNode->ePlayListType)
	{
	    case E_X_MEDIA_AUDIO_STREAM:
		{
			m_sPlaySession.ulAlterAudioSequenceId = ulNewSequenceId;
			m_sPlaySession.eAlterAudioInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
			break;
		}
	    case E_X_MEDIA_SUBTITLE_STREAM:
		{
			m_sPlaySession.ulAlterSubTitleSequenceId = ulNewSequenceId;
			m_sPlaySession.eAlterSubTitleInAdaptionStreamState = E_CHUNCK_FORCE_NEW_STREAM;
			break;
		}
	    default:
		{
			break;
		}
	}

	return 0;
}
