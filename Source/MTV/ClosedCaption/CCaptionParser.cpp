/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file CCaptionParser.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/

#include "CCaptionParser.h"
#include "CCCPacketParser.h"
#include "CCCH264PacketParser.h"
#include "voLog.h"
#include "fMacros.h"
#include "readutil.h"
#include "CDumper.h"
#include "CSubtilePacker.h"
#include "voIndex.h"
#ifdef _WIN32
#pragma comment(lib,"voVideoParser.lib")
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifdef _CLOSED_CAPTION_DEBUG
wchar_t tmpString[1024];
#endif

VO_U32 CCaptionParser::ProcessData(VO_CODECBUFFER *pInData,VO_H264_USERDATA_Params* SeiPayload)
{
	PatternFinder find(0x000001,0x00ffffff);

	VO_PBYTE pStart = pInData->Buffer;
	VO_PBYTE pDataStart = NULL;
	VO_U32 findSize = pInData->Length;
	VO_BOOL beFindData = VO_FALSE;
	VO_U32 Count = 0;
	while (pStart < pInData->Buffer + pInData->Length)
	{
		VO_PBYTE pFindStart = NULL;
		pFindStart = find.Find(pStart,pInData->Buffer + pInData->Length);
		if (pFindStart == NULL)
		{
			if (beFindData == VO_TRUE)
			{
				VO_U32 dwWrite = 0;
				VO_U32 addsize = (pInData->Buffer + pInData->Length)-pDataStart;
				SeiPayload->buffer = new VO_U8[addsize];
				memcpy(SeiPayload->buffer,pDataStart,addsize);
				SeiPayload->size[Count] = addsize;
				SeiPayload->count++;
				
			}
			break;
			
		}
		if (((*pFindStart)&0x1f)== 0x06 && *(pFindStart+1) == 0x04)
		{
			if (beFindData == VO_FALSE)
			{
				beFindData = VO_TRUE;
				pDataStart =pFindStart+1; 
			}
			else
			{
				
			}
			pStart = pFindStart;
			
		}
		else
		{
			if (beFindData == VO_TRUE)
			{
				beFindData = VO_FALSE;

				VO_U32 dwWrite = 0;
				VO_U32 addsize = pFindStart-3-pDataStart;
				SeiPayload->buffer = new VO_U8[addsize];
				memcpy(SeiPayload->buffer,pDataStart,addsize);
				SeiPayload->size[Count] = addsize;
				SeiPayload->count++;
			}	
			pStart = pFindStart;
		}
	}
	return 0;
}
CacheBufferList::CacheBufferList(VO_U32 nMaxItemCount)
:m_pCacheBufHead(NULL)
,m_pCacheBufTail(NULL)
,m_pTmpCacheItem(NULL)
,m_nMaxItemCount(nMaxItemCount)
{

}

CacheBufferList::~CacheBufferList()
{
	RemoveAll();
}

VO_VOID CacheBufferList::RemoveAll()
{
	pCacheBufferItem pCur = m_pCacheBufHead;
	pCacheBufferItem pTmp = VO_NULL;
	while(pCur)
	{
		pTmp = pCur;
		pCur = pCur->pNext;
		DeleteEntry(pTmp);
	}
	m_pCacheBufHead = m_pCacheBufTail = VO_NULL;
	if (m_pTmpCacheItem)
	{
		DeleteEntry(m_pTmpCacheItem);
	}
}

VO_VOID CacheBufferList::CopyItem(UserDataBuf * stDstItem,UserDataBuf * SrcItem)
{
	if (!stDstItem || !SrcItem)
	{
		return;
	}
	stDstItem->nTimeStamp = SrcItem->nTimeStamp;
	stDstItem->nTotleSize = SrcItem->nTotleSize;
	stDstItem->stDataInfo.count = SrcItem->stDataInfo.count;
	memcpy(stDstItem->stDataInfo.size,SrcItem->stDataInfo.size,255 * sizeof(VO_U32));
	if (stDstItem->stDataInfo.buffer && SrcItem->stDataInfo.buffer)
	{
		memcpy(stDstItem->stDataInfo.buffer,SrcItem->stDataInfo.buffer,SrcItem->nTotleSize);
	}
}
pCacheBufferItem CacheBufferList::NewEntry(UserDataBuf *pBuffer)
{
	pCacheBufferItem pTempItem;
	if (!m_pTmpCacheItem)
	{
		pTempItem = new CacheBufferItem;
		if(!pTempItem)
			return NULL;
		pTempItem->pNext = NULL;
		NewDataBufferForItem(pTempItem,pBuffer->nTotleSize);
		CopyItem(&pTempItem->pBuf,pBuffer);
	}
	else
	{	
		if (m_pTmpCacheItem->nMaxDataBufSize < pBuffer->nTotleSize)
		{
			delete []m_pTmpCacheItem->pBuf.stDataInfo.buffer;
			m_pTmpCacheItem->pBuf.stDataInfo.buffer = NULL;
			NewDataBufferForItem(m_pTmpCacheItem,pBuffer->nTotleSize);
		}
		CopyItem(&m_pTmpCacheItem->pBuf,pBuffer);
		pTempItem = m_pTmpCacheItem;
	}
	return pTempItem;
}

VO_VOID CacheBufferList::DeleteEntry(pCacheBufferItem pItem)
{
	if (pItem && pItem->pBuf.stDataInfo.buffer)
	{
		delete []pItem->pBuf.stDataInfo.buffer;
		pItem->pBuf.stDataInfo.buffer = NULL;
		delete pItem;
		pItem = NULL;
	}
}

VO_U32 CacheBufferList::GetEntryCount()
{
	VO_U32 nItemCount = 0;
	pCacheBufferItem pTmp = m_pCacheBufHead;
	while (pTmp)
	{
		nItemCount++;
		pTmp = pTmp->pNext;
	}
	return nItemCount;
}

VO_VOID CacheBufferList::AddEntry(UserDataBuf* pBuffer)
{
	CacheBufferItem * pTmpItem = NewEntry(pBuffer);
	if(!pTmpItem)
		return;
	
	if (!m_pCacheBufTail)
	{
		m_pCacheBufHead = m_pCacheBufTail = pTmpItem;
	}
	else
	{
		CacheBufferItem * pTmp = m_pCacheBufHead;
		CacheBufferItem * pPre = NULL;
		while (pTmp)
		{
			if (pTmp->pBuf.nTimeStamp > pTmpItem->pBuf.nTimeStamp )
			{
				break;
			}
			pPre = pTmp;
			pTmp = pTmp->pNext;
		}
		if (!pTmp)
		{
			pPre->pNext = pTmpItem;
			return;
		}
		if (!pPre)
		{
			CacheBufferItem * pTmp2 = m_pCacheBufHead;
			m_pCacheBufHead = pTmpItem;
			m_pCacheBufHead->pNext = pTmp2;
		}
		else
		{
			pPre->pNext = pTmpItem;
			pTmpItem->pNext = pTmp;
		}
	}
}

pCacheBufferItem CacheBufferList::GetEntry(VO_BOOL bForce)
{
	if (m_pCacheBufHead && (GetEntryCount() == m_nMaxItemCount + 1 || bForce))
	{
		pCacheBufferItem pTmpEntry = m_pCacheBufHead;
		m_pCacheBufHead = m_pCacheBufHead->pNext;
	
		if (!m_pCacheBufHead)
		{
			m_pCacheBufTail = NULL;
		}
		pTmpEntry->pNext = NULL;
		
		m_pTmpCacheItem = pTmpEntry;
		return m_pTmpCacheItem;
	}
	return NULL;
}

VO_VOID CacheBufferList::NewDataBufferForItem(pCacheBufferItem pItem,VO_U32 nSize)
{
	pItem->pBuf.stDataInfo.buffer = new VO_BYTE[nSize];
	pItem->nMaxDataBufSize = nSize;
}

CCaptionParser::CCaptionParser()
:m_pPacketParser(NULL)
,m_pCacheBuf(NULL)
,m_pVideoParser(NULL)
,m_nCurTimestamp(0)
,m_bFlush(VO_FALSE)
{
	
}

CCaptionParser::~CCaptionParser()
{
	Close();
}

VO_U32 CCaptionParser::Open(VO_CAPTION_PARSER_INIT_INFO* pParam)
{
	if (pParam->nType == VO_CAPTION_TYPE_EIA608 || pParam->nType == VO_CAPTION_TYPE_EIA708
		||pParam->nType == VO_CAPTION_TYPE_DEFAULT_608 || pParam->nType == VO_CAPTION_TYPE_DEFAULT_708)
	{
		m_pPacketParser = new CCCH264PacketParser(pParam);
		if (!m_pPacketParser)
			return VO_ERR_PARSER_OPEN_FAIL;
		
		VO_U32 ret = 0;

		ret = LoadVideoParser();
		if (ret != VO_ERR_NONE)
		{
			VOLOGE("ClosedCaption Error:Load VideoParser lib Failed");
			return VO_ERR_PARSER_OPEN_FAIL;
		}
		m_VideoParser.Init(&m_pVideoParser);
		if (!m_pVideoParser)
		{
			VOLOGE("ClosedCaption Error:VideoParser Init failed");
			FreeVideoParser();
			return VO_ERR_PARSER_OPEN_FAIL;
		}
		
		if (pParam->stDataInfo.nSize == 0 || !pParam->stDataInfo.pHeadData)
		{
			VOLOGE("ClosedCaption Error:Wrong DataInfo");
			FreeVideoParser();
			return VO_ERR_PARSER_OPEN_FAIL;
		}
		
		ret = CreateCacheBuffer(pParam->stDataInfo.pHeadData,pParam->stDataInfo.nSize);
	}
	else if (pParam->nType == VO_CAPTION_TYPE_ARIB)
	{
		m_pPacketParser = new CEIAHeaderPacketParser(pParam);
		if (!m_pPacketParser)
			return VO_ERR_PARSER_OPEN_FAIL;
	}
	else
		return VO_ERR_PARSER_OPEN_FAIL;

	if (VO_ERR_PARSER_OK != m_pPacketParser->Open(pParam))
	{
		SAFE_DELETE(m_pPacketParser);
		return VO_ERR_PARSER_OPEN_FAIL;
	}

	return VO_ERR_PARSER_OK;
}

VO_U32 CCaptionParser::Close()
{
	SAFE_DELETE(m_pPacketParser);
	SAFE_DELETE(m_pCacheBuf);
	FreeVideoParser();

	return VO_ERR_PARSER_OK;
}

VO_U32 CCaptionParser::Process(VO_CAPTION_PARSER_INPUT_BUFFER* pBuffer)
{
	if (!pBuffer || !pBuffer->nSize || !pBuffer->pData)
	{
		VOLOGE("Process Error data");
		return VO_ERR_PARSER_OK;
	}
	UserDataBuf stUserBuf;
	memset(&stUserBuf,0,sizeof(stUserBuf));
	stUserBuf.nTimeStamp = pBuffer->nTimeStamp;
	stUserBuf.stDataInfo.count = 1;
	stUserBuf.nTotleSize = stUserBuf.stDataInfo.size[0] = pBuffer->nSize;
	stUserBuf.stDataInfo.buffer = pBuffer->pData;

	UserDataType eDataType = UserDataType_Sei;
	if (m_pPacketParser->GetDataInfoType() == 0)
	{
		eDataType = UserDataType_Sei_Payload;
		VO_CODECBUFFER stBuffer;
		stBuffer.Length = stUserBuf.nTotleSize;
		stBuffer.Buffer = stUserBuf.stDataInfo.buffer;
		CDumper::DumpRawData((VO_PBYTE)&stBuffer.Length,4);
		CDumper::DumpRawData((VO_PBYTE)&pBuffer->nTimeStamp,8);
		CDumper::DumpRawData((VO_PBYTE)stBuffer.Buffer,stBuffer.Length);

		m_VideoParser.Process(m_pVideoParser,&stBuffer);

		VO_H264_USERDATA_Params SeiPayload;
		memset(&SeiPayload,0,sizeof(SeiPayload));
		m_VideoParser.GetParam(m_pVideoParser,VO_PID_VIDEOPARSER_USERDATA,&SeiPayload);
	//	ProcessData(&stBuffer,&SeiPayload);

		stUserBuf.stDataInfo = SeiPayload;
		VO_U32 nSeiCnt = 0;
		stUserBuf.nTotleSize = 0;
		while(nSeiCnt < SeiPayload.count && SeiPayload.size[nSeiCnt] > 0)
		{
			stUserBuf.nTotleSize += SeiPayload.size[nSeiCnt];
			nSeiCnt++;
		}
	}
	m_pCacheBuf->AddEntry(&stUserBuf);
	pCacheBufferItem pEntry = m_pCacheBuf->GetEntry(); 
	if (pEntry)
	{
		VO_U32 nCnt = 0,nBufPos = 0;
		while(nCnt < pEntry->pBuf.stDataInfo.count)
		{
			m_pPacketParser->Process(pEntry->pBuf.stDataInfo.buffer+nBufPos,pEntry->pBuf.stDataInfo.size[nCnt],eDataType);
			nBufPos += pEntry->pBuf.stDataInfo.size[nCnt];
			nCnt++;
		}	
		m_nCurTimestamp = pEntry->pBuf.nTimeStamp;
	}
#if 0
	if (stUserBuf.stDataInfo.buffer)
	{
		delete stUserBuf.stDataInfo.buffer;
	}
#endif
	return VO_ERR_PARSER_OK;
}

VO_U32 CCaptionParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
{
	if (m_bFlush == VO_TRUE)
	{
		if (m_pCacheBuf->m_pTmpCacheItem)
		{
			m_pCacheBuf->DeleteEntry(m_pCacheBuf->m_pTmpCacheItem);
			m_pCacheBuf->m_pTmpCacheItem = NULL;
		}
		pCacheBufferItem pEntry = m_pCacheBuf->GetEntry(VO_TRUE); 
		if (pEntry)
		{
			VO_U32 nCnt = 0;
			UserDataType eDataType = UserDataType_Sei;
			if (m_pPacketParser->GetDataInfoType() == 0)
			{
				eDataType = UserDataType_Sei_Payload;
			}
			else
				eDataType = UserDataType_Sei;

			while(nCnt < pEntry->pBuf.stDataInfo.count)
			{
				m_pPacketParser->Process(pEntry->pBuf.stDataInfo.buffer,pEntry->pBuf.stDataInfo.size[nCnt],eDataType);
				nCnt++;
			}	
			m_nCurTimestamp = pEntry->pBuf.nTimeStamp;
			m_pCacheBuf->DeleteEntry(pEntry);
			m_pCacheBuf->m_pTmpCacheItem = NULL;
		}
		else
		{
			return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
		}
	}
	VO_U32 ret = m_pPacketParser->GetData(pBuffer);
	if (pBuffer->pSubtitleInfo)
	{
		pBuffer->pSubtitleInfo->nTimeStamp = m_nCurTimestamp;
		if (pBuffer->pSubtitleInfo->pSubtitleEntry)
		{
			voSubtitleRect rect = pBuffer->pSubtitleInfo->pSubtitleEntry->stSubtitleRectInfo.stRect;
			VOLOGI("ClosedCaption Info:Windwo==nTop==%d:nLeft==%d:nBottom==%d:nRight==%d",rect.nTop
				,rect.nLeft
				,rect.nBottom
				,rect.nRight);
			if(pBuffer->pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo)
			{
				voSubtitleRect rect = pBuffer->pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->stTextRowDes.stDataBox.stRect;
				VOLOGI("ClosedCaption Info:Line==nTop==%d:nLeft==%d:nBottom==%d:nRight==%d",rect.nTop
					,rect.nLeft
					,rect.nBottom
					,rect.nRight);
			}
		}

#ifdef _CLOSED_CAPTION_DEBUG
		if (pBuffer->pSubtitleInfo->pSubtitleEntry)
		{
			if(pBuffer->pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo)
			{
				pvoSubtitleTextRowInfo pTextRowInfo = pBuffer->pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo;
				VO_U32 RowCnt = 0;
				VO_U32 iRow = 0;

				memset(tmpString,0x0,1024*sizeof(wchar_t));

				swprintf(tmpString,1024,L"\r\nTimeStamp==%lld\n",pBuffer->pSubtitleInfo->nTimeStamp);

				while(pTextRowInfo)
				{
					pvoSubtitleTextInfoEntry pTmpTextInfoEntry = pTextRowInfo->pTextInfoEntry;
					while (pTmpTextInfoEntry)
					{
						RowCnt += pTmpTextInfoEntry->nSize;

						pTmpTextInfoEntry = pTmpTextInfoEntry->pNext;
					}
					
					RowCnt /= 2;

					VO_U32 nStrLen = wcslen(tmpString);
					VO_U32 nCnt = 0;
					VO_U16 *pStr = (VO_U16*)pTextRowInfo->pTextInfoEntry->pString;

					while(nCnt < RowCnt)
					{
						*(tmpString+nStrLen+nCnt) = pStr[nCnt];
						nCnt++;
					}
					wcscat(tmpString,L"\n");

					DumpStringInfo(tmpString);
					memset(tmpString,0x0,1024*sizeof(TCHAR));

					pTextRowInfo = pTextRowInfo->pNext;
					RowCnt = 0;
				}
				
			}
			
		}
#endif
	}
	if (m_bFlush)
	{
		return VO_ERR_PARSER_OK;
	}
	return ret;
}

VO_U32 CCaptionParser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	VOLOGE("uID==%d",uID);
	if (VO_PARAMETER_ID_FLUSH == uID)
	{
		m_bFlush = VO_TRUE;
	}
	else if (VO_PARAMETER_ID_DATAINFO == uID)
	{
		VOLOGE("VO_PARAMETER_ID_DATAINFO");
		if (!pParam)
		{
			return VO_ERR_PARSER_INVALID_ARG;
		}
		
		VO_CAPTION_DATA_INFO * pDataInfo = (VO_CAPTION_DATA_INFO *)pParam;

		VOLOGE("VO_PARAMETER_ID_DATAINFO:%d:%d",pDataInfo->nType,pDataInfo->nSize);
		VO_U32 ret = CreateCacheBuffer(pDataInfo->pHeadData,pDataInfo->nSize);
		if (ret != VO_ERR_PARSER_OK)
		{
			return ret;
		}
		ResetStatus();
	}
	else if (VO_PARAMETER_ID_RESTART == uID)
	{
		
		m_pCacheBuf->RemoveAll();
		ResetStatus();
	}
	else if (VO_PID_COMMON_LOGFUNC == uID)
	{
		VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
//		vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}
	return m_pPacketParser->SetParam(uID,pParam);
}

VO_U32 CCaptionParser::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CCaptionParser::LoadVideoParser()
{
	return voGetVideoParserAPI(&m_VideoParser,VO_VIDEO_CodingH264);
}

VO_U32 CCaptionParser::FreeVideoParser()
{
	if (m_pVideoParser)
	{
		m_VideoParser.Uninit(m_pVideoParser);
		m_pVideoParser = NULL;
	}
	return 0;
}

VO_U32 CCaptionParser::CreateCacheBuffer(VO_PBYTE pHeadData,VO_U32 nSize)
{
	if (!pHeadData || nSize == 0)
	{
		return VO_ERR_PARSER_INVALID_ARG;
	}
	SAFE_DELETE(m_pCacheBuf);

	VO_CODECBUFFER stBuffer;
	stBuffer.Length = nSize;
	stBuffer.Buffer = pHeadData;

	CDumper::DumpRawData((VO_PBYTE)&stBuffer.Length,4);
	VO_U64 time = 0;
	CDumper::DumpRawData((VO_PBYTE)&time,8);
	CDumper::DumpRawData((VO_PBYTE)stBuffer.Buffer,stBuffer.Length);

	m_VideoParser.Process(m_pVideoParser,&stBuffer);

	VO_U32 DPBSize = 0;
	m_VideoParser.GetParam(m_pVideoParser,VO_PID_VIDEOPARSER_DPB_SIZE,&DPBSize);
	if ( DPBSize == 0 || DPBSize > 16 )
	{
		VOLOGE("ClosedCaption Error:Wrong DPBSize");
		return VO_ERR_PARSER_INVALID_ARG;
	}
	m_pCacheBuf = new CacheBufferList(DPBSize);
	if(!m_pCacheBuf)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	
	return VO_ERR_PARSER_OK;
}

VO_U32 CCaptionParser::GetLanguage(VO_CAPTION_LANGUAGE_INFO** pLangInfo)
{
	//VOLOGE("CCaptionParser::GetLanguage");
	if (!m_pPacketParser)
	{
		return VO_ERR_PARSER_FAIL;
	}
	return m_pPacketParser->GetLanguage(pLangInfo);
}

VO_U32 CCaptionParser::SelectLanguage(VO_U32 nLangNum)
{
	if (!m_pPacketParser)
	{
		return VO_ERR_PARSER_FAIL;
	}
	return m_pPacketParser->SelectLanguage(nLangNum);
}