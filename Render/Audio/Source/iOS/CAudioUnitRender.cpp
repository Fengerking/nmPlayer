/*
 *  CAudioUnitRender.cpp
 *  vompEngn
 *
 *  Created by Jeff Huang on 07/05/2011.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */
#import <AudioToolbox/AudioToolbox.h>

#include "voAudioRenderType.h"

#include "CAudioUnitRender.h"
#include <sys/time.h>
#include <unistd.h>

#include "voLog.h"
#include "voOSFunc.h"

//#define DUMP_RENDER_LOG
#define _MergeBuffer

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const int CAudioUnitRender::MAX_BUFFER = 4;

CAudioUnitRender::CAudioUnitRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
:CBaseAudioRender(hInst, pMemOP)
, m_cAudioUnit(nil)
, m_fLeftVolume(1.0)
, m_fRightVolume(1.0)
, m_nBufSize (0)
, m_bIsRunning (false)
, m_nBufferTimeEvaluate (0)
//, m_nLastBufferTime (0)
//, m_nPlayingTime (0)
//, m_nBufferTime (0)
, m_nBufferTimeUnchange(0)
, m_nInputFormat(kAudioFormatLinearPCM)
, m_bPaused(false)
, m_bStoped(false)
, m_nUsedBufferCount(0)
, m_nLastReadPosition(0)
, m_nToSubtractTimeBufferSize(0)
, m_bNeedStartInRender(false)
, m_cAudioUnitStopSuccess(true)
, m_pLastBuffer(NULL)
, m_nLastBufferSize(0)
, m_nAvgBytesPerSec(0)
{
#ifndef USE_HW_AUDIO_DEC
#endif
	memset(&m_AudioFormat, 0, sizeof(AudioStreamBasicDescription));
}

CAudioUnitRender::~CAudioUnitRender (void)
{
    VO_U64 nTime = voOS_GetSysTime();
    
	closeDevice();
    
	VOLOGI("CAudioUnitRender deconstruct finish use:%lld\n", voOS_GetSysTime() - nTime);
}

VO_U32 CAudioUnitRender::SetInputFormat(int nFormat)
{
//	if(nFormat == VO_INPUT_AAC)
//	{
//		m_nInputFormat = kAudioFormatMPEG4AAC;
//	}
//	else if(nFormat == VO_INPUT_MP3)
//	{
//		m_nInputFormat = kAudioFormatMPEGLayer3;
//	}
//	else
//	{
		m_nInputFormat = kAudioFormatLinearPCM;
//	}
	
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::SetVolume(float leftVolume, float rightVolume)
{
    m_fLeftVolume = leftVolume;
    m_fRightVolume = rightVolume;
    
    return VO_ERR_NONE;
}

bool CAudioUnitRender::IsEqual(Float64 a, Float64 b)
{
    const static Float64 V_RANGE = 0.000001;
    if (((a - b) > -V_RANGE)
        && ((a - b) < V_RANGE) ) {
        return true;
    }
    return false;
}

VO_U32 CAudioUnitRender::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if (NULL == pFormat) {
		return VO_ERR_WRONG_STATUS;
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
        return VO_ERR_NONE;
    }
    
    m_nBufferTimeEvaluate = 0;
	m_nBufferTimeUnchange = 0;
	
    memcpy(&m_AudioFormat, &formatTemp, sizeof(AudioStreamBasicDescription));
	
	m_nAvgBytesPerSec = ((pFormat->SampleRate) * (pFormat->Channels) * (pFormat->SampleBits)) / 8;
		
	VOLOGI("CAudioUnitRender SetFormat mSampleRate:%e, mFormatID:%ld, mBytesPerPacket:%ld, mBytesPerFrame:%ld, mChannelsPerFrame:%ld, mBitsPerChannel:%ld, m_nAvgBytesPerSec:%d\n", 
		   m_AudioFormat.mSampleRate, m_AudioFormat.mFormatID, m_AudioFormat.mBytesPerPacket, 
		   m_AudioFormat.mBytesPerFrame, m_AudioFormat.mChannelsPerFrame, m_AudioFormat.mBitsPerChannel, m_nAvgBytesPerSec);
	
	return InitDevice();
}

VO_U32 CAudioUnitRender::InitDevice(void)
{
	closeDevice();
	
    {
        voNSAutoLock lock (&m_cLockUnit);
        
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
        OSErr iErrCode = AudioComponentInstanceNew(ac, &m_cAudioUnit);
        
        if (nil == m_cAudioUnit) {
            VOLOGE("Error creating unit: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
    //	AudioUnitElement kOutputBus = 0;
    //	UInt32 setFlag = 1; 
    //	iErrCode = AudioUnitSetProperty(m_cAudioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, kOutputBus, &setFlag, sizeof(setFlag));
    //	if (noErr != iErrCode) {
    //		VOLOGE("Error setting kAudioOutputUnitProperty_EnableIO: %d \n", iErrCode);
    //		return VO_ERR_WRONG_STATUS;
    //	}
        
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
        
        iErrCode = AudioUnitSetProperty (m_cAudioUnit,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Input,
                                    0,
                                    &m_AudioFormat,
                                    sizeof(AudioStreamBasicDescription));
        
        if (noErr != iErrCode) {
            VOLOGE("Error setting stream format: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }    
    }
	
	// Disabling the automatic buffer allocation
//	UInt32 doNotSetFlag = 0; 
//	iErrCode = AudioUnitSetProperty(m_cAudioUnit, kAudioUnitProperty_ShouldAllocateBuffer, kAudioUnitScope_Output, kOutputBus, &doNotSetFlag, sizeof(doNotSetFlag));
//	if (noErr != iErrCode) {
//		VOLOGE("Error setting kAudioUnitProperty_ShouldAllocateBuffer: %d \n", iErrCode);
//		return VO_ERR_WRONG_STATUS;
//	}
	
	m_nBufSize = m_nAvgBytesPerSec / 2; // 1/2 second
	
	{
		voNSAutoLock lock (&m_csAQBuffers);
		
		for(int i=0; i < MAX_BUFFER; ++i)
		{
			AudioUbuffer *pAudioUbuffer = new AudioUbuffer();
			pAudioUbuffer->nSize = 0;
			pAudioUbuffer->nStart = 0;
			pAudioUbuffer->pBuffer = (VO_BYTE *) malloc (m_nBufSize);
			memset (pAudioUbuffer->pBuffer, 0, m_nBufSize);
			m_cBufferList.push_back(pAudioUbuffer);
		}
	}
	
	if (m_bIsRunning) {
		// restart
		m_bIsRunning = false;
		return Start();
	}
	
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::Start (void)
{
    VO_U64 nTime = voOS_GetSysTime();
	
    if (m_bStoped) {
		InitDevice();
		m_bStoped = false;
	}
    
	if (nil == m_cAudioUnit) {
		VOLOGE("Start m_cAudioUnit NULL \n");
		return VO_ERR_WRONG_STATUS;
	}
	
	if (m_bIsRunning) {
		return VO_ERR_NONE;
	}
	
	m_bIsRunning = true;
	
	if (m_bPaused) {
		m_bPaused = false;
		
//		OSErr iErrCode = AudioOutputUnitStart(m_cAudioUnit);
//		if (noErr != iErrCode) {
//			VOLOGE("Error starting unit from pause: %d \n", iErrCode);
//			return VO_ERR_WRONG_STATUS;
//		}
	}
	else {
//		m_nBufferTime = 0;
//		m_nPlayingTime = 0;
//		m_nLastBufferTime = 0;
		
		// Stop changing parameters on the unit
		OSErr iErrCode = AudioUnitInitialize(m_cAudioUnit);
		if (noErr != iErrCode) {
			VOLOGE("Error initializing unit: %d \n", iErrCode);
			return VO_ERR_WRONG_STATUS;
		}
		
//		// Start playback
//		iErrCode = AudioOutputUnitStart(m_cAudioUnit);
//		if (noErr != iErrCode) {
//			VOLOGE("Error starting unit: %d \n", iErrCode);
//			return VO_ERR_WRONG_STATUS;
//		}
	}
	
    m_bNeedStartInRender = true;
//	VOLOGI("CAudioUnitRender Start ok\n");	
	
	VOLOGI("CAudioUnitRender Start use:%lld\n", voOS_GetSysTime() - nTime);
    
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::Pause (void)
{	
	m_bPaused = true;
	m_bIsRunning = false;
	
    m_bNeedStartInRender = false;
	return VO_ERR_NONE;
}


VO_U32 CAudioUnitRender::Stop(void)
{
	VOLOGI("CAudioUnitRender Stop\n");
	
	m_bStoped = true;
	m_bIsRunning = false;
    m_bNeedStartInRender = false;
    
	return VO_ERR_NONE;
}

bool CAudioUnitRender::closeDevice(void)
{
	bool bRet = true;
	
	if (nil != m_cAudioUnit) {
		OSErr iErrCode = noErr;
        
        m_bIsRunning = false;
        
        bool bIsWaitingStopInIO = false;
        
        int i = 0;
        while (!m_cAudioUnitStopSuccess) {
            bIsWaitingStopInIO = true;
            
            if (10 < i) {
                break;
            }
            
            ++i;
            voOS_Sleep(2);
        }
        
        voNSAutoLock lock (&m_cLockUnit);
        
        if (bIsWaitingStopInIO) {
            voOS_Sleep(5);
        }
        
        // Must call stop before AudioUnitUninitialize since the stop in RenderProc not break AURemoteIO immediate
        iErrCode = AudioOutputUnitStop(m_cAudioUnit);
            
        if (noErr != iErrCode) {
            VOLOGE("Error Pause AudioOutputUnitStop: %d \n", iErrCode);
            return VO_ERR_WRONG_STATUS;
        }
        
        m_cAudioUnitStopSuccess = true;

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
	
	{
		voNSAutoLock lock (&m_csAQBuffers);
		
		while (0 < m_cBufferList.size()) {
			
			AudioUbuffer *pAudioUbuffer = m_cBufferList.front();
			if (NULL != pAudioUbuffer) {
				free(pAudioUbuffer->pBuffer);
				delete pAudioUbuffer;
			}
			m_cBufferList.pop_front();
		}
	}
    
    Flush();

	return bRet;
}

VO_U32 CAudioUnitRender::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
    if (!m_bIsRunning) {
        
        if (m_bPaused) {
            return TryKeepBuffer(pBuffer, nSize);
        }
        
		return VO_ERR_WRONG_STATUS;
	}
    
    if (0 == m_nBufferTimeEvaluate) {
        int nCount = nSize / m_nBufSize;
        if (0 != (nSize % m_nBufSize)) {
            ++nCount;
        }
        
        if ((0 != nCount) && (0 != m_nAvgBytesPerSec)) {
            m_nBufferTimeEvaluate = MAX_BUFFER * nSize * 1000 / (m_nAvgBytesPerSec * nCount);
        }
        
        VOLOGI("nSize:%ld m_nBufferTimeEvaluate:%d", nSize, m_nBufferTimeEvaluate);
    }
    
    if (NULL != m_pLastBuffer) {
        voNSAutoLock lock (&m_cLastBuffersMutex);
        
        if (NULL != m_pLastBuffer) {
            int nRet = RenderInner(m_pLastBuffer, m_nLastBufferSize, 0, VO_FALSE);
            
            if (VO_ERR_NONE != nRet) {
                return nRet;
            }
            
            free(m_pLastBuffer);
            m_pLastBuffer = NULL;
            m_nLastBufferSize = 0;
        }
    }
    
    return RenderInner(pBuffer, nSize, nStart, bWait);
}

VO_U32 CAudioUnitRender::TryKeepBuffer(VO_PBYTE pBuffer, VO_U32 nSize)
{
    voNSAutoLock lockList (&m_cLastBuffersMutex);
    
    if ((NULL == m_pLastBuffer) && (NULL != pBuffer) && (0 < nSize)) {
        m_pLastBuffer = (VO_BYTE *) malloc (nSize);
        m_nLastBufferSize = nSize;
        
        if (NULL == m_pLastBuffer) {
            return VO_ERR_OUTOF_MEMORY;
        }
        
        memcpy(m_pLastBuffer, pBuffer, nSize);
    }
    else {
        return VO_ERR_WRONG_STATUS;
    }
    
    return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::RenderInner(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
    int nRet = VO_ERR_NONE;
    
	while (nSize > 0)
	{
		int nTry = 0;
        
        if (m_nUsedBufferCount>= (MAX_BUFFER - 1)) {
            voNSAutoLock lock (&m_cLockUnit);

            if (m_bNeedStartInRender && m_cAudioUnitStopSuccess) {
                
                nRet = VO_ERR_FINISH;
                
                VOLOGI("Try start audio unit in render -->>");
                
                if (nil == m_cAudioUnit) {
                    VOLOGE("Pause m_cAudioUnit NULL \n");
                    return VO_ERR_WRONG_STATUS;
                }
                
                // Start playback
                OSErr iErrCode = AudioOutputUnitStart(m_cAudioUnit);
                if (noErr != iErrCode) {
                    VOLOGE("Error Render starting unit: %d \n", iErrCode);
                    return VO_ERR_WRONG_STATUS;
                }
                
                VOLOGI("Try start audio unit in render --<<");
                
                m_bNeedStartInRender = false;
                m_cAudioUnitStopSuccess = false;
            }
            
            if (m_bNeedStartInRender) {
                nRet = VO_ERR_FINISH;
                m_bNeedStartInRender = false;
            }
        }
        
		while (m_nUsedBufferCount >= MAX_BUFFER) 
		{
			voOS_Sleep(5);
			nTry++;
			if (nTry > 150 || !m_bIsRunning)
			{
                if (m_bPaused) {
                    return TryKeepBuffer(pBuffer, nSize);
                }
                
                if (m_bIsRunning) {
                    VOLOGI("*****************Audio render drop frame, ts = %d, %d\n", (int)nStart, m_nUsedBufferCount);
                }
                
				return VO_ERR_WRONG_STATUS;
			}
		}
		
        if (m_nUsedBufferCount < MAX_BUFFER - 1) {
            VOLOGI("*****************audio m_nUsedBufferCount %d\n", m_nUsedBufferCount);
        }
        
		int nInputSize = 0;
		
		if (m_nBufSize < nSize)
		{
			nInputSize = m_nBufSize;
		}
		else
		{
			nInputSize = nSize;
		}
		
		{
			voNSAutoLock lock (&m_csAQBuffers);
			
			std::list<AudioUbuffer *>::iterator itr = m_cBufferList.begin();
			
			int iIndex = 0;
			while ((iIndex < m_nUsedBufferCount) && (itr != m_cBufferList.end())) {
				++itr;
				++iIndex;
			}
			
			if (itr == m_cBufferList.end()) {
				VOLOGE("*****************render m_cBufferList end\n");
				return VO_ERR_WRONG_STATUS;
			}
			
			AudioUbuffer *pAudioUbuffer = *itr;
			
			if ((NULL == pAudioUbuffer) || (iIndex != m_nUsedBufferCount)) {
				VOLOGE("*****************render get m_cBufferList item error\n");
				return VO_ERR_WRONG_STATUS;
			}
			
			pAudioUbuffer->nSize = nInputSize;
			pAudioUbuffer->nStart = nStart;
			memset (pAudioUbuffer->pBuffer, 0, m_nBufSize);
			memcpy (pAudioUbuffer->pBuffer, pBuffer, nInputSize);
			++m_nUsedBufferCount;
		}
        
        if ((m_nUsedBufferCount == MAX_BUFFER) && (0 == m_nBufferTimeUnchange)) {

            voNSAutoLock lock (&m_csAQBuffers);
            
            VO_U32 nAllSize = 0;
            std::list<AudioUbuffer *>::iterator itrSize = m_cBufferList.begin();
			
			int iIndex = 0;
			while ((iIndex < m_nUsedBufferCount) && (itrSize != m_cBufferList.end())) {
                nAllSize += (*itrSize)->nSize;
				++itrSize;
			}
            
            m_nBufferTimeUnchange = nAllSize * 1000 / m_nAvgBytesPerSec;
            
            VOLOGI("nSize:%ld, nAllSize:%ld Buffer:%d",nSize, nAllSize, m_nBufferTimeUnchange);
        }
		
//		{
//			voNSAutoLock lockTime (&m_csTime);
//			m_nBufferTime = m_nBufferTime + nInputSize * 1000 / m_nAvgBytesPerSec;
//		}

//#define _DUMP_PCM
#ifdef _DUMP_PCM
        // ============dump start
        static FILE *pPCM = NULL;
        
        static void * pSelf = NULL;
        
        if (pSelf != this) {
            pSelf = this;
            if (NULL != pPCM) {
                fclose(pPCM);
                pPCM = NULL;
            }
        }
        
        if (NULL == pPCM) {
            char szTmp[256];
            voOS_GetAppFolder(szTmp, 256);
            strcat(szTmp, "out.pcm");
            pPCM = fopen(szTmp, "wb");
            
            if (NULL == pPCM) {
                VOLOGI(" ------------------Open %s file error!\n", szTmp);
            }
        }
        
        if ((NULL != pPCM) && (nInputSize > 0))
        {
            fwrite(pBuffer, nInputSize, 1, pPCM);
            fflush(pPCM);
        }
        // ============dump end
#endif
        
		pBuffer += nInputSize;
		nSize	-= nInputSize;
	}
    
	return nRet;
}

VO_U32 CAudioUnitRender::Flush (void)
{
	VOLOGI("CAudioUnitRender Flush\n");
	
	{
		voNSAutoLock lock (&m_csAQBuffers);
        m_nUsedBufferCount = 0;
        m_nLastReadPosition = 0;
        m_nToSubtractTimeBufferSize = 0;
//		m_nLastBufferTime = 0;
//		m_nPlayingTime = 0;
//		m_nBufferTime = 0;
        
        // Don't reset it until format change
//        m_nBufferTimeEvaluate = 0;
//        m_nBufferTimeUnchange = 0;
    }
    {
        voNSAutoLock lockLastBuffer (&m_cLastBuffersMutex);
        if (NULL != m_pLastBuffer) {
            free(m_pLastBuffer);
            m_pLastBuffer = NULL;
            m_nLastBufferSize = 0;
        }
	}
	
	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	if (NULL == pPlayingTime) {
		return VO_ERR_WRONG_STATUS;
	}

//	*pPlayingTime = m_nPlayingTime;

	return VO_ERR_NONE;
}

VO_U32 CAudioUnitRender::GetBufferTime (VO_S32	* pBufferTime)
{
	if (NULL == pBufferTime) {
		return VO_ERR_WRONG_STATUS;
	}
	
	//*pBufferTime = m_nBufferTime;
    if (0 != m_nBufferTimeUnchange) {
        *pBufferTime = m_nBufferTimeUnchange; // set a unchange buffer time(or it may cause play delay)
    }
    else {
        *pBufferTime = m_nBufferTimeEvaluate;
    }
    
	return VO_ERR_NONE;
}

bool CAudioUnitRender::RenderProcCallback(
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

bool CAudioUnitRender::RenderProc(
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
	
//	if (0 != m_nLastBufferTime)
//	{
//		voNSAutoLock lockTime (&m_csTime);
//		m_nPlayingTime = m_nPlayingTime + m_nLastBufferTime;
//		m_nBufferTime = m_nBufferTime - m_nLastBufferTime;
//		m_nLastBufferTime = 0;
//	}

	if (!m_bIsRunning) {
		// memset is better(no noise)
		VO_BYTE *buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
		memset (buffer, 0, ioData->mBuffers[0].mDataByteSize);
        
        if (m_cLockUnit.TryLock()) {
        
            if (nil != m_cAudioUnit && !m_cAudioUnitStopSuccess) {
                
                OSErr iErrCode = AudioOutputUnitStop(m_cAudioUnit);
                
                if (noErr != iErrCode) {
                    m_cLockUnit.Unlock();
                    VOLOGE("Error Pause AudioOutputUnitStop: %d \n", iErrCode);
                    return VO_ERR_WRONG_STATUS;
                }
                
                m_cAudioUnitStopSuccess = true;
                            
                VOLOGI("Uint stop in RenderProc");
            }
            
            m_cLockUnit.Unlock();
        }
        
		return true;
	}
    
    // Don't block audio unit, otherwise it will cause noise
	if (m_nUsedBufferCount == 0)
	{
        VO_BYTE *buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
        memset (buffer + inReadedSize, 0, ioData->mBuffers[0].mDataByteSize - inReadedSize);
        
        return true;
	}
	
	VO_U32 nReadSize = 0;
	{
		voNSAutoLock lockList (&m_csAQBuffers);
		if (0 >= m_cBufferList.size()) {
			VOLOGE("Audio RenderProcCallback m_cBufferList size 0\n");
			return false;
		}
		
		AudioUbuffer *pAudioUbuffer = m_cBufferList.front();
		
		if (NULL == pAudioUbuffer) {
			VOLOGE("Audio RenderProcCallback pAudioUbuffer null\n");
			return false;
		}
		
		nReadSize = pAudioUbuffer->nSize;
		if ((inReadedSize + nReadSize) > ioData->mBuffers[0].mDataByteSize)
		{
			nReadSize = ioData->mBuffers[0].mDataByteSize - inReadedSize;
		}
		
		pAudioUbuffer->nSize -= nReadSize;
		

#ifndef _MergeBuffer
		ioData->mBuffers[0].mDataByteSize = nReadSize;
#endif
		
		VO_BYTE *buffer = (VO_BYTE *)ioData->mBuffers[0].mData;
		
		memcpy (buffer + inReadedSize, (pAudioUbuffer->pBuffer) + m_nLastReadPosition, nReadSize);
		
//		m_nLastBufferTime = nReadSize * 1000 / m_nAvgBytesPerSec;
		m_nToSubtractTimeBufferSize += nReadSize;
		
		if (0 < pAudioUbuffer->nSize) {
			m_nLastReadPosition += nReadSize;
//			VOLOGI("RenderProcCallback extend, inReadedSize:%ld, nReadSize = %ld, pAudioUbuffer->nSize:%ld, mBuffers[0].mDataByteSize:%ld, m_nUsedBufferCount:%d \n", 
//				   inReadedSize, nReadSize, pAudioUbuffer->nSize, ioData->mBuffers[0].mDataByteSize, m_nUsedBufferCount);
		}
		else {
			
			m_nLastReadPosition = 0;
			
			m_cBufferList.pop_front();
			m_cBufferList.push_back(pAudioUbuffer);
			
			--m_nUsedBufferCount;
			
			//m_nLastBufferTime = m_nToSubtractTimeBufferSize * 1000 / m_nAvgBytesPerSec;
			m_nToSubtractTimeBufferSize = 0;
			
//			VOLOGI("RenderProcCallback ReadEd, inReadedSize:%ld, nReadSize = %ld, pAudioUbuffer->nSize:%ld mBuffers[0].mDataByteSize:%ld, m_nUsedBufferCount:%d \n", 
//				   inReadedSize, nReadSize, pAudioUbuffer->nSize, ioData->mBuffers[0].mDataByteSize, m_nUsedBufferCount);
		}
	}
	
#ifdef _MergeBuffer
	// merge two discontinuously buffer to the ioData
	if ((inReadedSize + nReadSize) < ioData->mBuffers[0].mDataByteSize)
	{
		return RenderProc(inReadedSize + nReadSize, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
	}
#endif
	
	return true;
}

VO_U32 CAudioUnitRender::AudioInterruption(VO_U32 inInterruptionState)
{
//	if (nil == m_cAudioUnit) {
//		VOLOGI("m_cAudioUnit null \n");
//		return;
//	}
	
	if (inInterruptionState == voAudioSessionEndInterruption) {
        Start();
        
		// make sure we are again the active session
//		if (noErr != AudioSessionSetActive(true)) {
//			VOLOGE("couldn't set audio session active \n");
//			return;
//		}
//		
//		if (noErr != AudioOutputUnitStart(m_cAudioUnit)) {
//			VOLOGE("couldn't start unit \n");
//			return;
//		}
	}
	
	if (inInterruptionState == voAudioSessionBeginInterruption) {
		
        voNSAutoLock lock (&m_cLockUnit);
        
        m_bStoped = true;
        m_bIsRunning = false;
        
        m_cAudioUnitStopSuccess = true;
        
//		if (noErr != AudioOutputUnitStop(m_cAudioUnit)) {
//			VOLOGE("couldn't stop unit \n");
//			return;
//		}
    }
    
    return VO_ERR_NONE;
}

OSStatus CAudioUnitRender::S_RenderProcCallback(
									  void *inRefCon, 
									  AudioUnitRenderActionFlags 	*ioActionFlags, 
									  const AudioTimeStamp			*inTimeStamp, 
									  UInt32 						inBusNumber, 
									  UInt32 						inNumberFrames, 
									  AudioBufferList				*ioData)

{
	CAudioUnitRender *cAudioUnit = (CAudioUnitRender *)inRefCon;
	
	if (NULL == cAudioUnit) {
		VOLOGE("Audio S_RenderProcCallback cAudioUnit null\n");
		return -1;
	}
	
	if (!cAudioUnit->RenderProcCallback(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData)) {
		return -1;
	}
	
	return noErr;
}


VO_U32 CAudioUnitRender::EffectAudioSample(VO_PBYTE pBuffer, VO_S32 nSize)
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

