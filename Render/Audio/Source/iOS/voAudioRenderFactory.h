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
 * @file voAudioRenderFactory.h
 * VisualOn Audio Render Factory header file
 *
 * VisualOn Audio Render Factory header file
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __VO_AUDIO_RENDER_FACTORY__
#define __VO_AUDIO_RENDER_FACTORY__

#import <list>
#import "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CBaseAudioRender;

class voAudioRenderFactory
{
public:
	static voAudioRenderFactory* Instance();
    
	CBaseAudioRender* CreateAudioRender();
	void ReclaimAudioRender(CBaseAudioRender* pRender);
protected:
    static void S_AudioSessionListener(void *inClientData, int nID, void *pParam1, void *pParam2);
    
    virtual void OnAudioSessionInterruptionListener(int inInterruptionState);
    
private:
	voAudioRenderFactory();
	voAudioRenderFactory& operator=(const voAudioRenderFactory&);
	virtual ~voAudioRenderFactory();
    
private:
    void * m_pAudioSession;
	
	static voAudioRenderFactory *m_pRenderManager;
    
	static voCMutex m_cRenderMutex;
	std::list<CBaseAudioRender *> m_cRenderList;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif
