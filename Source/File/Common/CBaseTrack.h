	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CBaseTrack.h

Contains:	CBaseTrack header file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#ifndef __CBaseTrack_H__
#define __CBaseTrack_H__

#include "voYYDef_filcmn.h"
#include "voFile.h"
#include "CvoBaseMemOpr.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum
{
	VOTT_VIDEO				= 0X00000001,	/*!< video track */
	VOTT_AUDIO				= 0X00000002,	/*!< audio track */
	VOTT_TEXT				= 0X00000003,   /*!< text track */
	VOTT_METADATA			= 0X00000004,	/*!< Meta track */
	VOTT_MAX				= VO_MAX_ENUM_VALUE
} VO_TRACKTYPE;

typedef struct
{
	VO_S64	llCurTs;
	VO_S64	llCurDuration;
	VO_BOOL EndOfFile;
}VO_PUSH_TRACK_INFO;

class CBaseTrack
	: public CvoBaseMemOpr
	, public CvoBaseObject
{
	friend class CBaseReader;

public:
	CBaseTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, VO_MEM_OPERATOR* pMemOp);
	virtual ~CBaseTrack();

public:
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetInfo(VO_SOURCE_TRACKINFO* pTrackInfo);
	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData) = 0;
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize) = 0;
	virtual VO_U32		GetCodec(VO_U32* pCodec);
	virtual VO_U32		GetCodecCC(VO_U32* pCC) = 0;
	virtual VO_U32		GetFirstFrame(VO_SOURCE_SAMPLE* pSample) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate) = 0;
	virtual VO_U32		GetFrameNum(VO_U32* pdwFrameNum) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetFrameTime(VO_U32* pdwFrameTime) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader);
	virtual VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime) {return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_U32		SetInUsed(VO_BOOL bInUsed);
	virtual VO_BOOL		IsInUsed() {return m_bInUsed;}

	virtual VO_TRACKTYPE	GetType() {return m_nType;}
	virtual VO_U32		SetEndOfStream(VO_BOOL bEndOfStream);
	virtual VO_BOOL		IsEndOfStream() {return m_bEndOfStream;}
	virtual VO_U8		GetStreamNum() {return m_btStreamNum;}
	virtual VO_U32		GetDuration() {return m_dwDuration;}
	virtual VO_U32		SetDuration(VO_U32 dwDuration);

	virtual VO_U32		SetPlayMode(VO_SOURCE_PLAYMODE PlayMode);
	virtual VO_SOURCE_PLAYMODE	GetPlayMode() {return m_PlayMode;}
	
	//10/18/2011 leon, add for push &play
	virtual VO_U64 GetRealDurationByFilePos(VO_U64 pos){ return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_VOID		SetLastTimeStamp(VO_U64 llTs){m_llLastTimeStamp = llTs;}
	virtual VO_U64		GetLastTimeStamp(){return m_llLastTimeStamp;}
protected:
	//initialize
	virtual VO_U32		Init(VO_U32 nSourceOpenFlags);
	//uninitialize
	virtual VO_U32		Uninit();

	//prepare for playback
	virtual VO_U32		Prepare();
	//unprepare for playback
	virtual VO_U32		Unprepare();

	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample) = 0;
	virtual VO_U32		SetPosN(VO_S64* pPos) = 0;
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample) = 0;
	virtual VO_U32		SetPosK(VO_S64* pPos) = 0;

	inline VO_U32		FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_U32 dwSize);
	virtual VO_BOOL		GetVideoHeadDataFromBuffer(VO_U32 nCodec, VO_PBYTE pBuffer, VO_U32 nBufferSize, VO_U32* pdwPos, VO_U32* pdwSize);
	
	virtual VO_U32		GetVideoResolution(VO_VIDEO_FORMAT* pInfo);
	virtual VO_U32		GetVideoProfileLevel(VO_VIDEO_PROFILELEVEL* pInfo);
	virtual VO_U32		IsVideoInterlace(VO_BOOL* bInterlace);

	virtual VO_BOOL		ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_BOOL		Hevc_ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_BOOL		ConvertData (VO_PBYTE pData, VO_U32 nSize);
	virtual VO_U32 	GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);

protected:
	VO_TRACKTYPE		m_nType;
	VO_U8				m_btStreamNum;
	VO_U32				m_dwDuration;			//<MS>

	VO_BOOL				m_bInUsed;
	VO_BOOL				m_bEndOfStream;
	VO_SOURCE_PLAYMODE	m_PlayMode;

	VO_PBYTE			m_pSampleData;

    VO_PBYTE            m_pThumbnailBuffer;
    VO_S32              m_ThumbnailBufferSize;
    VO_S32              m_ThumbnailBufferUsedSize;
    VO_S32              m_ThumbnallScanCount;

	VO_PBYTE			m_pSeqHeadData;
	VO_U32				m_nHeadSize;
	VO_U32				m_nNalLen;
	VO_U32				m_nNalWord;
	VO_PBYTE			m_pVideoData;
	VO_U32				m_nVideoSize;
	VO_U64				m_llLastTimeStamp;
	VO_CHAR				m_strLanguage[16];///<it should be ISo-639-2
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CBaseTrack_H__
