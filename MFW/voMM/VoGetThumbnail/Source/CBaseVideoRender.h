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

#include "voMMPlay.h"

class CBaseVideoRender : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CBaseVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseVideoRender (void);

	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect);
	virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
	virtual VO_U32 	Start (void);
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	WaitDone (void);
	virtual VO_U32 	Redraw (void);
	virtual VO_U32 	SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32 	GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	virtual VO_U32 	SetVideoMemOP (VO_MEM_VIDEO_OPERATOR * pVideoMemOP);
	virtual VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);

	virtual void	SetWorkPath (VO_TCHAR * pWorkPath) {m_pWorkPath = pWorkPath;}

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
	CCCRRRFunc *		m_pCCRRR;
	CCCRRRFunc *		m_pSoftCCRRR;
	VO_BOOL				m_bSoftForce;

	voCMutex			m_csDraw;
	VO_U32				m_nVideoWidth;
	VO_U32				m_nVideoHeight;
	VO_U32				m_nYUVWidth;
	VO_U32				m_nYUVHeight;
	VO_IV_COLORTYPE		m_nVideoColor;

	VO_BOOL				m_bSetDispRect;
	VO_RECT				m_rcDisplay;
	VO_IV_ZOOM_MODE		m_nZoomMode;
	VO_IV_ASPECT_RATIO	m_nRatio;

	VOVIDEOCALLBACKPROC	m_fCallBack;
	VO_PTR				m_pUserData;
	VO_VIDEO_FORMAT		m_fmtVideo;
	VO_VIDEO_BUFFER		m_bufRGB565;

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

	VO_BOOL				m_bRotate;
	VO_BOOL				m_bFullScreen;
	VO_IV_RTTYPE		m_bRotateType;

	VO_VIDEO_BUFFER		m_inData;

	VOMMPlayCallBack 	m_fPlayCallBack;
	VO_PTR				m_pPlayUserData;

	VO_VIDEO_BUFFER		m_bufOutput;

	VO_PBYTE			m_pUVData;

	VO_MEM_VIDEO_OPERATOR *	m_pOutMemOP;
	VO_LIB_OPERATOR *		m_pLibOP;
	VO_TCHAR *				m_pWorkPath;
};

#endif // __CBaseVideoRender_H__
