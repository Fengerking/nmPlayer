/*
 *  CCGRender.h
 *  vompEngn
 *
 *  Created by Lin Jun on 1/27/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */
#include "CBaseVideoRender.h"


class CCGRender : public CBaseVideoRender
{
public:
	CCGRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CCGRender (void);
	
public:
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect);
protected:
	virtual VO_U32 UpdateSize (void);	
protected:
	VO_BYTE*		m_pVideoBuf;
};