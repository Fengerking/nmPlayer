/*
 *  CAudioUnitRender.h
 *  Audio Render on iOS
 *
 *  Created by Jeff Huang on 07/05/2011.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef __CAUDIO_UNIT_RENDER__
#define __CAUDIO_UNIT_RENDER__

#import <AudioUnit/AudioUnit.h>
#include "CBaseAudioRender.h"

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <CoreFoundation/CoreFoundation.h>
#include <list>

#include "voNSRecursiveLock.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAudioUnitRender : public CBaseAudioRender
{	
public:
	
	CAudioUnitRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioUnitRender (void);
    
	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);
	
	virtual VO_U32		SetInputFormat(int nFormat);
	
    virtual VO_U32      SetVolume(float leftVolume, float rightVolume);
    
    virtual VO_U32      AudioInterruption(VO_U32 inInterruptionState);
    
protected:
    
    virtual VO_U32      EffectAudioSample(VO_PBYTE pBuffer, VO_S32 nSize);
    
    virtual VO_U32      RenderInner(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
    virtual VO_U32      TryKeepBuffer(VO_PBYTE pBuffer, VO_U32 nSize);
	
	static OSStatus S_RenderProcCallback(
                                         void *inRefCon,
                                         AudioUnitRenderActionFlags 	*ioActionFlags,
                                         const AudioTimeStamp 		*inTimeStamp,
                                         UInt32 						inBusNumber,
                                         UInt32 						inNumberFrames, 
                                         AudioBufferList 			*ioData);
    
	bool RenderProcCallback(
							AudioUnitRenderActionFlags 	*ioActionFlags, 
							const AudioTimeStamp 		*inTimeStamp, 
							UInt32 						inBusNumber, 
							UInt32 						inNumberFrames, 
							AudioBufferList 			*ioData);
	
	bool RenderProc(
							VO_U32						inReadedSize,
							AudioUnitRenderActionFlags 	*ioActionFlags, 
							const AudioTimeStamp 		*inTimeStamp, 
							UInt32 						inBusNumber, 
							UInt32 						inNumberFrames, 
							AudioBufferList 			*ioData);
	
	VO_U32 InitDevice (void);
	
	bool closeDevice();
    
    bool IsEqual(Float64 a, Float64 b);
	
protected:
	
	struct AudioUbuffer {
		VO_PBYTE pBuffer;
		VO_U32 nSize;
		VO_U64 nStart;
	};
	
    float                   m_fLeftVolume;
	float                   m_fRightVolume;
    
    bool					m_bNeedStartInRender;
	bool					m_bPaused;
	bool					m_bStoped;
	bool					m_bIsRunning;
	
	int						m_nInputFormat;
	int						m_nAvgBytesPerSec;
	int						m_nBufSize;
	
	int                     m_nUsedBufferCount;
	VO_U32					m_nLastReadPosition;
	VO_U32					m_nToSubtractTimeBufferSize;
	
    voNSRecursiveLock       m_cLastBuffersMutex;
	voNSRecursiveLock       m_csAQBuffers;
	std::list<AudioUbuffer *> m_cBufferList;
	
    VO_PBYTE                m_pLastBuffer;
    int                     m_nLastBufferSize;
    
    voNSRecursiveLock       m_cLockUnit;
    bool                    m_cAudioUnitStopSuccess;
    
	AudioComponentInstance	m_cAudioUnit;
	
	AudioStreamBasicDescription m_AudioFormat;
	
//	voNSRecursiveLock       m_csTime;
//	int						m_nPlayingTime;
//	int						m_nBufferTime;

    int						m_nBufferTimeEvaluate;
	int						m_nBufferTimeUnchange;
//	int						m_nLastBufferTime;
	
	static const int		MAX_BUFFER;
};

#ifdef _VONAMESPACE
}
#endif

#endif