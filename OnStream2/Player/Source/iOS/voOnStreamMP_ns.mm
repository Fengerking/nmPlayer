/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voOnStreamMP_ns.mm
 
 Contains:    VisualOn OnStream Media Player impletment file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-07-26   Jeff            Create file
 *******************************************************************************/

#import "voOnStreamMP_ns.h"
#import "voOnStreamMP.h"

@interface voOnStreamMP_ns ()
// Properties that don't need to be seen by the outside world.
@property (assign) void * m_pOSHandle;
@property (assign) voOnStreamMediaPlayerAPI * m_pOSApiSet;
@property (assign) id <voOnStreamMPDelegate> m_pDelegate;
@property (assign) id <voOnStreamMPOnRequestDelegate> m_pOnRequestDelegate;

- (int)voHandleMPEventC:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
- (int)voHandleMPOnRequestC:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;

@end

@implementation voOnStreamMP_ns

@synthesize m_pOSHandle;
@synthesize m_pOSApiSet;
@synthesize m_pDelegate;
@synthesize m_pOnRequestDelegate;

int voEventCallback_MP_ns(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    if (pUserData == nil) 
    {
        return VOOSMP_ERR_Pointer;
    }

    voOnStreamMP_ns * pOnStreamMP_ns = (voOnStreamMP_ns *)pUserData;
    
    int result = [pOnStreamMP_ns voHandleMPEventC:nID withParam1:pParam1 withParam2:pParam2];
    return result;
}

- (int)voHandleMPEventC:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == m_pDelegate) {
        return VOOSMP_ERR_None;
    }

    return [m_pDelegate voHandleMPEvent:nID withParam1:pParam1 withParam2:pParam2];
}

int voEventOnRequest_MP_ns(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    if (pUserData == nil)
    {
        return VOOSMP_ERR_Pointer;
    }
    
    voOnStreamMP_ns * pOnStreamMP_ns = (voOnStreamMP_ns *)pUserData;
    
    int result = [pOnStreamMP_ns voHandleMPOnRequestC:nID withParam1:pParam1 withParam2:pParam2];
    return result;
}

- (int)voHandleMPOnRequestC:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == m_pOnRequestDelegate) {
        return VOOSMP_ERR_None;
    }
    
    return [m_pOnRequestDelegate voHandleMPOnRequest:nID withParam1:pParam1 withParam2:pParam2];
}

- (id) init:(int)nPlayerType pInitParam:(void *)pInitParam nInitParamFlag:(int)nInitParamFlag
{
    if (nil != (self = [super init])) 
    {
        m_pOSApiSet = new voOnStreamMediaPlayerAPI;
        
        // judge alloc
        if (NULL == m_pOSApiSet) {
            [self release];
            return nil;
        }
        
        memset(m_pOSApiSet, 0, sizeof(voOnStreamMediaPlayerAPI));
        
        voGetOnStreamMediaPlayerAPI((voOnStreamMediaPlayerAPI *)m_pOSApiSet);
        
        if (NULL == m_pOSApiSet->Init) {
            [self release];
            return nil;
        }
        
        int iRet = m_pOSApiSet->Init(&m_pOSHandle, nPlayerType, pInitParam, nInitParamFlag);
        
        if (VOOSMP_ERR_None != iRet) {
            [self release];
            return nil;
        }
        
        if ((NULL == m_pOSApiSet->SetParam) || (NULL == m_pOSHandle)) {
            [self release];
            return nil;
        }
        
        m_pDelegate = nil;
        VOOSMP_LISTENERINFO cInfo;
        cInfo.pListener = voEventCallback_MP_ns;
        cInfo.pUserData = self;
        m_pOSApiSet->SetParam(m_pOSHandle, VOOSMP_PID_LISTENER, &cInfo);
        
        m_pOnRequestDelegate = nil;
        VOOSMP_LISTENERINFO cOnRequestInfo;
        cOnRequestInfo.pListener = voEventOnRequest_MP_ns;
        cOnRequestInfo.pUserData = self;
        m_pOSApiSet->SetParam(m_pOSHandle, VOOSMP_PID_ONREQUEST_LISTENER, &cOnRequestInfo);
    }

    return self;
}

- (void) dealloc
{
    if ((NULL != m_pOSApiSet) && (NULL != m_pOSApiSet->Uninit) && (NULL != m_pOSHandle)) {
        m_pOSApiSet->Uninit(m_pOSHandle);
        m_pOSHandle = NULL;
    }
    
    if (NULL != m_pOSApiSet) {
        delete m_pOSApiSet;
        m_pOSApiSet = NULL;
    }
    
    [super dealloc];
}

- (void)setDelegate:(id <voOnStreamMPDelegate>)delegate
{
    m_pDelegate = delegate;
}

- (void)setOnRequestDelegate:(id <voOnStreamMPOnRequestDelegate>)delegate
{
    m_pOnRequestDelegate = delegate;
}

- (int) setView:(void *)pView
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SetView) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SetView(m_pOSHandle, pView);
}

- (int) open:(void *)pSource nFlag:(int)nFlag nSourceType:(int)nSourceType
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->Open) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->Open(m_pOSHandle, pSource, nFlag, nSourceType);
}

- (int) getProgramCount:(int *)pProgramCount
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetProgramCount) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
        
    }
    
    return m_pOSApiSet->GetProgramCount(m_pOSHandle, pProgramCount);
}

- (int) getProgramInfo:(int)nProgramIndex ppProgramInfo:(VOOSMP_SRC_PROGRAM_INFO**)ppProgramInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetProgramInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetProgramInfo(m_pOSHandle, nProgramIndex, ppProgramInfo);
}

- (int) getCurTrackInfo:(int)nTrackType ppTrackInfo:(VOOSMP_SRC_TRACK_INFO**)ppTrackInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetCurTrackInfo) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetCurTrackInfo(m_pOSHandle, nTrackType, ppTrackInfo);
}

- (int) selectProgram:(int)nProgram
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SelectProgram) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SelectProgram(m_pOSHandle, nProgram);
}

- (int) selectStream:(int)nStream
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SelectStream) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SelectStream(m_pOSHandle, nStream);
}

- (int) selectTrack:(int)nTrack
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SelectTrack) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SelectTrack(m_pOSHandle, nTrack);
}

- (int) selectLanguage:(int)nIndex
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SelectLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SelectLanguage(m_pOSHandle, nIndex);
}

- (int) getLanguage:(VOOSMP_SUBTITLE_LANGUAGE_INFO**)ppLangInfo
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetLanguage(m_pOSHandle, ppLangInfo);
}

- (int) close
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->Close) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->Close(m_pOSHandle);
}

- (int) run
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->Run) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->Run(m_pOSHandle);
}

- (int) pause
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->Pause) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->Pause(m_pOSHandle);
}

- (int) stop
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->Stop) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->Stop(m_pOSHandle);
}

- (int) getPos
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetPos(m_pOSHandle);
}

- (int) setPos:(int)nPos
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SetPos(m_pOSHandle, nPos);
}

- (int) getDuration:(long long*)pDuration
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetDuration) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetDuration(m_pOSHandle, pDuration);
}

- (int) getParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->GetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->GetParam(m_pOSHandle, nParamID, pValue);
}

- (int) setParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == m_pOSApiSet) || (NULL == m_pOSApiSet->SetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return m_pOSApiSet->SetParam(m_pOSHandle, nParamID, pValue);
}

@end
