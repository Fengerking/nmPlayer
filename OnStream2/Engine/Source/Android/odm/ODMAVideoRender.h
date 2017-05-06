/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file ODMAVideoRender.h
 * wrap class of ODMA Video Render.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#ifndef __ODM_AVideoRender_h__
#define __ODM_AVideoRender_h__

#include "voType.h"
#include "voOnStreamType.h"
#include "voThread.h"
#include "voOSFunc.h"
#include "CDllLoad.h"
#include "voVideoRender.h"


class ODMAVomePlayer;

class ODMAVideoRender : public CDllLoad
{
public:
	ODMAVideoRender(ODMAVomePlayer * pEngine, void* pWnd);
	virtual	~ODMAVideoRender(void);

	int			start (void);
	int			pause (void);
    int			stop (void);

	int			setPos (int nPos);
	int			setRect (VOMP_RECT *pRect);

	// pass this informaiton to VideoRender
	bool        updateView(void* pWnd);
// function from CDllLoad	
	virtual VO_U32		LoadLib(VO_HANDLE hInst);
private:	
	static	int			threadProc (void * pParam);	
	int			        runThreadLoop (void);	
protected:
	int					mStatus;	
	voThreadHandle		mRenderThread;
	ODMAVomePlayer *	mEngine;

	VOOSMP_VIDEO_FORMAT	mVideoFormat;
	int					mColorType;
	bool				mSeeking;

	VOMP_BUFFERTYPE		mVideoBuffer;

	int					mRenderNum;
	VO_VIDEO_RENDERAPI  mFVRender;
	VO_HANDLE           mRenHandle;
	void*               mWnd;
	VOMP_RECT			mRect;
};

#endif //__ODM_AVideoRender_h__

