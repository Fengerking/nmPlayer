	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseVideoRender.h

	Contains:	CBaseVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBaseVideoRender_H__
#define __CBaseVideoRender_H__

#include "voVideoRender.h"
#include "CCCRRRFunc.h"
#include "voCMutex.h"
#include "CBaseConfig.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CBaseVideoRender : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CBaseVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseVideoRender (void);

public:
	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_RGB565_PACKED);
	virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
	virtual VO_U32 	Start (void);
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	WaitDone (void);
	virtual VO_U32 	Redraw (void);
	virtual VO_U32 	SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
    virtual VO_U32 	SetEventCallBack (VOVIDEO_EVENT_CALLBACKPROC pCallBack, VO_PTR pUserData);
    
	virtual VO_U32 	GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	virtual VO_U32 	SetVideoMemOP (VO_MEM_VIDEO_OPERATOR * pVideoMemOP);
	virtual VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32	RenderRGBBuffer (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pRGBBuffer);

	virtual void	SetWorkPath (VO_TCHAR * pWorkPath) {m_pWorkPath = pWorkPath;}
	virtual void	SetConfigFile (CBaseConfig * pCfgFile) {m_pConfig = pCfgFile;}
	virtual void	SetCPUVersion (int nVer);

	virtual VO_U32  ShowOverlay(VO_BOOL bShow) {return 0;}
	virtual VO_BOOL	CanRedraw() {return m_bCanRedraw;}
	virtual void	SetBlockEraseBackGround(VO_BOOL bBlockEraseBackGround) {m_bBlockEraseBackGround = bBlockEraseBackGround;}

protected:
	virtual VO_U32	UpdateSize (void);
	virtual bool	ConvertData (VO_VIDEO_BUFFER * pInBuffer, VO_VIDEO_BUFFER * pOutBuffer, VO_S64 nStart, VO_BOOL bWait);

	virtual bool	CreateCCRRR (void);

protected:
#if defined(LINUX)
	bool  getScreenSize(VO_U32& inOutWidth, VO_U32& inOutHeight);
#endif

protected:
	VO_PTR				m_hInst;
	VO_MEM_OPERATOR *	m_pMemOP;
	VO_PTR				m_hView;

	CBaseConfig *		m_pConfig;
	CBaseConfig *		m_pCfgFile;
	CCCRRRFunc *		m_pCCRRR;
	CCCRRRFunc *		m_pSoftCCRRR;
	VO_BOOL				m_bSoftForce;

	voCMutex			m_csDraw;
	VO_U32				m_nVideoWidth;
	VO_U32				m_nVideoHeight;
	VO_U32				m_nYUVWidth;
	VO_U32				m_nYUVHeight;
	VO_IV_COLORTYPE		m_nVideoColor;
	VO_S32				m_nBytesPixel;

	VO_BOOL				m_bSetDispRect;
	VO_RECT				m_rcDisplay;
	VO_IV_ZOOM_MODE		m_nZoomMode;
	VO_IV_ASPECT_RATIO	m_nRatio;
	VO_IV_COLORTYPE		m_nOutputColor;

	VOVIDEOCALLBACKPROC	m_fCallBack;
	VO_PTR				m_pUserData;
    
    VOVIDEO_EVENT_CALLBACKPROC m_fEventCallBack;
    VO_PTR              m_pEventUserData;
    
	VO_VIDEO_FORMAT		m_fmtVideo;
	VO_VIDEO_BUFFER		m_bufRGB565;
	VO_VIDEO_BUFFER		m_bufRGBRevert;

	VO_U32				m_nShowLeft;
	VO_U32				m_nShowTop;
	VO_U32				m_nShowWidth;
	VO_U32				m_nShowHeight;

	VO_U32				m_nDrawLeft;
	VO_U32				m_nDrawTop;
	VO_U32				m_nDrawWidth;
	VO_U32				m_nDrawHeight;

	VO_U32				m_nScreenWidth;
	VO_U32				m_nScreenHeight;

	VO_BOOL				m_bVideoUpsizeDown;
	VO_BOOL				m_bRotate;
	VO_BOOL				m_bFullScreen;
	VO_IV_RTTYPE		m_bRotateType;

	VO_VIDEO_BUFFER		m_inData;

	VO_VIDEO_BUFFER		m_bufOutput;

	VO_PBYTE			m_pUVData;

	VO_MEM_VIDEO_OPERATOR *	m_pOutMemOP;
	VO_LIB_OPERATOR *		m_pLibOP;
	VO_TCHAR *				m_pWorkPath;
	int						m_nCPUVer;
	voCMutex				m_mutexCCRR;

	VO_BOOL					m_bCanRedraw;
	VO_BOOL					m_bBlockEraseBackGround;

#ifdef _DRAW_RENDER_INFO
	VO_U32				m_nTimeoutCount;
	VO_U32				m_nLastRenderSysTime;
#endif	// _DRAW_RENDER_INFO
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CBaseVideoRender_H__
