/*
 *  CMacOSVideoRender.h
 *  vompEngn
 *
 *  Created by Lin Jun on 11/30/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#ifndef _CMAC_OS_VIDEO_RENDER_H__
#define _CMAC_OS_VIDEO_RENDER_H__

#include "CBaseVideoRender.h"
#include "vompType.h"

//#define _DUMP_YUV
//#define _YUV_PLAYER
//#define _DUMP_RGB

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CMacOSVideoRender : public CBaseVideoRender
{
public:
	CMacOSVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CMacOSVideoRender (void);
	
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor=VO_COLOR_RGB565_PACKED);
	virtual VO_U32 	Redraw (void);
	
    virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
    
    virtual VO_U32 	Start (void);
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);
    
    virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
    virtual VO_U32 	SetEventCallBack (VOVIDEO_EVENT_CALLBACKPROC pCallBack, VO_PTR pUserData);
    
protected:
	virtual VO_U32  UpdateSize (void);
    virtual bool	CreateCCRRR (void);
	
private:
	
#ifdef _DUMP_YUV
	FILE* m_hYUV;
#endif
    
#ifdef _DUMP_RGB
	FILE* m_hRGB;
#endif
    
	void* m_pRender;
    
    VO_BOOL m_bRunning;
    
    VO_U64 m_lastTimeStamp;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif
