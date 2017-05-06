	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseAudioRender.h

	Contains:	CBaseAudioRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CXAudio2Render_H__
#define __CXAudio2Render_H__

#include "XAudio2.h"
#include "voCMutex.h"
#include "voCSemaphore.h"
#include "CPtrList.h"
#include "CBaseAudioRender.h"

typedef struct _AudioBufInfo
{
	int					nIndex;
	XAUDIO2_BUFFER *	pXAudio2Buf;
	long long			llTimeStamp;
}AudioBufInfo;

class CXAudio2Render : public CBaseAudioRender , public IXAudio2VoiceCallback 
{
public:
	// Used to control the image drawing
	CXAudio2Render (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CXAudio2Render (void);

	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);

	VO_U32				GetRenderBuf(VO_BYTE **ppBuf , VO_U32 *pBufSize);
protected:

    // Voice callbacks from IXAudio2VoiceCallback
    //
    STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 bytesRequired);
    STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS);
    STDMETHOD_(void, OnStreamEnd) (THIS);
    STDMETHOD_(void, OnBufferStart) (THIS_ void* bufferContext);
    STDMETHOD_(void, OnBufferEnd) (THIS_ void* bufferContext);
    STDMETHOD_(void, OnLoopEnd) (THIS_ void* bufferContext);
    STDMETHOD_(void, OnVoiceError) (THIS_ void* bufferContext, HRESULT error);

protected:
	HRESULT				InitAudioDevices();
	HRESULT				UninitAudioDevices();

	VO_U32				InitAudioSource();
	int					GetBufIndex(BYTE *pBuf);
protected:
	IXAudio2 *					mpXAudio2;
	IXAudio2MasteringVoice*		mpMasteringVoice;
	IXAudio2SourceVoice*		mpSourceVoice;

	WAVEFORMATEX				mAudioFormat;
	bool						mbRunning;
	voCMutex					mcsStatus;

	int							mnBufCount;
	int							mnBufSize;
	AudioBufInfo *				mpAudioBuf;
	voCSemaphore				mcSemaphore;
				
	CObjectList<AudioBufInfo>	mFreeList;

	//time stamp	
	long long					mllMediaTime;

	int							mnSampleCount;
};

#endif // __CXAudio2Render_H__
