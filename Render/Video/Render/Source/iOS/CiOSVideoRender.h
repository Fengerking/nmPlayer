/*
 *  CiOSVideoRender.h
 *  
 *
 *  Created by Lin Jun on 11/19/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#ifndef __CIOS_VIDEO_RENDER__
#define __CIOS_VIDEO_RENDER__

#include "CBaseVideoRender.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
    
#define VO_PID_VIDEO_RENDER_IOS_APP_ACTIVE_STATUS  (VO_PID_VIDEO_BASE | 0X0382)  /*!< Set the active status of iOS application, bool * */

//#define _DUMP_YUV
//#define _YUV_PLAYER
//#define _DUMP_RGB

class voGLRenderBase;
    
class CiOSVideoRender : public CBaseVideoRender
{
public:
	CiOSVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CiOSVideoRender (void);
	
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_RGB565_PACKED);
	
	virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
    
    virtual VO_U32 	Start (void);
	virtual VO_U32 	Stop (void);
protected:
	virtual VO_U32	UpdateSize (void);
    virtual bool	CreateCCRRR (void);
	//virtual bool	ConvertData (VO_VIDEO_BUFFER * pInBuffer, VO_VIDEO_BUFFER * pOutBuffer, VO_S64 nStart, VO_BOOL bWait);
	
private:
#ifdef _DUMP_YUV
	FILE* m_hYUV;
#endif

#ifdef _DUMP_RGB
	FILE* m_hRGB;
#endif
	
	voGLRenderBase* m_pRender;
};

#ifdef _VONAMESPACE
}
#endif

#endif
