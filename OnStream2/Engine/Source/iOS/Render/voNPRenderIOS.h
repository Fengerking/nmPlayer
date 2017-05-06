
#ifndef __VO_NP_RENDER_IOS_
#define __VO_NP_RENDER_IOS_

#include "voType.h"
#include "voOnStreamType.h"
#include "voIVCommon.h"
#include "voIndex.h"
#include "voRunRequestOnMain.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voNPRenderIOS :private voRunRequestOnMain
{
public:
	voNPRenderIOS();
	virtual ~voNPRenderIOS();
    
	virtual VO_U32 	SetView(VO_PTR hView);
    virtual VO_U32  SetSecondView(VO_PTR hView);
    
	virtual VO_U32 	SetVideoInfo(VO_U32 nWidth, VO_U32 nHeight);
	virtual VO_U32 	SetDispRect(VOOSMP_RECT * pDispRect);
	virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
    
	virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
    
    virtual VO_U32 	Start (void);
	virtual VO_U32 	Stop (void);
protected:
	virtual VO_U32	UpdateSize (void);
    
    virtual void RunningRequestOnMain(int nID, void *pParam1, void *pParam2);
    
protected:
	VO_PTR				m_hView;
    VO_PTR				m_hSecondView;
    
	VO_U32				m_nVideoWidth;
	VO_U32				m_nVideoHeight;
	
    VO_BOOL				m_bSetDispRect;
	VOOSMP_RECT			m_rcDisplay;
	VO_IV_ZOOM_MODE		m_nZoomMode;
	VO_IV_ASPECT_RATIO	m_nRatio;
	VO_IV_COLORTYPE		m_nOutputColor;
    
	VO_U32				m_nShowLeft;
	VO_U32				m_nShowTop;
	VO_U32				m_nShowWidth;
	VO_U32				m_nShowHeight;
    
	VO_U32				m_nDrawLeft;
	VO_U32				m_nDrawTop;
	VO_U32				m_nDrawWidth;
	VO_U32				m_nDrawHeight;
    
    VO_U32				m_nDrawLeftLast;
	VO_U32				m_nDrawTopLast;
	VO_U32				m_nDrawWidthLast;
	VO_U32				m_nDrawHeightLast;
    
	VO_U32				m_nScreenWidth;
	VO_U32				m_nScreenHeight;
    
	VO_BOOL				m_bVideoUpsizeDown;
	VO_BOOL				m_bRotate;
	VO_BOOL				m_bFullScreen;
	VO_IV_RTTYPE		m_bRotateType;
};

#ifdef _VONAMESPACE
}
#endif

#endif
