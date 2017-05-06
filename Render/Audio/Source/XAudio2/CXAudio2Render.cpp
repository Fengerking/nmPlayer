
#include "CXAudio2Render.h"
#include "voLog.h"

const int gBufCount = 2;

CXAudio2Render::CXAudio2Render(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	:CBaseAudioRender(hInst , pMemOP)
	,mpXAudio2(nullptr)
	,mpMasteringVoice(nullptr)
	,mpSourceVoice(nullptr)
	,mbRunning(false)
	,mnBufCount(gBufCount)
	,mllMediaTime(0)
	,mnSampleCount(0)
{
	mpAudioBuf = new AudioBufInfo[mnBufCount];
	for(int i = 0 ; i < mnBufCount ; i++)
	{
		mpAudioBuf[i].nIndex = i;
		mpAudioBuf[i].pXAudio2Buf = new XAUDIO2_BUFFER;
		ZeroMemory(mpAudioBuf[i].pXAudio2Buf , sizeof(XAUDIO2_BUFFER));
	}

	m_pMemOP->Set(VO_INDEX_SNK_AUDIO , &mAudioFormat , 0 , sizeof(mAudioFormat));
}

CXAudio2Render::~CXAudio2Render ()
{
	if(mpAudioBuf != nullptr)
	{
		for(int i = 0 ; i < mnBufCount ; i++)
		{
			if(mpAudioBuf[i].pXAudio2Buf != nullptr && mpAudioBuf[i].pXAudio2Buf->pAudioData != nullptr)
			{
				delete [] mpAudioBuf[i].pXAudio2Buf->pAudioData;
				mpAudioBuf[i].pXAudio2Buf->pAudioData = nullptr;
			}

			if(mpAudioBuf[i].pXAudio2Buf != nullptr)
			{
				delete mpAudioBuf[i].pXAudio2Buf;
				mpAudioBuf[i].pXAudio2Buf = nullptr;
			}
		}
	
		delete mpAudioBuf;
		mpAudioBuf = nullptr;
	}

	UninitAudioDevices();
}

VO_U32 CXAudio2Render::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if(mAudioFormat.nChannels == pFormat->Channels && 
		mAudioFormat.nSamplesPerSec == pFormat->SampleRate && 
		mAudioFormat.wBitsPerSample == pFormat->SampleBits)
		return VO_ERR_NONE;

	voCAutoLock lockit(&mcsStatus);
	mAudioFormat.wFormatTag = WAVE_FORMAT_PCM;
	mAudioFormat.nChannels = pFormat->Channels;
	mAudioFormat.nSamplesPerSec = pFormat->SampleRate; 
	mAudioFormat.wBitsPerSample = pFormat->SampleBits;
	mAudioFormat.nBlockAlign = mAudioFormat.wBitsPerSample * mAudioFormat.nChannels / 8;
	mAudioFormat.nAvgBytesPerSec = mAudioFormat.nBlockAlign * mAudioFormat.nSamplesPerSec;
	mAudioFormat.cbSize = 0;

	return InitAudioSource();
}

VO_U32 CXAudio2Render::Start (void)
{
	if(mbRunning == true)
	{
		if(mnSampleCount > 0)
			mpSourceVoice->Start();

		return VO_ERR_NONE;
	}

	if(mpXAudio2 == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	if(mbRunning == true)
		return VO_ERR_NONE;
	
	voCAutoLock lockit(&mcsStatus);
	mpXAudio2->StartEngine();

	mbRunning = true;
	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::Pause (void)
{
	if(mpSourceVoice == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 
	
	voCAutoLock lockit(&mcsStatus);
	mpSourceVoice->Stop();

	mbRunning = true;
	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::Stop (void)
{
	if(mbRunning == false)
		return VO_ERR_NONE;

	if(mpSourceVoice == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	voCAutoLock lockit(&mcsStatus);
	HRESULT hr = mpSourceVoice->Stop();
	if(FAILED(hr))
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 
	
	hr = mpSourceVoice->FlushSourceBuffers();
	if(FAILED(hr))
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO;

	if(mpXAudio2 != nullptr)
		mpXAudio2->StopEngine();

	mbRunning = false;
	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
	if(mpSourceVoice == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	voCAutoLock lockit(&mcsStatus);

	int nIndex = GetBufIndex(pBuffer);
	if(nIndex == -1)
	{
		VO_BYTE *pBuf = NULL;
		VO_U32		nBufSize = 0;

		GetRenderBuf(&pBuf , &nBufSize);
		if(pBuf == NULL)
			return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

		if(nBufSize < nSize)
		{
			VOLOGE("Buffer size small nBufSize : %d  nSize = %d " , nBufSize , nSize);
			return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 
		}

		m_pMemOP->Copy(VO_INDEX_SNK_AUDIO , pBuf , pBuffer , nSize);
		 nIndex = GetBufIndex(pBuf);
	}

	mpAudioBuf[nIndex].llTimeStamp = nStart;
	mpAudioBuf[nIndex].nIndex = nIndex;
	mpAudioBuf[nIndex].pXAudio2Buf->AudioBytes = nSize;

	//VOLOGI("Audio info : time %lld   index %d size %d" , nStart , nIndex , nSize);
	
	HRESULT hr = mpSourceVoice->SubmitSourceBuffer(mpAudioBuf[nIndex].pXAudio2Buf);
	if(FAILED(hr))
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	if(mnSampleCount == 0)
	{
		mpSourceVoice->Start();
	}

	mnSampleCount ++;
	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::Flush (void)
{
	if(mpSourceVoice == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	voCAutoLock lockit(&mcsStatus);
	HRESULT hr = mpSourceVoice->FlushSourceBuffers();
	if(FAILED(hr))
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::GetPlayingTime (VO_S64	* pPlayingTime)
{
	if(mpSourceVoice == nullptr)
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO; 

	 XAUDIO2_VOICE_STATE voiceState;
	 mpSourceVoice->GetState(&voiceState);

	 *pPlayingTime = mllMediaTime + voiceState.SamplesPlayed * 1000 / mAudioFormat.nSamplesPerSec;

	return VO_ERR_NONE;
}

VO_U32 CXAudio2Render::GetBufferTime (VO_S32	* pBufferTime)
{
	*pBufferTime = 0;
	return VO_ERR_NONE;
}

HRESULT	CXAudio2Render::InitAudioDevices()
{
	voCAutoLock lockit(&mcsStatus);
	HRESULT hr = NOERROR;
	if(mpXAudio2 == nullptr)
	{
		hr = XAudio2Create(&mpXAudio2 , 0 , XAUDIO2_DEFAULT_PROCESSOR );
		if(FAILED(hr))
			return hr;
	}

	if(mpMasteringVoice == nullptr)
	{
		hr = mpXAudio2->CreateMasteringVoice(&mpMasteringVoice);
		if(FAILED(hr))
			return hr;
	}

	return hr;
}

HRESULT	 CXAudio2Render::UninitAudioDevices()
{
	voCAutoLock lockit(&mcsStatus);
	if(mpSourceVoice != nullptr)
	{
		mpSourceVoice->DestroyVoice();
		mpSourceVoice = nullptr;
	}

	if(mpMasteringVoice != nullptr)
	{
		 mpMasteringVoice->DestroyVoice();
        mpMasteringVoice = nullptr;
	}

	if(mpXAudio2 != nullptr)
	{
		mpXAudio2->Release();
		mpXAudio2 = nullptr;
	}

	return NOERROR;
}

VO_U32	CXAudio2Render::InitAudioSource()
{
	if(mpXAudio2 == nullptr)
	{
		HRESULT hr = InitAudioDevices();
		if(FAILED(hr))
			return VO_ERR_FAILED | VO_INDEX_SNK_AUDIO;
	}

	if(mbRunning)
	{
		Stop();
	}

	if(mpSourceVoice != nullptr)
	{
		mpSourceVoice->DestroyVoice();
		mpSourceVoice = nullptr;
	}

	HRESULT hr = mpXAudio2->CreateSourceVoice(&mpSourceVoice , &mAudioFormat , 0 , XAUDIO2_DEFAULT_FREQ_RATIO , this);
	if(FAILED(hr))
		return  VO_ERR_FAILED | VO_INDEX_SNK_AUDIO;

	for(int i = 0 ; i < mnBufCount ; i++)
	{
		if(mpAudioBuf[i].pXAudio2Buf->pAudioData != nullptr)
		{
			delete [] mpAudioBuf[i].pXAudio2Buf->pAudioData;
			mpAudioBuf[i].pXAudio2Buf->pAudioData = nullptr;
		}
	}

	mFreeList.RemoveAll();

	mnBufSize = mAudioFormat.nAvgBytesPerSec / 2;
	mnBufSize = mnBufSize / mAudioFormat.nBlockAlign *  mAudioFormat.nBlockAlign;

	for(int i = 0 ; i < mnBufCount ; i++)
	{
		mpAudioBuf[i].pXAudio2Buf->Flags = 0;
		mpAudioBuf[i].pXAudio2Buf->pAudioData = new BYTE[mnBufSize];
		mpAudioBuf[i].pXAudio2Buf->PlayBegin = 0;
		mpAudioBuf[i].pXAudio2Buf->PlayLength = 0;
		mpAudioBuf[i].pXAudio2Buf->LoopBegin = 0;
		mpAudioBuf[i].pXAudio2Buf->LoopLength = 0;
		mpAudioBuf[i].pXAudio2Buf->LoopCount  = 0;
		mpAudioBuf[i].pXAudio2Buf->pContext = &(mpAudioBuf[i]);
		mFreeList.AddTail(&(mpAudioBuf[i]));
		mcSemaphore.Up();
	}

	return VO_ERR_NONE;
}

int CXAudio2Render::GetBufIndex(BYTE *pBuf)
{
	for(int i = 0 ; i < mnBufCount ; i++)
	{
		if(pBuf == mpAudioBuf[i].pXAudio2Buf->pAudioData)
			return i;
	}

	return -1;
}

VO_U32	CXAudio2Render::GetRenderBuf(VO_BYTE **ppBuf , VO_U32 *pBufSize)
{
	mcSemaphore.Down();
	AudioBufInfo * pAudioBufInfo = mFreeList.RemoveHead();
	if(pAudioBufInfo != NULL)
	{
		*ppBuf = (VO_BYTE *)pAudioBufInfo->pXAudio2Buf->pAudioData;
		*pBufSize = mnBufSize;
	}
	else
	{
		*ppBuf = NULL;
		*pBufSize = 0;

		return VO_ERR_FAILED | VO_INDEX_SNK_AUDIO;
	}

	return VO_ERR_NONE;
}

void CXAudio2Render::OnVoiceProcessingPassStart(UINT32 /*bytesRequired*/)
{
}
void CXAudio2Render::OnVoiceProcessingPassEnd()
{
}
void CXAudio2Render::OnStreamEnd()
{
}
void CXAudio2Render::OnBufferStart(void* bufferContext)
{
   AudioBufInfo * pAudioInfo = (AudioBufInfo *)bufferContext;
   mllMediaTime = pAudioInfo->llTimeStamp;
}
void CXAudio2Render::OnBufferEnd(void* bufferContext)
{
	AudioBufInfo * pAudioInfo = (AudioBufInfo *)bufferContext;
	mFreeList.AddTail(pAudioInfo);
	mcSemaphore.Up();
}

void CXAudio2Render::OnLoopEnd(void* /*bufferContext*/)
{
}
void CXAudio2Render::OnVoiceError(void* /*bufferContext*/, HRESULT /*error*/)
{
}
