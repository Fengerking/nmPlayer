	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voOSFunc.h"
#include "CWaveOutRender.h"

#define LOG_TAG "CWaveOutRender"
#include "voLog.h"

#define MAXINPUTBUFFERS		3

// we need use global lock, for some devices, waveOutXXX API can't be called at same time even different HWAVEOUT
// found when debugging FireFox browser plug-in two pages, waveOutReset hang issue, East, 20130321
static voCMutex	m_csWaveOut;

CWaveOutRender::CWaveOutRender(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseAudioRender (hInst, pMemOP)
	, m_hWaveOut (NULL)
	, m_nBufSize (0)
	, m_status (VOMM_AR_LOADED)
	, m_bFlush (false)
	, m_nPlayingTime (0)
	, m_nBufferTime (0)
	, m_dwSysTime (0)
	, mCurWaveHdr(0)
{
	m_pMemOP->Set (VO_INDEX_SNK_AUDIO, &m_wavFormat, 0, sizeof (WAVEFORMATEX));

	m_memInfo.Size = sizeof (WAVEHDR);
	WAVEHDR * pWaveHeader = NULL;
	for (int i = 0; i < MAXINPUTBUFFERS; i++)
	{
		m_pMemOP->Alloc (VO_INDEX_SNK_AUDIO, &m_memInfo);
		pWaveHeader = (WAVEHDR *)m_memInfo.VBuffer;
		m_pMemOP->Set (VO_INDEX_SNK_AUDIO, pWaveHeader, 0, sizeof (WAVEHDR));

		m_lstFree.AddTail (pWaveHeader);
	}
}

CWaveOutRender::~CWaveOutRender ()
{
	ReleaseBuffer();
	CloseDevice();

	WAVEHDR * pWaveHeader = NULL;
	while (m_lstFree.GetCount () > 0)
	{
		pWaveHeader = m_lstFree.RemoveHead ();
		m_pMemOP->Free (VO_INDEX_SNK_AUDIO, pWaveHeader);
	}
}

VO_U32 CWaveOutRender::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if(pFormat->Channels == m_wavFormat.nChannels && pFormat->SampleBits == m_wavFormat.wBitsPerSample && pFormat->SampleRate == m_wavFormat.nSamplesPerSec)
		return VO_ERR_NONE;

	m_pMemOP->Set (VO_INDEX_SNK_AUDIO, &m_wavFormat, 0, sizeof (WAVEFORMATEX));
		
	//cbSize(extra information size) should be 0!!
//	m_wavFormat.cbSize = sizeof (WAVEFORMATEX);
	m_wavFormat.nSamplesPerSec = pFormat->SampleRate;
	m_wavFormat.nChannels = (WORD)pFormat->Channels;
	m_wavFormat.wBitsPerSample = (WORD)pFormat->SampleBits;
	m_wavFormat.nBlockAlign = (WORD)(pFormat->Channels * pFormat->SampleBits / 8);
	m_wavFormat.nAvgBytesPerSec = pFormat->SampleRate * m_wavFormat.nBlockAlign;
	m_wavFormat.wFormatTag = 1;

	if (m_wavFormat.nSamplesPerSec <= 8000)
		m_wavFormat.nSamplesPerSec = 8000;
	else if (m_wavFormat.nSamplesPerSec <= 11025)
		m_wavFormat.nSamplesPerSec = 11025;
	else if (m_wavFormat.nSamplesPerSec <= 12000)
		m_wavFormat.nSamplesPerSec = 12000;
	else if (m_wavFormat.nSamplesPerSec <= 16000)
		m_wavFormat.nSamplesPerSec = 16000;
	else if (m_wavFormat.nSamplesPerSec <= 22050)
		m_wavFormat.nSamplesPerSec = 22050;
	else if (m_wavFormat.nSamplesPerSec <= 24000)
		m_wavFormat.nSamplesPerSec = 24000;
	else if (m_wavFormat.nSamplesPerSec <= 32000)
		m_wavFormat.nSamplesPerSec = 32000;
	else if (m_wavFormat.nSamplesPerSec <= 44100)
		m_wavFormat.nSamplesPerSec = 44100;
	else if (m_wavFormat.nSamplesPerSec <= 48000)
		m_wavFormat.nSamplesPerSec = 48000;

	if (m_wavFormat.nChannels > 2)
		m_wavFormat.nChannels = 2;
	else if (m_wavFormat.nChannels <= 0)
		m_wavFormat.nChannels = 1;

	VOMM_AR_STATUS status = m_status;

	if (!InitDevice ())
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (status == VOMM_AR_RUNNING)
		Start ();

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::Start (void)
{
	if (m_hWaveOut == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOMM_AR_RUNNING)
		return VO_ERR_NONE;

	if (m_status != VOMM_AR_PAUSED)
	{
		m_nPlayingTime = 0;
		m_nBufferTime = 0;
	}
	m_dwSysTime = 0;

	m_csWaveOut.Lock();
	MMRESULT mr = ::waveOutRestart (m_hWaveOut);
	m_csWaveOut.Unlock();

	m_status = VOMM_AR_RUNNING;

	if (mr != MMSYSERR_NOERROR)
	{
		VOLOGE("failed to waveOutRestart 0x%08X", mr);
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
	}

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::Pause (void)
{
	if (m_hWaveOut == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOMM_AR_PAUSED)
		return VO_ERR_NONE;

	if (m_status != VOMM_AR_RUNNING)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	m_csWaveOut.Lock();
	MMRESULT mr = ::waveOutPause (m_hWaveOut);
	m_csWaveOut.Unlock();

	m_status = VOMM_AR_PAUSED;

	if (mr != MMSYSERR_NOERROR)
	{
		VOLOGE("failed to waveOutPause 0x%08X", mr);
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
	}

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::Stop (void)
{
	if (m_hWaveOut == NULL)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	if (m_status == VOMM_AR_STOPPED)
		return VO_ERR_NONE;

	m_csWaveOut.Lock();
	MMRESULT mr = ::waveOutReset (m_hWaveOut);
	m_csWaveOut.Unlock();

	m_nPlayingTime = 0;
	m_nBufferTime = 0;

	m_status = VOMM_AR_STOPPED;

	if (mr != MMSYSERR_NOERROR)
	{
		VOLOGE("failed to waveOutReset 0x%08X", mr);
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;
	}

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
	if (m_status != VOMM_AR_RUNNING || m_bFlush)
		return VO_ERR_WRONG_STATUS | VO_INDEX_SNK_AUDIO;

	WAVEHDR * pWaveHeader = NULL;

	VO_PBYTE	pDataBuff = pBuffer;
	VO_U32		nDataSize = nSize;

	while (true)
	{
		int nTryTimes = 0;
		while (mCurWaveHdr == NULL && m_lstFree.GetCount () <= 0)
		{
			Sleep (10);
			nTryTimes++;
			if (nTryTimes > 50)
				return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
		}

		if(mCurWaveHdr == NULL)
		{
			voCAutoLock lock (&m_csList);
			mCurWaveHdr = m_lstFree.RemoveHead ();
			mCurWaveHdr->dwBufferLength = 0;
		}

		if (mCurWaveHdr->dwBufferLength + nDataSize > m_nBufSize)
		{
			int nCopySize = m_nBufSize - mCurWaveHdr->dwBufferLength;
			memcpy (mCurWaveHdr->lpData + mCurWaveHdr->dwBufferLength , pDataBuff, nCopySize);
			mCurWaveHdr->dwBufferLength = m_nBufSize;

			nDataSize -= nCopySize; 
			pDataBuff += nCopySize;

			mCurWaveHdr->dwUser = (DWORD_PTR)nStart;
		}
		else
		{
			memcpy (mCurWaveHdr->lpData + mCurWaveHdr->dwBufferLength , pDataBuff, nDataSize);
			mCurWaveHdr->dwBufferLength += nDataSize;

			break;
		}

		{
			voCAutoLock lockTime (&m_csTime);
			m_nBufferTime = m_nBufferTime + m_nBufSize * 1000 / m_wavFormat.nAvgBytesPerSec;
		}

		{
			voCAutoLock lockList (&m_csList);
			m_lstFull.AddTail(mCurWaveHdr);
		}

		m_csWaveOut.Lock();
		MMRESULT mr = ::waveOutWrite (m_hWaveOut, mCurWaveHdr, sizeof (WAVEHDR));
		m_csWaveOut.Unlock();

		if (mr != MMSYSERR_NOERROR)
		{
			VOLOGE("failed to waveOutWrite 0x%08X", mr);
		}

		{
			voCAutoLock lockList (&m_csList);
			mCurWaveHdr = NULL;
		}

		nStart = nStart + m_nBufSize * 1000 / m_wavFormat.nAvgBytesPerSec;
	}

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::Flush (void)
{
	m_bFlush = true;

	// flush the data in wave out, East 20130911
	m_csWaveOut.Lock();
	MMRESULT mr = ::waveOutReset (m_hWaveOut);
	m_csWaveOut.Unlock();

	VOMM_AR_STATUS ePreStatus = m_status;
	if(VOMM_AR_PAUSED == ePreStatus)
		Start();

	if(mCurWaveHdr != NULL)
	{
		voCAutoLock lockList (&m_csList);
		m_lstFree.AddTail (mCurWaveHdr);
		mCurWaveHdr = NULL;
	}

	//while (m_lstFree.GetCount () < MAXINPUTBUFFERS)
	//	Sleep (2);

	m_nPlayingTime = 0;
	m_nBufferTime = 0;
	m_dwSysTime = 0;

	m_bFlush = false;

	if(VOMM_AR_PAUSED == ePreStatus)
		Pause();
	else if(VOMM_AR_RUNNING == ePreStatus)
		Start();

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	voCAutoLock lockTime (&m_csTime);

	if (m_nPlayingTime == 0)
		*pPlayingTime = 0;

	if (m_dwSysTime == 0)
		m_dwSysTime = GetTickCount ();

	*pPlayingTime = m_nPlayingTime + (GetTickCount () - m_dwSysTime);

	return VO_ERR_NONE;
}

VO_U32 CWaveOutRender::GetBufferTime (VO_S32 * pBufferTime)
{
	*pBufferTime =  m_nBufferTime;

	if (*pBufferTime  < 0)
		*pBufferTime  = 0;

	return VO_ERR_NONE;
}

bool CWaveOutRender::AllocBuffer (void)
{
	ReleaseBuffer ();

	voCAutoLock lock (&m_csList);

	m_nBufSize = m_wavFormat.nAvgBytesPerSec / 6;
	//must keep m_nBufSize multiple of nBlockAlign!!
	//East, 2009/09/25
	m_nBufSize = (m_nBufSize + m_wavFormat.nBlockAlign - 1) / m_wavFormat.nBlockAlign * m_wavFormat.nBlockAlign;
	m_memInfo.Size = m_nBufSize;

	WAVEHDR * pWaveHeader = NULL;
	POSITION pos = m_lstFree.GetHeadPosition ();
	while (pos != NULL)
	{
		m_pMemOP->Alloc (VO_INDEX_SNK_AUDIO, &m_memInfo);
		if (m_memInfo.VBuffer == NULL)
			return false;

		m_pMemOP->Set (VO_INDEX_SNK_AUDIO, m_memInfo.VBuffer, 0, m_memInfo.Size);

		pWaveHeader = m_lstFree.GetNext (pos);
		if(pWaveHeader->lpData != NULL)
		{
			m_pMemOP->Free(VO_INDEX_SNK_AUDIO , pWaveHeader->lpData);
		}
		pWaveHeader->lpData = (char *) m_memInfo.VBuffer;
		pWaveHeader->dwBufferLength = m_nBufSize;

#ifdef _WIN32_WCE
		pWaveHeader->dwFlags = 0;
#else
		pWaveHeader->dwFlags = WHDR_ENDLOOP;//WHDR_INQUEUE | WHDR_DONE;
#endif // _WIN32_WCE
		pWaveHeader->reserved = 0;

		m_csWaveOut.Lock();
		MMRESULT mr = ::waveOutPrepareHeader (m_hWaveOut, pWaveHeader, sizeof (WAVEHDR));
		m_csWaveOut.Unlock();
		if (mr != MMSYSERR_NOERROR)
		{
			VOLOGE("failed to waveOutPrepareHeader 0x%08X", mr);
		}
	}

	return true;
}

bool CWaveOutRender::ReleaseBuffer (void)
{
	m_csList.Lock();
	if(mCurWaveHdr != NULL)
	{
		m_lstFree.AddTail (mCurWaveHdr);
		mCurWaveHdr = NULL;
	}
	m_csList.Unlock();

	// wait all buffers returned by audio driver, to avoid wdmaud.drv crash, East 20130319
	while(true)
	{
		m_csList.Lock();
		if(m_lstFull.GetCount() == 0)
		{
			m_csList.Unlock();
			break;
		}
		m_csList.Unlock();

		VOLOGI("%d buffer 0x%08X still in audio driver", m_lstFull.GetCount(), m_lstFull.GetHead());
		voOS_Sleep(2);
	}

	voCAutoLock lock (&m_csList);
	WAVEHDR * pWaveHeader = NULL;
	POSITION pos = m_lstFree.GetHeadPosition ();
	while (pos != NULL)
	{
		pWaveHeader = m_lstFree.GetNext (pos);
		if (pWaveHeader->lpData != NULL)
		{
			m_csWaveOut.Lock();
			MMRESULT mr = ::waveOutUnprepareHeader(m_hWaveOut, pWaveHeader, sizeof (WAVEHDR));
			m_csWaveOut.Unlock();
			if (mr != MMSYSERR_NOERROR)
			{
				VOLOGE("failed to waveOutUnprepareHeader 0x%08X", mr);
			}
			m_pMemOP->Free (VO_INDEX_SNK_AUDIO, pWaveHeader->lpData);
			pWaveHeader->lpData = NULL;
		}
		m_pMemOP->Set (VO_INDEX_SNK_AUDIO, pWaveHeader, 0, sizeof(WAVEHDR));
	}

	return true;
}

bool CWaveOutRender::InitDevice (void)
{
	if (!CloseDevice ())
		return false;

	m_csWaveOut.Lock();
	MMRESULT mr = ::waveOutOpen (&m_hWaveOut, WAVE_MAPPER, &m_wavFormat, (DWORD)VoiceWaveOutProc, (DWORD)this, CALLBACK_FUNCTION);
	m_csWaveOut.Unlock();
	if (mr != MMSYSERR_NOERROR)
	{
		VOLOGE("failed to waveOutOpen 0x%08X", mr);

		m_hWaveOut = NULL;
		return false;
	}

	if (!AllocBuffer ())
		return false;

	return true;
}

bool CWaveOutRender::CloseDevice (void)
{
	if (m_hWaveOut == NULL)
		return true;

	if (Stop () != VO_ERR_NONE)
		return false;

	MMRESULT mr = MMSYSERR_NOERROR;
	do 
	{
		m_csWaveOut.Lock();
		mr = ::waveOutClose (m_hWaveOut);
		m_csWaveOut.Unlock();
		if(mr == WAVERR_STILLPLAYING)
			Sleep(10);
	} while (mr == WAVERR_STILLPLAYING);
	
	m_hWaveOut = NULL;

	if (mr != MMSYSERR_NOERROR)
	{
		VOLOGE("failed to waveOutClose 0x%08X", mr);
		return false;
	}
	
	return true;
}

bool CWaveOutRender::AudioDone (WAVEHDR * pWaveHeader)
{
	{
		voCAutoLock lockTime (&m_csTime);

		m_nPlayingTime = pWaveHeader->dwUser;
		m_nPlayingTime = m_nPlayingTime + pWaveHeader->dwBufferLength * 1000 / m_wavFormat.nAvgBytesPerSec;
		m_dwSysTime = GetTickCount ();

		m_nBufferTime = m_nBufferTime - pWaveHeader->dwBufferLength * 1000 / m_wavFormat.nAvgBytesPerSec;
	}

	{
		voCAutoLock lockList (&m_csList);
		if(m_lstFull.GetCount() > 0)
		{
			m_lstFull.RemoveHead ();
			m_lstFree.AddTail (pWaveHeader);
		}
		
	}

	return true;
}


bool CALLBACK CWaveOutRender::VoiceWaveOutProc(HWAVEOUT hwo, UINT uMsg,  DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	CWaveOutRender * WaveOut = (CWaveOutRender *)dwInstance;

	switch (uMsg)
	{
	case WOM_CLOSE:
		break;

	case WOM_OPEN:
		break;

	case WOM_DONE:
		WaveOut->AudioDone ((WAVEHDR *)dwParam1);
		break;

	default:
		break;
	}

	return true;
}
