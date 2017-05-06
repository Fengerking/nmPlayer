	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMETranscode.h

	Contains:	CVOMETranscode header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CVOMETranscode_H__
#define __CVOMETranscode_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#include <utils/String16.h>
#include <android_runtime/ActivityManager.h>
#endif

#ifdef _LINUX_ANDROID
#include "binder/Parcel.h"
#endif // _LINUX_ANDROID

#include "vomeAPI.h"
#include "voString.h"

#include "CVOMEEngine.h"
#include "CVideoResize.h"
#include "CAudioResample.h"

#include "CVOMERecorder.h"

class COMXTranscodeItem
{
public:
	// Used to control the image drawing
	COMXTranscodeItem (void)
	{
		vostrcpy (m_szVideoFile, _T(""));
		m_nFD = 0;
		m_nBeginPos = 0;
		m_nEndPos = 0;
		m_pNext = NULL;
	}
	virtual ~COMXTranscodeItem (void)
	{
#ifdef _LINUX_ANDROID
		if (m_nFD != 0)
			close (m_nFD);
		m_nFD = 0;
#endif //_LINUX_ANDROID
	};

	VO_TCHAR			m_szVideoFile[256];
	OMX_S32				m_nFD;
	OMX_S32				m_nBeginPos;
	OMX_S32				m_nEndPos;

	COMXTranscodeItem *	m_pNext;
};

class CVOMETranscode : public CVOMERecorder
{
public:
	// Used to control the image drawing
	CVOMETranscode (void);
	virtual ~CVOMETranscode (void);

	OMX_ERRORTYPE	AddVideoSource (VO_TCHAR * pVideoFile, OMX_S32 nFD, OMX_S32 nBeginPos, OMX_S32 nEndPos);

	OMX_ERRORTYPE	Transcode (void);

	virtual int 	SetDataSource (const char *source, int offset);

	virtual int		Prepare (void);
	virtual int		Start (void);
	virtual int		Pause (void);
	virtual int		Stop (void);
	virtual int		Close (void);

	virtual int		SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime);
	virtual int		SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime);

protected:
	virtual int		CreateOutputComponents (void);
	OMX_ERRORTYPE	TryToTrim (void);

	OMX_ERRORTYPE	TransVideoBuffer (OMX_BUFFERHEADERTYPE * pHeadBuffer, OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer);
	OMX_ERRORTYPE	TransAudioBuffer (OMX_BUFFERHEADERTYPE * pAudioBuffer, OMX_AUDIO_PARAM_PCMMODETYPE * pAudioFormat);

protected:
	CVOMEEngine					m_vomeSource;
	FILE *						m_hInputFile;

	VOME_VIDEOSIZETYPE			m_videoSize;
	VOME_AUDIOFORMATTYPE		m_audioFormat;
	OMX_S32						m_nDuration;

	CVideoResize *				m_pVideoResize;
	ClrConvData					m_resizeData;
	OMX_U8 *					m_pVideoBuffer;
	OMX_S32						m_nVideoBufSize;
	OMX_S64						m_llVideoTime;
	OMX_S32						m_nResizeWidth;
	OMX_S32						m_nResizeHeight;
	OMX_S64						m_llFirstTime;
	OMX_S32						m_nEncFrames;

	CAudioResample *			m_pAudioResample;
	VO_CODECBUFFER				m_audioInBuff;
	VO_CODECBUFFER				m_audioOutBuff;
	OMX_U8 *					m_pAudioBuffer;
	OMX_S32						m_nAudioBufSize;
	OMX_S64						m_llAudioTime;


	COMXTranscodeItem *			m_pFirstItem;
	COMXTranscodeItem *			m_pCurrentItem;
	OMX_BOOL					m_bTranscoding;
	OMX_BOOL					m_bStopTrans;

protected:
	static OMX_ERRORTYPE		vomeSourceVideoBuffer (OMX_HANDLETYPE hComponent, 
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat);

	static OMX_ERRORTYPE		vomeSourceAudioBuffer (OMX_HANDLETYPE hComponent, 
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat);

	static OMX_S32				vomeSourceCallBack	   (OMX_PTR pUserData, 
														OMX_S32 nID, 
														OMX_PTR pParam1, 
														OMX_PTR pParam2);
};

#endif // __CVOMETranscode_H__
