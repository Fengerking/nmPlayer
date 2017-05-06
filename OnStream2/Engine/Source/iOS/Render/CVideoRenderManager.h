/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CIOSVideoRender.h

	Contains:	CIOSVideoRender header file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/

#ifndef __CVIDEORENDER_MANAGER__
#define __CVIDEORENDER_MANAGER__

#include "CBaseVideoRender.h"
#include "voBaseRender.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CVideoRenderManager : public voBaseRender
{
public:
	CVideoRenderManager(void* pUserData, RenderQueryCallback pQuery);
	virtual	~CVideoRenderManager(void);

	virtual int			SetSurface (void* hWnd);

	virtual int			Start (void);
	virtual int         Stop (void);
	
	virtual int			SetPos (int nPos);

	virtual int			SetColorType (VOMP_COLORTYPE nColor);
	virtual int			SetDrawRect(int nLeft, int nTop, int nRight, int nBottom);
	virtual int         SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
    
	virtual	int			RenderThreadLoop (void);

    virtual int         GetDrawRect(VO_RECT *pValue);
    
    virtual VO_U32      SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32      GetParam (VO_U32 nID, VO_PTR pValue);
    
protected:
    
    virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);
	static	VO_S32      OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
    
	virtual void		VideoSizeChanged();
	virtual void		DrawBlack( VOMP_BUFFERTYPE * m_pVideoBuffer );
	virtual void        UpdateRect();
	
protected:
	voCMutex				m_mtStatus;

	void*					m_pSurface;
	VOMP_VIDEO_FORMAT		m_sVideoFormat;
	int						m_nColorType;
	VOMP_RECT				m_rcDraw;
	int						m_nBytesPixel;
	int						m_nStride;
    int                     m_nZoomMode;
    int                     m_nAspectRatio;
    
	bool					m_bSetPos;

	bool					m_bSurfaceNeedChange;
	CBaseVideoRender*		m_pVideoRender;
};

#endif //__CVIDEORENDER_MANAGER__

