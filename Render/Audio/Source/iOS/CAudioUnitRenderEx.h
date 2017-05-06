/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved
 
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
 * @file CAudioUnitRenderEx.h
 * Audio unit
 *
 * Audio unit
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __CAUDIO_UNIT_EX_RENDER__
#define __CAUDIO_UNIT_EX_RENDER__

#import <AudioUnit/AudioUnit.h>
#include "CBaseAudioRender.h"

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <CoreFoundation/CoreFoundation.h>
#include <list>
#include "voRunRequestOnMain.h"

#include "voNSRecursiveLock.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAudioUnitRenderEx : public CBaseAudioRender, private voRunRequestOnMain
{	
public:
	
	CAudioUnitRenderEx (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioUnitRenderEx (void);
    
	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
    
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);
	
	virtual VO_U32		SetInputFormat(int nFormat);
	
    virtual VO_U32      AudioInterruption(VO_U32 inInterruptionState);
    
    virtual VO_U32      SetVolume(float leftVolume, float rightVolume);
protected:
    
    virtual bool        IsFormatChange(VO_AUDIO_FORMAT * pFormat);
    
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
    
    virtual void RunningRequestOnMain(int nID, void *pParam1, void *pParam2);
    
    virtual void StopAudioUnit();
	
    virtual VO_U32 EffectAudioSample(VO_PBYTE pBuffer, VO_S32 nSize);
    
protected:
    
    float                   m_fLeftVolume;
	float                   m_fRightVolume;
    
    bool                    m_bInit;
	
	bool					m_bPaused;
	bool					m_bIsRunning;
	
	int						m_nInputFormat;
	
    voNSRecursiveLock       m_cLockUnit;
    bool                    m_cAudioUnitWorking;
    
	AudioComponentInstance	m_cAudioUnit;
	
	AudioStreamBasicDescription m_AudioFormat;
    
    VO_CODECBUFFER          m_cBuffer;
    VO_AUDIO_FORMAT         m_cFormat;
    
    // iOS limitation: Sometime can't start success when try start after stop immediately
    VO_U64                  m_nLastStopTime;
};

#ifdef _VONAMESPACE
}
#endif

#endif