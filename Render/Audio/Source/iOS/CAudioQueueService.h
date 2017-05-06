/*
 *  CAudioQueueService.h
 *  vompEngn
 *
 *  Created by Lin Jun on 12/28/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CWaveOutAudio.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAudioQueueService : public CWaveOutAudio
{
public:
	CAudioQueueService (void);
	virtual ~CAudioQueueService (void);
	
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
	
	unsigned long		timeGetTime();
	
	AudioStreamPacketDescription aspd;
};

#ifdef _VONAMESPACE
}
#endif