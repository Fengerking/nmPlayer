/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file COSNdkVomePlayer.h
 * wrap class of player for jni.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#ifndef __COSNdkVomePlayer_H__
#define __COSNdkVomePlayer_H__

#include "COSVomePlayer.h"
#include "COSNdkVideoRender.h"
#include "voCMutex.h"

class COSNdkVomePlayer : public COSVomePlayer
{
public:
	COSNdkVomePlayer ();
	virtual ~COSNdkVomePlayer (void);

	virtual int			Init(void);
	virtual int			Uninit(void);

	virtual int			SetDataSource (void * pSource, int nFlag);
	virtual int			Run (void);
	virtual int			Pause (void); 
	virtual int			Stop (void);
	virtual int			Flush (void);
	virtual int			Close();

	virtual int 		SetParam (int nID, void * pValue);
	virtual int 		GetParam (int nID, void * pValue);

	virtual int			SetPos (int nCurPos);

	virtual int			SetView(void* pView);

	virtual	int			SetJavaVM(void *pJavaVM, void* obj);
	virtual void*		GetJavaObj(){return (void*)m_jOSmpEng;	};

	virtual int			GetVideoData (unsigned char * pData, int nSize, JNIEnv *pEnv);
	virtual int			GetAudioData (unsigned char * pData, unsigned int nSize, int * pFilled);

	virtual	int			HandleEvent (int nID, void * pParam1, void * pParam2);

	static void*		vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			vomtFreeLib (void * pUserData, void * hLib, int nFlag);
	static void*		vomtLoadLib (void * pUserData, char * pLibName, int nFlag);

protected:

	virtual int				InitVideoRender();

	virtual int				ShowBlackFrame();
	void					internalReturnVideoBuffer(const int rendered = 0);

protected:
	VOOSMP_LIB_FUNC			m_sLibFunc;

	long					m_lRenderType;
	COSNdkVideoRender *		m_pVideoRender;
	jobject					m_pSurface;
	jobject					m_pBitmap;
	VOOSMP_ZOOM_MODE		m_nZoomMode;
	unsigned int			m_nColorType;

	VOMP_BUFFERTYPE	*		m_pBufVideo;
	VOMP_BUFFERTYPE	*		m_pBufAudio;

	VOMP_BUFFERTYPE			m_sBufVideo;
	VOMP_BUFFERTYPE			m_sBufAudio;

	voCMutex				m_mtGetAudio;
	voCMutex				m_mtGetVideo;

	VOMP_VIDEO_FORMAT		m_fmtVideo;

	int						m_nVideoRndNum;
	int						m_nStartSysTime;
	int						m_nVideoBufferSize;

	VOOSMP_AUDIO_RENDER_FORMAT	audioRenderFormat;

	// only for debug usage
	int                     mADumpFd;   // for dump the audio pcm data
	VO_BOOL					m_bShowBlackScreen;
};

#endif // __COSNdkVomePlayer_H__

