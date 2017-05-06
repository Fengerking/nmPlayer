/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderFactory
 
 Contains:    VisualOn OpenGL ES Render Factory cpp file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-17   Jeff            Create file
 *******************************************************************************/

#import "voGLRenderFactory.h"
#import "voGLRenderES1.h"
#import "voGLRenderES2.h"

#ifndef _UNUSE_FTU
#import "voGLRenderES2_FTU.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voGLRenderFactory* voGLRenderFactory::m_pRenderManager = NULL;
voCMutex voGLRenderFactory::m_cRenderMutex;

voGLRenderFactory* voGLRenderFactory::Instance() {
    voCAutoLock lock (&m_cRenderMutex);
    
	if (NULL == m_pRenderManager) {
		m_pRenderManager = new voGLRenderFactory();
	}
	return m_pRenderManager;
}

voGLRenderBase* voGLRenderFactory::CreateGLRender()
{
    voCAutoLock lock (&m_cRenderMutex);
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	voGLRenderBase* pRender = NULL;

    EAGLContext* pContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2]; // NULL;//
    
    if (nil != pContext)
    {
#ifndef _UNUSE_FTU
        if (supportsFastTextureUpload()) {
            pRender = new voGLRenderES2_FTU(pContext);
        }
#endif
        if (NULL == pRender) {
            pRender = new voGLRenderES2(pContext);
        }
    }
    else
    {
        pContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        pRender = new voGLRenderES1(pContext);
    }
    
    pRender->SetLock(&m_cRenderMutex);
    pRender->init();
    
    m_cRenderList.push_back(pRender);
    
    [pool release];
    
	return pRender;
}

void voGLRenderFactory::ReclaimGLRender(voGLRenderBase* pRender)
{
	voCAutoLock lock (&m_cRenderMutex);
	
	std::list<voGLRenderBase *>::iterator itr = m_cRenderList.begin();
	while (itr != m_cRenderList.end()) {
		if (pRender == *itr) {
            EAGLContext* pContext = pRender->GetGLContext();
			delete pRender;
            
            if ([EAGLContext currentContext] == pContext) {
                [EAGLContext setCurrentContext: nil];
            }
            [pContext release];
            
            m_cRenderList.remove(*itr);
			break;
		}
		
		++itr;
	}
}

bool voGLRenderFactory::supportsFastTextureUpload()
{
#if TARGET_IPHONE_SIMULATOR || defined _UNUSE_FTU
    return false;
#else
    return (CVOpenGLESTextureCacheCreate != NULL);
#endif
}

/*-------------------------------------------------------------------------------------*
 * Internal methods
 *-------------------------------------------------------------------------------------*/
voGLRenderFactory::voGLRenderFactory()
{
}

voGLRenderFactory::~voGLRenderFactory()
{
    voCAutoLock lock (&m_cRenderMutex);
    
    while (0 < m_cRenderList.size()) {
        
        voGLRenderBase *pRender = m_cRenderList.front();
        if (nil != pRender) {
            
            EAGLContext* pContext = pRender->GetGLContext();
			delete pRender;
            
            if ([EAGLContext currentContext] == pContext) {
                [EAGLContext setCurrentContext: nil];
            }
            [pContext release];
        }
        m_cRenderList.pop_front();
    }
}