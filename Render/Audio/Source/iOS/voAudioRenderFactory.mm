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
 * @file voAudioRenderFactory.mm
 * VisualOn Audio Render Factory cpp file
 *
 * VisualOn Audio Render Factory cpp file
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import "voAudioRenderFactory.h"
#import "voLog.h"
#import "voAudioSession.h"

#if defined(_USE_OLD_AUDIO_RENDER)
#import "CAudioUnitRender.h"
#else
#import "CAudioUnitRenderEx.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voAudioRenderFactory* voAudioRenderFactory::m_pRenderManager = NULL;
voCMutex voAudioRenderFactory::m_cRenderMutex;

voAudioRenderFactory* voAudioRenderFactory::Instance() {
    voCAutoLock lock (&m_cRenderMutex);
    
	if (NULL == m_pRenderManager) {
		m_pRenderManager = new voAudioRenderFactory();
	}
	return m_pRenderManager;
}

CBaseAudioRender* voAudioRenderFactory::CreateAudioRender()
{
    voCAutoLock lock (&m_cRenderMutex);
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

#if defined(_USE_OLD_AUDIO_RENDER)
	CBaseAudioRender* pRender = new CAudioUnitRender(NULL, NULL);
#else
    CBaseAudioRender* pRender = new CAudioUnitRenderEx(NULL, NULL);
#endif
    
    if (NULL == pRender) {
        [pool release];
        return pRender;
    }
    
    if (0 == m_cRenderList.size()) {
        NSError *err = nil;
        if (NO == [[AVAudioSession sharedInstance] setActive:YES error:&err]) {
            VOLOGE("setActive error:%d", err.code);
        }
    }
    
    m_cRenderList.push_back(pRender);
    
    [pool release];
    
	return pRender;
}

void voAudioRenderFactory::ReclaimAudioRender(CBaseAudioRender* pRender)
{
	voCAutoLock lock (&m_cRenderMutex);
	
	std::list<CBaseAudioRender *>::iterator itr = m_cRenderList.begin();
	while (itr != m_cRenderList.end()) {
		if (pRender == *itr) {
            delete pRender;
            m_cRenderList.remove(*itr);
			break;
		}
		
		++itr;
	}
    
    if (0 == m_cRenderList.size()) {
        NSError *err = nil;
        if (NO == [[AVAudioSession sharedInstance] setActive:NO error:&err]) {
            VOLOGE("setActive error:%d", err.code);
        }
    }
}

void voAudioRenderFactory::S_AudioSessionListener(void *inClientData, int nID, void *pParam1, void *pParam2)
{
    voAudioRenderFactory *pFactory = (voAudioRenderFactory *)inClientData;
	
	if (NULL == pFactory) {
		VOLOGE("S_AudioSessionInterruptionListener pFactory null\n");
		return;
	}
	
	pFactory->OnAudioSessionInterruptionListener(nID);
}

void voAudioRenderFactory::OnAudioSessionInterruptionListener(int inInterruptionState)
{
	VOLOGI("voAudioRenderFactory AudioSessionInterruptionListener :%d\n", inInterruptionState);
    
    voCAutoLock lock (&m_cRenderMutex);
    
//    if (voAudioSessionEndInterruption == inInterruptionState) {
//        NSError *err = nil;
//        if (NO == [[AVAudioSession sharedInstance] setActive:YES error:&err]) {
//            VOLOGE("setActive error:%d", err.code);
//        }
//    }
    
    std::list<CBaseAudioRender *>::iterator itr = m_cRenderList.begin();
	while (itr != m_cRenderList.end()) {
		
        if (NULL != *itr) {
			(*itr)->AudioInterruption(inInterruptionState);
        }
        
		++itr;
	}
}

/*-------------------------------------------------------------------------------------*
 * Internal methods
 *-------------------------------------------------------------------------------------*/
voAudioRenderFactory::voAudioRenderFactory()
:m_pAudioSession(NULL)
{
    m_pAudioSession = [[voAudioSession alloc] init];
    
    if (nil != m_pAudioSession) {
        voAudioSessionListenerInfo cInfo;
        memset(&cInfo, 0, sizeof(cInfo));
        cInfo.pListener = S_AudioSessionListener;
        cInfo.pUserData = this;
        
        [(voAudioSession *)m_pAudioSession setDelegateCB:&cInfo];
    }
}

voAudioRenderFactory::~voAudioRenderFactory()
{
    voCAutoLock lock (&m_cRenderMutex);
    
    while (0 < m_cRenderList.size()) {
        
        CBaseAudioRender *pRender = m_cRenderList.front();
        if (nil != pRender) {
			delete pRender;
        }
        m_cRenderList.pop_front();
    }
    
    if (nil != m_pAudioSession) {
        [(voAudioSession *)m_pAudioSession release];
        m_pAudioSession = nil;
    }
}