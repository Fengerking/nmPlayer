/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CTrimCtrl.h

Contains:	CTrimCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef __CTrimCtrl_H__
#define __CTrimCtrl_H__

#include "CFileSource.h"
#include "CFileFormatCheck.h"
#include "videoEditorType.h"
#include "voedtType.h"

class CTrimCtrl
{
public:
	CTrimCtrl(void* pUserData,VOEDTENCODERCALLBACKPROC proc);
	~CTrimCtrl();
	VO_S32 OpenFile(VO_PTR pSource);
	VO_S32 GetSample(VO_U32 nSampleType, VO_U32 *nTime);
	VO_S32 SeekPos(VO_U32 *pos);
	VO_S32 CloseFile();

	VO_FILE_FORMAT		m_VO_FileFormat;
	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_VIDEO_FORMAT		m_fmtVideo;
	VO_VIDEO_CODINGTYPE m_nVideoCodecType;
	VO_AUDIO_CODINGTYPE m_nAudioCodecType;

	VO_S32 m_nVideoFrameTime;
private:
	VOEDTENCODERCALLBACKPROC m_proc;
	VO_SOURCE_SAMPLE	m_videoSourceSample;
	VO_SOURCE_SAMPLE	m_audioSourceSample;

	VO_VOID*      m_pUserData;
	CFileSource *m_pFileSource;
	VO_SOURCE_INFO	m_FileInfo;
	VO_BOOL		m_bStopPlay;

	VO_S32 GetAudioSample(VO_S32 *nTime);
	VO_S32 GetVideoSample(VO_S32 *nTime);

	int m_nAudioTrack;
	int m_nVideoTrack;
	VO_S32 m_nStartPosition;
	VO_S32 m_nCurPosition;
	VO_SOURCE_TRACKINFO m_VideoTracktype;
	VO_SOURCE_TRACKINFO m_AudioTracktype;


	VO_S32 av_find_stream_info(VO_SOURCE_INFO &fileInfo);
	VO_S32 av_Open_Input_File(VO_PTR pSource);
	VO_S32 av_find_track_info(VO_SOURCE_INFO *fileInfo);
};
#endif