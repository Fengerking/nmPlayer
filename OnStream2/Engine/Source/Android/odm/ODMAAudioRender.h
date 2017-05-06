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
 * @file ODMAAudioRender.h
 * wrap class of ODMA Audio Render.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#ifndef __ODM_AAudioRender_h__
#define __ODM_AAudioRender_h__


#include "voType.h"
#include "vompType.h"
#include "voOnStreamType.h"
#include "voOSFunc.h"
#include "voAudioRender.h"
#include "CDllLoad.h"

#include "voThread.h"

class ODMAVomePlayer;

class ODMAAudioRender : public CDllLoad
{
public:
	ODMAAudioRender(ODMAVomePlayer * pEngine);
	virtual	~ODMAAudioRender(void);

	int			start (void);
	int			pause (void);
	int			stop (void);
    int			flush (void);
	int         setParam(int nID, void* pValue);

// function from CDllLoad	
	virtual VO_U32		LoadLib(VO_HANDLE hInst);
	
private:
	static	int			threadProc (void * pParam);	
	virtual	int			runThreadLoop (void);

protected:
	voThreadHandle      mRenderThread;	
	ODMAVomePlayer *	mEngine;
	int					mStatus;
	VOOSMP_AUDIO_FORMAT	mAudioFormat;
	VOMP_BUFFERTYPE *	mAudioBuffer;
	unsigned char*		mBuffer;
	unsigned int		mRenderSize;
	int					mRenderNum;
	VO_AUDIO_RENDERAPI  mFARender;
	VO_HANDLE           mRenHandle;
};

#endif //__ODM_AAudioRender_h__


