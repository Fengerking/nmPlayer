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

class Parcel;
template<typename T> class SortedVector;

#define OMX_COLOR_Format_QcomHW		0X7FA30C00

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

	virtual int			Init (void);
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

	virtual int			Invoke (const Parcel& request, Parcel *reply);
#ifdef _LINUX_ANDROID
	virtual int			GetMetadata (const android::media::Metadata::Filter& ids, Parcel *records);
#endif // _LINUX_ANDROID

	virtual int			Reset (void);
	virtual int			Uninit (void);

protected:
	virtual void		NotifyOpenSource (void);
	int		 			CallBackFunc (int nID, void * pParam1, void * pParam2);
	int					RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer);
	virtual int			LoadModule (void);

	virtual OMX_U32		vomeLoadSource (void);
	virtual OMX_U32		vomeWaitLoadSource (int nWaitTime);

	virtual OMX_U32		vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);

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
	bool							m_bAudioPaused4Seek;

	OMX_VO_FILE_OPERATOR *			m_pFilePipe;
	VOA_VIDEO_BUFFERTYPE			m_VideoBuff;

	bool							m_bNotifyVideoSize;
	bool							m_bSetVideoBuffer;

	VOACallBack						m_pCallBack;
	void *							m_pUserData;

	voOMXThreadHandle				m_hLoadThread;
	OMX_U32							m_nLoadThreadID;

	voCOMXThreadMutex				m_mutLoad;
	voCOMXThreadMutex				m_mutPlayer;
	voCOMXThreadMutex				m_mutAVRender;

	CDrmEngine *					m_pDrmEngine;

	static int						g_voAndroidStreamSource;
};

#endif // __CVOMEPlayer_H__
