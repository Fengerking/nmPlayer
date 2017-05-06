	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDumpPlayFile.h

	Contains:	CDumpPlayFile header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CDumpPlayFile_H__
#define __CDumpPlayFile_H__

#include "voFile.h"

#include "CPlayFile.h"

class CDumpPlayFile : public CPlayFile
{
public:
	// Used to control the image drawing
	CDumpPlayFile (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CDumpPlayFile (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32 		Run (void);
	virtual VO_U32 		Stop (void);

#ifdef _WIN32
	virtual VO_U32		PlaybackIdleLoop (void);
#endif // _WIN32

protected:
	virtual VO_U32		ReadAudioData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual  VO_U32		DecoderAudioData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		RenderAudioData (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart);

	virtual VO_U32		ReadVideoData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		SetInputVideoData (VO_CODECBUFFER * pInput);
	virtual VO_U32		DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_BOOL * pMoreOutput);
	virtual VO_U32		RenderVideoData (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

	virtual VO_U32		EndOfSource (void);
	virtual void		ReleaseBuffer (void);

protected:
	char			m_szFileName[256];

	VO_S32 *		m_pAudioReadThreadNum;
	VO_S32 *		m_pAudioReadSystemNum;
	VO_S32 *		m_pAudioReadSampleTime;
	VO_S32 *		m_pAudioReadSampleSize;

	VO_S32 *		m_pAudioDecThreadNum;
	VO_S32 *		m_pAudioDecSystemNum;

	VO_S32 *		m_pAudioRendThreadNum;
	VO_S32 *		m_pAudioRendSystemNum;
	VO_S32 *		m_pAudioRendSystemTime;
	VO_S32 *		m_pAudioRendSampleTime;
	VO_S32 *		m_pAudioRendPlayingTime;
	VO_S32 *		m_pAudioRendBufferTime;

	VO_S32 *		m_pVideoReadThreadNum;
	VO_S32 *		m_pVideoReadSystemNum;
	VO_S32 *		m_pVideoReadSampleTime;
	VO_S32 *		m_pVideoReadKeyFrame;

	VO_S32 *		m_pVideoDecThreadNum;
	VO_S32 *		m_pVideoDecSystemNum;

	VO_S32 *		m_pVideoRenThreadNum;
	VO_S32 *		m_pVideoRenSystemNum;
	VO_S32 *		m_pVideoRenSysTime;

	VO_S32 *		m_pVideoPlayTime;
	VO_S32 *		m_pVideoSampleTime;
	VO_S32 *		m_pVideoSystemTime;
	VO_S32 *		m_pVideoKeyFrame;

	VO_U32			m_nMaxAudioIndex;
	VO_U32 			m_nAudioReadIndex;
	VO_U32 			m_nAudioDecIndex;
	VO_U32 			m_nAudioRenIndex;

	VO_U32			m_nMaxVideoIndex;
	VO_U32 			m_nVideoReadIndex;
	VO_U32 			m_nVideoDecIndex;
	VO_U32 			m_nVideoRenIndex;

	VO_U32			m_nVideoThreadStart;
	VO_U32			m_nVideoThreadTime;
	VO_U32			m_nAudioThreadStart;
	VO_U32			m_nAudioThreadTime;

	VO_U32			m_nIdleThreadStart;
	VO_U32			m_nIdleThreadTime;

	VO_U32			m_nPlaybackStartTime;
	VO_U32			m_nPlaybackEndTime;
};

#endif // __CDumpPlayFile_H__
