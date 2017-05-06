	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CQcpReader.h

Contains:	CQcpReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "QcpFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CQcpReader :
	public CBaseAudioReader
{
public:
	CQcpReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CQcpReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

protected:
	VO_BOOL				ParseFileHeader();
	VO_BOOL				DoScanFile();

	virtual VO_BOOL		ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync);

	virtual VO_U32		IsNeedScan(VO_BOOL* pIsNeedScan);

protected:
	DECLARE_USE_QCP_GLOBAL_VARIABLE

protected:
	QcpFormat			m_Format;
	QcpVariableRate		m_VariableRate;

	VO_U16				m_wFrameSize;
	VO_U32				m_dwDataSize;
	VO_U16				m_wPacketSizes[16];

	CFramePosChain		m_fpc;
	VO_U32				m_dwCurrIndex;
};

#ifdef _VONAMESPACE
}
#endif
