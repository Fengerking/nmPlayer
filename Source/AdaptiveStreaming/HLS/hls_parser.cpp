#include "string.h"
#include "voDSType.h"
#include "voSource2.h"
#include "hls_parser.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


C_M3U_Parser::C_M3U_Parser()
{  
	m_pManifestData = NULL;
	m_ulManifestDataMaxLength = 0;
	m_pTagNodeHeader = NULL;
	m_pTagNodeTail = NULL;
	m_eCurrentManifestType = M3U_UNKNOWN_PLAYLIST;
	m_eCurrentChuckPlayListType = M3U_INVALID_CHUNK_PLAYLIST_TYPE;
	m_eCurrentChuckPlayListTypeEx = M3U_NORMAL;

    m_ppTagName = NULL;
    m_pAttrMaxCountSet = NULL;
    m_ulTagTypeCount = 0;
    InitParseContext();
}

C_M3U_Parser::~C_M3U_Parser()
{
    if(m_pManifestData != NULL)
	{
        delete[] m_pManifestData;
        m_pManifestData = NULL;
	}

    ReleaseParseContext();
}


VO_VOID   C_M3U_Parser::ReleaseAllTagNode()
{
    S_TAG_NODE*  pTagNode = NULL;

    if(m_pTagNodeHeader == NULL)
	{
        return;
	}

	pTagNode = m_pTagNodeHeader;
	while(pTagNode != NULL)
	{
		m_pTagNodeHeader = pTagNode->pNext;
		ReleaseTagNode(pTagNode);
		pTagNode = m_pTagNodeHeader;
	}

	m_pTagNodeHeader = m_pTagNodeTail = NULL;
}

VO_VOID   C_M3U_Parser::ReleaseTagNode(S_TAG_NODE*  pTagNode)
{
    VO_U32   ulIndex = 0;
	if(pTagNode == NULL)
    {
		return;
    }
    
    for(ulIndex=0; ulIndex<pTagNode->ulAttrMaxCount; ulIndex++)
    {
		if(pTagNode->ppAttrArray != NULL && pTagNode->ppAttrArray[ulIndex] != NULL)
		{
            switch(pTagNode->ppAttrArray[ulIndex]->ulDataValueType)
            {
			    case M3U_STRING:
				{
					delete[]  pTagNode->ppAttrArray[ulIndex]->pString;
					pTagNode->ppAttrArray[ulIndex]->pString = NULL;
					break;
				}
				case M3U_DECIMAL_RESOLUTION:
			    {
					delete  pTagNode->ppAttrArray[ulIndex]->pResolution;
					pTagNode->ppAttrArray[ulIndex]->pResolution = NULL;
					break;
				}
				case M3U_BYTE_RANGE:
				{
                    delete  pTagNode->ppAttrArray[ulIndex]->pResolution;
                    pTagNode->ppAttrArray[ulIndex]->pResolution = NULL;
                    break;
				}

				case M3U_INT:
			    case M3U_FLOAT:
				case M3U_HEX_DATA:
				case M3U_UNKNOWN:
				{
					break;
				}
			}

			delete pTagNode->ppAttrArray[ulIndex];
            pTagNode->ppAttrArray[ulIndex] = NULL;
		}
	}

	if(pTagNode->ulAttrMaxCount >0)
	{
		delete[]    pTagNode->ppAttrArray;
	}

	delete pTagNode;
}

VO_U32   C_M3U_Parser::ParseManifest(VO_BYTE*   pManifestData, VO_U32 ulDataLength)
{
    VO_U32 ulRet = 0;
    VO_CHAR*   pLine = NULL;
    VO_CHAR*   pNext = NULL;
    VO_CHAR*   pEnd = NULL;
    VO_CHAR*   pCur = NULL;
    VO_CHAR    strOutput[8192] = {0};
    if(pManifestData == NULL)
    {
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    memcpy(strOutput, pManifestData, (ulDataLength>8191)?8191:ulDataLength);
    VOLOGI("M3U content:%s", strOutput);


    ulRet = VerifyHeader(pManifestData);
    if(ulRet != 0)
    {
		return ulRet;
    }

    ulRet = CheckWorkMemory(ulDataLength);
    if(ulRet != 0)
    {
		return VO_RET_SOURCE2_FAIL;
    }

	memset(m_pManifestData, 0, m_ulManifestDataMaxLength);
    memcpy(m_pManifestData, pManifestData, ulDataLength);
    ResetContext();
	pCur = (VO_CHAR*)m_pManifestData;
    pEnd = (VO_CHAR*)(m_pManifestData+ulDataLength);
	ulRet = ReadNextLineWithoutCopy(pCur, pEnd, &pLine, &pNext);
	while(ulRet == 0)
	{
		ParseLine(pLine);
		pCur = pNext;
		ulRet = ReadNextLineWithoutCopy(pCur, pEnd, &pLine, &pNext);
	}

    return 0;
}


VO_U32   C_M3U_Parser::ParseLine(VO_CHAR* pManifestLine)
{
    VO_U32   ulRet = 0;
    VO_CHAR*  pFirst = NULL;
    VO_U32   ulLineLength = 0;
    VO_U32   ulTagType = 0xffffffff;

    if(pManifestLine == NULL || strlen(pManifestLine) == 0)
    {
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
    }

    ulLineLength = strlen(pManifestLine);
    
	if((*pManifestLine) == '#')
    {
        if((ulLineLength < 4) ||
		   *(pManifestLine+1) != 'E'||
		   *(pManifestLine+2) != 'X'||
		   *(pManifestLine+3) != 'T')
		{
			return HLS_ERR_WRONG_MANIFEST_FORMAT;
		}

		ulRet = ParseTagLine(pManifestLine);
	}
	else
	{
		ulRet = AddURILine(pManifestLine);
	}

	return ulRet;
}


VO_U32   C_M3U_Parser::AddURILine(VO_CHAR*   pLine)
{
    VO_U32      ulRet = 0;
    S_ATTR_VALUE*    pAttrValue = NULL;
	S_TAG_NODE*  pTagNodeNew = NULL;
    VO_CHAR*     pLineContent = NULL;
    VO_U32       ulLineContentLength = 0;
    
    ulRet = CreateTagNode(&pTagNodeNew, NORMAL_URI_NAME_INDEX);
    if(ulRet != 0)
    {
		return ulRet;
    }

	ParseTotalLine(pLine, pTagNodeNew, URI_LINE_ATTR_ID);
	AddTag(pTagNodeNew);
	return 0;
}


VO_U32   C_M3U_Parser::GetTagType(VO_CHAR*   pLine)
{
    VO_U32  ulIndex = 0;

    for(ulIndex=0; ulIndex<V12_SPEC_TAG_TYPE_COUNT; ulIndex++)
    {
		if(memcmp(pLine, m_ppTagName[ulIndex], strlen(m_ppTagName[ulIndex])) == 0)
		{
			break;
		}
	}
    
	if(ulIndex<V12_SPEC_TAG_TYPE_COUNT)
	{
		return ulIndex;
	}
	else
	{
		return 0xffffffff;
	}
}


VO_U32   C_M3U_Parser::ParseTagLine(VO_CHAR*   pLine)
{
    VO_U32  ulRet = 0;
    VO_U32  ulTagIndex = 0xffffffff;

    ulTagIndex = GetTagType(pLine);
    switch(ulTagIndex)
    {
        case TARGETDURATION_NAME_INDEX:
        {
			ulRet = ParseTargeDuration(pLine);
            break;
        }
		case MEDIA_SEQUENCE_NAME_INDEX:
		{
            ulRet = ParseMediaSequence(pLine);
			break;
		}
		case BYTERANGE_NAME_INDEX:
		{
            ulRet = ParseByteRange(pLine);
			break;
		}
		case INF_NAME_INDEX:
		{
            ulRet = ParseInf(pLine);
			break;
		}
		case KEY_NAME_INDEX:
		{
            ulRet = ParseKey(pLine);
			break;
		}
		case STREAM_INF_NAME_INDEX:
		{
            ulRet = ParseStreamInf(pLine);
			break;
		}
		case PROGRAM_DATE_TIME_NAME_INDEX:
		{
            ulRet = ParseProgramDataTime(pLine);
			break;
		}
		case I_FRAME_STREAM_INF_NAME_INDEX:
		{
            ulRet = ParseIFrameStreamInf(pLine);
			break;
		}
		case ALLOW_CACHE_NAME_INDEX:
		{
			ulRet = ParseAllowCache(pLine);
			break;
		}
		case MEDIA_NAME_INDEX:
		{
            ulRet = ParseXMedia(pLine);
			break;
		}
		case PLAYLIST_TYPE_NAME_INDEX:
		{
            ulRet = ParsePlayListType(pLine);
			break;
		}
		case I_FRAMES_ONLY_NAME_INDEX:         
		{
            ulRet = ParseIFrameOnly(pLine);
			break;
		}
		case DISCONTINUITY_NAME_INDEX:
		{
            ulRet = ParseDisContinuity(pLine);
			break;
		}
		case ENDLIST_NAME_INDEX:
		{
			ulRet = ParseEndList(pLine);
			break;
		}
		case VERSION_NAME_INDEX:
		{
            ulRet = ParseVersion(pLine);
			break;
		}
		case MAP_NAME_INDEX:
		{
            ulRet = ParseXMap(pLine);
			break;
		}
		case START_NAME_INDEX:
		{
            ulRet = ParseXStart(pLine);
			break;
		}
		case DISCONTINUITY_SEQUENCE_NAME_INDEX:
		{
            ulRet = ParseDisSequence(pLine);
			break;
		}
		default:
        {
			break;
		}
	}
    return 0;
}


VO_VOID   C_M3U_Parser::AddTag(S_TAG_NODE*  pTagNode)
{
    if(m_pTagNodeTail == NULL)
	{
		m_pTagNodeHeader = m_pTagNodeTail = pTagNode;
	}
	else
	{
		m_pTagNodeTail->pNext = pTagNode;
		m_pTagNodeTail = pTagNode;
	}
}

VO_U32   C_M3U_Parser::CreateTagNode(S_TAG_NODE**  ppTagNode, VO_U32  ulTagType)
{
    S_TAG_NODE*    pTagNode = NULL;
    S_ATTR_VALUE**  ppAttrArray = NULL;

	if(ulTagType >= V12_SPEC_TAG_TYPE_COUNT)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

    pTagNode = new S_TAG_NODE;
    if(pTagNode == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

    memset(pTagNode, 0, sizeof(S_TAG_NODE));

	if(m_pAttrMaxCountSet[ulTagType] != 0)
    {
		ppAttrArray = new S_ATTR_VALUE*[m_pAttrMaxCountSet[ulTagType]];
	}

    if(m_pAttrMaxCountSet[ulTagType] != 0 && ppAttrArray == NULL)
    {
		delete pTagNode;
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    memset(ppAttrArray, 0, sizeof(S_ATTR_VALUE*)*m_pAttrMaxCountSet[ulTagType]);
	pTagNode->ulAttrMaxCount = m_pAttrMaxCountSet[ulTagType];
	pTagNode->ulTagIndex = ulTagType;
    pTagNode->ulAttrSet = 0;
    pTagNode->pNext = NULL;
    pTagNode->ppAttrArray = ppAttrArray;

    *ppTagNode = pTagNode;
	return 0;
}


VO_U32   C_M3U_Parser::GetManifestType(M3U_MANIFEST_TYPE*  peManifestType, M3U_CHUNCK_PLAYLIST_TYPE* peChucklistType, M3U_CHUNCK_PLAYLIST_TYPE_EX*  peChunklistTypeEx)
{
    S_TAG_NODE*    pTagNode = NULL;
    
	pTagNode = m_pTagNodeHeader;
    while(pTagNode != NULL)
    {
		switch(pTagNode->ulTagIndex)
        {
		    case STREAM_INF_NAME_INDEX:
			{
				m_eCurrentManifestType = M3U_STREAM_PLAYLIST;
				break;
			}
			case INF_NAME_INDEX:
            {
				m_eCurrentManifestType = M3U_CHUNK_PLAYLIST;
				m_eCurrentChuckPlayListType = M3U_LIVE;
                break;
            }
			case ENDLIST_NAME_INDEX:
			{
				m_eCurrentChuckPlayListType = M3U_VOD;
                break;
			}
			case PLAYLIST_TYPE_NAME_INDEX:
            {
				if(pTagNode->ppAttrArray[PALYLIST_TYPE_VALUE_ATTR_ID] != NULL && 
				   pTagNode->ppAttrArray[PALYLIST_TYPE_VALUE_ATTR_ID]->pString != NULL)
				{
					if(memcmp(pTagNode->ppAttrArray[PALYLIST_TYPE_VALUE_ATTR_ID]->pString, "VOD", 3) == 0)
					{
						m_eCurrentChuckPlayListType = M3U_VOD;
					}
					if(memcmp(pTagNode->ppAttrArray[PALYLIST_TYPE_VALUE_ATTR_ID]->pString, "EVENT", 5) == 0)
					{
						m_eCurrentChuckPlayListType = M3U_EVENT;
					}
				}
				break;
			}
		}

		pTagNode = pTagNode->pNext;
    }

	*peManifestType = m_eCurrentManifestType;
    *peChucklistType = m_eCurrentChuckPlayListType;
    *peChunklistTypeEx = m_eCurrentChuckPlayListTypeEx;
    return 0;
}

VO_U32   C_M3U_Parser::InitParseContext()
{
    VO_CHAR**     ppAttrNameArray = NULL;
    M3U_DATA_TYPE*   pAttrTypeArray = NULL;
    VO_U32           ulTagMaxAttrCount = 0;


    m_ulTagTypeCount = V12_SPEC_TAG_TYPE_COUNT;
    m_ppTagName = new VO_CHAR*[m_ulTagTypeCount];
	m_pAttrMaxCountSet = new VO_U32[m_ulTagTypeCount];

    if(m_ppTagName == NULL || m_pAttrMaxCountSet == NULL)
    {
		return VO_RET_SOURCE2_FAIL;
    }

    memset(m_ppTagName, 0, sizeof(VO_CHAR*)*m_ulTagTypeCount);
	memset(m_pAttrMaxCountSet, 0, sizeof(VO_U32)*m_ulTagTypeCount);

    //Add every Tag information;

    //Add BEGIN_NAME_INDEX;
	m_ppTagName[BEGIN_NAME_INDEX] =(VO_CHAR*) "#EXTM3U";
    m_pAttrMaxCountSet[BEGIN_NAME_INDEX] = 0;

    //Add TARGETDURATION_NAME_INDEX
	m_ppTagName[TARGETDURATION_NAME_INDEX] = (VO_CHAR*) "#EXT-X-TARGETDURATION";
    m_pAttrMaxCountSet[TARGETDURATION_NAME_INDEX] = TARGETDURATION_MAX_ATTR_COUNT;

	//Add MEDIA_SEQUENCE_NAME_INDEX
	m_ppTagName[MEDIA_SEQUENCE_NAME_INDEX] = (VO_CHAR*)"#EXT-X-MEDIA-SEQUENCE";
	m_pAttrMaxCountSet[MEDIA_SEQUENCE_NAME_INDEX] = MEDIA_SEQUENCE_MAX_ATTR_COUNT;

	//Add BYTERANGE_NAME_INDEX
	m_ppTagName[BYTERANGE_NAME_INDEX] =(VO_CHAR*)"#EXT-X-BYTERANGE";
	m_pAttrMaxCountSet[BYTERANGE_NAME_INDEX] = BYTERANGE_MAX_ATTR_COUNT;

	//Add INF_NAME_INDEX
	m_ppTagName[INF_NAME_INDEX] = (VO_CHAR*) "#EXTINF";
	m_pAttrMaxCountSet[INF_NAME_INDEX] = INF_MAX_ATTR_COUNT;

	//Add KEY_NAME_INDEX
	m_ppTagName[KEY_NAME_INDEX] = (VO_CHAR*) "#EXT-X-KEY";
	m_pAttrMaxCountSet[KEY_NAME_INDEX] = KEY_MAX_ATTR_COUNT;

	//Add STREAM_INF_NAME_INDEX
	m_ppTagName[STREAM_INF_NAME_INDEX] =(VO_CHAR*) "#EXT-X-STREAM-INF";
	m_pAttrMaxCountSet[STREAM_INF_NAME_INDEX] = STREAM_MAX_ATTR_COUNT;

	//Add PROGRAM_DATE_TIME_NAME_INDEX
	m_ppTagName[PROGRAM_DATE_TIME_NAME_INDEX] = (VO_CHAR*) "#EXT-X-PROGRAM-DATE-TIME";
	m_pAttrMaxCountSet[PROGRAM_DATE_TIME_NAME_INDEX] = PROGRAM_MAX_ATTR_COUNT;

	//Add I_FRAME_STREAM_INF_NAME_INDEX
	m_ppTagName[I_FRAME_STREAM_INF_NAME_INDEX] = (VO_CHAR*) "#EXT-X-I-FRAME-STREAM-INF";
	m_pAttrMaxCountSet[I_FRAME_STREAM_INF_NAME_INDEX] = IFRAME_STREAM_MAX_ATTR_COUNT;

	//Add ALLOW_CACHE_NAME_INDEX
	m_ppTagName[ALLOW_CACHE_NAME_INDEX] = (VO_CHAR*) "#EXT-X-ALLOW-CACHE";
	m_pAttrMaxCountSet[ALLOW_CACHE_NAME_INDEX] = ALLOW_CACHE_MAX_ATTR_COUNT;

	//Add MEDIA_NAME_INDEX
	m_ppTagName[MEDIA_NAME_INDEX] = (VO_CHAR*) "#EXT-X-MEDIA";
	m_pAttrMaxCountSet[MEDIA_NAME_INDEX] = MEDIA_MAX_ATTR_COUNT;

	//Add PLAYLIST_TYPE_NAME_INDEX
	m_ppTagName[PLAYLIST_TYPE_NAME_INDEX] = (VO_CHAR*) "#EXT-X-PLAYLIST-TYPE";
	m_pAttrMaxCountSet[PLAYLIST_TYPE_NAME_INDEX] = PALYLIST_TYPE_MAX_ATTR_COUNT;

	//Add I_FRAMES_ONLY_NAME_INDEX
	m_ppTagName[I_FRAMES_ONLY_NAME_INDEX] = (VO_CHAR*) "#EXT-X-I-FRAMES-ONLY";
	m_pAttrMaxCountSet[I_FRAMES_ONLY_NAME_INDEX] = 0;

	//Add DISCONTINUITY_NAME_INDEX
	m_ppTagName[DISCONTINUITY_NAME_INDEX] = (VO_CHAR*) "#EXT-X-DISCONTINUITY";
	m_pAttrMaxCountSet[DISCONTINUITY_NAME_INDEX] = 0;

	//Add ENDLIST_NAME_INDEX
	m_ppTagName[ENDLIST_NAME_INDEX] = (VO_CHAR*) "#EXT-X-ENDLIST";
	m_pAttrMaxCountSet[ENDLIST_NAME_INDEX] = 0;

	//Add VERSION_NAME_INDEX
	m_ppTagName[VERSION_NAME_INDEX] = (VO_CHAR*) "#EXT-X-VERSION";
	m_pAttrMaxCountSet[VERSION_NAME_INDEX] = VERSION_MAX_ATTR_COUNT;

	//Add MAP_NAME_INDEX
	m_ppTagName[MAP_NAME_INDEX] = (VO_CHAR*) "#EXT-X-MAP";
	m_pAttrMaxCountSet[MAP_NAME_INDEX] = XMAP_MAX_ATTR_COUNT;

	//Add START_NAME_INDEX
	m_ppTagName[START_NAME_INDEX] = (VO_CHAR*) "#EXT-X-START";
	m_pAttrMaxCountSet[START_NAME_INDEX] = X_START_MAX_ATTR_COUNT;

	//Add DISCONTINUITY_SEQUENCE_NAME_INDEX
	m_ppTagName[DISCONTINUITY_SEQUENCE_NAME_INDEX] = (VO_CHAR*) "#EXT-X-DISCONTINUITY-SEQUENCE";
	m_pAttrMaxCountSet[DISCONTINUITY_SEQUENCE_NAME_INDEX] = DISCONTINUITY_SEQUENCE_MAX_ATTR_COUNT;

	//Add NORMAL_URI_NAME_INDEX
	m_ppTagName[NORMAL_URI_NAME_INDEX] = (VO_CHAR*) "";
	m_pAttrMaxCountSet[NORMAL_URI_NAME_INDEX] = URI_LINE_MAX_ATTR_COUNT;
	return 0;
}

VO_VOID  C_M3U_Parser::ReleaseParseContext()
{
    if(m_ppTagName != NULL)
    {
        delete []m_ppTagName;
        m_ppTagName = NULL;
    }

	if(m_pAttrMaxCountSet != NULL)
	{
        delete []m_pAttrMaxCountSet;
        m_pAttrMaxCountSet = NULL;
	}
}


VO_VOID  C_M3U_Parser::ResetContext()
{
	m_eCurrentManifestType = M3U_UNKNOWN_PLAYLIST;
	m_eCurrentChuckPlayListType = M3U_INVALID_CHUNK_PLAYLIST_TYPE;
	m_eCurrentChuckPlayListTypeEx = M3U_NORMAL;
	ReleaseAllTagNode();
}

VO_U32  C_M3U_Parser::CheckWorkMemory(VO_U32  ulNewDataLength)
{
	if(m_pManifestData == NULL)
	{
		m_pManifestData = new VO_BYTE[ulNewDataLength*2];
		if(m_pManifestData == NULL)
		{
			return VO_RET_SOURCE2_FAIL;
		}

		memset(m_pManifestData, 0, ulNewDataLength*2);
		m_ulManifestDataMaxLength = 2*ulNewDataLength;
	}
	else
	{
		if(ulNewDataLength > m_ulManifestDataMaxLength)
		{
			delete []m_pManifestData;
			m_pManifestData = new VO_BYTE[ulNewDataLength*2];
			if(m_pManifestData == NULL)
			{
				return VO_RET_SOURCE2_FAIL;
			}

			memset(m_pManifestData, 0, ulNewDataLength*2);
			m_ulManifestDataMaxLength = 2*ulNewDataLength;
		}
	}
	return 0;
}


VO_U32   C_M3U_Parser::VerifyHeader(VO_BYTE*   pManifestData)
{
    VO_CHAR*   pFind = NULL;
	pFind = strstr((VO_CHAR*)pManifestData, "#EXT");
	if(pFind == NULL)
	{
		VOLOGI("Invalid manifest!");
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}
	else
	{
		if(memcmp(pFind, "#EXTM3U", strlen("#EXTM3U")) != 0)
		{
			VOLOGI("can't find the M3U Begin!");
			return HLS_ERR_WRONG_MANIFEST_FORMAT;
		}

		return 0;;
	}
}


VO_U32   C_M3U_Parser::ReadNextLineWithoutCopy(VO_CHAR* pSrc, VO_CHAR* pEnd, VO_CHAR** ppLine, VO_CHAR** pNext)
{
	VO_CHAR*   pLineStart = NULL;
	VO_CHAR*   pLineEnd = NULL;
	VO_CHAR*   pCur = NULL;

	if(pSrc == NULL || pSrc>=pEnd)
	{
		*ppLine = NULL;
		return 1;
	}

	pCur = pSrc;
	while(pCur<pEnd)
	{
		if((*pCur) != '\0' && (*pCur) != '\r' && (*pCur) != '\n')
		{
			if(pLineStart == NULL)
			{
				pLineStart = pCur;
				pLineEnd = pCur;
			}
			else
			{
				pLineEnd = pCur;
			}
		}
		else
		{
			if(pLineStart != NULL)
			{
				*ppLine = pLineStart;
				*pCur = '\0';
				*pNext = pCur+1;
				return 0;
			}
		}

		pCur++;
	}


	if(pLineStart == NULL)
	{
		*ppLine = NULL;
	}
	else
	{
		if(pCur == pEnd )
		{
            *ppLine = pLineStart;
			*pNext = pEnd;
			return 0;
		}
	}

    

	*pNext = pEnd;
	return 1;
}

VO_U32   C_M3U_Parser::FindAttrValueByName(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pAttrValue, VO_U32 ulAttrValueSize, VO_CHAR*   pAttrName)
{
	VO_CHAR*  pStart = NULL;
    VO_CHAR*  pEnd = NULL;
    VO_BOOL   bFindQuoteStringStart = VO_FALSE;    

	if(pOriginalXMediaLine == NULL || pAttrValue == NULL)
	{
		return 1;
	}

	pStart = strstr(pOriginalXMediaLine, pAttrName);
	if(pStart == NULL)
	{
		return 1;
	}
	else
	{
		pEnd = pOriginalXMediaLine+strlen(pOriginalXMediaLine);
		pStart = pStart+strlen(pAttrName);
		if(*pStart == '\"')
		{
			bFindQuoteStringStart = VO_TRUE;
			pStart++;
		}

		while((*pStart)!= '\"' &&
			(*pStart)!= '\0' &&
             pStart< pEnd )
		{
			if( *(pStart) ==',')
			{
                if(bFindQuoteStringStart == VO_TRUE)
			    {
					*(pAttrValue++) = *(pStart++);
			    }
			    else
			    {
					break;
			    }
			}
			else
			{
				*(pAttrValue++) = *(pStart++);
			}
		}

		return 0;
	}
}

VO_U32   C_M3U_Parser::ParseTargeDuration(VO_CHAR*   pLine)
{
    S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

    ulRet = CreateTagNode(&pTagNode, TARGETDURATION_NAME_INDEX);
    if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)":", pTagNode, TARGETDURATION_VALUE_ATTR_ID);
    AddTag(pTagNode);
    return 0;
}

VO_U32   C_M3U_Parser::ParseMediaSequence(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, MEDIA_SEQUENCE_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)":", pTagNode, MEDIA_SEQUENCE_VALUE_ATTR_ID);
    AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseByteRange(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, BYTERANGE_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseByteRangeInfo(pLine, (VO_CHAR*)":", pTagNode, BYTERANGE_RANGE_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseInf(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, INF_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseFloat(pLine, (VO_CHAR*)":", pTagNode, INF_DURATION_ATTR_ID);
    ParseString(pLine, (VO_CHAR*)",", pTagNode, INF_DESC_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseKey(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32    ulRet = 0;
    VO_U32    ulLineContentLength = 0;
    VO_CHAR*  pLineContent = NULL;
    S_ATTR_VALUE*   pAttrValue = NULL;

	ulRet = CreateTagNode(&pTagNode, KEY_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}
    
	ParseTotalLine(pLine, pTagNode, KEY_LINE_CONTENT);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseStreamInf(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32    ulRet = 0;
	VO_U32    ulLineContentLength = 0;
	VO_CHAR*  pLineContent = NULL;
	S_ATTR_VALUE*   pAttrValue = NULL;

	ulRet = CreateTagNode(&pTagNode, STREAM_INF_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)"BANDWIDTH=", pTagNode, STREAM_INF_BANDWIDTH_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"CODECS=", pTagNode, STREAM_INF_CODECS_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"VIDEO=", pTagNode, STREAM_INF_VIDEO_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"AUDIO=", pTagNode, STREAM_INF_AUDIO_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"SUBTITLES=", pTagNode, STREAM_INF_SUBTITLE_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"CLOSED-CAPTIONS=", pTagNode, STREAM_INF_CLOSED_CAPTIONS_ATTR_ID);
	ParseResolution(pLine, (VO_CHAR*)"RESOLUTION=", pTagNode, STREAM_INF_RESOLUTION_ATTR_ID);

	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseProgramDataTime(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, PROGRAM_DATE_TIME_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseString(pLine, (VO_CHAR*)":", pTagNode, PROGRAM_DATE_TIME_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseIFrameStreamInf(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32    ulRet = 0;
	VO_U32    ulLineContentLength = 0;
	VO_CHAR*  pLineContent = NULL;
	S_ATTR_VALUE*   pAttrValue = NULL;

	ulRet = CreateTagNode(&pTagNode, I_FRAME_STREAM_INF_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)"BANDWIDTH=", pTagNode, IFRAME_STREAM_BANDWIDTH_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"URI=", pTagNode, IFRAME_STREAM_URI_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"VIDEO=", pTagNode, IFRAME_STREAM_CODECS_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseAllowCache(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, ALLOW_CACHE_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseString(pLine, (VO_CHAR*)":", pTagNode, ALLOW_CACHE_VALUE_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseXMedia(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32    ulRet = 0;
	VO_U32    ulLineContentLength = 0;
	VO_CHAR*  pLineContent = NULL;
	S_ATTR_VALUE*   pAttrValue = NULL;

	ulRet = CreateTagNode(&pTagNode, MEDIA_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseString(pLine, (VO_CHAR*)"TYPE=", pTagNode, MEDIA_TYPE_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"GROUP-ID=", pTagNode, MEDIA_GROUP_ID_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"NAME=", pTagNode, MEDIA_NAME_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"DEFAULT=", pTagNode, MEDIA_DEFAULT_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"URI=", pTagNode, MEDIA_URI_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"AUTOSELECT=", pTagNode, MEDIA_AUTOSELECT_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"LANGUAGE=", pTagNode, MEDIA_LANGUAGE_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"ASSOC-LANGUAGE=", pTagNode, MEDIA_ASSOC_LANGUAGE_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"FORCED=", pTagNode, MEDIA_FORCED_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"INSTREAM-ID=", pTagNode, MEDIA_INSTREAM_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"CHARACTERISTICS=", pTagNode, MEDIA_CHARACTERISTICS_ATTR_ID);

	AddTag(pTagNode);
	return 0;}

VO_U32   C_M3U_Parser::ParsePlayListType(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, PLAYLIST_TYPE_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseString(pLine, (VO_CHAR*)":", pTagNode, PALYLIST_TYPE_VALUE_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseIFrameOnly(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, I_FRAMES_ONLY_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseDisContinuity(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, DISCONTINUITY_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseEndList(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, ENDLIST_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseVersion(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, VERSION_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)":", pTagNode, VERSION_NUMBER_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseXMap(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, MAP_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)"URI=", pTagNode, XMAP_URI_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"BYTERANGE=", pTagNode, XMAP_BYTERANGE_ATTR_ID);
	AddTag(pTagNode);
	return 0;}

VO_U32   C_M3U_Parser::ParseXStart(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, START_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)"TIME-OFFSET=", pTagNode, X_START_TIMEOFFSET_ATTR_ID);
	ParseString(pLine, (VO_CHAR*)"PRECISE=", pTagNode, X_START_PRECISE_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseDisSequence(VO_CHAR*   pLine)
{
	S_TAG_NODE*   pTagNode = NULL;
	VO_U32   ulRet = 0;

	ulRet = CreateTagNode(&pTagNode, DISCONTINUITY_SEQUENCE_NAME_INDEX);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	ParseInt(pLine, (VO_CHAR*)":", pTagNode, DISCONTINUITY_SEQUENCE_ATTR_ID);
	AddTag(pTagNode);
	return 0;
}

VO_U32   C_M3U_Parser::ParseInt(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
	VO_S64 illValue = 0;
	VO_U32   ulRet = 0;
	S_ATTR_VALUE*   pAttrValue = NULL;
	VO_CHAR    strAttrValue[1024] = {0};

	ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, pAttrName);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	if(sscanf(strAttrValue, "%llu", &illValue) <= 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	pAttrValue = new S_ATTR_VALUE;
	if(pAttrValue == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
	pAttrValue->ulDataLength = 8;
	pAttrValue->ulDataValueType = M3U_INT;
	pAttrValue->illIntValue = illValue;

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
	return 0;
}

VO_U32   C_M3U_Parser::ParseFloat(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
	float   fValue = 0;
	VO_U32   ulRet = 0;
	S_ATTR_VALUE*   pAttrValue = NULL;
	VO_CHAR    strAttrValue[1024] = {0};

	ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, pAttrName);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	if(sscanf(strAttrValue, "%f", &fValue) <= 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	pAttrValue = new S_ATTR_VALUE;
	if(pAttrValue == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
	pAttrValue->ulDataLength = 8;
	pAttrValue->ulDataValueType = M3U_FLOAT;
	pAttrValue->fFloatValue = fValue;

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
    return 0;
}

VO_U32   C_M3U_Parser::ParseString(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
    VO_CHAR*    pStringValue = NULL;
	VO_U32   ulRet = 0;
	S_ATTR_VALUE*   pAttrValue = NULL;
	VO_CHAR    strAttrValue[1024] = {0};
    VO_U32     ulSringValueLength = 0;

	ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, pAttrName);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

    if(strlen(strAttrValue) == 0)
    {
        return 0;
    }

    ulSringValueLength = strlen(strAttrValue)/4*4+8;

	pAttrValue = new S_ATTR_VALUE;
    pStringValue = new VO_CHAR[ulSringValueLength];
    
	if(pAttrValue == NULL || pStringValue == NULL)
	{
		if(pAttrValue != NULL)
		{
			delete pAttrValue;
		}
        
		if(pStringValue != NULL)
        {
			delete []pStringValue;
	    }
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
    memset(pStringValue, 0, ulSringValueLength);
    memcpy(pStringValue, strAttrValue, strlen(strAttrValue));

	pAttrValue->ulDataLength = ulSringValueLength;
	pAttrValue->ulDataValueType = M3U_STRING;
	pAttrValue->pString = pStringValue;

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
	return 0;
}

VO_U32   C_M3U_Parser::ParseByteRangeInfo(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
	VO_U32      ulRet = 0;
    VO_S64      illLength = 0;
    VO_CHAR*    pFind = NULL;
	S_ATTR_VALUE*   pAttrValue = NULL;
    S_BYTE_RANGE*   pByteRange = NULL;
	VO_CHAR    strAttrValue[1024] = {0};

	ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, pAttrName);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	if(strlen(strAttrValue) == 0)
	{
		return 0;
	}

	pFind = strstr(strAttrValue, "@");
	if(pFind != NULL)
	{
		*pFind = '\0';
		if(sscanf(strAttrValue, "%llu", &illLength) <= 0)
		{
			return HLS_ERR_WRONG_MANIFEST_FORMAT;
		}
	}
	else
	{
		if(sscanf(strAttrValue, "%llu", &illLength) <= 0)
		{
			return HLS_ERR_WRONG_MANIFEST_FORMAT;
		}
	}
	
    pAttrValue = new S_ATTR_VALUE;
    pByteRange = new S_BYTE_RANGE;
	if(pAttrValue == NULL || pByteRange == NULL)
	{
		if(pAttrValue != NULL)
        {
			delete pAttrValue;
		}

		if(pByteRange != NULL)
		{
			delete pAttrValue;
		}
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    memset(pByteRange, 0, sizeof(S_BYTE_RANGE));
	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
	pAttrValue->pRangeInfo = pByteRange;
	pAttrValue->ulDataLength = 16;
	pAttrValue->ulDataValueType = M3U_BYTE_RANGE;
    pAttrValue->pRangeInfo->ullOffset = INAVALIBLEU64;
	pAttrValue->pRangeInfo->ullLength = (VO_U64)illLength;

    if(pFind != NULL)
	{
		memset(strAttrValue, 0, 1024);
		ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, (VO_CHAR*)"@");
		if(ulRet == 0)
		{
			if(sscanf(strAttrValue, "%llu", &illLength) > 0)
			{
				pAttrValue->pRangeInfo->ullOffset = (VO_U64)illLength;
			}
		}
	}

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
	return 0;
}

VO_U32   C_M3U_Parser::ParseTotalLine(VO_CHAR*   pLine, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
	VO_U32      ulRet = 0;
	S_ATTR_VALUE*    pAttrValue = NULL;
	VO_CHAR*     pLineContent = NULL;
	VO_U32       ulLineContentLength = 0;

	ulLineContentLength = strlen(pLine)/4*4+8;
	pLineContent = new VO_CHAR[ulLineContentLength];
	pAttrValue = new S_ATTR_VALUE;
	if(pAttrValue == NULL || pLineContent == NULL)
	{
		if(pAttrValue != NULL)
		{
			delete pAttrValue;
		}

		if(pLineContent != NULL)
		{
			delete []pLineContent;
		}
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
	memset(pLineContent, 0, ulLineContentLength);	
	pAttrValue->ulDataValueType = M3U_STRING;
	memcpy(pLineContent, pLine, strlen(pLine));
	pAttrValue->pString = pLineContent;
	pAttrValue->ulDataLength = ulLineContentLength;

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
	return 0;
}

VO_U32   C_M3U_Parser::ParseResolution(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex)
{
	VO_U32      ulRet = 0;
	VO_S64      illWidth = 0;
    VO_S64      illHeight = 0;
	VO_CHAR*    pFind = NULL;
	S_ATTR_VALUE*   pAttrValue = NULL;
	S_RESOLUTION*   pResolution = NULL;
	VO_CHAR    strAttrValue[1024] = {0};

	ulRet = FindAttrValueByName(pLine, strAttrValue, 1024, pAttrName);
	if(ulRet != 0)
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	if(strlen(strAttrValue) == 0)
	{
		return 0;
	}

	pFind = strstr(strAttrValue, "x");
	if(pFind != NULL)
	{
		if(sscanf(strAttrValue, "%llux%llu", &illWidth, &illHeight) <= 0)
		{
			return HLS_ERR_WRONG_MANIFEST_FORMAT;
		}
	}
	else
	{
		return HLS_ERR_WRONG_MANIFEST_FORMAT;
	}

	pAttrValue = new S_ATTR_VALUE;
	pResolution = new S_RESOLUTION;
	if(pAttrValue == NULL || pResolution == NULL)
	{
		if(pAttrValue != NULL)
		{
			delete pAttrValue;
		}

		if(pResolution != NULL)
		{
			delete pAttrValue;
		}
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(pResolution, 0, sizeof(S_RESOLUTION));
	memset(pAttrValue, 0, sizeof(S_ATTR_VALUE));
	pAttrValue->pResolution = pResolution;
	pAttrValue->ulDataLength = 16;
	pAttrValue->ulDataValueType = M3U_DECIMAL_RESOLUTION;
	pAttrValue->pResolution->ulWidth = (VO_U32)illWidth;
	pAttrValue->pResolution->ulHeight = (VO_U32)illHeight;

	pTagNode->ppAttrArray[ulAttrIndex] = pAttrValue;
	pTagNode->ulAttrSet |=(1<<ulAttrIndex);
	return 0;
}

VO_U32   C_M3U_Parser::GetTagList(S_TAG_NODE** ppTagNode)
{
    VO_U32   ulRet = 0;
    if(ppTagNode == NULL && m_pTagNodeHeader == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
    }

	*ppTagNode = m_pTagNodeHeader;
    return 0;
}
