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
 * @file CAudioUnitRenderEx.cpp
 * Audio unit
 *
 * Audio unit
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <AudioToolbox/AudioToolbox.h>

#include "voAudioRenderType.h"

#include "CAudioUnitRenderEx.h"
#include <sys/time.h>
#include <unistd.h>

#include "voLog.h"
#include "voOSFunc.h"

//#define DUMP_RENDER_LOG

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAudioUnitRenderEx::CAudioUnitRenderEx (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
:CBaseAudioRender(hInst, pMemOP)
, m_cAudioUnit(nil)
, m_fLeftVolume(1.0)
, m_fRightVolume(1.0)
, m_bIsRunning (false)
, m_nInputFormat(kAudioFormatLinearPCM)
, m_bPaused(false)
, m_cAudioUnitWorking(false)
, m_bInit(false)
, m_nLastStopTime(0)
{
    memset(&m_cFormat, 0, sizeof(m_cFormat));
	memset(&m_AudioFormat, 0, sizeof(AudioStreamBasicDescription));
}

CAudioUnitRenderEx::~CAudioUnitRenderEx (void)
{
    VO_U64 nTime = voOS_GetSysTime();
    
	closeDevice();
    
	VOLOGI("CAudioUnitRenderEx deconstruct finish use:%lld\n", voOS_GetSysTime() - nTime);
}

VO_U32 CAudioUnitRenderEx::SetInputFormat(int nFormat)
{
    m_nInputFormat = kAudioFormatLinearPCM;
	
	return VO_ERR_NONE;
}

bool CAudioUnitRenderEx::IsEqual(Float64 a, Float64 b)
{
    const static Float64 V_RANGE = 0.000001;
    if (((a - b) > -V_RANGE)
        && ((a - b) < V_RANGE) ) {
        return true;
    }
    return false;
}

bool CAudioUnitRenderEx::IsFormatChange(VO_AUDIO_FORMAT * pFormat)
{
    voNSAutoLock lock(&m_cLockUnit);
    
	if (NULL == pFormat) {
		return false;
	}
	
    AudioStreamBasicDescription formatTemp;
    memset(&formatTemp, 0, sizeof(AudioStreamBasicDescription));
    
    formatTemp.mSampleRate       = pFormat->SampleRate;
    formatTemp.mFormatID         = m_nInputFormat;
    formatTemp.mFormatFlags      = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    formatTemp.mBytesPerPacket   = (pFormat->SampleBits / 8) * pFormat->Channels;
    formatTemp.mFramesPerPacket  = 1;
    formatTemp.mBytesPerFrame    = (pFormat->SampleBits / 8) * pFormat->Channels;
    formatTemp.mChannelsPerFrame = pFormat->Channels;
    formatTemp.mBitsPerChannel   = pFormat->SampleBits;
    formatTemp.mReserved         = 0;
    
    if (formatTemp.mSampleRate <= 8000)
		formatTemp.mSampleRate = 8000;
	else if (formatTemp.mSampleRate <= 11025)
		formatTemp.mSampleRate = 11025;
	else if (formatTemp.mSampleRate <= 12000)
		formatTemp.mSampleRate = 12000;
	else if (formatTemp.mSampleRate <= 16000)
		formatTemp.mSampleRate = 16000;
	else if (formatTemp.mSampleRate <= 22050)
		formatTemp.mSampleRate = 22050;
	else if (formatTemp.mSampleRate <= 24000)
		formatTemp.mSampleRate = 24000;
	else if (formatTemp.mSampleRate <= 32000)
		formatTemp.mSampleRate = 32000;
	else if (formatTemp.mSampleRate <= 44100)
		formatTemp.mSampleRate = 44100;
	else if (formatTemp.mSampleRate <= 48000)
		formatTemp.mSampleRate = 48000;
	
	if (formatTemp.mChannelsPerFrame > 2)
		formatTemp.mChannelsPerFrame = 2;
	else if ( formatTemp.mChannelsPerFrame <= 0)
		formatTemp.mChannelsPerFrame = 1;
    
    if (IsEqual(formatTemp.mSampleRate, m_AudioFormat.mSampleRate)
        && (formatTemp.mFormatID == m_AudioFormat.mFormatID)
        && (formatTemp.mBytesPerPacket == m_AudioFormat.mBytesPerPacket)
        && (formatTemp.mBytesPerFrame == m_AudioFormat.mBytesPerFrame)
        && (formatTemp.mChannelsPerFrame == m_AudioFormat.mChannelsPerFrame)
        && (formatTemp.mBitsPerChannel == m_AudioFormat.mBitsPerChannel)
        ) {
        return false;
    }
	
    memcpy(&m_AudioFormat, &formatTemp, sizeof(AudioStreamBasicDescription));
    
    return true;
}

VO_U32 CAudioUnitRenderEx::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
    voNSAutoLock lock(&m_cLockUnit);
    
    if (IsFormatChange(pFormat)) {
         return InitDevice();
    }
    
    return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::InitDevice(void)
{
    voNSAutoLock lock (&m_cLockUnit);
    
    Flush();
    
    OSErr iErrCode = noErr;
    
    if (!m_bInit)
    {
        AudioComponentDescription acd;
        acd.componentType = kAudioUnitType_Output;
        acd.componentSubType = kAudioUnitSubType_RemoteIO;
        acd.componentManufacturer = kAudioUnitManufacturer_Apple;
        acd.componentFlags = 0;
        acd.componentFlagsMask = 0;
        
        // get output
        AudioComponent ac = AudioComponentFindNext(NULL, &acd);
        if (NULL == ac) {
            VOLOGE("Can't find default output \n");
            return VO_ERR_WRONG_STATUS;
        }
        
        // Create a new unit based on this that we'll use for output
        iErrCode = AudioComponentInstanceNew(ac, &m_cAudioUnit);
        
        if (nil == m_cAudioUnit) {
            VOLOGE("Error creating unit: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
        // disable IO for recording
        UInt32 flag = 0;
        AudioUnitElement kInputBus = 1;
        iErrCode = AudioUnitSetProperty(m_cAudioUnit, 
                                      kAudioOutputUnitProperty_EnableIO, 
                                      kAudioUnitScope_Input, 
                                      kInputBus,
                                      &flag, 
                                      sizeof(flag));
        if (noErr != iErrCode) {
            VOLOGE("Error setting kAudioOutputUnitProperty_EnableIO: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
        // Set our tone rendering function on the unit
        AURenderCallbackStruct input;
        input.inputProc = S_RenderProcCallback;
        input.inputProcRefCon = this;
        iErrCode = AudioUnitSetProperty(m_cAudioUnit, 
                                   kAudioUnitProperty_SetRenderCallback, 
                                   kAudioUnitScope_Input,
                                   0, 
                                   &input, 
                                   sizeof(input));
        if (noErr != iErrCode) {
            VOLOGE("Error setting callback: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
        iErrCode = AudioUnitInitialize(m_cAudioUnit);
        if (noErr != iErrCode) {
            VOLOGE("Error initializing unit: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
    }

    iErrCode = AudioUnitSetProperty (m_cAudioUnit,
                                     kAudioUnitProperty_StreamFormat,
                                     kAudioUnitScope_Input,
                                     0,
                                     &m_AudioFormat,
                                     sizeof(AudioStreamBasicDescription));
    
    VOLOGI("CAudioUnitRenderEx SetFormat mSampleRate:%e, mFormatID:%ld, mBytesPerPacket:%ld, mBytesPerFrame:%ld, mChannelsPerFrame:%ld, mBitsPerChannel:%ld, m_nAvgBytesPerSec:%d\n",
           m_AudioFormat.mSampleRate, m_AudioFormat.mFormatID, m_AudioFormat.mBytesPerPacket,
           m_AudioFormat.mBytesPerFrame, m_AudioFormat.mChannelsPerFrame, m_AudioFormat.mBitsPerChannel);
    
    memset(&m_cFormat, 0, sizeof(m_cFormat));
    
    if (noErr != iErrCode) {
        VOLOGE("Error setting stream format: %d \n", iErrCode);
        return VO_ERR_WRONG_STATUS;
    }
    
    m_bInit = true;
	
	// Disabling the automatic buffer allocation
//	UInt32 doNotSetFlag = 0; 
//	iErrCode = AudioUnitSetProperty(m_cAudioUnit, kAudioUnitProperty_ShouldAllocateBuffer, kAudioUnitScope_Output, kOutputBus, &doNotSetFlag, sizeof(doNotSetFlag));
//	if (noErr != iErrCode) {
//		VOLOGE("Error setting kAudioUnitProperty_ShouldAllocateBuffer: %d \n", iErrCode);
//		return VO_ERR_WRONG_STATUS;
//	}
		
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::Start (void)
{
    VO_U64 nTime = voOS_GetSysTime();
    {
        voNSAutoLock lock (&m_cLockUnit);
        
        VOLOGI("CAudioUnitRenderEx Start\n");
        
        if (nil == m_cAudioUnit) {
            VOLOGE("Start m_cAudioUnit NULL \n");
            return VO_ERR_WRONG_STATUS;
        }
        
        if (m_bPaused) {
            m_bPaused = false;
        }
        
        if (m_bIsRunning) {
            return VO_ERR_NONE;
        }
        
        m_bIsRunning = true;
    }
    
    int diffTime = voOS_GetSysTime() - m_nLastStopTime;
    if ((28 > diffTime) && (0 < diffTime)) {
        int nTime = 4;
        diffTime = (28 - diffTime) / nTime;
        if (0 == diffTime) {
            diffTime = 1;
        }
        
        VOLOGI("waiting stop flush :%d", diffTime * 4);
        
        while (m_bIsRunning && (nTime > 0)) {
            --nTime;
            voOS_Sleep(diffTime);
        }
    }

    {
        voNSAutoLock lock (&m_cLockUnit);
        
        if (m_bIsRunning && !m_cAudioUnitWorking) {
            VOLOGI("Try start audio unit in render -->>");

            // Start playback
            OSErr iErrCode = AudioOutputUnitStart(m_cAudioUnit);
            if (noErr != iErrCode) {
                m_bIsRunning = false;
                VOLOGE("Error Render starting unit: %d \n", iErrCode);
                return VO_ERR_WRONG_STATUS;
            }
            
            m_cAudioUnitWorking = true;
        }
        
        VOLOGI("CAudioUnitRenderEx Start:%d use:%lld\n", m_bIsRunning, voOS_GetSysTime() - nTime);
    }
    
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::Pause (void)
{
	VOLOGI("CAudioUnitRenderEx Pause\n");
    
    // use the same as stop for remote control
    m_bIsRunning = false;
	//m_bPaused = true;
	
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::Stop(void)
{
	VOLOGI("CAudioUnitRenderEx Stop\n");
	
	m_bIsRunning = false;
    
	return VO_ERR_NONE;
}

bool CAudioUnitRenderEx::closeDevice(void)
{
	bool bRet = true;
	
	if (nil != m_cAudioUnit) {
		OSErr iErrCode = noErr;
        
        m_bIsRunning = false;
        
        int i = 0;
        while (m_cAudioUnitWorking) {
            if (10 < i) {
                break;
            }
            
            ++i;
            voOS_Sleep(2);
        }
        
        voNSAutoLock lock (&m_cLockUnit);
        
        int diffTime = voOS_GetSysTime() - m_nLastStopTime;
        if ((26 > diffTime) && (0 < diffTime)) {
            VOLOGI("waiting closeDevice flush:%d", 26 - diffTime);
            voOS_Sleep(26 - diffTime);
        }
        
        // Must call stop before AudioUnitUninitialize since the stop in RenderProc not break AURemoteIO immediate
        iErrCode = AudioOutputUnitStop(m_cAudioUnit);
            
        if (noErr != iErrCode) {
            VOLOGE("Error Pause AudioOutputUnitStop: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
        m_cAudioUnitWorking = false;

		iErrCode = AudioUnitUninitialize(m_cAudioUnit);
		if (noErr != iErrCode) {
			VOLOGE("error closeDevice AudioUnitUninitialize: %d \n", iErrCode);
			bRet = false;
		}

		iErrCode = AudioComponentInstanceDispose(m_cAudioUnit);
		if (noErr != iErrCode) {
			VOLOGE("error closeDevice AudioUnitUninitialize: %d \n", iErrCode);
			bRet = false;
		}

		m_cAudioUnit = nil;
		VOLOGI("close m_cAudioUnit success\n");
	}

	return bRet;
}

VO_U32 CAudioUnitRenderEx::Flush (void)
{
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::GetPlayingTime (VO_S64	* pPlayingTime)
{
	if (NULL == pPlayingTime) {
		return VO_ERR_WRONG_STATUS;
	}

	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::GetBufferTime (VO_S32	* pBufferTime)
{
	if (NULL == pBufferTime) {
		return VO_ERR_WRONG_STATUS;
	}
    
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRenderEx::SetVolume(float leftVolume, float rightVolume)
{
    m_fLeftVolume = leftVolume;
    m_fRightVolume = rightVolume;
    
    return VO_ERR_NONE;
}

bool CAudioUnitRenderEx::RenderProcCallback(
						AudioUnitRenderActionFlags 	*ioActionFlags, 
						const AudioTimeStamp 		*inTimeStamp, 
						UInt32 						inBusNumber, 
						UInt32 						inNumberFrames, 
						AudioBufferList 			*ioData)
{
	bool bRet = RenderProc(0, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
    
    EffectAudioSample((VO_BYTE *)ioData->mBuffers[0].mData, ioData->mBuffers[0].mDataByteSize);
    
#ifdef DUMP_RENDER_LOG
	// ============dump start
	static FILE *pFileAudioRenderOut = NULL;
	
    static void * pSelf = NULL;
    
    if (pSelf != this) {
        pSelf = this;
        if (NULL != pFileAudioRenderOut) {
            fclose(pFileAudioRenderOut);
            pFileAudioRenderOut = NULL;
        }
    }
    
	if (NULL == pFileAudioRenderOut) {
		char szTmp[256];
		voOS_GetAppFolder(szTmp, 256);
		strcat(szTmp, "AudioRenderOut.pcm");
		pFileAudioRenderOut = fopen(szTmp, "wb");
		
		if (NULL == pFileAudioRenderOut) {
			VOLOGI("------------------Open AudioRenderOut.pcm file error!\n");
		}
	}
	
	if ((NULL != pFileAudioRenderOut) && (ioData->mBuffers[0].mDataByteSize > 0))
	{
		fwrite(ioData->mBuffers[0].mData, ioData->mBuffers[0].mDataByteSize, 1, pFileAudioRenderOut);
		fflush(pFileAudioRenderOut);
	}
	// ============dump end
//	printf("RenderProcCallback size:%ld, time:%ld\n", ioData->mBuffers[0].mDataByteSize, voOS_GetSysTime());
#endif
	
	return bRet;
}

bool CAudioUnitRenderEx::RenderProc(
				VO_U32						inReadedSize,
				AudioUnitRenderActionFlags 	*ioActionFlags, 
				const AudioTimeStamp 		*inTimeStamp, 
				UInt32 						inBusNumber, 
				UInt32 						inNumberFrames, 
				AudioBufferList 			*ioData)
{
	if (NULL == ioData) {
		return false;
	}

	if (!m_bIsRunning) {
		// memset is better(no noise)
		VO_BYTE *buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
		memset (buffer, 0, ioData->mBuffers[0].mDataByteSize);
        
        StopAudioUnit();
        
		return true;
	}
    
    if (m_bPaused) {
		VO_BYTE *buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
		memset (buffer, 0, ioData->mBuffers[0].mDataByteSize);
        return true;
    }

    int nCount = 0;
    while (0 != m_cFormat.SampleRate) {
        voOS_Sleep(2);
        ++nCount;
        if (nCount > 5) {
            VOLOGI("Return by audio format need change");
            ioData->mBuffers[0].mDataByteSize = 0;
            return true;
        }
    }
    
    memset(&m_cBuffer, 0, sizeof(m_cBuffer));
    memset(&m_cFormat, 0, sizeof(m_cFormat));
    
    m_cBuffer.Buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
    m_cBuffer.Length = ioData->mBuffers[0].mDataByteSize;
    
//    if (m_cBuffer.Length > 2000) {
//        m_cBuffer.Length /= 2;
//    }
    
    if (NULL != m_fCallBack) {
        int nRet = m_fCallBack(m_pUserData, &m_cBuffer, &m_cFormat, -1);
        
        if (VO_ERR_NONE == nRet) {
            if (0 != m_cFormat.SampleRate) {

                if (IsFormatChange(&m_cFormat)) {
                    PostRunOnMainRequest(false, 0, 0, 0);
                }
                else {
                    memset(&m_cFormat, 0, sizeof(m_cFormat));
                }
                
                // maybe have some data need to render
                ioData->mBuffers[0].mDataByteSize = m_cBuffer.Length;
                
                return true;
            }
            
            if (0 >= m_cBuffer.Length) {
                memset(ioData->mBuffers[0].mData, 0, ioData->mBuffers[0].mDataByteSize);
            }
            else if (m_cBuffer.Length < ioData->mBuffers[0].mDataByteSize) {
                ioData->mBuffers[0].mDataByteSize = m_cBuffer.Length;
            }
            return true;
        }
    }
    
    memset(ioData->mBuffers[0].mData, 0, ioData->mBuffers[0].mDataByteSize);
    
	return true;
}

VO_U32 CAudioUnitRenderEx::AudioInterruption(VO_U32 inInterruptionState)
{
	if (inInterruptionState == voAudioSessionEndInterruption) {
        Start();
	}
	
	if (inInterruptionState == voAudioSessionBeginInterruption) {
		
        voNSAutoLock lock (&m_cLockUnit);
        
        StopAudioUnit();
        
        m_bIsRunning = false;
        m_cAudioUnitWorking = false;
    }
    
    return VO_ERR_NONE;
}

OSStatus CAudioUnitRenderEx::S_RenderProcCallback(
									  void *inRefCon, 
									  AudioUnitRenderActionFlags 	*ioActionFlags, 
									  const AudioTimeStamp			*inTimeStamp, 
									  UInt32 						inBusNumber, 
									  UInt32 						inNumberFrames, 
									  AudioBufferList				*ioData)

{
#ifdef _VOLOG_INFO
    static CFAbsoluteTime last = CFAbsoluteTimeGetCurrent() * 1000;
    CFAbsoluteTime fTimeS = CFAbsoluteTimeGetCurrent() * 1000;
#endif
    
	CAudioUnitRenderEx *cAudioUnit = (CAudioUnitRenderEx *)inRefCon;
	
	if (NULL == cAudioUnit) {
		VOLOGE("Audio S_RenderProcCallback cAudioUnit null\n");
		return -1;
	}
	
	if (!cAudioUnit->RenderProcCallback(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData)) {
		return -1;
	}
    
#ifdef _VOLOG_INFO
    CFAbsoluteTime fTimeE = CFAbsoluteTimeGetCurrent() * 1000;
	//VOLOGR("size:%ld, use:%f, diff time :%f\n", ioData->mBuffers[0].mDataByteSize, fTimeE - fTimeS, fTimeS - last);

    last = fTimeS;
#endif
    
	return noErr;
}

void CAudioUnitRenderEx::StopAudioUnit()
{
    VOLOGI("Try to stop audio unit");
    
    if (m_cLockUnit.TryLock()) {
        
        if (nil != m_cAudioUnit && m_cAudioUnitWorking) {
            
            OSErr iErrCode = AudioOutputUnitStop(m_cAudioUnit);
            
            if (noErr != iErrCode) {
                m_cLockUnit.Unlock();
                VOLOGE("Error AudioOutputUnitStop: %d \n", iErrCode);
                return;
            }
            
            m_nLastStopTime = voOS_GetSysTime();
            m_cAudioUnitWorking = false;
            
            VOLOGI("Audio uint stop success");
        }
        
        m_cLockUnit.Unlock();
    }
}

void CAudioUnitRenderEx::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if (0 == m_cFormat.SampleRate) {
        return;
    }
    
    InitDevice();
    //Start();
}


VO_U32 CAudioUnitRenderEx::EffectAudioSample(VO_PBYTE pBuffer, VO_S32 nSize)
{
    int m_nAudioValume = m_fLeftVolume * 100;
    
    if ((m_nAudioValume >= 0 && m_nAudioValume < 100) || (m_nAudioValume > 100 && m_nAudioValume <= 200))
	{
		if (m_nAudioValume == 0)
		{
			memset (pBuffer, 0, nSize);
		}
		else
		{
			if (m_AudioFormat.mBitsPerChannel == 16)
			{
				int nTmp;
				short * pSData;
				pSData = (short *)pBuffer;
				for (int i = 0; i < nSize; i+=2)
				{
					nTmp = ((*pSData) * m_nAudioValume / 100);
					
					if(nTmp >= -32768 && nTmp <= 32767)
					{
						*pSData = (short)nTmp;
					}
					else if(nTmp < -32768)
					{
						*pSData = -32768;
					}
					else if(nTmp > 32767)
					{
						*pSData = 32767;
					}
                    
					pSData++;
				}
			}
			else if (m_AudioFormat.mBitsPerChannel == 8)
			{
				int nTmp;
				char * pCData;
				pCData = (char *)pBuffer;
				for (int i = 0; i < nSize; i++)
				{
					nTmp = (*pCData) * m_nAudioValume / 100;
                    
					if(nTmp >= -256 && nTmp <= 255)
					{
						*pCData = (char)nTmp;
					}
					else if(nTmp < -256)
					{
						*pCData = (char)-256;
					}
					else if(nTmp > 255)
					{
						*pCData = (char)255;
					}
                    
					pCData++;
				}
			}
		}
	}
    
    return VO_ERR_NONE;
}
