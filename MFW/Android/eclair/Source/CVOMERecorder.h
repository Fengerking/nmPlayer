	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMERecorder.h

	Contains:	CVOMERecorder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#ifndef __CVOMERecorder_H__
#define __CVOMERecorder_H__

#ifdef _LINUX_ANDROID
#include <sys/types.h>
#include <utils/Errors.h>
#endif // _LINUX_ANDROID

#include "OMX_Component.h"
#include "voOMXBase.h"
#include "voOMX_Index.h"
#include "vomeAPI.h"
#include "voaEngine.h"
#include "CVOMEEngine.h"

#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"
#include "CDllLoad.h"

enum audio_source {
    AUDIO_SOURCE_DEFAULT = 0,
    AUDIO_SOURCE_MIC = 1,
    AUDIO_SOURCE_VOICE_UPLINK = 2,
    AUDIO_SOURCE_VOICE_DOWNLINK = 3,
    AUDIO_SOURCE_VOICE_CALL = 4,
    AUDIO_SOURCE_CAMCORDER = 5,
    AUDIO_SOURCE_VOICE_RECOGNITION = 6,
    AUDIO_SOURCE_MAX = AUDIO_SOURCE_VOICE_RECOGNITION,

    AUDIO_SOURCE_LIST_END  // must be last - used to validate audio source type
};

enum video_source {
    VIDEO_SOURCE_DEFAULT = 0,
    VIDEO_SOURCE_CAMERA = 1,

    VIDEO_SOURCE_LIST_END  // must be last - used to validate audio source type
};

//Please update media/java/android/media/MediaRecorder.java if the following is updated.
enum output_format {
    OUTPUT_FORMAT_DEFAULT = 0,
    OUTPUT_FORMAT_THREE_GPP = 1,
    OUTPUT_FORMAT_MPEG_4 = 2,


    OUTPUT_FORMAT_AUDIO_ONLY_START = 3, // Used in validating the output format.  Should be the
                                        //  at the start of the audio only output formats.

    /* These are audio only file formats */
    OUTPUT_FORMAT_RAW_AMR = 3, //to be backward compatible
    OUTPUT_FORMAT_AMR_NB = 3,
    OUTPUT_FORMAT_AMR_WB = 4,
    OUTPUT_FORMAT_AAC_ADIF = 5,
    OUTPUT_FORMAT_AAC_ADTS = 6,

    OUTPUT_FORMAT_LIST_END // must be last - used to validate format type
};

enum audio_encoder {
    AUDIO_ENCODER_DEFAULT = 0,
    AUDIO_ENCODER_AMR_NB = 1,
    AUDIO_ENCODER_AMR_WB = 2,
    AUDIO_ENCODER_AAC = 3,
    AUDIO_ENCODER_AAC_PLUS = 4,
    AUDIO_ENCODER_EAAC_PLUS = 5,

    AUDIO_ENCODER_LIST_END // must be the last - used to validate the audio encoder type
};

enum video_encoder {
    VIDEO_ENCODER_DEFAULT = 0,
    VIDEO_ENCODER_H263 = 1,
    VIDEO_ENCODER_H264 = 2,
    VIDEO_ENCODER_MPEG_4_SP = 3,

    VIDEO_ENCODER_LIST_END // must be the last - used to validate the video encoder type
};

/*
 * The state machine of the media_recorder uses a set of different state names.
 * The mapping between the media_recorder and the pvauthorengine is shown below:
 *
 *    mediarecorder                        pvauthorengine
 * ----------------------------------------------------------------
 *    MEDIA_RECORDER_ERROR                 ERROR
 *    MEDIA_RECORDER_IDLE                  IDLE
 *    MEDIA_RECORDER_INITIALIZED           OPENED
 *    MEDIA_RECORDER_DATASOURCE_CONFIGURED
 *    MEDIA_RECORDER_PREPARED              INITIALIZED
 *    MEDIA_RECORDER_RECORDING             RECORDING
 */
enum media_recorder_states {
    MEDIA_RECORDER_ERROR                 =      0,
    MEDIA_RECORDER_IDLE                  = 1 << 0,
    MEDIA_RECORDER_INITIALIZED           = 1 << 1,
    MEDIA_RECORDER_DATASOURCE_CONFIGURED = 1 << 2,
    MEDIA_RECORDER_PREPARED              = 1 << 3,
    MEDIA_RECORDER_RECORDING             = 1 << 4,
};

// The "msg" code passed to the listener in notify.
enum media_recorder_event_type {
    MEDIA_RECORDER_EVENT_ERROR                    = 1,
    MEDIA_RECORDER_EVENT_INFO                     = 2
};

enum media_recorder_error_type {
    MEDIA_RECORDER_ERROR_UNKNOWN                  = 1
};

// The codes are distributed as follow:
//   0xx: Reserved
//   8xx: General info/warning
//
enum media_recorder_info_type {
    MEDIA_RECORDER_INFO_UNKNOWN                   = 1,
    MEDIA_RECORDER_INFO_MAX_DURATION_REACHED      = 800,
    MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED      = 801
};


class CVOMERecorder : public CDllLoad
{
public:
	CVOMERecorder (void);
	virtual ~CVOMERecorder (void);

	virtual int 	getMaxAmplitude  (int *max);
	virtual int 	setAudioSource   (audio_source  as);
	virtual int 	setVideoSource   (video_source  vs);
	virtual int 	setOutputFormat  (output_format of);
	virtual int 	setAudioEncoder  (audio_encoder ae);
	virtual int 	setVideoEncoder  (video_encoder ve);
	virtual int 	setVideoFrameRate(int fps);
	virtual int		setAudioFormat	 (int nSampleRate, int nChannels);
	virtual int 	setColorType     (VOA_COLORTYPE nColorType);
	virtual int 	setVideoSize     (int w, int h);
	virtual int 	SetDataSource    (const char *source, int offset);
	virtual int 	setOutputFile    (const char *path);
	virtual int 	setOutputFile    (int fd, int64_t offset, int64_t length);
	virtual int 	setParameters    (const char * pParam);

	virtual int		Prepare (void);
	virtual int		Start (void);
	virtual int		Pause (void);
	virtual int		Stop (void);
	virtual int		Close (void);

	virtual int		SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime);
	virtual int		SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime);

	virtual int		SetCallBack (VOACallBack pCallBack, void * pUserData);
	virtual int		SetFilePipe (void * pFilePipe);

protected:
	virtual int		CreateOutputComponents (void);

protected:
	static int       vomeOutputCallBack (void* pUserData, int nID, void* pParam1, void* pParam2);

protected:
	voCOMXThreadMutex		m_mutAudio;
	audio_source			m_nAudioSrc;
	video_source			m_nVideoSrc;
	output_format			m_nOutFormat;
	audio_encoder			m_nAudioEnc;
	video_encoder			m_nVideoEnc;

	OMX_AUDIO_CODINGTYPE	m_omxAudioEnc;
	OMX_VIDEO_CODINGTYPE	m_omxVideoEnc;


	VOMESTATUS				m_nStatus;
	bool					m_bTranscode;

	int						m_nSampleRate;
	int						m_nChannels;

	OMX_COLOR_FORMATTYPE	m_nColorType;
	int						m_nFrameRate;
	int						m_nVideoWidth;
	int						m_nVideoHeight;

	char *					m_pOutputFile;
	int						m_nFD;
	int						m_nOffset;
	int						m_nLength;
	FILE *					m_hFile;
	int64_t					m_nMaxFileSize;
	int64_t					m_nMaxDuration;
	
	int						m_nSysStartTime;
	int						m_nImgStartTime;

	VOACallBack				m_pCallBack;
	void *					m_pUserData;

	bool					m_bTrackMaxAmplitude;
	int						m_nMaxAmplitude;

protected:
	CVOMEEngine					m_vomeOutput;

	OMX_COMPONENTTYPE *    		m_pAudioCap;
	OMX_COMPONENTTYPE *   		m_pVideoCap;
	OMX_COMPONENTTYPE *   		m_pVideoEnc;
	VOOMX_ENGINEAPI       		m_vomeAPI;
	OMX_BUFFERHEADERTYPE  		m_bufVideo;
	OMX_BUFFERHEADERTYPE  		m_bufAudio;


};

#endif // __CVOMERecorder_H__
