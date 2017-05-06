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

#ifdef _LINUX_ANDROID
#include <sys/types.h>
#include <utils/Errors.h>
#include <media/Metadata.h>
#include "binder/Parcel.h"
#endif // _LINUX_ANDROID

#include "OMX_Component.h"
#include "voOMX_Index.h"
#include "voOMX_FilePipe.h"
#include "voOMX_MediaEvent.h"
#include "vomeAPI.h"
#include "voaEngine.h"
#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"
#include "CDllLoad.h"

#include "CDrmEngine.h"
#include "voPreHTTP.h"

#define OMX_COLOR_Format_QcomHW		0X7FA30C00
#define OMX_COLOR_Format_QcomHW2	0X7FA30C03	//for 8660
#define OMX_COLOR_Format_NvidiaHW	0x32315659	// HAL_PIXEL_FORMAT_YV12 YCrCb 4:2:0 Planar
#define OMX_COLOR_Format_STEHW      0x7FFFFFFE  // STE hardware decoder color format OMX_COLOR_FormatMax -1
#define OMX_COLOR_Format_TI_DUCATI1 0x100  // Galaxy Nexus TI's platform
#define OMX_COLOR_Format_SamSuangFK 0x7F000010 // Fake SamSung's hardware decoder

class CVOMEPlayer : public CDllLoad
{
public:
	CVOMEPlayer (void);
	virtual ~CVOMEPlayer (void);

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

	static OMX_U32			vomeLoadSourceProc (OMX_PTR pParam);

	static OMX_U32			vomeSourceDrmProc (OMX_PTR pUserData, 
											   OMX_U32 nFlag, 
											   OMX_PTR pParam, 
											   OMX_U32 nReserved);

public:
	virtual int			SetCallBack (VOACallBack pCallBack, void * pUserData);
	virtual int			SetFilePipe (void * pFilePipe);
	virtual int			SetSurface (void * pSurface);

	virtual int			Init (void);
	virtual int			SetSource (const char * pSource, const char * pHeaders);
	virtual int			SetSource (int fd, int64_t offset, int64_t length);
	virtual int			SetLoop (int nLoop);
	virtual int			Prepare (void);
	virtual int			PrepareAsync (void);

	virtual int			BuildGraph (void);
	virtual int			Run (bool bForceSeek = false);
	virtual int			Pause (void);
	virtual int			Stop (void);
	virtual bool		IsPlaying (void);

	virtual int			SetPosB (int nPos);
	virtual int			SetPos (int	nPos);
	virtual int			GetPos (int * pPos);
	virtual int			GetDuration (int * pPos);

	virtual int			Suspend (void);
	virtual int			Resume (void);

	virtual int			ChangeSurface (VOAUpdateSurfaceCallBack fCallBack, void * pUserData);

	virtual int			SendAudioCodec (OMX_AUDIO_CODINGTYPE nAudioCoding) {m_nAudioCoding = nAudioCoding; return 0;}
	virtual int			SendVideoCodec (OMX_VIDEO_CODINGTYPE nVideoCoding) {m_nVideoCoding = nVideoCoding; return 0;}

	virtual int			SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime);
	virtual int			SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime);

#ifdef _LINUX_ANDROID
	virtual int			Invoke (android::Parcel *request, android::Parcel *reply);
	virtual int			GetSeekAble ();
#endif // _LINUX_ANDROID

	virtual int			Reset (void);
	virtual int			Uninit (bool bSuspend = false);

	static OMX_U32		vomeMessageHandleProc(OMX_PTR pParam);

	//add by matao at 2011.4.13
	virtual int			SetDrmApi(void *pDrmApi) {mpDRMAPI = (VO_DRM_API *)pDrmApi ; return 0;}
protected:
	virtual void		NotifyOpenSource (void);
	int		 			CallBackFunc (int nID, void * pParam1, void * pParam2);
	int					RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer);
	int					RenderAudio (OMX_AUDIO_PARAM_PCMMODETYPE * pAudioFormat, OMX_BUFFERHEADERTYPE * pBuffer);
	int					Set3DVideoType(void* pParam);
	virtual int			LoadModule (void);

	virtual OMX_U32		MessageHandleProc (void);
	virtual OMX_U32		vomeLoadSource (void);
	
	virtual OMX_U32		vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);
	virtual void		vomeNotifyEvent(int nMsg, int nExt1 = 0, int nExt2 = 0);

	VO_U32				CheckHTTPType(VO_LIVESRC_FORMATTYPE *type);

protected:
	int								m_nStatus;
	int								m_nLoop;
	bool							m_bUninit;

	OMX_COMPONENTTYPE *				m_pDM3;

	int								m_nVideoWidth;
	int								m_nVideoHeight;
	VOA_AUDIO_FORMAT				m_fmtAudio;

	OMX_PTR							m_hPlay;
	VOOMX_ENGINEAPI					m_fAPI;
	VOOMX_COMPONENTAPI				m_fComp;

	char							m_szFile[1024];
	int								m_nFile;
	signed long long				m_nOffset;
	signed long long				m_nLength;
	OMX_VO_DISPLAYAREATYPE			m_dspArea;

	// 0 is local file, 1 is RTSP, 2. HTTP
	int								m_nSourceType;
	bool							m_bSeeking;
	bool							m_bSeekable;
	int								m_nStartPos;
	int								m_nDuration;
	bool							m_bPrepareAsync;
	bool							m_bAutoStart;
	bool							m_bStreamSourceCount;
	bool							m_bSourceLoading;

	OMX_VO_FILE_OPERATOR *			m_pFilePipe;
	VOA_VIDEO_BUFFERTYPE			m_VideoBuff;
	OMX_VO_SOURCEDRM_CALLBACK		m_drmCallback;

	bool							m_bNotifyVideoSize;

	VOACallBack						m_pCallBack;
	void *							m_pUserData;

	voOMXThreadHandle				m_hMsgThread;
	OMX_U32							m_nMsgThreadID;
	OMX_BOOL						m_bMsgThreadStop;

	voCOMXThreadMutex				m_mutMsg;
	int								m_nMsgType;	//0 - none; 1 - load; 2 - setpos;

	voCOMXThreadMutex				m_mutStatus;
	voCOMXThreadMutex				m_mutNotify;
	// East add this lock to avoid crash(m_fAPI.Uninit when BuildGraphing)
	voCOMXThreadMutex				m_mutBuildGraph;

	bool							m_bLoading;
	bool							m_bNotifyError;

	VO_DRM_API *					mpDRMAPI;
	CDrmEngine *					m_pDrmEngine;
	bool							m_bDrmEngineCommit;

	int								m_nDivXDRMMode;//0 - old normal mode; 1 - JIT;

	int								m_iRentalCountInfo;

	OMX_AUDIO_CODINGTYPE			m_nAudioCoding;
	OMX_VIDEO_CODINGTYPE			m_nVideoCoding;

	static int						g_voAndroidStreamSource;

	int								m_nSuspendStatus;

	//yang_yi add this to support cookie
	char *							m_pHeaders;
	bool							m_bAudioSinkToStart;
	bool							m_bBuffering;
	bool							m_bAfterEOSSetPos;
};

#endif // __CVOMEPlayer_H__
