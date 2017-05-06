/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
 *																		*
 ************************************************************************/
/*******************************************************************************
    File:		voOnStreamEngine_ns.h
 
    Contains:	VisualOn OnStream Media Player Engine cpp file
 
    Written by:	Jeff
 
    Change History (most recent first):
    2012-07-25		Jeff			Create file
 *******************************************************************************/

#import "voOnStreamEngine_ns.h"
#include "voOnStreamEngine.h"

#define VO_EngineApi ((voOnStreamEngnAPI *)m_pOSApiSet)

@interface voOnStreamEngine_ns ()
// Properties that don't need to be seen by the outside world.

- (int)voHandleEngineEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
@end

@implementation voOnStreamEngine_ns


int voEventCallback_Engine_ns(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	if (pUserData == nil) 
	{
		return -1;
	}
	
	voOnStreamEngine_ns * pOnStreamEngine_ns = (voOnStreamEngine_ns *)pUserData;
	
	int result = [pOnStreamEngine_ns voHandleEngineEvent:nID withParam1:pParam1 withParam2:pParam2];
	return result;
}

- (int)voHandleEngineEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == m_pDelegate) {
        return VOOSMP_ERR_None;
    }
    
    return [m_pDelegate voHandleEngineEvent:nID withParam1:pParam1 withParam2:pParam2];
}

- (id) init:(int)nPlayerType pInitParam:(void *)pInitParam nInitParamFlag:(int)nInitParamFlag
{
    if (nil != (self = [super init])) 
    {
        m_pOSApiSet = new voOnStreamEngnAPI;
        
        memset(m_pOSApiSet, 0, sizeof(voOnStreamEngnAPI));
        
        voGetOnStreamEngnAPI((voOnStreamEngnAPI *)m_pOSApiSet);
        
        if (NULL == VO_EngineApi->Init) {
            [self release];
            return nil;
        }
        
        int iRet = VO_EngineApi->Init(&m_pOSHandle, nPlayerType, pInitParam, nInitParamFlag);
        
        if (VOOSMP_ERR_None != iRet) {
            [self release];
            return nil;
        }
        
        m_pDelegate = nil;
        
        VOOSMP_LISTENERINFO _cInfo;
        _cInfo.pListener = voEventCallback_Engine_ns;
        _cInfo.pUserData = self;
        
        if ((NULL == VO_EngineApi->SetParam) || (NULL == m_pOSHandle)) {
            [self release];
            return nil;
        }
        
        VO_EngineApi->SetParam(m_pOSHandle, VOOSMP_PID_LISTENER, &_cInfo);
    }
    
    return self;
}

- (void) dealloc
{
	if ((NULL != VO_EngineApi) && (NULL != VO_EngineApi->Uninit) && (NULL != m_pOSHandle)) {
        VO_EngineApi->Uninit(m_pOSHandle);
        m_pOSHandle = NULL;
    }
    
    if (NULL != VO_EngineApi) {
        delete VO_EngineApi;
        m_pOSApiSet = NULL;
    }
    
	[super dealloc];
}


- (void)setDelegate:(id <voOnStreamEngineDelegate>)delegate
{
    m_pDelegate = delegate;
}

- (int) setView:(void *)pView
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->SetView) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->SetView(m_pOSHandle, pView);
}

- (int) open:(void *)pSource nFlag:(int)nFlag
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->Open) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->Open(m_pOSHandle, pSource, nFlag);
}

- (int) close
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->Close) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->Close(m_pOSHandle);
}

- (int) run
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->Run) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->Run(m_pOSHandle);
}

- (int) pause
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->Pause) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->Pause(m_pOSHandle);
}

- (int) stop
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->Stop) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->Stop(m_pOSHandle);
}

- (int) getPos
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->GetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->GetPos(m_pOSHandle);
}

- (int) setPos:(int)nPos
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->SetPos) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->SetPos(m_pOSHandle, nPos);
}

- (int) selectLanguage:(int)nIndex
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->SelectLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->SelectLanguage(m_pOSHandle, nIndex);
}

- (int) getLanguage:(VOOSMP_SUBTITLE_LANGUAGE_INFO**)ppLangInfo
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->GetLanguage) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->GetLanguage(m_pOSHandle, ppLangInfo);
}

- (int) getParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->GetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->GetParam(m_pOSHandle, nParamID, pValue);
}

- (int) setParam:(int)nParamID pValue:(void *)pValue
{
    if ((NULL == VO_EngineApi) || (NULL == VO_EngineApi->SetParam) || (NULL == m_pOSHandle)) {
        return VOOSMP_ERR_Pointer;
    }
    
    return VO_EngineApi->SetParam(m_pOSHandle, nParamID, pValue);
}

@end
