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
 * @file ODMAVomePlayer.h
 * wrap class of ODMA Vome Player interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#ifndef __ODMAVomePlayer_H__
#define __ODMAVomePlayer_H__

#include "COSVomePlayer.h"

class ODMAAudioRender;
class ODMAVideoRender;

class ODMAVomePlayer : public COSVomePlayer
{
public:
	// Used to control the image drawing
	ODMAVomePlayer ();
	virtual ~ODMAVomePlayer (void);

	virtual int			Init();
	virtual int			Uninit();

	virtual int			SetDataSource (void * pSource, int nFlag);

	virtual int			SetView(void* pView);

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Close (void);
	virtual int 		Flush (void);
	virtual int 		SetParam (int nID, void * pValue);
	
protected:
	void*				mWnd;

	ODMAAudioRender*    mAudioRender;
	ODMAVideoRender*    mVideoRender;


};

#endif // __ODMAVomePlayer_H__
