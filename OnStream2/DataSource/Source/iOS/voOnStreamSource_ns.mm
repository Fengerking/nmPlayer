/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voOnStreamSource_ns.mm
 
 Contains:	VisualOn OnStream Source impletment file
 
 Written by:	Jeff
 
 Change History (most recent first):
 2012-07-26		Jeff			Create file
 *******************************************************************************/

#import "voOnStreamSource_ns.h"
#include "voOnStreamType.h"
#include "voOnStreamSource.h"

@interface voOnStreamSource_ns ()
// Properties that don't need to be seen by the outside world.

- (int)voHandleSourceEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
@end

@implementation voOnStreamSource_ns

int voEventCallback_Source_ns(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	if (pUserData == nil) 
	{
		return -1;
	}
	
	voOnStreamSource_ns * pOnStreamSource_ns = (voOnStreamSource_ns *)pUserData;
	
	int result = [pOnStreamSource_ns voHandleSourceEvent:nID withParam1:pParam1 withParam2:pParam2];
	return result;
}

- (int)voHandleSourceEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == m_pDelegate) {
        return VOOSMP_ERR_None;
    }
    
    return [m_pDelegate voHandleSourceEvent:nID withParam1:pParam1 withParam2:pParam2];
}

- (id) init:(void*)pSource nSourceFlag:(int)nSourceFlag nSourceType:(int)nSourceType pInitParam:(void*)pInitParam nInitParamFlag:(int)nInitParamFlag
{
    if (nil != (self = [super init])) 
    {
        m_pOSApiSet = new voOSMPSourceAPI;
        NSLog(@"m_pOSApiSet:%d", (int)m_pOSApiSet);
        
        memset(m_pOSApiSet, 0, sizeof(voOSMPSourceAPI));
        
        voGetOnStreamSourceAPI((voOSMPSourceAPI *)m_pOSApiSet);
        
        if (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Init) {
            [self release];
            return nil;
        }
        
        int iRet =  ((voOSMPSourceAPI *)m_pOSApiSet)->Init(&m_pOSHandle, pSource, nSourceFlag, nSourceType, pInitParam, nInitParamFlag);
        
        if (VOOSMP_ERR_None != iRet) {
            [self release];
            return nil;
        }
        
        m_pDelegate = nil;
        
        m_cInfo.pListener = voEventCallback_Source_ns;
        m_cInfo.pUserData = self;
        
        if ((NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SetParam) || (NULL == m_pOSHandle)) {
            [self release];
            return nil;
        }
        
        ((voOSMPSourceAPI *)m_pOSApiSet)->SetParam(m_pOSHandle, VOOSMP_PID_LISTENER, &m_cInfo);
    }
    
    return self;
}

- (void) dealloc
{
	if ((NULL != m_pOSApiSet) && (NULL != ((voOSMPSourceAPI *)m_pOSApiSet)->Uninit) && (NULL != m_pOSHandle)) {
        ((voOSMPSourceAPI *)m_pOSApiSet)->Uninit(m_pOSHandle);
        m_pOSHandle = NULL;
    }
    
    if (NULL != m_pOSApiSet) {
        delete ((voOSMPSourceAPI *)m_pOSApiSet);
        m_pOSApiSet = NULL;
    }
    
	[super dealloc];
}

- (void)setDelegate:(id <voOnStreamSourceDelegate>)delegate
{
    m_pDelegate = delegate;
}

- (int) open
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Open) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->Open(m_pOSHandle);
}

- (int) close
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Close) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->Close(m_pOSHandle);
}

- (int) run
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Run) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->Run(m_pOSHandle);
}

- (int) pause
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Pause) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->Pause(m_pOSHandle);
}

- (int) stop
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->Stop) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->Stop(m_pOSHandle);
}

- (int) setPos:(long long*)pTimeStamp
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SetPos(m_pOSHandle, pTimeStamp);
}

- (int) getDuration:(long long*)pDuration
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetDuration) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetDuration(m_pOSHandle, pDuration);
}

- (int) getSample:(int)nTrackType pSample:(void*)pSample
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetSample) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetSample(m_pOSHandle, nTrackType, pSample);
}

- (int) getProgramCount:(int *)pProgramCount
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetProgramCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
        
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetProgramCount(m_pOSHandle, pProgramCount);
}

- (int) getProgramInfo:(int)nProgramIndex ppProgramInfo:(VOOSMP_SRC_PROGRAM_INFO**)ppProgramInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetProgramInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetProgramInfo(m_pOSHandle, nProgramIndex, ppProgramInfo);
}

- (int) getCurTrackInfo:(int)nTrackType ppTrackInfo:(VOOSMP_SRC_TRACK_INFO**)ppTrackInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetCurTrackInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetCurTrackInfo(m_pOSHandle, nTrackType, ppTrackInfo);
}

- (int) selectProgram:(int)nProgram
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SelectProgram) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SelectProgram(m_pOSHandle, nProgram);
}

- (int) selectStream:(int)nStream
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SelectStream) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SelectStream(m_pOSHandle, nStream);
}

- (int) selectTrack:(int)nTrack
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SelectTrack) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SelectTrack(m_pOSHandle, nTrack);
}

- (int) selectLanguage:(int)nIndex
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SelectLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SelectLanguage(m_pOSHandle, nIndex);
}

- (int) getLanguage:(VOOSMP_SUBTITLE_LANGUAGE_INFO**)ppLangInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetLanguage(m_pOSHandle, ppLangInfo);
}

- (int) sendBuffer:(const VOOSMP_BUFFERTYPE&)buffer
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SendBuffer) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SendBuffer(m_pOSHandle, buffer);
}

- (int) getParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->GetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->GetParam(m_pOSHandle, nParamID, pValue);
}

- (int) setParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == m_pOSApiSet) || (NULL == ((voOSMPSourceAPI *)m_pOSApiSet)->SetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return ((voOSMPSourceAPI *)m_pOSApiSet)->SetParam(m_pOSHandle, nParamID, pValue);
}

@end
