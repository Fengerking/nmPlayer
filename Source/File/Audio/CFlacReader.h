	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CFlacReader.h

Contains:	CFlacReader header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseAudioReader.h"
#include "FlacFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagVOFlacSeekPoint 
{
//	VO_S64					sample_number;		//Sample number of first sample in the target frame, or 0xFFFFFFFFFFFFFFFF for a placeholder point. 
//	VO_S64					offsets;			//Offset (in bytes) from the first byte of the first frame header to the first byte of the target frame's header.
//	VO_U16					samples;			//Number of samples in the target frame. 

	VO_U32					time_stamp;			//<MS>
	VO_U32					duration;			//<MS>
	VO_U32					file_pos;			//<byte>
	VO_U32					length;				//<byte>
	tagVOFlacSeekPoint*		next;
} VOFlacSeekPoint, *PVOFlacSeekPoint;

class CFlacReader :
	public CBaseAudioReader
{
public:
	CFlacReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CFlacReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	VO_BOOL				ParseFileHeader();
	VO_BOOL				ParseBlock_StreamInfo(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_Padding(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_Application(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_SeekTable(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_VorbisComment(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_Cuesheet(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_Picture(VO_U32 dwBlockSize);
	VO_BOOL				ParseBlock_Other(VO_U32 dwBlockSize);
	VO_BOOL				FindFrameHeaderInFile();
	VO_BOOL				AddSeekPoint(VO_U32 dwTimeStamp, VO_U32 dwFilePos);
	VO_VOID				ReleaseSeekPoints();
	VO_S64				GetTimeStampByCurPos();

protected:
	VO_BYTE				m_btStreamInfo[FLAC_STREAM_INFO_BLOCK_SIZE];

	VO_U32				m_dwSamplesPerSec;
	VO_U16				m_wChannel;
	VO_U16				m_wBitsPerSample;
	VO_U16				m_wMinBlockSize;
	VO_U16				m_wMaxBlockSize;
	VO_U32				m_dwMinFrameSize;
	VO_U32				m_dwMaxFrameSize;

	VO_S64				m_llSampleCounts;
	//0 - variable
	//>0 - fixed
	VO_U16				m_wFixedBlockSize;	//<Samples>
	VO_BYTE				m_magicByte1;
	VO_BYTE				m_magicByte2;
	VO_U32				m_framesPerSecond;

	VO_U32				m_dwSeekPoints;
	PVOFlacSeekPoint	m_pSeekPoints;
	PVOFlacSeekPoint	m_pLastSeekPoint;
	PVOFlacSeekPoint	m_pCurrSeekPoint;
	VO_U32				m_dwCurrPosInSeekPoint;

	char				m_title[512];
	char				m_album[512];
	char				m_discNumber[512];
	char				m_trackNumber[512];
	char				m_artist[512];
	char				m_composer[512];
	char				m_performer[512];
	char				m_genre[512];
	char				m_date[512];
	char				m_year[512];
	PMetaDataImage		m_pImage;
};

#ifdef _VONAMESPACE
}
#endif
