/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderFactory
 
 Contains:    VisualOn OpenGL ES Render Factory header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-17   Jeff            Create file
 *******************************************************************************/

#ifndef __VO_GLRENDER_MANAGER__
#define __VO_GLRENDER_MANAGER__

#import <list>
#import "voGLRenderBase.h"
#import "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voGLRenderFactory
{
public:
	static voGLRenderFactory* Instance();
    
	voGLRenderBase* CreateGLRender();
	void ReclaimGLRender(voGLRenderBase* pRender);
	
    bool supportsFastTextureUpload();
    
private:
	voGLRenderFactory();
	voGLRenderFactory& operator=(const voGLRenderFactory&);
	virtual ~voGLRenderFactory();
    
private:
	
	static voGLRenderFactory *m_pRenderManager;
    
	static voCMutex m_cRenderMutex;
	std::list<voGLRenderBase *> m_cRenderList;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif
