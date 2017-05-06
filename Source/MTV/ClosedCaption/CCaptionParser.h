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
* @file CCaptionParser.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CCAPTIONPARSER_H_
#define _CCAPTIONPARSER_H_

#include "voCaptionParser.h"
#include "voType.h"
//#include "CDllLoad.h"
#include "voVideoParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

const VO_U32 MAX_CACHE_FRAME_COUNT = 16;

typedef VO_S32 (VO_API *VOGETVIDEOPARSERAPI)(VO_VIDEO_PARSERAPI * pParserHandle, VO_VIDEO_CODINGTYPE vType);

///<declair
class CCCPacketParser;

typedef struct
{
	VO_U64						nTimeStamp;
	VO_U32						nTotleSize;
	VO_H264_USERDATA_Params		stDataInfo;		
}UserDataBuf;
typedef struct CacheBuffer
{
	UserDataBuf							pBuf;
	VO_U32								nMaxDataBufSize;
	struct CacheBuffer *				pNext;
}CacheBufferItem,*pCacheBufferItem;

class CacheBufferList
{
public:
	CacheBufferList(VO_U32 nMaxItemCount);
	virtual ~CacheBufferList();
	VO_VOID								AddEntry(UserDataBuf* pBuffer);
	pCacheBufferItem					GetEntry(VO_BOOL bForce = VO_FALSE);
	VO_U32								GetEntryCount();
	VO_VOID								RemoveAll();
	VO_VOID								DeleteEntry(pCacheBufferItem pItem);
private:
	pCacheBufferItem					NewEntry(UserDataBuf *pBuffer);
	VO_VOID								CopyItem(UserDataBuf * stDstItem,UserDataBuf * SrcItem);
	VO_VOID								NewDataBufferForItem(pCacheBufferItem pItem,VO_U32 nSize);
public:
	pCacheBufferItem					m_pCacheBufHead;
	pCacheBufferItem					m_pCacheBufTail;
	pCacheBufferItem					m_pTmpCacheItem;
	VO_U32								m_nMaxItemCount;
};

////<class define
class CCaptionParser
{
public:
	CCaptionParser();
	virtual ~CCaptionParser();
public:
	virtual VO_U32					Open(VO_CAPTION_PARSER_INIT_INFO* pParam);
	virtual	VO_U32					Close();
	virtual	VO_U32					Process(VO_CAPTION_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32					GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);
	virtual VO_U32					SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32					GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32					GetLanguage(VO_CAPTION_LANGUAGE_INFO** pLangInfo);
	virtual VO_U32					SelectLanguage(VO_U32 nLangNum);
private:
	VO_U32							LoadVideoParser();
//	VO_U32							InitVideoParser();
	VO_U32							FreeVideoParser();
	VO_U32							ProcessData(VO_CODECBUFFER *pInData,VO_H264_USERDATA_Params* SeiPayload);
	VO_VOID							ResetStatus(){m_nCurTimestamp = 0;m_bFlush = VO_FALSE;}
	VO_U32							CreateCacheBuffer(VO_PBYTE pHeadData,VO_U32 nSize);
private:
	CCCPacketParser *				m_pPacketParser;
	CacheBufferList	*				m_pCacheBuf;
	VO_VIDEO_PARSERAPI				m_VideoParser;
	VO_HANDLE						m_pVideoParser;
	VO_U64							m_nCurTimestamp;
	VO_BOOL							m_bFlush;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CCAPTIONPARSER_H_
