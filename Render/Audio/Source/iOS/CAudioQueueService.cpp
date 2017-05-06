/*
 *  CAudioQueueService.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 12/28/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CAudioQueueService.h"

#include "CWaveOutAudio.h"
#include <sys/time.h>
#include <unistd.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//static int start = 0;

unsigned long CAudioQueueService::timeGetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}


CAudioQueueService::CAudioQueueService(void)
{
	memset(m_EmptyBuffers, 0, sizeof(m_EmptyBuffers));
	m_nEmptyBufferCount = 0;
}

CAudioQueueService::~CAudioQueueService (void)
{
	CloseDevice();
}


bool CAudioQueueService::SetAudioFormat (int nSampleRate, int nChannels, int nBits)
{
	memset(&m_AudioFormat, 0, sizeof(AudioStreamBasicDescription));
	m_nAvgBytesPerSec = 0;
	
	if(m_nInputFormat == kAudioFormatMPEG4AAC)
	{
		m_AudioFormat.mSampleRate       = nSampleRate; 
		m_AudioFormat.mFormatID         = m_nInputFormat;// 1633772320;
		m_AudioFormat.mFormatFlags      = 0;//kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;// 0
		m_AudioFormat.mBytesPerPacket   = 0;//(nBits / 8) * nChannels;
		m_AudioFormat.mFramesPerPacket  = 1024;//1;
		m_AudioFormat.mBytesPerFrame    = 0;//(nBits / 8) * nChannels;
		m_AudioFormat.mChannelsPerFrame = nChannels;
		m_AudioFormat.mBitsPerChannel   = 0;//nBits;
		m_AudioFormat.mReserved         = 0;
	}
	else if(m_nInputFormat == kAudioFormatMPEGLayer3)
	{
		m_AudioFormat.mSampleRate       = nSampleRate; 
		m_AudioFormat.mFormatID         = m_nInputFormat;
		m_AudioFormat.mFormatFlags      = 0;//
		m_AudioFormat.mBytesPerPacket   = 0;	//To indicate variable packet size, set this field to 0. 
		
		//For uncompressed audio, the value is 1. For variable bit-rate formats, 
		//the value is a larger fixed number, such as 1024 for AAC. For formats 
		//with a variable number of frames per packet, such as Ogg Vorbis, set this field to 0.
		m_AudioFormat.mFramesPerPacket  = 576;	
		
		m_AudioFormat.mBytesPerFrame    = 0;	//Set this field to 0 for compressed formats. 
		m_AudioFormat.mChannelsPerFrame = nChannels;
		m_AudioFormat.mBitsPerChannel   = 0;	//Set this field to 0 for compressed formats.
		m_AudioFormat.mReserved         = 0;
	}
	
	
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
	
	// tag: 20100926
	//InitDevice ();
	
	return true;
}

bool CAudioQueueService::InitDevice (void)
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

bool CAudioQueueService::CloseDevice (void)
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

bool CAudioQueueService::Start (void)
{
	if(m_AudioQueue == NULL)
		return false;
	
	if(m_bIsRunning)
		return true;
	
	m_bIsRunning = true;
	
	m_nPlayingTime = 0;
	m_nBufferTime = 0;
	m_dwSysTime = 0;
	
	OSStatus ret = AudioQueueStart(m_AudioQueue, NULL);
	if(ret != noErr)
	{
		return false;
	}
	else {
		printf("Audio Queue start success...\n");
	}

	
	return true;
}

bool CAudioQueueService::Stop (void)
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
	
	m_nEmptyBufferCount = MAXINPUTBUFFERS;
	
	return true;
}

bool CAudioQueueService::Pause (void)
{	
	if(m_AudioQueue == NULL)
		return true;
	
	if(!m_bIsRunning)
		return true;
	
	m_bIsRunning = false;
	m_bIsFlushing = false;
	
	//return Stop();
	
	 OSStatus ret = AudioQueuePause(m_AudioQueue);
	 if(ret != noErr)
		 return false;
	 
	 return true;
}

bool CAudioQueueService::Flush (void)
{
	//	m_bIsFlushing = true;
	//	AudioQueueFlush(m_AudioQueue);
	
	AudioQueueReset(m_AudioQueue);
	
	m_nPlayingTime = 0;
	m_nBufferTime = 0;
	m_dwSysTime = 0;
	
	return true;
}


int CAudioQueueService::Render (unsigned char * pData, int nSize, unsigned int nStart, unsigned int nEnd, bool bWait)
{
	if (!m_bIsRunning)
		return -1;
	
	// tag: 20110105
	//if(nSize == 12)
		//return -1;
	
	//printf("render size = %d, time = %d, %02x %02x %02x %02x %02x %02x\n", nSize, nStart, pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
	
	//printf("BufferTime = %d    EmptyBufferCount = %d \n", m_nBufferTime, m_nEmptyBufferCount);
	int nTry = 0;
	while (m_nEmptyBufferCount == 0) 
	{
		usleep (1000 * 10);
		//printf("sleep 10ms\n");
		nTry++;
		if (nTry > 50)
		{
			printf("Audio render drop frame, ts = %d\n", nStart);
			//printf("BufferTime = %d    EmptyBufferCount = %d \n", m_nBufferTime, m_nEmptyBufferCount);			
			return -1;
		}
	}
	
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
	
	//int size = nSize;
	//printf("render size = %d, %02x %02x %02x %02x %02x %02x ----- %02x %02x %02x %02x %02x %02x\n", size, pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[size-6],pData[size-5],pData[size-4],pData[size-3],pData[size-2],pData[size-1]);
	
	OSStatus ret;
	if(INPUT_PCM == m_nInputFormat)
		ret = AudioQueueEnqueueBuffer(m_AudioQueue, pAQBuffer, 0, NULL);
	else
	{
		
		memset(&aspd, 0, sizeof(aspd));
		aspd.mDataByteSize = nSize;
		ret = AudioQueueEnqueueBuffer(m_AudioQueue, pAQBuffer, 1, &aspd);
	}
	
	if(ret != noErr)
	{
		printf("Enqueue buf failed...\n");
		return false;
	}
		
	
	voCAutoLock lockTime (&m_csTime);
	m_nBufferTime = m_nBufferTime + nSize * 1000 / m_nAvgBytesPerSec;
	
	return true;
}

int CAudioQueueService::GetPlayingTime(void)
{
	return m_nPlayingTime;
}

int CAudioQueueService::GetPlayTime (void)
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

int CAudioQueueService::GetBufferTime (void)
{
	return m_nBufferTime;
}

void CAudioQueueService::AudioDone (AudioQueueBufferRef inCompleteAQBuffer)
{
	voCAutoLock lockTime (&m_csTime);
	
	// tag:20100925
	//m_nPlayingTime = (unsigned int)inCompleteAQBuffer->mUserData;
	m_nPlayingTime = m_nPlayingTime + inCompleteAQBuffer->mAudioDataByteSize * 1000 / m_nAvgBytesPerSec;
	m_dwSysTime = timeGetTime ();
	
	m_nBufferTime = m_nBufferTime - inCompleteAQBuffer->mAudioDataByteSize * 1000 / m_nAvgBytesPerSec;
	
	voCAutoLock lockList (&m_csAQBuffers);
	m_EmptyBuffers[m_nEmptyBufferCount] = inCompleteAQBuffer;
	++m_nEmptyBufferCount;
	
	//printf("Audio done: empty buf count = %d\n", m_nEmptyBufferCount);
	
	/*
	static int t = 0;
	t = timeGetTime() - start;
	printf("time = %d\n", t);
	*/
	
	return;
}
