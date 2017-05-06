	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoRender.h

	Contains:	CVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoRender_H__
#define __CVideoRender_H__

#include <voVideo.h>
#include <voVideoRender.h>

#include "CBaseNode.h"
#include "CBaseVideoRender.h"

class CVideoRender : public CBaseNode
{
public:
	CVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoRender (void);

	virtual VO_U32		Init (VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32		SetDispRect (VO_PTR hView, VO_RECT * pDispRect);
	virtual VO_U32		SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
	virtual VO_U32		Start (void);
	virtual VO_U32		Pause (void);
	virtual VO_U32		Stop (void);
	virtual VO_U32		Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32		WaitDone (void);
	virtual VO_U32		Redraw (void);
	virtual VO_U32		SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32		GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);

	virtual VO_U32		SetParam(VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		GetParam(VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	virtual void		SetLibOperator (VO_LIB_OPERATOR * pLibOP);

protected:
	VO_PTR				m_hView;

	VO_VIDEO_RENDERAPI	m_funRender;
	VO_HANDLE			m_hRender;

	VO_VIDEO_FORMAT		m_fmtVideo;
	VOVIDEOCALLBACKPROC m_pCallBack;
	VO_PTR				m_pUserData;

	CBaseVideoRender *	m_pRender;
};

#endif // __CVideoRender_H__

