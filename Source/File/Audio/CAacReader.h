	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAacReader.h

Contains:	CAacReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "AacFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAacReader :
	public CBaseAudioReader
{
public:
	CAacReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CAacReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

protected:
	VO_BOOL				ReadADIFHeader();
	//process header function!!
	VO_BOOL				ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit = VO_FALSE);
	//return next header, NULL - fail!!
	VO_BOOL				GetNextHeader(VO_PBYTE pHeader);
	VO_PBYTE			FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	inline VO_BOOL		IsHeader(VO_PBYTE pHeader);

	VO_BOOL				DoScanFile(VO_BOOL bExactScan);
	virtual VO_BOOL		ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync);

	virtual VO_U32		InitMetaDataParser();

	virtual VO_U32		IsNeedScan(VO_BOOL* pIsNeedScan);

protected:
	DECLARE_USE_AAC_GLOBAL_VARIABLE

protected:
	VO_BOOL			m_bADIF;	//Audio Data Interchange Format(Raw Data)
	VO_U32			m_dwAvgBytesPerSec;

	VO_BYTE			m_btHeader[4];

	VO_PBYTE		m_pHeadData;
	VO_U16			m_dwHeadData;

	CFramePosChain	m_fpc;

	VO_U8			m_btChannels;
	VO_U8			m_btSampleRateIndex;
	VO_U8			m_btProfile;
	double			m_dSampleTime;			//aac sample time is not integer

	VO_U32			m_dwCurrIndex;
};

#ifdef _VONAMESPACE
}
#endif
