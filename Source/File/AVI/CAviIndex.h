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
File:		CAviIndex.h

Contains:	The wrapper for avi index 

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

#define IS_KEY_FRAME(fs)	(!(fs & 0x80000000))
#define IS_DD_FRAME(fs)		(fs & 0x40000000)
#define GET_SIZE(fs)		(fs & 0x3FFFFFFF)

#define AVI_INDEX_IDX1_ONCE		0x1000		//4K

typedef struct tagAviMediaSampleIndexBlock {
	VO_U32	dwPrevEntries;
	VO_U32	dwEntries;
	VO_U64	ullIndexFilePos;
	VO_U64	ullStartPosInFile;
} AviMediaSampleIndexBlock, *PAviMediaSampleIndexBlock;

typedef enum
{
	AVI_INDEX_INDX			= 0X01,   /*!< every track has its own index */
	AVI_INDEX_IDX1			= 0X02,   /*!< all tracks share the file index */
	AVI_INDEXTYPE_MAX		= VO_MAX_ENUM_VALUE
} VOAVIINDEXTYPE;

//Base Class
class CAviBaseIndex
	: public CvoFileBaseParser
{
public:
	CAviBaseIndex(VOAVIINDEXTYPE Type, CGFileChunk* pFileChunk, VO_U32 dwReadCountOnce, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviBaseIndex();

public:
	virtual VO_BOOL			Init() {return VO_TRUE;}
	virtual	VO_U32			GetCount() {return m_dwCount;}
	virtual VOAVIINDEXTYPE	GetType() {return m_Type;}

public:
	//for common
	virtual VO_BOOL			GetEntryByIndex(VO_U32 dwIndex, VO_U64* pullPosInFile, VO_U32* pdwSize, VO_U64* pullDdPosInFile = VO_NULL, VO_U32* pdwDdSize = VO_NULL);

	//only for audio
	virtual VO_BOOL			GetEntryByTotalSize(VO_U32& dwTotalSize, VO_U32* pdwIndex);
	//previous only for video
	//but now 3.0.X.XXX will not use this interface!!
	virtual VO_U32			GetEntryPrevKeyFrame(VO_U32 dwIndex);
	virtual VO_U32			GetEntryNextKeyFrame(VO_U32 dwIndex);

	virtual VO_BOOL			IsChunkSizeEquilong();

protected:
	virtual VO_U32			ReadIndexFromFileB(VO_U32 dwStart, VO_U32 dwCount) = 0;
	virtual VO_U64			GetStartPos(VO_U32 dwIndex) = 0;

protected:
	VOAVIINDEXTYPE				m_Type;
	VO_U32						m_dwCount;

	VO_U32						m_dwReadCountOnce;

	PAviStandardIndexEntry		m_pEntries;
	VO_U32						m_dwEntryStart;
	VO_U32						m_dwEntryCount;
};

//AVI_INDEX_INDX
class CAviIndxIndex : public CAviBaseIndex
{
public:
	CAviIndxIndex(CGFileChunk* pFileChunk, VO_U32 dwIndexPoses, VO_U64* pullFilePos, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviIndxIndex();

public:
	virtual VO_BOOL			Init();
	virtual VO_U32			GetMaxSampleSize() {return m_dwMaxSampleSize;}
	virtual VO_U64			GetTotalSize() {return m_ullTotalSize;}

	virtual VO_U32			GetIndexBlocks(PAviMediaSampleIndexBlock* ppEntryBlocks);

protected:
	virtual VO_U32			ReadIndexFromFileB(VO_U32 dwStart, VO_U32 dwCount);
	virtual VO_U64			GetStartPos(VO_U32 dwIndex);

protected:
	VO_U32						m_dwIndexPoses;
	PAviMediaSampleIndexBlock	m_pEntryBlocks;
	VO_U32						m_dwMaxSampleSize;
	VO_U64						m_ullTotalSize;
};

//AVI_INDEX_IDX1
typedef VO_VOID (*ONINDEXENTRYCALLBACK)(VO_PTR pParent, VO_PTR pParam, PAviOriginalIndexEntry pEntry, VO_U32 dwCurrIndexNum);
class CAviIdx1Index : public CAviBaseIndex
{
public:
	CAviIdx1Index(CGFileChunk* pFileChunk, VO_U64 qwFilePos, VO_U32 dwSize, VO_BOOL bVideo, VO_U16 wStreamTwocc, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviIdx1Index();

public:
	VO_BOOL					DoScan(VO_PTR pParent, VO_PTR pParam, ONINDEXENTRYCALLBACK fCallback, VO_U32& dwFirstChunkOffset);

public:
	virtual VO_BOOL			Init();
	virtual VO_VOID			SetStartPos(VO_U64 qwStartPosInFile) {m_qwStartPosInFile = qwStartPosInFile;}
	virtual VO_VOID			AddCount(VO_U32 dwCurrIndexNum);
	virtual VO_VOID			SetAudioInfo(VO_U32 dwAvgBytesPerSec, VO_U16 nBlockAlign) {m_dwAvgBytesPerSec = dwAvgBytesPerSec; m_nBlockAlign = nBlockAlign;}

	virtual VO_BOOL			GetEntryByIndex(VO_U32 dwIndex, VO_U64* pullPosInFile, VO_U32* pdwSize, VO_U64* pullDdPosInFile, VO_U32* pdwDdSize);

protected:
	virtual VO_U32			ReadIndexFromFileB(VO_U32 dwStart, VO_U32 dwCount);
	virtual VO_U64			GetStartPos(VO_U32 dwIndex);

protected:
	VO_U64						m_qwIndexFilePos;
	VO_U64						m_qwStartPosInFile;			//(index's position + m_dwStartPosInFile) -> file's position

	//total index in file(both tracks)
	VO_U32						m_dwTotalIndex;
	//temp memory used to read index information from file
	PAviOriginalIndexEntry		m_pFileEntries;
	//index for index
	VO_U32						m_dwIndexIndex;
	VO_U32*						m_pdwIndexIndex;

	VO_BOOL						m_bVideo;
	VO_U16						m_wStreamTwocc;

	VO_U32						m_dwAvgBytesPerSec;
	VO_U16						m_nBlockAlign;

	PAviStandardIndexEntry		m_pActEntries;
};

#ifdef _VONAMESPACE
}
#endif
