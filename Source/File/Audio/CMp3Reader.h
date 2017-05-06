	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CMp3Reader.h

Contains:	CMp3Reader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "Mp3FileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MP3_EXT_DATA_LEN		12
class CMp3Reader :
	public CBaseAudioReader
{
public:
	CMp3Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CMp3Reader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

protected:
	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam);

protected:
	//process header function!!
	VO_BOOL				ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit = VO_FALSE);
	//return next header, NULL - fail!!
	VO_BOOL				GetNextHeader(VO_PBYTE pHeader);
	VO_PBYTE			FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	inline VO_BOOL		IsHeader(VO_PBYTE pHeader);

	VO_BOOL				ReadVBRHeader();
	VO_BOOL				CheckVBRHeader(VO_MP3VBRHEADER_TYPE& HeaderType);
	VO_BOOL				ReadXINGHeader(VO_BOOL bIsRealVBR);
	VO_BOOL				ReadVBRIHeader();

	virtual VO_BOOL		ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync);

	virtual VO_U32		InitMetaDataParser();

protected:
	DECLARE_USE_MP3_GLOBAL_VARIABLE

protected:
	CFramePosChain	m_fpc;

	MPA_VERSION		m_Version;
	MPA_LAYER		m_Layer;
	MPA_CHANNEL		m_ChannelMode;

	VO_U32			m_dwSamplesPerSec;
	VO_U32			m_dwSamplesPerFrame;
	VO_U32			m_dwAvgBytesPerSec;
	VO_U32			m_dwFrameSize;			//frame size, 0 indicate VBR
	VO_BOOL			m_bLSF;					//true means lower sampling frequencies (=MPEG2/MPEG2.5)
	VO_U32			m_dwFrames;

	double			m_dSampleTime;			//mp3 sample time is not integer
	VO_BYTE			m_btHeader[4];
	VO_BYTE			m_btHeadData[MP3_EXT_DATA_LEN];

	EncoderDelays	m_enDelays;				// encoder delays for Mp3 with Info Tag

	VO_U32			m_dwCurrIndex;
};

#ifdef _VONAMESPACE
}
#endif
