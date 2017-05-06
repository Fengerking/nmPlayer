#ifndef __OnStreamCVideoRender_h__
#define __OnStreamCVideoRender_h__

#include "voThread.h"
#include "voOSFunc.h"
#include "CBaseVideoRender.h"
#include "COSWinVomePlayer.h"

class COSWinVideoRender
{
public:
	COSWinVideoRender(COSWinVomePlayer * pEngine, Windows::UI::Core::CoreWindow^ windows);
	virtual	~COSWinVideoRender(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);

	virtual int			SetPos (int nPos);
	virtual int			SetRect (VOMP_RECT *pRect);

	static	int			VideoRenderThreadProc (void * pParam);	
	virtual	int			VideoRenderThreadLoop (void);

protected:
	int					m_nStatus;
	
	CBaseVideoRender*	m_pVideoRender;
	Windows::UI::Core::CoreWindow^				m_Windows;

	voThreadHandle		m_pRenderThread;

	COSWinVomePlayer *	m_pEngine;

	VOOSMP_VIDEO_FORMAT	m_sVideoFormat;
	int					m_nColorType;
	VOMP_RECT			m_rcDraw;
	bool				m_bSeeking;

	VOMP_BUFFERTYPE		m_VideoBuffer;

	int					m_nRenderNum;
};

#endif //#define __OnStreamCVideoRender_h__

