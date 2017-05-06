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
 * @file ODMAVomePlayer.cpp
 * wrap class of ODMA Vome Player interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include "ODMAVomePlayer.h"
#include "ODMAAudioRender.h"
#include "ODMAVideoRender.h"

#define LOG_TAG "ODMAVomePlayer"

#include "voLog.h"

ODMAVomePlayer::ODMAVomePlayer()
	: mAudioRender (NULL)
	, mVideoRender (NULL)
	, mWnd(NULL)
{
}

ODMAVomePlayer::~ODMAVomePlayer ()
{
	Uninit();
}

int	ODMAVomePlayer::Init()
{
	int nRC = COSVomePlayer::Init();
	
	if(nRC != VOOSMP_ERR_None) 
		return nRC;

	int nRndNum = 6;
	nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);
	if (nRC != VOMP_ERR_None) return ReturnCode(nRC);

	int nPlayMode = VOMP_PULL_MODE;
	nRC = COSVomeEngine::SetParam(VOMP_PID_AUDIO_PLAYMODE, &nPlayMode);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);
	nRC = COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAYMODE, &nPlayMode);
	
	return nRC;
}

int	ODMAVomePlayer::Uninit()
{
	if( mAudioRender != NULL )
	{
		delete 	mAudioRender;
		mAudioRender = NULL;
	}

	if( mVideoRender != NULL)
	{
		delete mVideoRender;
		mVideoRender = NULL;
	}

	return COSVomePlayer::Uninit();
}

int ODMAVomePlayer::SetDataSource (void * pSource, int nFlag)
{
	if(mAudioRender != NULL)
	{
		delete mAudioRender;
		mAudioRender =NULL;
	}

	if(mVideoRender != NULL)
	{
		delete mVideoRender;
		mVideoRender = NULL;
	}	

	mAudioRender = new ODMAAudioRender(this);
	
	if(mWnd != NULL)
		mVideoRender = new ODMAVideoRender(this, mWnd);
	//mVideoRender->SetRect(&m_rcDraw);
	
	return COSVomePlayer::SetDataSource(pSource, nFlag);
}

int ODMAVomePlayer::Run (void)
{
	int nRC =  COSVomePlayer::Run();

	if(mAudioRender)
		mAudioRender->start();

	if(mVideoRender)
		mVideoRender->start();
	return nRC;
}

int ODMAVomePlayer::Pause (void)
{
	if(mAudioRender)
		mAudioRender->pause();

	if(mVideoRender)
		mVideoRender->pause();

	return COSVomePlayer::Pause();
}

int ODMAVomePlayer::Stop (void)
{
	if(mAudioRender)
		mAudioRender->stop();

	if(mVideoRender)
		mVideoRender->stop();

	return COSVomePlayer::Stop();
}

int ODMAVomePlayer::Close (void)
{
	int nRC = Stop();

	if(mAudioRender != NULL)
	{
		delete mAudioRender;
		mAudioRender =NULL;
	}

	if(mVideoRender != NULL)
	{
		delete mVideoRender;
		mVideoRender = NULL;
	}

	return nRC;
}

int ODMAVomePlayer::Flush (void)
{
	if(mAudioRender)
		mAudioRender->flush();

	return COSVomePlayer::Flush();
}

int ODMAVomePlayer::SetView(void* pView)
{
	if(mWnd && mVideoRender)
	{
		mVideoRender->updateView(pView);
	}
	mWnd = pView;
	return VOOSMP_ERR_None;
}

int ODMAVomePlayer::SetParam (int nID, void * pValue)
{
	VOLOGE("%s::%s::%d nID = %d\n", __FILE__, __FUNCTION__, __LINE__, nID);
	/*
	if(nID == VOOSMP_PID_DRAW_RECT)
	{
		VOOSMP_RECT	*pRect = (VOOSMP_RECT	*)pValue;
		m_rcDraw.bottom = pRect->nBottom;
		m_rcDraw.right  = pRect->nRight;
		m_rcDraw.top	= pRect->nTop;
		m_rcDraw.left	= pRect->nLeft;

		if(m_pVideoRender)
			m_pVideoRender->SetRect(&m_rcDraw);
	}
	*/
	if( VOOSMP_PID_AUDIO_SINK == nID)
	{
		if(mAudioRender)
		{
			return mAudioRender->setParam(nID, pValue);
		}
		VOLOGE(" AudioSink Should not come here %s::%s::%d\n", __FILE__, __FUNCTION__, __LINE__);
	}
	return COSVomePlayer::SetParam(nID, pValue);
}



