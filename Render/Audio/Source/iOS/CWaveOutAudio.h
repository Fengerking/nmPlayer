#ifndef __CWaveOutAudio_H__
#define __CWaveOutAudio_H__

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <CoreFoundation/CoreFoundation.h>
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#ifdef USE_HW_AUDIO_DEC
#define MAXINPUTBUFFERS		60
#else
#define MAXINPUTBUFFERS		8
#endif

enum 
{
	INPUT_PCM	= 0,
	INPUT_AAC,
	INPUT_MP3
};

class CWaveOutAudio
{
public:
	static void AQBufferCallback(void * inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inCompleteAQBuffer);	
	//static void IsRunningProc(void * inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID);

public:
	CWaveOutAudio (void);
	virtual ~CWaveOutAudio (void);

	virtual bool SetInputFormat(int nFormat);
	virtual bool SetAudioFormat (int nSampleRate, int nChannels, int nBits);

	virtual	bool InitDevice (void);
	virtual bool CloseDevice (void);

	virtual bool Start (void);
	virtual bool Stop (void);
	virtual bool Pause(void);

	virtual bool Flush (void);

	virtual int	 Render (unsigned char * pData, int nSize, unsigned int nStart, unsigned int nEnd, bool bWait);

	// get play and buffer time 
	virtual int	GetPlayTime (void);
	virtual int	GetBufferTime (void);
	virtual int GetPlayingTime(void);

protected:
	virtual void		AudioDone (AudioQueueBufferRef inCompleteAQBuffer);

protected:
	voCMutex				m_csAQ;
	AudioQueueRef           m_AudioQueue;
	
	voCMutex                m_csAQBuffers;
	AudioQueueBufferRef     m_EmptyBuffers[MAXINPUTBUFFERS];
	int                     m_nEmptyBufferCount;
	int						m_nBufSize;
	
	AudioStreamBasicDescription m_AudioFormat;
	int                         m_nAvgBytesPerSec;

	bool                    m_bIsInitialized;
	bool					m_bIsRunning;
	bool					m_bIsFlushing;

	voCMutex				m_csTime;
	unsigned long			m_dwSysTime;
	int						m_nPlayingTime;
	int						m_nBufferTime;
	int						m_nInputFormat;
	bool					m_bPaused;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __CWaveOutAudio_H__