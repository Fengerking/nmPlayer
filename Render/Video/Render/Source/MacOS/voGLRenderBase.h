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
 * @file voGLRenderBase.h
 * GL render of Mac OS
 *
 * GL render of Mac OS
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef _GL_RENDER_MAC_H__
#define _GL_RENDER_MAC_H__

#import <Cocoa/Cocoa.h>
#import "voCMutex.h"
#import "voIVCommon.h"
#import "voVideoRender.h"

//#define _READ_YUV_FILE

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static const int VO_GL_RET_OK         = 0;
static const int VO_GL_RET_FAIL       = 1;
static const int VO_GL_RET_IMPLEMENT  = 2;
static const int VO_GL_RET_RETRY      = 3;
static const int VO_GL_RET_ERR_STATUS = 4;

static const int VO_GL_SUPPORT_NONE   = 0;
static const int VO_GL_SUPPORT_RGB    = 0x1;
static const int VO_GL_SUPPORT_Y_U_V  = 0x1 << 1;
static const int VO_GL_SUPPORT_Y_UV   = 0x1 << 2;

typedef enum
{
	VO_GL_ROTATION_0,
    VO_GL_ROTATION_0FLIP,
    VO_GL_ROTATION_180,
    VO_GL_ROTATION_180FLIP,
}
VO_GL_ROTATION;

static const int GL_BUFFER_COUNT = 2;
static const int GL_INVAILD = -1;

typedef enum
{
	VO_RENDER_INIT        = 0,
    VO_RENDER_CONVERTING  = 1,
    VO_RENDER_READY       = 2,
    VO_RENDER_RENDERING   = 3,
    VO_RENDER_BE_RENDERED = 4,
}VO_RENDER_STATUS;

typedef struct
{
    unsigned char* pBuffer;
    VO_RENDER_STATUS eRender;
}VO_RENDER_BUFFER;

@interface voGLRenderBase : CAOpenGLLayer {
@protected
    CALayer* _layerSet;
    voCMutex *_renderLock;
    	
	int		m_nTextureWidth;
	int		m_nTextureHeight;
    
    int     m_nZoomMode;
    int     m_nRatio;
    
	VO_RENDER_BUFFER m_pFrameData[GL_BUFFER_COUNT];
    
    voCMutex m_cBufferMutex;
    int m_iLastOkBuffer;
    int m_iLastRenderBuffer;
    VO_RENDER_STATUS m_iLastStatus;
    bool m_bNeedRedraw;
    
    int frame_w;
    int frame_h;
    
    GLfloat frame_draw_x;
    GLfloat frame_draw_y;
    GLfloat frame_draw_h;
    GLfloat frame_draw_w;
    
    GLuint g_textureID;
    
    voCMutex *m_pLock;
    
    VOVIDEO_EVENT_CALLBACKPROC m_fEventCallBack;
    VO_PTR m_pEventUserData;
    
    GLfloat m_fTextureVertices[8];
    GLfloat m_fSquareVertices[8];
    
#ifdef _READ_YUV_FILE
    CVOpenGLTextureRef texture;
    CVOpenGLTextureCacheRef texture_cache;
    
    GLuint m_nTexturePlanarY;
    GLuint m_nTexturePlanarU;
    GLuint m_nTexturePlanarV;
    
    GLint m_nTextureUniformY;
    GLint m_nTextureUniformU;
    GLint m_nTextureUniformV;
    
    GLuint m_nProgramHandle;
    
    int m_nPositionSlot;
    int m_nTexCoordSlot;
    
    GLuint m_nColorRenderBuffer;
    GLuint m_nFrameBuffer;
#endif
    
    int m_nLastRenderTime;
    
    int m_nFrameDiffTime;
}

@property (nonatomic, retain) CALayer *layerSet;

-(id) init;
-(int) setView:(CALayer *)layer;
-(int) setEventCallBack:(VOVIDEO_EVENT_CALLBACKPROC)callback userData:(void *)userData;

-(void) setVideoSize:(int)nInputWidth InputHeight:(int)nInputHeight;

-(unsigned char*) lockFrameData;
-(void) unlockFrameData:(BOOL)bWriteOk unlockBuffer:(unsigned char* )unlockBuffer;

-(int) getTextureWidth;
-(int) getTextureHeight;

-(int) getSupportType;
-(int) setRotation:(VO_GL_ROTATION)type;

-(int) renderFrameData;
-(int) renderYUV:(VO_VIDEO_BUFFER *)pVideoBuffer;

-(int) redraw;

-(int) setDispType:(VO_IV_ZOOM_MODE)zoomMode ration:(VO_IV_ASPECT_RATIO)ration;

-(void) setFrameDiffTime:(int)time;

//pravate:
-(void)	drawInMainThread;

-(void) deleteBuffer;

-(void) reInitVideoBuffer;

-(void) updateVertices;

-(Rect) getDrawRect;

@end

#endif
