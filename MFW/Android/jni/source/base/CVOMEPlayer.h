	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEPlayer.h

	Contains:	CVOMEPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/

#ifndef __CVOMEPlayer_H__
#define __CVOMEPlayer_H__

#include "OMX_Component.h"
#include "voOMX_Index.h"
#include "voOMX_FilePipe.h"
#include "vomeAPI.h"

#include "voaEngine.h"

#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"

enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
};

#define MEDIAEVENT_DRM_BASE		0x8610

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
    // 0xx
    MEDIA_ERROR_UNKNOWN = 1,
    // 1xx
    MEDIA_ERROR_SERVER_DIED = 100,
    // 2xx
    MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
    // 3xx

	MEDIA_ERROR_DRM_EXPIRED	= (MEDIAEVENT_DRM_BASE | 0x1), 
	MEDIA_ERROR_DRM_NOAUTH = (MEDIAEVENT_DRM_BASE | 0x2), 
};


// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
// 
enum media_info_type {
    // 0xx
    MEDIA_INFO_UNKNOWN = 1,
    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
    MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,
    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
    MEDIA_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
    MEDIA_INFO_NOT_SEEKABLE = 801,

	MEDIA_INFO_DRM_PURCHASE = (MEDIAEVENT_DRM_BASE | 0x1),	/* this type is reserved */
	MEDIA_INFO_DRM_RENTAL = (MEDIAEVENT_DRM_BASE | 0x2),	/* 0xRRRRUUTT R: reserved, U: used view count, T: total view count*/
};

#define OMX_COLOR_Format_QcomHW		0X7FA30C00

class CVOMEPlayer
{
public:
	CVOMEPlayer (void);
	virtual ~CVOMEPlayer (void);

	virtual int			SetCallBack (VOACallBack pCallBack, void * pUserData);
	virtual int			SetFilePipe (void * pFilePipe);

	virtual int			Init (const char* path = NULL);
	virtual int			SetSource (const char* pSource);
	virtual int			SetSource (int fd, int64_t offset, int64_t length);
	virtual int			SetLoop (int nLoop);
	virtual int			PrepareAsync (void);

	virtual int			BuildGraph (void);
	virtual int			Run (void);
	virtual int			Pause (void);
	virtual int			Stop (void);
	virtual bool		IsPlaying (void);

	virtual int			SetPos (int	nPos);
	virtual int			GetPos (int * pPos);
	virtual int			GetDuration (int * pPos);

	virtual int			Reset (void);
	virtual int			Uninit (void);

	char*						getWorkingPath() { return m_szworkingpath; }
protected:
	virtual void		NotifyOpenSource (void);
	int							CallBackFunc (int nID, void * pParam1, void * pParam2);
	int							RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer);
	virtual int			LoadModule (void);

protected:
	int								m_nStatus;
	int								m_nLoop;
	bool							m_bUninit;

	int								m_nVideoWidth;
	int								m_nVideoHeight;
	VOA_AUDIO_FORMAT				m_fmtAudio;

	OMX_PTR	 						m_hModule;
	OMX_PTR							m_hPlay;
	VOOMX_ENGINEAPI					m_fAPI;

	char							m_szFile[1024];
	char							m_szworkingpath[128];
	int								m_nFile;
	FILE *							m_hFile;
	signed long long				m_nOffset;
	signed long long				m_nLength;
	OMX_VO_DISPLAYAREATYPE			m_dspArea;

	// 0 is local file, 1 is RTSP, 2. HTTP
	int								m_nSourceType;
	bool							m_bSeeking;
	int								m_nStartPos;
	int								m_nDuration;
	bool							m_bPrepareAsync;
	bool							m_bAutoStart;
	bool							m_bStreamSourceCount;
	bool							m_bSourceLoading;

	OMX_VO_FILE_OPERATOR *			m_pFilePipe;
	VOA_VIDEO_BUFFERTYPE			m_VideoBuff;

	bool							m_bNotifyVideoSize;
	bool							m_bSetVideoBuffer;

	VOACallBack						m_pCallBack;
	void *							m_pUserData;

	OMX_VO_SOURCEDRM_CALLBACK		m_drmCB;
	OMX_VO_LIB_OPERATOR				m_libOP;


public:
	static OMX_S32			vomeCallBack (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2);

	static OMX_ERRORTYPE	vomeVideoRenderBufferProc (OMX_HANDLETYPE hComponent,
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat);

	static OMX_ERRORTYPE	vomeAudioRenderBufferProc (OMX_HANDLETYPE hComponent,
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat);

	static OMX_PTR OMX_voLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag);
	static OMX_PTR OMX_voGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag);
	static OMX_S32 OMX_voFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag);
	static OMX_U32 OMX_voSourceDrmCallBack (OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);


public:
	static OMX_U32			vomeLoadSourceProc (OMX_PTR pParam);
	virtual OMX_U32			vomeLoadSource (void);
	virtual OMX_U32			vomeWaitLoadSource (int nWaitTime);

	voOMXThreadHandle		m_hLoadThread;
	OMX_U32					m_nLoadThreadID;

	voCOMXThreadMutex		m_mutLoad;
	voCOMXThreadMutex		m_mutPlayer;
	voCOMXThreadMutex		m_mutAVRender;

	static int				g_voAndroidStreamSource;
};

#endif // __CVOMEPlayer_H__
