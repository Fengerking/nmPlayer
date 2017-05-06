	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAmrReader.h

Contains:	CAmrReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "AmrFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagAmrFramePos
{
	//0xSSSSSSNN NNNNNNNN NNNNNNNN NNNNNNNN
	//first six bit is packet size index!!
	VO_U32			frame_num;
	tagAmrFramePos*	next;

	inline VO_VOID	AddRef() {frame_num++;}
	inline VO_U8	GetPacketSize() {return (frame_num >> 26);}
	inline VO_U32	GetRefCount() {return (frame_num & 0x3FFFFFF);}
	inline VO_U32	GetTotalPacketSize() {return GetRefCount() * GetPacketSize();}
} AmrFramePos, *PAmrFramePos;

class CAmrFramePosChain : 
	public CvoBaseObject
{
public:
	CAmrFramePosChain(CMemPool* pMemPool);
	virtual ~CAmrFramePosChain();

public:
	VO_BOOL		Add(VO_U8 btPacketSize);
	VO_BOOL		GetFrameByIndex(VO_U32 dwIndex, VO_U32& dwPos, VO_U32& dwSize);
	VO_U32		GetFramIndex(VO_U32 dwPos);
	VO_U32		GetCount() {return m_dwCount;}

	VO_VOID		Release();

protected:
	PAmrFramePos	m_pHead;
	PAmrFramePos	m_pTail;
	VO_U32			m_dwCount;
	CMemPool*		m_pMemPool;
};

class CAmrReader :
	public CBaseAudioReader
{
public:
	CAmrReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CAmrReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

protected:
	VO_BOOL				DoScanFile(VO_U32 dwFrameLimit = VO_MAXU32);
	virtual VO_BOOL		ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync);

	virtual VO_U32		IsNeedScan(VO_BOOL* pIsNeedScan);

protected:
	DECLARE_USE_AMR_GLOBAL_VARIABLE

protected:
	VO_BOOL				m_bWB;
	VO_BOOL				m_bOnStream;
 	VO_U32				m_dwFrameSize;
	VO_U16				m_wPacketSizes[16];
	CAmrFramePosChain	m_fpc;
	VO_U32				m_dwCurrIndex;
};

#ifdef _VONAMESPACE
}
#endif
