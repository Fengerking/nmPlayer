/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voHLSManifestWriter.cpp

	Contains:	voHLSManifestWriter class file

	Written by:	Terry

	Change History (most recent first):
	2013-09-23		Terry			Create file

*******************************************************************************/

#include <string.h>
#include "voHLSManifestWriter.h"
#include "SDownloaderLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

C_VoHLSManifestWriter::C_VoHLSManifestWriter()
{
    m_pManifestGroup = NULL;
	memset(m_pMediaManifestArrayNew, 0, sizeof(MANIFEST_INFO*)*MAX_HLS_MANIFEST_DUMP_COUNT);
    memset(m_pMediaManifestArrayOriginal, 0, sizeof(MANIFEST_INFO*)*MAX_HLS_MANIFEST_DUMP_COUNT);
    memset(m_pMediaManifestCompareString, 0, sizeof(VO_CHAR*)*MAX_HLS_MANIFEST_DUMP_COUNT);
    memset(m_aMediaManifestId, 0, sizeof(VO_U32)*MAX_HLS_MANIFEST_DUMP_COUNT);
    m_ulMediaManifestCount = 0;

    m_pMasterManifestNew = NULL;
    m_pMasterManifestOriginal = NULL;
    m_bExistAudioSegment = VO_FALSE;
    m_bExistVideoSegment = VO_FALSE;
    m_bExistSubtitleSegment = VO_FALSE;
}

C_VoHLSManifestWriter::~C_VoHLSManifestWriter()
{
	if(m_pManifestGroup != NULL)
	{
		DeleteManifestGroupContext();
	}
}

VO_U32 C_VoHLSManifestWriter::WriteManifest(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID, MANIFEST_GROUP** ppNewManifestGroup, VO_BOOL bEnd)
{
	VO_U32 ulIndex = 0;
    VO_U32   ulRet = 0;
	MANIFEST_INFO**    ppManifestArray = NULL;

    if(m_pManifestGroup != NULL)
	{
		DeleteManifestGroupContext();
	}

	ulRet = FindMediaPlayListAndCreateDumpPathFromChunkList(pManifestList, pChunkList, pManifestID);
	ulRet = FindMasterPlayListAndGenerateDumpPath(pManifestList, pManifestID);

    if(m_pMasterManifestOriginal != NULL)
    {
		CreateManifest(m_pMasterManifestNew, m_pMasterManifestOriginal, pChunkList, MASTER_PLAYLIST, 0xffffffff);
	}

    for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
	{
		CreateManifest(m_pMediaManifestArrayNew[ulIndex], m_pMediaManifestArrayOriginal[ulIndex], pChunkList, MEDIA_PLAYLIST, ulIndex);
		if(bEnd == VO_TRUE)
        {
			memcpy(m_pMediaManifestArrayNew[ulIndex]->pManifestData+m_pMediaManifestArrayNew[ulIndex]->nLength, "#EXT-X-ENDLIST\r\n", strlen("#EXT-X-ENDLIST\r\n"));
			m_pMediaManifestArrayNew[ulIndex]->nLength += strlen("#EXT-X-ENDLIST\r\n");
        }
		else
        {
			memcpy(m_pMediaManifestArrayNew[ulIndex]->pManifestData+m_pMediaManifestArrayNew[ulIndex]->nLength, "#EXT-X-PLAYLIST-TYPE:PD\r\n", strlen("#EXT-X-PLAYLIST-TYPE:PD\r\n"));
			m_pMediaManifestArrayNew[ulIndex]->nLength += strlen("#EXT-X-PLAYLIST-TYPE:PD\r\n");
        }
	}
    
    m_pManifestGroup = new MANIFEST_GROUP;
	memset(m_pManifestGroup, 0, sizeof(MANIFEST_GROUP));

    if(m_ulMediaManifestCount == 0)
    {
        VOLOGI("no manifest available!");
        return VO_RET_SOURCE2_FAIL;
    }
    
    if(m_pMasterManifestNew != NULL)
	{
		m_pManifestGroup->nCount = 1+m_ulMediaManifestCount;
		ppManifestArray = new MANIFEST_INFO*[m_pManifestGroup->nCount];
		ppManifestArray[0] = m_pMasterManifestNew;
		for(ulIndex=1; ulIndex<m_pManifestGroup->nCount; ulIndex++)
		{
			ppManifestArray[ulIndex] = m_pMediaManifestArrayNew[ulIndex-1];
		}

		m_pManifestGroup->ppInfo = ppManifestArray;
		memcpy(m_pManifestGroup->strMaster, m_pMasterManifestNew->szUrl,strlen(m_pMasterManifestNew->szUrl));
	}
	else
	{
		m_pManifestGroup->nCount = 1;
		ppManifestArray = new MANIFEST_INFO*[m_pManifestGroup->nCount];
		for(ulIndex=0; ulIndex<m_pManifestGroup->nCount; ulIndex++)
		{
			ppManifestArray[ulIndex] = m_pMediaManifestArrayNew[ulIndex];
		}

		m_pManifestGroup->ppInfo = ppManifestArray;
		memcpy(m_pManifestGroup->strMaster, m_pMediaManifestArrayNew[0]->szUrl,strlen(m_pMediaManifestArrayNew[0]->szUrl));
	}

    *ppNewManifestGroup = m_pManifestGroup;
    return 0;
}

VO_U32 C_VoHLSManifestWriter::GenerateMasterManifestPath()
{
    return 0;
}

VO_U32 C_VoHLSManifestWriter::GeneratePlayListManifestPath()
{
    return 0;
}

VO_VOID C_VoHLSManifestWriter::AddEndTagToManifest(MANIFEST_INFO* pManifestNew)
{
    VO_CHAR*   pEndTag = (VO_CHAR*)"#EXT-X-ENDLIST\r\n";
    if(pManifestNew != NULL)
    {
		memcpy(pManifestNew->pManifestData+pManifestNew->nLength, pEndTag, strlen(pEndTag));
		pManifestNew->nLength += strlen(pEndTag);
    }
}


VO_U32 C_VoHLSManifestWriter::FindMediaPlayListAndCreateDumpPathFromChunkList(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID)
{
	list_T<CHUNKINFO *>::iterator iterChunk;
	CHUNKINFO*           pChunkNode = NULL;
    VO_CHAR*             pChunkRootURL = NULL;
    VO_CHAR*             pNewMediaManifestURL = NULL;
	VO_CHAR              strNewPath[1024] = {0};
    MANIFEST_INFO*       pNewManifestNode = NULL;
    MANIFEST_INFO*       pOriginalManifestNode = NULL;
    VO_CHAR*             pTypeString = NULL;
	VO_U32               ulAudioIndex = 0;
	VO_U32               ulVideoIndex = 0;
	VO_U32               ulAVIndex = 0;
	VO_U32               ulSubtitleIndex = 0;
	VO_U32               ulOutputIndex = 0;


	if(pChunkList == NULL)
	{
		return 1;
	}

	for(iterChunk=pChunkList->begin(); iterChunk!=pChunkList->end(); iterChunk++)
	{
		pChunkNode = (*iterChunk);
		if(pChunkNode == NULL)
		{
			continue;
		}
		else
		{
			if((pChunkNode->Flag &CHUNK_FLAG_TYPE_MAX) == 0)
			{
				continue;
			}

			if((pChunkNode->Flag & CHUNK_FLAG_STATUS_DOWNLOAD_OK) == CHUNK_FLAG_STATUS_DOWNLOAD_OK)
            {
				if((pChunkNode->Flag & CHUNK_FLAG_TYPE_AUDIO) == CHUNK_FLAG_TYPE_AUDIO)
				{
					pTypeString = (VO_CHAR*)"Audio.m3u8";
					ulOutputIndex = ulAudioIndex++;
					m_bExistAudioSegment = VO_TRUE;
				}

				if((pChunkNode->Flag & CHUNK_FLAG_TYPE_VIDEO) == CHUNK_FLAG_TYPE_VIDEO)
				{
					pTypeString = (VO_CHAR*)"Video.m3u8";
					ulOutputIndex = ulVideoIndex++;
					m_bExistVideoSegment = VO_TRUE;
				}
#if 0
				if((pChunkNode->Flag & CHUNK_FLAG_TYPE_AUDIOVIDEO) == CHUNK_FLAG_TYPE_AUDIOVIDEO)
				{
					pTypeString = "AV.m3u8";
					ulOutputIndex = ulAVIndex++;
				}
#endif
				if((pChunkNode->Flag & CHUNK_FLAG_TYPE_SUBTITLE) == CHUNK_FLAG_TYPE_SUBTITLE)
				{
					pTypeString = (VO_CHAR*)"Subtitle.m3u8";
					ulOutputIndex = ulSubtitleIndex++;
					m_bExistSubtitleSegment = VO_TRUE;
				}

				if(m_ulMediaManifestCount == 0)
				{
					memset(strNewPath, 0, 1024);
					sprintf(strNewPath, "%s_%d_%s", pManifestID, pChunkNode->pChunk->sKeyID.uTrackID, pTypeString);
					CreateManifestNode(&pNewManifestNode);
					pChunkRootURL = new VO_CHAR[strlen(pChunkNode->pChunk->szRootUrl)+4];
					memset(pChunkRootURL, 0, strlen(pChunkNode->pChunk->szRootUrl)+4);
					memcpy(pChunkRootURL, pChunkNode->pChunk->szRootUrl, strlen(pChunkNode->pChunk->szRootUrl));
					memcpy(pNewManifestNode->szUrl, strNewPath, strlen(strNewPath));
					pOriginalManifestNode = FindMediaPlayListByChunkRootURL(pManifestList, pChunkNode->pChunk->szRootUrl);
					if(pOriginalManifestNode == NULL)
					{
						SD_LOGI("Can't find the playlist!");
						delete pNewManifestNode;
						pNewManifestNode = NULL;
						continue;
					}

					m_pMediaManifestArrayNew[m_ulMediaManifestCount] = pNewManifestNode;
					m_pMediaManifestArrayOriginal[m_ulMediaManifestCount] = pOriginalManifestNode;
					m_pMediaManifestCompareString[m_ulMediaManifestCount] = pChunkRootURL;
                    m_aMediaManifestId[m_ulMediaManifestCount] = pChunkNode->pChunk->sKeyID.uTrackID;
					m_ulMediaManifestCount++;
				}
				else
				{
					if(JudgeManifestExistInLocal(pChunkNode->pChunk->szRootUrl, pChunkNode->pChunk->sKeyID.uTrackID) == VO_FALSE)
					{
						memset(strNewPath, 0, 1024);
						sprintf(strNewPath, "%s_%d_%s", pManifestID, pChunkNode->pChunk->sKeyID.uTrackID, pTypeString);
						CreateManifestNode(&pNewManifestNode);
						pChunkRootURL = new VO_CHAR[strlen(pChunkNode->pChunk->szRootUrl)+4];
						memset(pChunkRootURL, 0, strlen(pChunkNode->pChunk->szRootUrl)+4);
						memcpy(pChunkRootURL, pChunkNode->pChunk->szRootUrl, strlen(pChunkNode->pChunk->szRootUrl));
						memcpy(pNewManifestNode->szUrl, strNewPath, strlen(strNewPath));
						pOriginalManifestNode = FindMediaPlayListByChunkRootURL(pManifestList, pChunkNode->pChunk->szRootUrl);
						if(pOriginalManifestNode == NULL)
						{
							SD_LOGI("Can't find the playlist!");
						}

						m_pMediaManifestArrayNew[m_ulMediaManifestCount] = pNewManifestNode;
						m_pMediaManifestArrayOriginal[m_ulMediaManifestCount] = pOriginalManifestNode;
						m_pMediaManifestCompareString[m_ulMediaManifestCount] = pChunkRootURL;
                        m_aMediaManifestId[m_ulMediaManifestCount] = pChunkNode->pChunk->sKeyID.uTrackID;
						m_ulMediaManifestCount++;
					}
				}
			}
		}
	}

	return 0;
}


VO_U32 C_VoHLSManifestWriter::FindMasterPlayListAndGenerateDumpPath(list_T<MANIFEST_INFO*>* pManifestList, VO_PCHAR pManifestID)
{
	list_T<MANIFEST_INFO *>::iterator iterManifest;
	MANIFEST_INFO*           pManifest = NULL;
	MANIFEST_INFO*           pManifestNew = NULL;

	VO_CHAR                  strMasterIdentity[256] = {0};
	VO_CHAR                  strMasterManifestPath[1024] = {0};
	VO_U32          ulRet = 0;
    VO_U32          ulType = 0;    


	if(pManifestList == NULL)
	{
		return 0;
	}

	for(iterManifest=pManifestList->begin(); iterManifest!=pManifestList->end(); iterManifest++)
	{
		pManifest = (*iterManifest);
		if(pManifest == NULL)
		{
			continue;
		}
		else
		{
			ulType = JudgePlayListType(pManifest->pManifestData);
			if(ulType == MASTER_PLAYLIST)
			{
				break;
			}
		}
	}

	if(ulType == MASTER_PLAYLIST)
    {
		m_pMasterManifestOriginal = pManifest;
        MakeCombinedStringForMaster(strMasterIdentity);
        sprintf(strMasterManifestPath, "%s_%s%s", pManifestID, strMasterIdentity, "Master.m3u8");
		CreateManifestNode(&pManifestNew);
		memcpy(pManifestNew->szUrl, strMasterManifestPath, strlen(strMasterManifestPath));
		m_pMasterManifestNew = pManifestNew;
    }

	return 0;
}

VO_BOOL      C_VoHLSManifestWriter::JudgeManifestExistInLocal(VO_CHAR* pRootURL, VO_U32 ulPlayListId)
{
	VO_U32  ulIndex = 0;
	for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
	{
		if(memcmp(m_pMediaManifestCompareString[ulIndex], pRootURL, strlen(pRootURL)) == 0 && m_aMediaManifestId[ulIndex] == ulPlayListId)
		{
			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_U32      C_VoHLSManifestWriter::FindURIExistInLocalByIndex(VO_CHAR* pRootURL, VO_U32 ulPlayListId)
{
	VO_CHAR*        pFind = NULL;
    VO_U32   ulIndex = 0;
    VO_CHAR*        pLongURI = NULL;
    VO_CHAR*        pShortURI = NULL;


    for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
    {
		pShortURI = (strlen(m_pMediaManifestCompareString[ulIndex]) <= strlen(pRootURL))?(m_pMediaManifestCompareString[ulIndex]):(pRootURL);
		pLongURI = (pShortURI == m_pMediaManifestCompareString[ulIndex])?pRootURL:m_pMediaManifestCompareString[ulIndex];

		pFind = strstr(pLongURI, pShortURI);
		if(pFind != NULL)
		{
			if(*pShortURI == '/' || memcmp(pShortURI, "http", 4) == 0 && ulPlayListId == m_aMediaManifestId[ulIndex])
			{
				return ulIndex;
			}
			else
			{
				if(pFind > pLongURI && (*(pFind-1)) == '/' && ulPlayListId == m_aMediaManifestId[ulIndex])
				{
					return ulIndex;
				}
			}
		}
	}

	return 0xffffffff;
}



MANIFEST_INFO*    C_VoHLSManifestWriter::FindMediaPlayListByChunkRootURL(list_T<MANIFEST_INFO*>* pManifestList, VO_CHAR* pRootURL)
{
	list_T<MANIFEST_INFO *>::iterator iterManifest;
	MANIFEST_INFO*           pManifest = NULL;
	VO_U32          ulRet = 0;
	VO_CHAR*        pFind = NULL;
    VO_CHAR*        pLongURI = NULL;
    VO_CHAR*        pShortURI = NULL;

	if(pManifestList == NULL)
	{
		return NULL;
	}

 
	for(iterManifest=pManifestList->begin(); iterManifest!=pManifestList->end(); iterManifest++)
	{
		pManifest = (*iterManifest);
		if(pManifest == NULL)
		{
			continue;
		}
		else
		{
			pShortURI = (strlen(pManifest->szUrl) <= strlen(pRootURL))?(pManifest->szUrl):(pRootURL);
			pLongURI = (pShortURI == pManifest->szUrl)?pRootURL:pManifest->szUrl;


			pFind = strstr(pLongURI, pShortURI);
			if(pFind != NULL)
			{
				if(*pShortURI == '/' || memcmp(pShortURI, "http", 4) == 0)
			    {
					return pManifest;
				}
				else
				{
					if(pFind > pLongURI && (*(pFind-1)) == '/')
					{
						return pManifest;
					}
				}
			}
		}
	}
	return NULL;
}



VO_U32 C_VoHLSManifestWriter::CreateManifestGroup(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID)
{
	return 0;
}

VO_VOID C_VoHLSManifestWriter::DeleteManifestGroupContext()
{
	VO_U32   ulIndex = 0;
    for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
	{
		if(m_pMediaManifestCompareString[ulIndex] != NULL)
		{
			delete m_pMediaManifestCompareString[ulIndex];
			m_pMediaManifestCompareString[ulIndex] = 0;
		}

		if(m_pMediaManifestArrayNew[ulIndex] != NULL)
		{
			DeleteManifest(m_pMediaManifestArrayNew[ulIndex]);
			m_pMediaManifestArrayNew[ulIndex] = 0;
		}
	}


    m_bExistAudioSegment = VO_FALSE;
    m_bExistVideoSegment = VO_FALSE;
    m_bExistSubtitleSegment = VO_FALSE;

	if(m_pMasterManifestNew != NULL)
	{
		DeleteManifest(m_pMasterManifestNew);
		m_pMasterManifestNew = NULL;
	}

	if(m_pManifestGroup != NULL)
	{
		if(m_pManifestGroup->ppInfo != NULL)
		{
			delete[] m_pManifestGroup->ppInfo;
			m_pManifestGroup->ppInfo = NULL;
		}

		delete m_pManifestGroup;
		m_pManifestGroup = NULL;
	}

	m_ulMediaManifestCount = 0;

}

VO_U32 C_VoHLSManifestWriter::JudgePlayListType(VO_BYTE*  pManifestData)
{
	if(pManifestData == NULL)
	{
		return 3;
	}

	if(strstr((VO_CHAR*)pManifestData, "#EXT-X-STREAM-INF") != NULL)
	{
		return MASTER_PLAYLIST;
	}

    if(strstr((VO_CHAR*)pManifestData, "#EXTINF") != NULL)
    {
		return MEDIA_PLAYLIST;
    }

	return 3;
}

VO_U32 C_VoHLSManifestWriter::CreateManifest(MANIFEST_INFO* pManifestNew, MANIFEST_INFO* pManifestOriginal, list_T<CHUNKINFO*>* pChunkList, VO_U32  ulManifestType, VO_U32 ulIndex)
{
    if(pManifestNew == NULL || pManifestOriginal == NULL || pChunkList == NULL )
    {
		return 1;
	}

	switch(ulManifestType)
    {
	    case MASTER_PLAYLIST:
		{
			WriteMasterManifest(pManifestNew, pManifestOriginal);
            break;
		}
	    case MEDIA_PLAYLIST:
		{
			WriteMediaManifest(pManifestNew, pManifestOriginal, pChunkList, ulIndex);
			break;
		}
	}
    return 0;
}

VO_U32 C_VoHLSManifestWriter::DeleteManifest(MANIFEST_INFO* pManifest)
{
	if(pManifest != NULL)
	{
		if(pManifest->pManifestData != NULL)
		{
			delete pManifest->pManifestData;
		}
		delete pManifest;
	}
	return 0;
}

VO_U32 C_VoHLSManifestWriter::CreateManifestNode(MANIFEST_INFO** ppManifest)
{
    MANIFEST_INFO*  pManifest = NULL;

    if(ppManifest == NULL)
    {
		return 1;
    }

	pManifest = new MANIFEST_INFO;
    if(pManifest == NULL)
    {
		return 1;
    }

	memset(pManifest, 0, sizeof(MANIFEST_INFO));
    *ppManifest = pManifest;
    return 0;
}

VO_U32 C_VoHLSManifestWriter::WriteMasterManifest(MANIFEST_INFO* pMasterManifestNew, MANIFEST_INFO* pMasterManifestOriginal)
{
    VO_CHAR*  pNewM3u8Content = NULL;
    VO_U32    ulContentLength = 0;
    VO_U32    ulNewManifestLength = 0;
	VO_CHAR strLine[8192] = {0};
    VO_CHAR*  pLineCur = NULL;
    VO_CHAR*  pLineEnd = NULL;
    VO_CHAR   strEXT_X_STREAM[1024] = {0};
	VO_CHAR   strEXT_X_STREAM_URI[1024] = {0};
    VO_CHAR   strEXT_X_Media[1024] = {0};
    VO_U32    ulRet = 0;
      
    pNewM3u8Content =(VO_CHAR*) new VO_BYTE[pMasterManifestOriginal->nLength*2];
    ulContentLength = pMasterManifestOriginal->nLength*2;
    memset(pNewM3u8Content, 0, ulContentLength);

    pLineCur = (VO_CHAR*)pMasterManifestOriginal->pManifestData;
	pLineEnd = pLineCur+pMasterManifestOriginal->nLength;
	while(pLineCur<pLineEnd)
    {
		memset(strLine, 0, 8192);
		GetLine(pLineCur, strLine, 8192);
        if(strlen(strLine) == 0)
        {
			pLineCur++;
			continue;
        }

		if(strstr(strLine, "#EXT-X-MEDIA") != NULL)
		{
			memset(strEXT_X_Media, 0, 1024);
            ulRet = JudgeXMediaLineAndReplaceURI(strLine, strEXT_X_Media, 1024);
            if(ulRet == 0)
			{
				memcpy(pNewM3u8Content+ulNewManifestLength, strEXT_X_Media, strlen(strEXT_X_Media));
                ulNewManifestLength += strlen(strEXT_X_Media);
				memcpy(pNewM3u8Content+ulNewManifestLength, "\r\n", 2);
				ulNewManifestLength += 2;
			}
			pLineCur += strlen(strLine);
		}
		else if(strstr(strLine, "#EXT-X-STREAM-INF") != NULL)
		{
			memset(strEXT_X_STREAM, 0, 1024);
			memcpy(strEXT_X_STREAM, strLine, strlen(strLine));

            memset(strEXT_X_STREAM_URI, 0, 1024);
			pLineCur += strlen(strLine);
			memset(strLine, 0, 8192);
			if(pLineCur<pLineEnd)
			{
                while(strlen(strLine) == 0 && pLineCur<pLineEnd)
				{
					pLineCur++;
					GetLine(pLineCur, strLine, 1024);
				}

				if(pLineCur>=pLineEnd)
				{
					break;
				}
			}

            ulRet = JudgeXStreamURILineAndReplaceURI(strLine, strEXT_X_STREAM_URI, 1024);
            if(ulRet == 0)
            {
				memcpy(pNewM3u8Content+ulNewManifestLength, strEXT_X_STREAM, strlen(strEXT_X_STREAM));
				ulNewManifestLength += strlen(strEXT_X_STREAM);
				memcpy(pNewM3u8Content+ulNewManifestLength, "\r\n", 2);
				ulNewManifestLength += 2;
				memcpy(pNewM3u8Content+ulNewManifestLength, strEXT_X_STREAM_URI, strlen(strEXT_X_STREAM_URI));
				ulNewManifestLength += strlen(strEXT_X_STREAM_URI);
				memcpy(pNewM3u8Content+ulNewManifestLength, "\r\n", 2);
				ulNewManifestLength += 2;
			}

			pLineCur += strlen(strLine);
		}
		else if(strstr(strLine, "#EXT-X-I-FRAME-STREAM-INF") != NULL)
		{
			pLineCur += strlen(strLine);
		}
		else
		{
			memcpy(pNewM3u8Content+ulNewManifestLength, strLine, strlen(strLine));
			ulNewManifestLength += strlen(strLine);
			memcpy(pNewM3u8Content+ulNewManifestLength, "\r\n", 2);
			ulNewManifestLength += 2;
			pLineCur += strlen(strLine);
		}
    }

	m_pMasterManifestNew->pManifestData = (VO_PBYTE)pNewM3u8Content;
    m_pMasterManifestNew->nLength = ulNewManifestLength;

	//
	memset(strLine, 0, 8192);
	sprintf(strLine, "%s%s", "C:/dumpfile/", m_pMasterManifestNew->szUrl);
	FILE*  pFile = fopen(strLine, "wb");
	if(pFile != NULL)
	{
		fwrite(pNewM3u8Content, 1, ulNewManifestLength, pFile);
		fclose(pFile);
	}
	//
	return 0;
}

VO_U32 C_VoHLSManifestWriter::JudgeXMediaLineAndReplaceURI(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pNewXMediaLine, VO_U32 ulNewLineSize)
{
    VO_CHAR*  pFindStart = NULL;
    VO_CHAR*  pFindEnd = NULL;
    VO_CHAR*  pPre = NULL;
    VO_CHAR*  pPost = NULL;
    VO_CHAR*  pFindMediaType = NULL;
	VO_CHAR   strURI[256] = {0};
    VO_U32    ulRet = 0;
    VO_U32    ulMediaManifestIndex = 0xffffffff;
    VO_U32    ulPlayListId = 0xffffffff;
    VO_U32    ulPlayListType = 0xffffffff;

	if(pOriginalXMediaLine == NULL || strlen(pOriginalXMediaLine) == 0)
	{
		return 0;
	}


    if(strstr(pOriginalXMediaLine, "TYPE=AUDIO") != NULL)
    {
        ulPlayListType = HLS_WRITER_CHUNK_PLAYLISTTYPE_XAUDIO;
	}

	if(strstr(pOriginalXMediaLine, "TYPE=VIDEO") != NULL)
	{
		ulPlayListType = HLS_WRITER_CHUNK_PLAYLISTTYPE_XVIDEO;
	}

	if(strstr(pOriginalXMediaLine, "TYPE=SUBTITLES") != NULL)
	{
		ulPlayListType = HLS_WRITER_CHUNK_PLAYLISTTYPE_XSUBTITLE;
	}

	pFindStart = strstr(pOriginalXMediaLine, "URI=");
    if(pFindStart == NULL)
    {
        pFindMediaType =  strstr(pOriginalXMediaLine, "TYPE=AUDIO");
        if(pFindMediaType != NULL)
        {
            if(m_bExistAudioSegment == VO_TRUE)
            {
                //Use the extra audio;
                return 1;
            }
        }
		
        pFindMediaType =  strstr(pOriginalXMediaLine, "TYPE=SUBTITLES");
        if(pFindMediaType != NULL)
        {
            if(m_bExistSubtitleSegment == VO_TRUE)
            {
				//Use the extra audio;
				return 1;
            }
        }

		memset(pNewXMediaLine, 0, ulNewLineSize);
		memcpy(pNewXMediaLine, pOriginalXMediaLine, strlen(pOriginalXMediaLine));
		return 0;
    }
	else
	{
        ulRet = FindAttrValueByName(pOriginalXMediaLine, strURI, 256, (VO_CHAR*)"URI=");
        if(0 != ulRet)
		{
			return 1;
		}

		pPre = pFindStart+strlen("URI=");
		pFindStart = pPre;
		if(*pFindStart == '\"')
		{
			pFindStart++;
		}

		pFindEnd = pFindStart;
		while((*pFindEnd) != '\"' && 
			(*pFindEnd) != ',' &&
			(*pFindEnd) != '\0')
		{
			pFindEnd++;
		}

		if(*pFindEnd == '\"')
		{
			pPost = pFindEnd+1;
		}
		else
		{
			pPost = pFindEnd;
		}
        
		ulPlayListId = FindPlayListIdByType(ulPlayListType);
		if(ulPlayListId == 0xffffffff)
		{
			return 1;
		}

		ulMediaManifestIndex = FindURIExistInLocalByIndex(strURI, ulPlayListId);
		if(ulMediaManifestIndex != 0xffffffff)
		{
			memcpy(pNewXMediaLine, pOriginalXMediaLine, pPre-pOriginalXMediaLine);
			strcat(pNewXMediaLine, "\"");
			memcpy(pNewXMediaLine+strlen(pNewXMediaLine), m_pMediaManifestArrayNew[ulMediaManifestIndex]->szUrl, strlen(m_pMediaManifestArrayNew[ulMediaManifestIndex]->szUrl));
			strcat(pNewXMediaLine, "\"");
			memcpy(pNewXMediaLine+strlen(pNewXMediaLine),pPost, strlen(pOriginalXMediaLine)-(pPost-pOriginalXMediaLine));return 0;
		}
		else
		{
			return 1;
		}
	}
}

VO_U32 C_VoHLSManifestWriter::JudgeXStreamURILineAndReplaceURI(VO_CHAR*   pOriginalStreamURI, VO_CHAR*  pNewStreamURI, VO_U32 ulNewLineSize)
{
    VO_U32    ulMediaManifestIndex = 0xffffffff;
    VO_U32    ulPlayListId = 0xffffffff;

    ulPlayListId = FindPlayListIdByType(HLS_WRITER_CHUNK_PLAYLISTTYPE_MAINSTREAM);
    if(ulPlayListId == 0xffffffff)
    {
		return 1;
    }

	
    ulMediaManifestIndex = FindURIExistInLocalByIndex(pOriginalStreamURI, ulPlayListId);
    if(ulMediaManifestIndex != 0xffffffff)
	{
		memcpy(pNewStreamURI, m_pMediaManifestArrayNew[ulMediaManifestIndex]->szUrl, strlen(m_pMediaManifestArrayNew[ulMediaManifestIndex]->szUrl));
		memcpy(pNewStreamURI+strlen(pNewStreamURI), "\r\n", 2);
		return 0;
	}

    return 1;
}

VO_U32 C_VoHLSManifestWriter::WriteMediaManifest(MANIFEST_INFO* pMediaManifestNew, MANIFEST_INFO* pMediaManifestOriginal, list_T<CHUNKINFO*>* pChunkList, VO_U32 ulIndex)
{
	list_T<CHUNKINFO *>::iterator iterChunk;
	CHUNKINFO*           pChunkNode = NULL;
	list_T<VO_CHAR *>    pExtInfoURIList;
	list_T<VO_CHAR *>::iterator iterExtURI;
	VO_CHAR            strLine[8192] = {0};
	VO_CHAR            strLineConvert[8192] = {0};
    VO_CHAR*           pOriginalCur = NULL;
	VO_CHAR*           pOriginalEnd = NULL;
    VO_BYTE*           pNewM3u8Content = NULL;
    VO_U32             ulNewM3u8ContentLength = 0;
    VO_BOOL            bFindSequence = VO_FALSE;
    VO_CHAR*           pFind = NULL;

    if(pChunkList == NULL)
    {
        return 1;
    }

    pNewM3u8Content = new VO_BYTE[pMediaManifestOriginal->nLength*2];
	if(pNewM3u8Content == NULL)
	{
		return 1;
	}

	memset(pNewM3u8Content, 0, pMediaManifestOriginal->nLength*2);

	for(iterChunk=pChunkList->begin(); iterChunk!=pChunkList->end(); iterChunk++)
	{
		pChunkNode = (*iterChunk);
		if(pChunkNode == NULL)
		{
			continue;
		}
		else
		{
			if((pChunkNode->Flag &CHUNK_FLAG_TYPE_MAX) == 0)
			{
				continue;
			}

			if((pChunkNode->Flag & CHUNK_FLAG_STATUS_DOWNLOAD_OK) == CHUNK_FLAG_STATUS_DOWNLOAD_OK)
			{
				if(memcmp(m_pMediaManifestCompareString[ulIndex], pChunkNode->pChunk->szRootUrl, strlen(pChunkNode->pChunk->szRootUrl)) == 0 && 
				   m_aMediaManifestId[ulIndex] == pChunkNode->pChunk->sKeyID.uTrackID)
				{
					pExtInfoURIList.push_back((VO_CHAR*)(pChunkNode->FileName));
				}
			}
			else
			{
				continue;
			}
		}
	}
    


	iterExtURI = pExtInfoURIList.begin();
	pOriginalCur = (VO_CHAR*)pMediaManifestOriginal->pManifestData;
	pOriginalEnd = pOriginalCur+pMediaManifestOriginal->nLength;

    pFind = strstr(pOriginalCur, "#EXT-X-MEDIA-SEQUENCE:");
    if(pFind == NULL)
    {
		bFindSequence = VO_FALSE;
    }
	else
    {
		bFindSequence = VO_TRUE;
    }


	while(iterExtURI != pExtInfoURIList.end())
	{
		memset(strLine, 0, 8192);
		GetLine(pOriginalCur, strLine, 8192);
		if(strlen(strLine) == 0)
		{
			pOriginalCur++;
			continue;
		}

		if(strLine[0] == '#')
		{
			if(strLine[1] == 'E'&& strLine[2] == 'X')
			{
				if(memcmp(strLine, "#EXT-X-BYTERANGE", strlen("#EXT-X-BYTERANGE")) != 0 && 
				   memcmp(strLine, "#EXT-X-PLAYLIST-TYPE:VOD", strlen("#EXT-X-PLAYLIST-TYPE:VOD")) != 0	)
			    {
					if(memcmp(strLine, "#EXT-X-KEY:", strlen("#EXT-X-KEY:")) == 0)
					{
                        memset(strLineConvert, 0, 8192);
						ConvertTheKeyLine(pMediaManifestOriginal->szUrl, strLine, strLineConvert, 8192);
					}
					else
					{
						memset(strLineConvert, 0, 8192);
						memcpy(strLineConvert, strLine, strlen(strLine));
					}

					memcpy(pNewM3u8Content+ulNewM3u8ContentLength, strLineConvert, strlen(strLineConvert));
					ulNewM3u8ContentLength += strlen(strLineConvert);
					memcpy(pNewM3u8Content+ulNewM3u8ContentLength, "\r\n", 2);
					ulNewM3u8ContentLength += 2;

					if(memcmp(strLineConvert, "#EXTM3U", strlen("#EXTM3U")) == 0 && bFindSequence == VO_FALSE)
					{
						memcpy(pNewM3u8Content+ulNewM3u8ContentLength, "#EXT-X-MEDIA-SEQUENCE:0\r\n", strlen("#EXT-X-MEDIA-SEQUENCE:0\r\n"));
						ulNewM3u8ContentLength += strlen("#EXT-X-MEDIA-SEQUENCE:0\r\n");
					}
				}
			}
		}
		else
		{
			//URI Line
			memcpy(pNewM3u8Content+ulNewM3u8ContentLength, (*iterExtURI), strlen((*iterExtURI)));
			ulNewM3u8ContentLength += strlen(*iterExtURI);
			memcpy(pNewM3u8Content+ulNewM3u8ContentLength, "\r\n", 2);
			ulNewM3u8ContentLength += 2;
			iterExtURI++;
		}
		pOriginalCur += strlen(strLine);
	}

    pMediaManifestNew->pManifestData = pNewM3u8Content;
	pMediaManifestNew->nLength = ulNewM3u8ContentLength;

    //
	memset(strLine, 0, 8192);
	sprintf(strLine, "%s%s", "C:/dumpfile/", pMediaManifestNew->szUrl);
	FILE*  pFile = fopen(strLine, "wb");
	if(pFile != NULL)
	{
        fwrite(pNewM3u8Content, 1, ulNewM3u8ContentLength, pFile);
        fclose(pFile);
	}
	//
	return 0;
}


VO_U32 C_VoHLSManifestWriter::ConvertTheKeyLine(VO_CHAR*  pManifestDownLoadPath, VO_CHAR*   pOriginalLine, VO_CHAR*  pNewLine, VO_U32 ulNewLineSize)
{
	VO_CHAR   strURI[1024] = {0};
	VO_CHAR   strNewURI[1024] = {0};
	VO_CHAR   strBeforeURI[1024] = {0};
	VO_CHAR   strAfterURI[1024] = {0};
	VO_CHAR*  pFindStart = NULL;
	VO_CHAR*  pFindEnd = NULL;
	VO_U32    ulMediaManifestIndex = 0xffffffff;


    pFindStart = strstr(pOriginalLine, "METHOD=AES-128,");
    if(pFindStart == NULL)
	{
		memset(pNewLine, 0, ulNewLineSize);
        memcpy(pNewLine, pOriginalLine, strlen(pOriginalLine));
		return 0;
	}

	pFindStart = strstr(pOriginalLine, "URI=\"");
	if(pFindStart == NULL)
	{
		memset(pNewLine, 0, ulNewLineSize);
		memcpy(pNewLine, pOriginalLine, strlen(pOriginalLine));
		return 0;
	}
	else
	{
		memcpy(strBeforeURI, pOriginalLine, pFindStart-pOriginalLine);
		pFindEnd = strstr(pFindStart+strlen("URI=\""), "\"");
		memcpy(strURI, pFindStart+strlen("URI=\""), pFindEnd-(pFindStart+strlen("URI=\"")));
		memcpy(strAfterURI, pFindEnd+1, strlen(pOriginalLine)-(pFindEnd-pOriginalLine+1));
        GetTheAbsolutePath(strNewURI, strURI, pManifestDownLoadPath);
		memcpy(pNewLine, strBeforeURI, strlen(strBeforeURI));
		memcpy(pNewLine+strlen(strBeforeURI), "URI=\"", strlen("URI=\""));
		memcpy(pNewLine+strlen(pNewLine), strNewURI, strlen(strNewURI));
		memcpy(pNewLine+strlen(pNewLine), "\"", 1);
		memcpy(pNewLine+strlen(pNewLine), strAfterURI, strlen(strAfterURI));
		return 0;
	}

	return 0;
}


VO_U32 C_VoHLSManifestWriter::GetLine(VO_CHAR * pStrSource , VO_CHAR * pStrDest, VO_U32 ulMaxDesSize)
{
	VO_U32    ulIndex = 0;
	if(pStrSource == NULL || pStrDest == NULL)
	{
		return 1;
	}
	while( *pStrSource != '\r' && *pStrSource != '\n' && *pStrSource != '\0'  && ulIndex<(ulMaxDesSize-8))
	{
		*pStrDest = *pStrSource;
		pStrDest++;
		pStrSource++;
		ulIndex++;
	}

    return 0;
}

VO_U32    C_VoHLSManifestWriter::GetTheAbsolutePath(VO_CHAR* pstrDes, VO_CHAR* pstrInput, VO_CHAR* pstrRefer)
{
	VO_CHAR*    pFindForRefer = NULL;
	VO_CHAR*    pFindForInput = NULL;
	VO_CHAR*    pFindForReferForParam = NULL;
	VO_CHAR     strHelp[1024] = {0};

	if(pstrDes == NULL || pstrInput == NULL || pstrRefer == NULL)
	{
		return 1;
	}

	if(strstr(pstrInput, "http://") != NULL ||
		strstr(pstrInput, "https://") != NULL)
	{
		memcpy(pstrDes, pstrInput, strlen(pstrInput));
		return 0;
	}
	else
	{
		if((*pstrInput) == '/')
		{
			pFindForRefer = strstr(pstrRefer+strlen("https://"), "/");
			if(pFindForRefer == NULL)
			{
				memcpy(pstrDes, pstrRefer, strlen(pstrRefer));
				memcpy(pstrDes+strlen(pstrRefer), pstrInput, strlen(pstrInput));
				return 0;
			}
			else
			{
				memcpy(pstrDes, pstrRefer, pFindForRefer-pstrRefer);
				memcpy(pstrDes+(pFindForRefer-pstrRefer), pstrInput, strlen(pstrInput));
				return 0;
			}
		}
		else
		{
			pFindForReferForParam = strrchr(pstrRefer, '?');
			pFindForRefer = strrchr(pstrRefer, '/');
			if(pFindForReferForParam != NULL && (pFindForRefer > pFindForReferForParam))
			{
				memcpy(strHelp, pstrRefer, pFindForReferForParam-pstrRefer);
				pFindForRefer = strrchr(strHelp, '/');
				memcpy(pstrDes, strHelp, pFindForRefer-strHelp+1);
				memcpy(pstrDes+(pFindForRefer-strHelp+1), pstrInput, strlen(pstrInput));
			}
			else
			{
				memcpy(pstrDes, pstrRefer, pFindForRefer-pstrRefer+1);
				memcpy(pstrDes+(pFindForRefer-pstrRefer+1), pstrInput, strlen(pstrInput));
			}

			return 0;
		}
	}

	return 0;
}


VO_VOID      C_VoHLSManifestWriter::MakeCombinedStringForMaster(VO_CHAR*   pOutputString)
{
	VO_CHAR  strItem[64] = {0};
    VO_U32 ulIndex = 0;
    if(pOutputString == NULL)
    {
        return;
    }

    if(m_pMasterManifestOriginal == NULL)
    {
		memcpy(pOutputString, "", strlen(""));
		return;
    }
    
    for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
    {
        memset(strItem, 0, 64);
		sprintf(strItem, "%d_", m_aMediaManifestId[ulIndex]);
		strcat(pOutputString, strItem);
    }

	return;
}



VO_U32      C_VoHLSManifestWriter::FindAttrValueByName(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pAttrValue, VO_U32 ulAttrValueSize, VO_CHAR*   pAttrName)
{
	VO_CHAR*  pStart = NULL;
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
		pStart = pStart+strlen(pAttrName);
		if(*pStart == '\"')
		{
			pStart++;
		}

		while((*pStart)!= '\"' &&
			(*pStart)!= ',' &&
			(*pStart)!= '\0' )
		{
			*(pAttrValue++) = *(pStart++);
		}

		return 0;
	}
}

VO_U32      C_VoHLSManifestWriter::FindPlayListIdByType(VO_U32  ulType)

{
    VO_U32   ulIndex = 0;
    VO_U32   ulPlayListId = 0xffffffff;
    switch(ulType)
    {
        case HLS_WRITER_CHUNK_PLAYLISTTYPE_MAINSTREAM:
        {
            for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
			{
				if(m_aMediaManifestId[ulIndex] >= HLS_WRITER_INDEX_OFFSET_MAIN_STREAM && m_aMediaManifestId[ulIndex]<HLS_WRITER_INDEX_OFFSET_X_VIDEO)
				{
					return m_aMediaManifestId[ulIndex];
				}
			}
			break;
		}

	    case HLS_WRITER_CHUNK_PLAYLISTTYPE_XAUDIO:
        {
			for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
			{
				if(m_aMediaManifestId[ulIndex] >= HLS_WRITER_INDEX_OFFSET_X_AUDIO && m_aMediaManifestId[ulIndex]<HLS_WRITER_INDEX_OFFSET_X_SUBTITLE)
				{
					return m_aMediaManifestId[ulIndex];
				}
			}
			break;
		}
	    case HLS_WRITER_CHUNK_PLAYLISTTYPE_XVIDEO:
		{
			for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
			{
				if(m_aMediaManifestId[ulIndex] >= HLS_WRITER_INDEX_OFFSET_X_VIDEO && m_aMediaManifestId[ulIndex]<HLS_WRITER_INDEX_OFFSET_X_AUDIO)
				{
					return m_aMediaManifestId[ulIndex];
				}
			}
			break;	  
		}

		case HLS_WRITER_CHUNK_PLAYLISTTYPE_XSUBTITLE:
		{
			for(ulIndex=0; ulIndex<m_ulMediaManifestCount; ulIndex++)
			{
				if(m_aMediaManifestId[ulIndex] >= HLS_WRITER_INDEX_OFFSET_X_SUBTITLE && m_aMediaManifestId[ulIndex]<(HLS_WRITER_INDEX_OFFSET_X_SUBTITLE+0x100))
				{
					return m_aMediaManifestId[ulIndex];
				}
			}
			break;	  
		}
    }

	return 0xffffffff;
}

