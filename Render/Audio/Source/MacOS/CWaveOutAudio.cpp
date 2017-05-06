#include "CWaveOutAudio.h"
#include <sys/time.h>
#include <unistd.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
    
unsigned long timeGetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}

void CWaveOutAudio::AQBufferCallback(void * inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inCompleteAQBuffer)
{
	CWaveOutAudio * pWaveOutAudio = (CWaveOutAudio *)inUserData;
	pWaveOutAudio->AudioDone(inCompleteAQBuffer);
}
/*
void CWaveOutAudio::IsRunningProc(void * inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID)
{
	CWaveOutAudio * pWaveOutAudio = (CWaveOutAudio *)inUserData;
	UInt32 nSize = sizeof(pWaveOutAudio->m_bIsRunning);
	OSStatus ret = AudioQueueGetProperty(inAQ, kAudioQueueProperty_IsRunning, &pWaveOutAudio->m_bIsRunning, &nSize);
	if(ret != noErr)
		return;
}
*/
CWaveOutAudio::CWaveOutAudio(void)
: m_AudioQueue (NULL)
, m_nBufSize (0)
, m_bIsInitialized (false)
, m_bIsRunning (false)
, m_bIsFlushing (false)
, m_dwSysTime (0)
, m_nPlayingTime (0)
, m_nBufferTime (0)
, m_bPaused(false)
{
	memset(m_EmptyBuffers, 0, sizeof(m_EmptyBuffers));
	m_nEmptyBufferCount = 0;
}

CWaveOutAudio::~CWaveOutAudio ()
{
	CloseDevice();
}

bool CWaveOutAudio::SetAudioFormat (int nSampleRate, int nChannels, int nBits)
{
	memset(&m_AudioFormat, 0, sizeof(AudioStreamBasicDescription));
	m_nAvgBytesPerSec = 0;
	
    m_AudioFormat.mSampleRate       = nSampleRate; 
    m_AudioFormat.mFormatID         = kAudioFormatLinearPCM;
    m_AudioFormat.mFormatFlags      = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_AudioFormat.mBytesPerPacket   = (nBits / 8) * nChannels;
    m_AudioFormat.mFramesPerPacket  = 1;
    m_AudioFormat.mBytesPerFrame    = (nBits / 8) * nChannels;
    m_AudioFormat.mChannelsPerFrame = nChannels;
    m_AudioFormat.mBitsPerChannel   = nBits;
    m_AudioFormat.mReserved         = 0;
	
	m_nAvgBytesPerSec = nSampleRate * nChannels * nBits / 8;

	if (m_AudioFormat.mSampleRate <= 8000)
		m_AudioFormat.mSampleRate = 8000;
	else if (m_AudioFormat.mSampleRate <= 11025)
		m_AudioFormat.mSampleRate = 11025;
	else if (m_AudioFormat.mSampleRate <= 12000)
		m_AudioFormat.mSampleRate = 12000;
	else if (m_AudioFormat.mSampleRate <= 16000)
		m_AudioFormat.mSampleRate = 16000;
	else if (m_AudioFormat.mSampleRate <= 22050)
		m_AudioFormat.mSampleRate = 22050;
	else if (m_AudioFormat.mSampleRate <= 24000)
		m_AudioFormat.mSampleRate = 24000;
	else if (m_AudioFormat.mSampleRate <= 32000)
		m_AudioFormat.mSampleRate = 32000;
	else if (m_AudioFormat.mSampleRate <= 44100)
		m_AudioFormat.mSampleRate = 44100;
	else if (m_AudioFormat.mSampleRate <= 48000)
		m_AudioFormat.mSampleRate = 48000;

	if (m_AudioFormat.mChannelsPerFrame > 2)
		m_AudioFormat.mChannelsPerFrame = 2;
	else if ( m_AudioFormat.mChannelsPerFrame <= 0)
		m_AudioFormat.mChannelsPerFrame = 1;

	return true;
}

bool CWaveOutAudio::InitDevice (void)
{
	if (!CloseDevice ())
		return false;

	OSStatus ret = AudioQueueNewOutput(&m_AudioFormat, AQBufferCallback, this, NULL, kCFRunLoopCommonModes, 0, &m_AudioQueue); //CFRunLoopGetCurrent()
	if(ret != noErr)
		return false;
	
	m_nBufSize = m_nAvgBytesPerSec / 4; // 1/4 second
	
    //AudioQueueAddPropertyListener(m_AudioQueue, kAudioQueueProperty_IsRunning, IsRunningProc, this);
	
	for(int i=0; i<MAXINPUTBUFFERS; ++i)
	{
		AudioQueueAllocateBuffer(m_AudioQueue, m_nBufSize, &m_EmptyBuffers[i]);
	}
	m_nEmptyBufferCount = MAXINPUTBUFFERS;
	
	//AudioQueueSetParameter(m_AudioQueue, kAudioQueueParam_Volume, 1.0);
	
	m_bIsInitialized = true;

	//Start ();

	return true;
}

bool CWaveOutAudio::CloseDevice (void)
{
	Stop();
	
	if(m_AudioQueue != NULL)
	{
		AudioQueueDispose(m_AudioQueue, true);
		m_AudioQueue = NULL;
	}
	
	m_bIsInitialized = false;
	return true;
}

bool CWaveOutAudio::Start (void)
{
	if(m_AudioQueue == NULL)
		return false;
	
	if(m_bIsRunning)
		return true;

	m_bIsRunning = true;

	m_nPlayingTime = 0;
	
	if(!m_bPaused)
		m_nBufferTime = 0;
	else
	{
		m_bPaused = false;
	}

	m_dwSysTime = 0;

	OSStatus ret = AudioQueueStart(m_AudioQueue, NULL);
	if(ret != noErr)
		return false;
	
	return true;
}

bool CWaveOutAudio::Stop (void)
{	
	if(m_AudioQueue == NULL)
		return true;

	if(!m_bIsRunning)
		return true;
	
	m_bIsRunning = false;
	m_bIsFlushing = false;
	
	OSStatus ret = AudioQueueStop(m_AudioQueue, true);
	if(ret != noErr)
		return false;
	
	return true;
}

bool CWaveOutAudio::Pause (void)
{	
	//return Stop();
	
	if(m_AudioQueue == NULL)
		return true;
	
	if(!m_bIsRunning)
		return true;
	
	m_bPaused = true;
	
	m_bIsRunning = false;
	m_bIsFlushing = false;
	
	OSStatus ret = AudioQueuePause(m_AudioQueue);
	if(ret != noErr)
		return false;
	
	return true;
}

bool CWaveOutAudio::Flush (void)
{
//	m_bIsFlushing = true;
//	AudioQueueFlush(m_AudioQueue);

	AudioQueueReset(m_AudioQueue);
	
	m_nPlayingTime = 0;
	m_nBufferTime = 0;
	m_dwSysTime = 0;

	return true;
}

int CWaveOutAudio::Render (unsigned char * pData, int nSize, unsigned int nStart, unsigned int nEnd, bool bWait)
{
	if (!m_bIsRunning)
		return -1;
	
	//printf("BufferTime = %d    EmptyBufferCount = %d \n", m_nBufferTime, m_nEmptyBufferCount);
	int nTry = 0;
	while (m_nEmptyBufferCount == 0) 
	{
		usleep (1000 * 10);
		nTry++;
		if (nTry > 50)
		{
			printf("Audio render drop frame, ts = %d\n", nStart);
			printf("BufferTime = %d    EmptyBufferCount = %d \n", m_nBufferTime, m_nEmptyBufferCount);			
			return -1;
		}
	}
	
	/*
	if(m_nEmptyBufferCount == 0)
	{

		return -1;
	}
	 */
		
	
	AudioQueueBufferRef pAQBuffer = NULL;
	{
		voCAutoLock lock (&m_csAQBuffers);
		--m_nEmptyBufferCount;
		pAQBuffer = m_EmptyBuffers[m_nEmptyBufferCount];
		m_EmptyBuffers[m_nEmptyBufferCount] = NULL;
	}

	if (m_nBufSize < nSize)
	{
		nSize = m_nBufSize;
	}

	pAQBuffer->mUserData = (void *)nStart;
	memcpy (pAQBuffer->mAudioData, pData, nSize);
	pAQBuffer->mAudioDataByteSize = nSize;

	AudioQueueEnqueueBuffer(m_AudioQueue, pAQBuffer, 0, NULL);
	
	voCAutoLock lockTime (&m_csTime);
	m_nBufferTime = m_nBufferTime + nSize * 1000 / m_nAvgBytesPerSec;

	return true;
}

int CWaveOutAudio::GetPlayingTime(void)
{
	return m_nPlayingTime;
}

int CWaveOutAudio::GetPlayTime (void)
{
	voCAutoLock lockTime (&m_csTime);

//	if(m_nEmptyBufferCount == MAXINPUTBUFFERS)
//		return m_nPlayingTime;
	
	if(m_nPlayingTime == 0)
		return 0;
	
	if (m_dwSysTime == 0)
		m_dwSysTime = timeGetTime ();

	unsigned long dwPlayTime = m_nPlayingTime + (timeGetTime () - m_dwSysTime);
	return dwPlayTime;
}

int CWaveOutAudio::GetBufferTime (void)
{
	return m_nBufferTime;
}

void CWaveOutAudio::AudioDone (AudioQueueBufferRef inCompleteAQBuffer)
{
	voCAutoLock lockTime (&m_csTime);

	m_nPlayingTime = m_nPlayingTime + inCompleteAQBuffer->mAudioDataByteSize * 1000 / m_nAvgBytesPerSec;
	m_dwSysTime = timeGetTime ();

	m_nBufferTime = m_nBufferTime - inCompleteAQBuffer->mAudioDataByteSize * 1000 / m_nAvgBytesPerSec;

	voCAutoLock lockList (&m_csAQBuffers);
	m_EmptyBuffers[m_nEmptyBufferCount] = inCompleteAQBuffer;
	++m_nEmptyBufferCount;
	
	return;
}
