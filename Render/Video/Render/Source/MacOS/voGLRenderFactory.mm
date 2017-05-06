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
	voGLRenderBase* pRender = NULL;

    pRender = [[voGLRenderBase layer] retain];
    //[pRender setLock:&m_cRenderMutex];
    
    m_cRenderList.push_back(pRender);
    
	return pRender;
}

void voGLRenderFactory::ReclaimGLRender(voGLRenderBase* pRender)
{
	voCAutoLock lock (&m_cRenderMutex);
	
	std::list<voGLRenderBase *>::iterator itr = m_cRenderList.begin();
	while (itr != m_cRenderList.end()) {
		if (pRender == *itr) {
			[pRender release];
            
            m_cRenderList.remove(*itr);
			break;
		}
		
		++itr;
	}
}

bool voGLRenderFactory::supportsFastTextureUpload()
{
    return false;
//#if TARGET_IPHONE_SIMULATOR
//    return false;
//#else
//    return (CVOpenGLESTextureCacheCreate != NULL);
//#endif
}

/*-------------------------------------------------------------------------------------*
 * Internal methods
 *-------------------------------------------------------------------------------------*/
voGLRenderFactory::voGLRenderFactory()
{
}


voGLRenderFactory::~voGLRenderFactory()
{
    while (0 < m_cRenderList.size()) {
        
        voGLRenderBase *pRender = m_cRenderList.front();
        if (nil != pRender) {
            
			[pRender release];
        }
        m_cRenderList.pop_front();
    }
}