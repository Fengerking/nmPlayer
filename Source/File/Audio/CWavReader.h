	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CWavReader.h

Contains:	CWavReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "WavFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CWavReader :
	public CBaseAudioReader
{
public:
	CWavReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CWavReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx);

protected:
	VO_BOOL				IsDTS();
	VO_U32				DTSSyncCode( const VO_U8 *p_buf );
	VO_BOOL				ParseFileHeader();

protected:
	VO_PBYTE		m_pbFormat;
	VO_U32			m_extFormatSize;
	VO_U32			m_dwSampleSize;
	VO_U32			m_dwDataSize;
	VO_U16			m_wFileBitsPerSample;
	VO_U16			m_nFileBlockAlign;
	VO_U32			m_nFileAvgBytesPerSec;
	double			m_dSampleTime;			//mp3 sample time is not integer

	VO_U32			m_dwCurrIndex;
};

#ifdef _VONAMESPACE
}
#endif
