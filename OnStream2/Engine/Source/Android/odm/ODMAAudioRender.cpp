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
 * @file ODMAAudioRender.cpp
 * wrap class of ODMA Audio Render.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include <assert.h>
#include "ODMAAudioRender.h"
#include "ODMAVomePlayer.h"

#define LOG_TAG "ODMAAudioRender"

#include "voLog.h"


ODMAAudioRender::ODMAAudioRender(ODMAVomePlayer * pEngine)
	: mEngine(pEngine)
	, mAudioBuffer (NULL)
	, mRenderThread (NULL)
	, mBuffer(NULL)
	, mRenderSize (0)
	, mStatus (0)
	, mRenderNum(0)
	, mRenHandle(NULL)
{
	mAudioFormat.Channels = 2;
	mAudioFormat.SampleRate = 44100;
	mAudioFormat.SampleBits = 16;

	mAudioBuffer =  new VOMP_BUFFERTYPE;

	mRenderSize = 44100*2*sizeof(short);
	mBuffer = new unsigned char[mRenderSize];
	if(!LoadLib(NULL))
		VOLOGE("Can not load the Andorid Audio Render implement library \n");
}

ODMAAudioRender::~ODMAAudioRender()
{
	stop();

	while (mRenderThread != NULL)
		voOS_Sleep (10);	
	
	if(mAudioBuffer)
	{
		delete mAudioBuffer;
		mAudioBuffer = NULL;
	}

	if(mBuffer)
	{
		delete mBuffer;
		mBuffer = NULL;
	}
	if(mRenHandle)
	{
		mFARender.Uninit(mRenHandle);
		mRenHandle = NULL;
	}
}

int	ODMAAudioRender::start (void)
{
	if (mStatus == 1)
	{
		return 0;
	}
	else if (mStatus == 2)
	{
		if (mRenHandle != NULL)
			mFARender.Start(mRenHandle);				
	}

	mStatus = 1;
	mRenderNum = 0;

	if(mRenderThread == NULL)
	{
		VO_U32 ThdID;
		voThreadCreate (&mRenderThread, &ThdID, (voThreadProc)threadProc, this, 0);
	}

	return 0;
}

int ODMAAudioRender::pause (void)
{
	if(mRenHandle == NULL)
		return -1;

	mStatus = 2;
	
	return mFARender.Pause(mRenHandle);
}

int ODMAAudioRender::stop (void)
{
	if(mRenHandle == NULL)
		return -1;

	mStatus = 0;

	mFARender.Stop(mRenHandle);


	if(mRenHandle) 
	{
		while (mRenderThread != NULL)
			voOS_Sleep (10);	
	}
	return 0;
}

int ODMAAudioRender::flush (void)
{
	if(mRenHandle == NULL)
		return -1;

	return 	mFARender.Flush(mRenHandle);
}

int ODMAAudioRender::setParam(int nID, void* pValue)
{	
	if(!mRenHandle)
	{
		mFARender.Init(&mRenHandle, NULL, 0);
		if(!mRenHandle)
		{
			VOLOGE("Can not get the android audio render handle\n");
			return -1;
		}
	}
	VOLOGE("nID = %d(%x), pValue = %p\n", nID, nID, pValue);
	return mFARender.SetParam(mRenHandle, nID, pValue);
}
int ODMAAudioRender::runThreadLoop (void)
{

	if(mRenHandle == NULL)
	{
		mFARender.Init(&mRenHandle, NULL, 0);
		if(!mRenHandle)
		{
			VOLOGE("Can not get the android audio render handle\n");
			return -1;
		}
		assert(mRenHandle != NULL);
		assert(mEngine != NULL);
		
		//mEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &mAudioFormat);
		//VOLOGE("mAudioFormat.SampleRate = %d, .Channels = %d, .SampleBits = %d\n", mAudioFormat.SampleRate, mAudioFormat.Channels, mAudioFormat.SampleBits);
		//mFARender.SetFormat(mRenHandle, (VO_AUDIO_FORMAT *)&mAudioFormat);
		//mFARender.Start(mRenHandle);
	}
	
	if(mAudioBuffer != NULL)
	{
		mAudioBuffer->pBuffer = mBuffer;
		mAudioBuffer->nSize = mRenderSize;
	}

	int nRC = mEngine->GetAudioBuffer(&mAudioBuffer);
	if (nRC == VOOSMP_ERR_None && mAudioBuffer != NULL)
	{
		if(mRenderNum == 0)
		{
			memset (&mAudioFormat, 0, sizeof (VOOSMP_AUDIO_FORMAT));
			mEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &mAudioFormat);

			VOLOGE("mAudioFormat.SampleRate = %d, .Channels = %d, .SampleBits = %d\n", mAudioFormat.SampleRate, mAudioFormat.Channels, mAudioFormat.SampleBits);
			mFARender.SetFormat(mRenHandle, (VO_AUDIO_FORMAT *)&mAudioFormat);
			
			VO_U32 uRenderSize  = mAudioFormat.SampleRate * mAudioFormat.Channels * mAudioFormat.SampleBits/8;
			if(uRenderSize > mRenderSize)
			{
				mRenderSize = uRenderSize;

				unsigned char *pTemp = mBuffer;
				mBuffer = new unsigned char[uRenderSize];

				if(!mBuffer)
					return -1;

				memcpy(mBuffer , mAudioBuffer->pBuffer , mAudioBuffer->nSize);
				mAudioBuffer->pBuffer = mBuffer;

				delete pTemp;
			}

			
			if (mRenHandle != NULL)
				mFARender.Start(mRenHandle);
		}
		
		mRenderNum++;

		if (mRenHandle != NULL)
			mFARender.Render(mRenHandle, (VO_PBYTE)mAudioBuffer->pBuffer, mAudioBuffer->nSize, mAudioBuffer->llTime, VO_TRUE);
	}
	else if (nRC == VOOSMP_ERR_FormatChange)
	{
		memset (&mAudioFormat, 0, sizeof (VOOSMP_AUDIO_FORMAT));
		mEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &mAudioFormat);

		VOLOGE("mAudioFormat.SampleRate = %d, .Channels = %d, .SampleBits = %d\n", mAudioFormat.SampleRate, mAudioFormat.Channels, mAudioFormat.SampleBits);
		mFARender.SetFormat(mRenHandle, (VO_AUDIO_FORMAT *)&mAudioFormat);


		mRenderSize = mAudioFormat.SampleRate * mAudioFormat.Channels * mAudioFormat.SampleBits/8;
		if(mBuffer) 
		{
			delete mBuffer;
			mBuffer = NULL;
		}

		mBuffer = new unsigned char[mRenderSize];

		if (mRenHandle != NULL)
			mFARender.Start(mRenHandle);
		
	}
	else
	{
		voOS_Sleep(5);
	}
	return 0;
}

int	ODMAAudioRender::threadProc (void * pParam)
{
	ODMAAudioRender * pRender = (ODMAAudioRender *)pParam;

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

typedef VO_S32 (VO_API * VOAUDIORENDERAPI) (VO_AUDIO_RENDERAPI * pFunc, VO_U32 uFlag);

VO_U32 ODMAAudioRender::LoadLib(VO_HANDLE hInst)
{
	vostrcpy (m_szDllFile, _T("voosmp"));
	vostrcpy (m_szAPIName, _T("voGetAudioRenderAPI"));

	vostrcat (m_szDllFile, _T(".so"));


	if (CDllLoad::LoadLib (hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOAUDIORENDERAPI pAPI = (VOAUDIORENDERAPI)m_pAPIEntry;
	pAPI (&mFARender, 0);
	return 1;
}
