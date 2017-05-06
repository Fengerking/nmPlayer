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
 * @file ODMAVideoRender.cpp
 * wrap class of ODMA Video Render.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include <assert.h>
#include "ODMAVomePlayer.h"
#include "ODMAVideoRender.h"


#define LOG_TAG "ODMAVideoRender" 

#include "voLog.h"

ODMAVideoRender::ODMAVideoRender(ODMAVomePlayer * pEngine, void* pWnd)
	: mStatus (0)
	,mRenderThread (NULL)
	, mEngine (pEngine)
	, mColorType (VOOSMP_COLOR_RGB565_PACKED)
	, mSeeking (false)
	, mRenderNum(0)
	, mRenHandle(NULL)
	, mWnd(pWnd)
{
	memset (&mVideoFormat, 0, sizeof (VOOSMP_VIDEO_FORMAT));

	memset(&mRect, 0, sizeof(VOMP_RECT));
	
	memset(&mVideoBuffer, 0, sizeof(VOMP_BUFFERTYPE));
	if(!LoadLib(NULL))
		VOLOGE("Can not load the Andorid Video Render implement library \n");


}

ODMAVideoRender::~ODMAVideoRender()
{
	stop();

	while (mRenderThread != NULL)
		voOS_Sleep (10);	

	if(mRenHandle)
	{
		mFVRender.Uninit(mRenHandle);
		mRenHandle = NULL;
	}
}

int	ODMAVideoRender::start (void)
{
	if (mSeeking)
		mSeeking = false;

	if (mStatus == 1)
		return 0;

	mStatus = 1;
	mRenderNum = 0;

	if(mRenderThread == NULL)
	{
		VO_U32 ThdID;
	
		voThreadCreate (&mRenderThread, &ThdID, (voThreadProc)threadProc, this, 0);
	}

	return 0;
}

int ODMAVideoRender::stop (void)
{
	mStatus = 0;
	
	if(mRenHandle)
	{
		while (mRenderThread != NULL)
			voOS_Sleep (10);
		mFVRender.Stop(mRenHandle);
	}
	return 	0;
}

int ODMAVideoRender::pause (void)
{
	mStatus = 2;
	
	if(mRenHandle)
		mFVRender.Pause(mRenHandle);

	return 0;
}

int	ODMAVideoRender::setRect (VOMP_RECT *pRect)
{
	mRect.bottom = pRect->bottom;
	mRect.left = pRect->left;
	mRect.right = pRect->right;
	mRect.top = pRect->top;

	if(mRenHandle != NULL)
		mFVRender.SetDispRect(mRenHandle, mWnd, (VO_RECT*)&mRect);

	return 0;
}

int	ODMAVideoRender::setPos (int nPos)
{
	if (mStatus == 2)
	{
		mSeeking = true;			
		mStatus = 1;
	}

	return 0;
}

bool ODMAVideoRender::updateView(void* pWnd)
{
	assert(mRenHandle != NULL);
	mWnd = pWnd;
	mFVRender.SetDispRect (mRenHandle, mWnd, (VO_RECT *)&mRect);
}
int ODMAVideoRender::runThreadLoop (void)
{
	VOMP_BUFFERTYPE * pVideoBuffer = NULL;
	int nRC = 0;	
	if(mRenHandle == NULL)
	{
    	mFVRender.Init(&mRenHandle, mWnd, NULL, 0);
		if(!mRenHandle)
		{
			VOLOGE("Can not get the Android Video Render implement\n");
			return -1;
		}
		mEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &mVideoFormat);
		mFVRender.SetVideoInfo (mRenHandle,mVideoFormat.Width, mVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		mFVRender.SetDispRect (mRenHandle, mWnd, (VO_RECT *)&mRect);
	}

	
	pVideoBuffer = NULL;
	

	//VOLOGE("mEngine->GetVideoBuffer(&pVideoBuffer)\n");
	nRC = mEngine->GetVideoBuffer(&pVideoBuffer);

	assert(mRenHandle != NULL);

	//VOLOGE(" nRC = %d (%x)\n", nRC, nRC);
	if ((mVideoFormat.Width == 0 || mVideoFormat.Height == 0) && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange))
	{
		//VOLOGE("(mVideoFormat.Width == 0 || mVideoFormat.Height == 0) && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange)");
		mEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &mVideoFormat);
		mFVRender.SetVideoInfo (mRenHandle, mVideoFormat.Width, mVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		mFVRender.SetDispRect (mRenHandle, mWnd, (VO_RECT *)&mRect);		
		return VOOSMP_ERR_FormatChange;
	}

	if (nRC == VOOSMP_ERR_Retry)
	{
		//VOLOGE(" VOOSMP_ERR_Retry why why\n");
		voOS_Sleep (2);
		return 0;
	}
	else if (nRC == VOOSMP_ERR_FormatChange)
	{
		//VOLOGE("VOOSMP_ERR_FormatChange why why\n");
		mEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &mVideoFormat);
		mFVRender.SetVideoInfo (mRenHandle, mVideoFormat.Width, mVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		mFVRender.SetDispRect (mRenHandle, mWnd, (VO_RECT *)&mRect);		
		if(pVideoBuffer == NULL)
			return 0;
	}
	else if (nRC != VOMP_ERR_None)
	{
		return 0;
	}
	else if (pVideoBuffer == NULL)
	{
		voOS_Sleep (2);
		//VOLOGE("why why\n");
		return 0;
	}

	if(nRC == VOOSMP_ERR_None)
	{
		if(mRenderNum == 0)
		{
			mEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &mVideoFormat);
			mFVRender.SetVideoInfo (mRenHandle, mVideoFormat.Width, mVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			mFVRender.SetDispRect (mRenHandle, mWnd, (VO_RECT *)&mRect);		
		}

		mRenderNum++;
	}	

	if (mRenHandle != NULL && mRenderNum > 1)
	{
		if (pVideoBuffer && pVideoBuffer->nSize == sizeof (VO_VIDEO_BUFFER))
		{
			VO_VIDEO_BUFFER *pvoVideoBuffer = (VO_VIDEO_BUFFER *)pVideoBuffer->pBuffer;
			//VOLOGE("why why pvoVideoBuffer->ColorType = %d(%x)\n", pvoVideoBuffer->ColorType, pvoVideoBuffer->ColorType);
			mFVRender.Render (mRenHandle, (VO_VIDEO_BUFFER *)pVideoBuffer->pBuffer, pVideoBuffer->llTime, VO_TRUE);
		}
	}
	return 0;
}

int	ODMAVideoRender::threadProc (void * pParam)
{
	ODMAVideoRender * pRender = (ODMAVideoRender *)pParam;

	while (pRender->mStatus == 1 || pRender->mStatus == 2)
	{
		if (pRender->mStatus == 2)
		{
			voOS_Sleep(2);
			continue;
		}

		pRender->runThreadLoop ();
	}

	pRender->mRenderThread = NULL;

	return 0;
}

typedef VO_S32 (VO_API * VOVIDEORENDERAPI) (VO_VIDEO_RENDERAPI * pFunc, VO_U32 uFlag);

VO_U32 ODMAVideoRender::LoadLib(VO_HANDLE hInst)
{
	vostrcpy (m_szDllFile, _T("voosmp"));
	vostrcpy (m_szAPIName, _T("voGetVideoRenderAPI"));

	vostrcat (m_szDllFile, _T(".so"));


	if (CDllLoad::LoadLib (hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}
	
	VOVIDEORENDERAPI pAPI = (VOVIDEORENDERAPI)m_pAPIEntry;
	pAPI (&mFVRender, 0);
	return 1;
}

