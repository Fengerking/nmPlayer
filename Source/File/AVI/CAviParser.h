/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviParser.h

Contains:	provides the interfaces to parse an avi file according to the avi specification 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#pragma once

#include "CvoFileParser.h"
#include "AviFileDataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef struct tagAviTrackInfo {
	VO_U8			btStreamNum;
	AviStreamHeader	StreamHeader;
	VO_U32			dwPropBufferSize;
	VO_PBYTE		pPropBuffer;
	tagAviTrackInfo	*next;
} AviTrackInfo, *PAviTrackInfo;

typedef struct tagAviProposedIndexInfo {
	VO_U32						dwChunkId;
	VO_U32						dwEntriesInUse;
	VO_U64*						pullFilePos;
	tagAviProposedIndexInfo*	next;
} AviProposedIndexInfo, *PAviProposedIndexInfo;

#define FLAG_INDX			0x1
#define FLAG_IDX1			0x2
class CAviHeaderParser : 
	public CvoFileHeaderParser
{
public:
	CAviHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviHeaderParser();

public:
	//parse file header to get information
	virtual VO_BOOL			ReadFromFile();

protected:
	//get stream information and index data in the middle of file(probably it is not occur)
	virtual VO_BOOL			ReadHdrl();
	//base function of ReadHdrl()
	virtual VO_U32			ReadHdrlB();
	//base function of Read Strl List
	virtual VO_U32			ReadStrlB();
	//base function of Read INFO List
	virtual VO_U32			ReadInfoB();
	//base function of Read Odml List
	virtual VO_U32			ReadOdmlB();

	//base function of Read Other data except Hdrl
	//return value: 0 - error; 1 - success; 2 - success & end
	virtual VO_U32			ReadDataB();

public:
	VO_VOID					ReleaseTmpInfo();

	VO_U8					GetStreamCount() {return m_btStreamCount;}
	PAviTrackInfo			GetTrackInfoPtr() {return m_paTrackInfo;}
	PAviProposedIndexInfo	GetProposedIndexInfo(VO_U16 wStreamTwocc);
	PAviProposedIndexInfo	GetProposedIndexInfoPtr() {return m_paProposedIndexInfo;}
	VO_U64					GetIdx1FilePos() {return m_ullIdx1FilePos;}
	VO_U32					GetIdx1Size() {return m_dwIdx1Size;}
	VO_BOOL					HasIndxIndex() {return (m_btIndexFlags & FLAG_INDX) ? VO_TRUE : VO_FALSE;}
	VO_BOOL					HasIdx1Index() {return (m_btIndexFlags & FLAG_IDX1) ? VO_TRUE : VO_FALSE;}
	VO_BOOL					HasFileIndex() {return m_btIndexFlags ? VO_TRUE : VO_FALSE;}
	VO_VOID					RemoveIndex(VO_U8 btIndexFlag) {m_btIndexFlags &= (~btIndexFlag);}
	VO_BOOL					GetStrdContent(VO_PBYTE* ppStrdContent, VO_U32* pdwStrdLength);
	VO_U64					GetMediaDataFilePos() {return m_ullMediaDataFilePos;}
	VO_U64					GetMediaDataSize() {return m_ullMediaDataSize;}

protected:
	VO_PBYTE				m_pStrdContent;
	VO_U32					m_dwStrdLength;

	VO_U64					m_ullMediaDataFilePos;
	VO_U64					m_ullMediaDataSize;

	VO_U8					m_btStreamCount;
	PAviTrackInfo			m_paTrackInfo;

	//0x000000BA
	//B: 'indx' index
	//A: 'idx1' index
	VO_U8					m_btIndexFlags;

	PAviProposedIndexInfo	m_paProposedIndexInfo;

	VO_U64					m_ullIdx1FilePos;
	VO_U32					m_dwIdx1Size;
};

typedef struct tagAviDataParserInitParam
{
	VO_U8		btStreamCount;

	tagAviDataParserInitParam(VO_U8 stream_count)
		: btStreamCount(stream_count)
	{
	}
} AviDataParserInitParam, *PAviDataParserInitParam;

typedef struct tagAviDataParserSample
{
	VO_U16			wStreamTwocc;
	VO_U32			dwLen;			//first bit mean if 'dd' chunk
	CGFileChunk*	pFileChunk;
} AviDataParserSample, *PAviDataParserSample;
class CAviDataParser : 
	public CvoFileDataParser
{
public:
	CAviDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam);
	VO_BOOL					SetStartFilePos(VO_U64 ullStartFilePos);

protected:
	virtual VO_BOOL			StepB();
	inline VO_BOOL			ReadAviObject(AviObject& ao);

protected:
	VO_U8					m_btStreamCount;
};

#ifdef _VONAMESPACE
}
#endif
