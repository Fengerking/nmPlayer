#ifndef __OnStreamCVideoRender_h__
#define __OnStreamCVideoRender_h__

#include "voThread.h"
#include "voOSFunc.h"
#include "CBaseVideoRender.h"
#include "COSWinVomePlayer.h"
#define TRY_TO_HOLD_VIDEO_BUF 0

class COSWinVideoRender
{
public:
	COSWinVideoRender(COSWinVomePlayer * pEngine, HWND hWnd);
	virtual	~COSWinVideoRender();

public:
	virtual int			Start();
	virtual int			Pause();
	virtual int			Stop();

	virtual int			SetPos(int nPos);
	virtual int			SetRect(VOMP_RECT * pRect);
	virtual int			SetUpsideDown(VO_BOOL bValue);
	virtual int			SetWorkPath(VO_TCHAR * pWorkpath);
	virtual void		ShowOverlay(VO_BOOL bValue);
	virtual int			SetView(void * pView);
	virtual int			SetAspectRatio(VOOSMP_ASPECT_RATIO * pAspect);

	static	int			VideoRenderThreadProc(void * pParam);	
	virtual	int			VideoRenderThreadLoop(int decoder_or_render);
    virtual int         SetParam(int nID, void * pParam);
	
    virtual int         Redraw();

	static VO_S32		OnDDrawRenderCallback(VO_PTR pUserData, HDC hDC, LPRECT pRect, VO_S64 llVideoTime);
	virtual BOOL		InitVideoRenderer();

protected:
	virtual	void		UninitVideoRenderer();
	void				internalReturnVideoBuffer(const int rendered = 0);

protected:
	int					m_nStatus;

	CBaseVideoRender *	m_pVideoRender;
	HWND				m_hWnd;

	voThreadHandle		m_pRenderThread;

	voCMutex			m_RenderLock;
    voCMutex			m_RenderDrawLock;

	VO_TCHAR *			m_pWorkpath;

	COSWinVomePlayer *	m_pEngine;

	VO_IV_ASPECT_RATIO	m_sAspect;
	VOOSMP_VIDEO_FORMAT	m_sVideoFormat;
	int					m_nColorType;
	VOMP_RECT			m_rcDraw;
	bool				m_bSeeking;

	VOMP_BUFFERTYPE		m_VideoBuffer;

	int					m_nRenderNum;
	VO_BOOL				mbUpsideDown;

	VOOSMP_RENDER_TYPE	m_nVideoRenderType;
	VO_BOOL				m_bVideoRenderCallback;
	VOMP_BUFFERTYPE *	m_pVideoBuffer;
	bool				m_bWindowlessMode;
	bool				m_bDDrawFailed;
	HDC					m_hWDC;
};

#endif //#define __OnStreamCVideoRender_h__

