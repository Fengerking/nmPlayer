	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCPlayer.h

	Contains:	vompCPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __vompCPlayer_H__
#define __vompCPlayer_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif //_WIN32

#include "voThread.h"
#include "vompCEngine.h"

#include "voCMutex.h"
#include "vome2CBaseRender.h"

#include "vompDrm.h"
#include "vompCFile.h"
#include "vompCSourceBuffer.h"

#include "CBaseLiveSource.h"

#define	WM_TIMER_STATUS		201

class vompCPlayer : public vompCEngine
{
public:
	// Used to control the image drawing
	vompCPlayer (VO_PLAYER_OPEN_PARAM* pParam);
	virtual ~vompCPlayer (void);

	virtual int         init();
	
	virtual int 		SetDataSource (void * pSource, int nFlag, int nMode);
	
	virtual int 		SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		SetCurPos (int nCurPos);
    virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);
    
	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);

	int					SetDrawRect(VOMP_RECT* pRect);
	static  int			vompThreadCreate (void ** pHandle, int * pID, void * fProc, void * pParam);

private:
	int					SendEvent (int nID, void * pParam1, void * pParam2);
	int					GetAudioBufferTime (void);
	int					GetVideoBufferTime (void);

protected:
	int CreateAudioRender (void);
	int CreateVideoRender (void);

protected:
	bool					m_bOpenSync;
	int						m_nPlayMode;

	CBaseLiveSource *		m_pLiveSource;

	vome2CBaseRender *		m_pAudioRender;
	vome2CBaseRender *		m_pVideoRender;

	VOMP_AUDIO_FORMAT		m_sAudioFormat;
	VOMP_VIDEO_FORMAT		m_sVideoFormat;

	int						m_nColorType;
	VOMP_RECT				m_rcDraw;
	long long				m_llLastVideoMediaTime;
	int						m_nLastVideoSystemTime;

	bool					m_bSeeking;
	bool					m_bRenderOut;

	vompCBaseFileOP			m_opFile;
	VOMP_DATASOURCE_FUNC	m_sDataSource;
	VOMP_LIB_FUNC			m_sLibFunc;
	VOMP_SOURCEDRM_CALLBACK	m_sDrmCB;

	VOMP_READBUFFER_FUNC	m_sReadBuffer;
	vompCSourceBuffer		m_sSourceBuffer;
	
	bool					m_bIsSourceModule;
	bool					m_bUseLiveSourceModule;

	VO_CHAR					m_szDataSource[1024];
	
public:
	bool				m_bStopPlay;
	voCMutex 			m_mutexSendBuf;

	TCHAR				m_szStatus[256];
	
	VO_PLAYER_OPEN_PARAM	m_openParam;

public:
	static void *		mmLoadLib (void * pUserData, char * pLibName, int nFlag);
	static void *		mmGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			mmFreeLib (void * pUserData, void * hLib, int nFlag);
	static int			vompSourceDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved);


	static VO_VOID		NotifyStatus (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static VO_VOID		SendData  (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
};

#endif // __vompCPlayer_H__
