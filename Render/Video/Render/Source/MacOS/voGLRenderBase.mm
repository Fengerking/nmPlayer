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
 * @file voGLRenderBase.cpp
 * GL render of Mac OS
 *
 * GL render of Mac OS
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import "voGLRenderBase.h"
#import <OpenGL/glu.h>
#import "voGLRenderBase.h"
#import "voLog.h"
#import "voOSFunc.h"

//#define _FRAME_LOG

#ifdef _READ_YUV_FILE
static const GLchar *G_VO_VERTEX_SHADER_Y_U_V = " \
attribute vec4 position; \
attribute vec4 textureCoordinate; \
varying vec2 coordinate; \
void main() \
{ \
gl_Position = position; \
coordinate = textureCoordinate.xy; \
}";


static const GLchar *G_VO_FRAGMENT_SHADER_Y_U_V = " \
  float; \
uniform sampler2D SamplerY; \
uniform sampler2D SamplerU; \
uniform sampler2D SamplerV; \
\
varying  vec2 coordinate; \
\
void main() \
{ \
 vec3 yuv,yuv1; \
 vec3 rgb; \
\
yuv.x = texture2D(SamplerY, coordinate).r; \
\
yuv.y = texture2D(SamplerU, coordinate).r-0.5; \
\
yuv.z = texture2D(SamplerV, coordinate).r-0.5 ; \
\
rgb = mat3(      1,       1,      1, \
0, -.34414, 1.772, \
1.402, -.71414,      0) * yuv; \
\
gl_FragColor = vec4(rgb, 1); \
}";
#endif

@implementation voGLRenderBase

@synthesize layerSet = _layerSet;

#ifdef _READ_YUV_FILE

-(int) DeleteRenderBuffer
{
    if (m_nColorRenderBuffer)
	{
		glDeleteRenderbuffers(1, &m_nColorRenderBuffer);
		m_nColorRenderBuffer = 0;
	}
    return VO_GL_RET_OK;
}

-(int) DeleteFrameBuffer
{
	if (m_nFrameBuffer)
	{
		glDeleteFramebuffers(1, &m_nFrameBuffer);
		m_nFrameBuffer = 0;
	}
    
    return VO_GL_RET_OK;
}

-(int) SetupFrameBuffer
{
    [self DeleteFrameBuffer];
    glGenFramebuffers(1, &m_nFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, m_nColorRenderBuffer);
    
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nColorRenderBuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        VOLOGE("Failure with framebuffer generation");
		return VO_GL_RET_FAIL;
	}
    
    return VO_GL_RET_OK;
}

-(int) SetupRenderBuffer
{
    [self DeleteRenderBuffer];
    glGenRenderbuffers(1, &m_nColorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_nColorRenderBuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_nColorRenderBuffer);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nColorRenderBuffer);
    
    //[m_pContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)m_pVideoView.layer];
    
    return VO_GL_RET_OK;
}

-(GLuint) CompileShader:(const GLchar *)pBuffer shaderType:(GLenum)shaderType
{
    NSString* shaderString = [NSString stringWithFormat:@"%s", pBuffer];
    
    // 2
    GLuint shaderHandle = glCreateShader(shaderType);
    
    // 3
    const char* shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringLength = [shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength);
    
    // 4
    glCompileShader(shaderHandle);
    
    // 5
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
#ifdef _VOLOG_ERROR
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        VOLOGE("%s", [messageString UTF8String]);
#endif
        return -1;
    }
    
    return shaderHandle;
}

-(int) CompileAllShaders
{
    GLuint vertexShader = [self CompileShader:G_VO_VERTEX_SHADER_Y_U_V shaderType:GL_VERTEX_SHADER];
    
    GLuint fragmentShader = [self CompileShader:G_VO_FRAGMENT_SHADER_Y_U_V shaderType:GL_FRAGMENT_SHADER];
    
    if (m_nProgramHandle) {
        glDeleteProgram(m_nProgramHandle);
        m_nProgramHandle = 0;
    }
    
    // Setup program
    m_nProgramHandle = glCreateProgram();
    glAttachShader(m_nProgramHandle, vertexShader);
    glAttachShader(m_nProgramHandle, fragmentShader);
    glLinkProgram(m_nProgramHandle);
    
    // Link program
    GLint linkSuccess;
    glGetProgramiv(m_nProgramHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
#ifdef _VOLOG_ERROR
        GLchar messages[256];
        glGetProgramInfoLog(m_nProgramHandle, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        VOLOGE("%s", [messageString UTF8String]);
#endif
        return VO_GL_RET_FAIL;
    }
    
    // Use Program
    glUseProgram(m_nProgramHandle);
    
    // Get Attrib
    m_nPositionSlot = glGetAttribLocation(m_nProgramHandle, "position");
    glEnableVertexAttribArray(m_nPositionSlot);
    
    m_nTexCoordSlot = glGetAttribLocation(m_nProgramHandle, "textureCoordinate");
    glEnableVertexAttribArray(m_nTexCoordSlot);
    
    m_nTextureUniformY = glGetUniformLocation(m_nProgramHandle, "SamplerY");
    m_nTextureUniformU = glGetUniformLocation(m_nProgramHandle, "SamplerU");
    m_nTextureUniformV = glGetUniformLocation(m_nProgramHandle, "SamplerV");
    
    // Release vertex and fragment shaders.
    if (vertexShader) {
        glDetachShader(m_nProgramHandle, vertexShader);
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDetachShader(m_nProgramHandle, fragmentShader);
        glDeleteShader(fragmentShader);
    }
    
    return VO_GL_RET_OK;
}

-(int) GLTexImage2D:(GLuint)nTexture pDate:(Byte *)pDate nWidth:(int)nWidth nHeight:(int)nHeight
{
    glBindTexture(GL_TEXTURE_2D, nTexture);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nWidth, nHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pDate);
    
    return VO_GL_RET_OK;
}

-(int) DeleteTexture
{
    if (m_nTexturePlanarY)
	{
		glDeleteTextures(1, &m_nTexturePlanarY);
		m_nTexturePlanarY = 0;
	}
    
    if (m_nTexturePlanarU)
	{
		glDeleteTextures(1, &m_nTexturePlanarU);
		m_nTexturePlanarU = 0;
	}
    
    if (m_nTexturePlanarV)
	{
		glDeleteTextures(1, &m_nTexturePlanarV);
		m_nTexturePlanarV = 0;
	}
    
    return VO_GL_RET_OK;
}

-(int) SetupTexture
{
    [self DeleteTexture];
    glGenTextures(1, &m_nTexturePlanarY);
    glGenTextures(1, &m_nTexturePlanarU);
    glGenTextures(1, &m_nTexturePlanarV);
    return VO_GL_RET_OK;
}
#endif

-(CGLPixelFormatObj)copyCGLPixelFormatForDisplayMask:(uint32_t)mask
{
	return [super copyCGLPixelFormatForDisplayMask:mask];
}

-(CGLContextObj)copyCGLContextForPixelFormat:(CGLPixelFormatObj)pixelFormat
{
    return [super copyCGLContextForPixelFormat:pixelFormat];
}

-(BOOL)canDrawInCGLContext:(CGLContextObj)glContext pixelFormat:(CGLPixelFormatObj)pixelFormat forLayerTime:(CFTimeInterval)timeInterval displayTime:(const CVTimeStamp *)timeStamp
{
    int nTimeNow = voOS_GetSysTime();
    
#ifdef _VOLOG_INFO
    static int nLastCallIn = voOS_GetSysTime();
    if (nTimeNow - nLastCallIn > 22) {
        VOLOGI("canDrawInCGLContext enter diff time:%d, off:%d", nTimeNow - nLastCallIn, nTimeNow - m_nLastRenderTime);
    }
    nLastCallIn = nTimeNow;
#endif
    
    if ((m_nFrameDiffTime != 0)
            && ((nTimeNow - m_nLastRenderTime) >= (m_nFrameDiffTime - 8))) {
        return YES;
    }
    
    if (m_bNeedRedraw) {
        return YES;
    }
    
    GLsizei width = CGRectGetWidth([self bounds]), height = CGRectGetHeight([self bounds]);
    
    if (width!=frame_w || height!=frame_h) {
        return YES;
    }
    
    int iIndexToRender = 0;
    if (0 == m_iLastRenderBuffer) {
        iIndexToRender = 1;
    }
    
    if (m_pFrameData[iIndexToRender].eRender != VO_RENDER_READY) {
        return NO;
    }
    
	return YES;
}

-(void)drawInCGLContext:(CGLContextObj)ctx pixelFormat:(CGLPixelFormatObj)pf forLayerTime:(CFTimeInterval)t displayTime:(const CVTimeStamp *)ts
{
#ifdef _VOLOG_INFO
    static int nTimeLastStart = voOS_GetSysTime();
    
    int nTimeTmp = voOS_GetSysTime();
    if (nTimeTmp - nTimeLastStart > (m_nFrameDiffTime + 10)) {
        VOLOGI("drawInCGLContext enter diff time:%d", nTimeTmp - nTimeLastStart);
    }
    nTimeLastStart = nTimeTmp;
#endif
    
    GLsizei width = CGRectGetWidth([self bounds]), height = CGRectGetHeight([self bounds]);
    
    if (width!=frame_w || height!=frame_h
#ifndef _READ_YUV_FILE
        || !g_textureID
#endif
        )
    {
        frame_w=width;
        frame_h=height;
        glViewport(0, 0, width, height);
        glEnable( GL_TEXTURE_2D );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glScalef(1.0f, -1.0f, 1.0f);
        glOrtho(0, width, 0, height, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

#ifndef _READ_YUV_FILE
        if(g_textureID)
        {
            glDeleteTextures( 1, &g_textureID );
        }
        g_textureID=0;
        glGenTextures( 1, &g_textureID );
        glBindTexture( GL_TEXTURE_2D, g_textureID );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#else
        [self SetupTexture];
        [self CompileAllShaders];
        
        [self SetupRenderBuffer];
        [self SetupFrameBuffer];
#endif
        glColor4f(0.9, 0.3, 0.4, 1.00);
        [self updateVertices];
    }
    
    
    int iIndexToRender = 0;
    if (0 == m_iLastRenderBuffer) {
        iIndexToRender = 1;
    }
    
    {
        int i = 0;
        while (m_pFrameData[iIndexToRender].eRender != VO_RENDER_READY) {
            voOS_Sleep(2);
            ++i;
            if (i >= 5) {
                break;
            }
        }
        
        voCAutoLock lock (&m_cBufferMutex);
        
        // break if be reset
        if (GL_INVAILD == m_iLastOkBuffer) {
            
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            
            VOLOGI("iIndexToRender GL_INVAILD");
            
            return [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
        }
        
        // Use last ready buffer if still not new buffer ready
        if (m_pFrameData[iIndexToRender].eRender != VO_RENDER_READY) {
            VOLOGI("m_iLastRenderBuffer:%d iIndexToRender:%d type:%d change to :%d", m_iLastRenderBuffer, iIndexToRender, m_pFrameData[iIndexToRender].eRender, m_iLastOkBuffer);
            iIndexToRender = m_iLastOkBuffer;
        }
        
        m_pFrameData[iIndexToRender].eRender = VO_RENDER_RENDERING;
    }

    
#ifdef _VOLOG_INFO
    static int nTimeLastR = voOS_GetSysTime();
    
    int nTimeNow = voOS_GetSysTime();
    if (nTimeNow - nTimeLastR > (m_nFrameDiffTime + 8)) {
        VOLOGI("drawInCGLContext real render diff time:%d", nTimeNow - nTimeLastR);
    }
    nTimeLastR = nTimeNow;
#endif
    
#ifdef _FRAME_LOG
    VOLOGI("_FRAME_LOG iIndexToRender :%d m_iLastRenderBuffer:%d", iIndexToRender, m_iLastRenderBuffer);
#endif
    
//#ifdef _READ_YUV_FILE
//   
////    static FILE* m_hYUV = fopen("/Users/huangjiafa/Desktop/640x480'25f.yuv", "rb");
////    int nTmpW = 640;
////    int nTmpH = 480;
////    int in_stride = 640;
//    
//    static FILE* m_hYUV = fopen("/Volumes/MacData/Movie/yuv/15491755_MPEG4_MPEG4V'S1_AAC'LC_6081K'16s44ms'720x480'30f'2c'44KHz.yuv", "rb");
//    int nTmpW = 720;
//    int nTmpH = 480;
//    int in_stride = 720;
//    
//    if (NULL == m_hYUV) {
//        return [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
//    }
//        
//    static unsigned char * pRead = NULL;
//    if (NULL == pRead) {
//        pRead = new unsigned char[in_stride*nTmpH*3/2];
//    }
//    
//    size_t iRead = fread(pRead, 1, in_stride*nTmpH*3/2, m_hYUV);
//    
//    if (iRead != in_stride*nTmpH*3/2) {
//        fseek(m_hYUV, 0, SEEK_SET);
//    }
//    
//    //glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nTmpW, nTmpH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pRead);
//    
//    [self GLTexImage2D:m_nTexturePlanarY pDate:pRead nWidth:nTmpW nHeight:nTmpH];
//    [self GLTexImage2D:m_nTexturePlanarU pDate:(pRead + (nTmpW * nTmpH)) nWidth:nTmpW / 2 nHeight:nTmpH / 2];
//    [self GLTexImage2D:m_nTexturePlanarV pDate:(pRead + (nTmpW * nTmpH * 5 / 4)) nWidth:nTmpW / 2 nHeight:nTmpH / 2];
//    
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarY);
//    glUniform1i(m_nTextureUniformY, 0);
//    
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarU);
//    glUniform1i(m_nTextureUniformU, 1);
//    
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarV);
//    glUniform1i(m_nTextureUniformV, 2);
//    
//    
//    glBegin(GL_QUADS);
//    glColor4f(1,1,1,1);
//    glTexCoord2f(0.0, 0.0);
//    glVertex3f(0,      0,      -1.0f);
//    glTexCoord2f(1.0, 0.0);
//    glVertex3f(width,   0,      -1.0f);
//    glTexCoord2f(1.0, 1.0);
//    glVertex3f(width,   height, -1.0f);
//    glTexCoord2f(0.0, 1.0);
//    glVertex3f(0,       height, -1.0f);
//    glEnd();
//    
//    return;
//    
////    // Set the current context to the one given to us.
////	CGLSetCurrentContext(ctx);
////    
////    CVReturn err1;
////    
////    if (0 == texture_cache) {
////        
////        err1 = CVOpenGLTextureCacheCreate(NULL,NULL,ctx,pf,NULL,&texture_cache);
////        assert(kCVReturnSuccess == err1);
////    }
////    
////    CFAbsoluteTime nTime = 1000.0 * CFAbsoluteTimeGetCurrent();
////    
////    uint32_t w = CGRectGetWidth([self bounds]), h = CGRectGetHeight([self bounds]);
////    
////    
////    {
////        voCAutoLock lock (&m_cBufferMutex);
////        if (GL_INVAILD == m_iLastOkBuffer) {
////            [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
////            return;
////        }
////        m_iUsingBuffer = m_iLastOkBuffer;
////    }
////    
////    w = nTmpW; h = nTmpH;
////    
////    uint8_t *data  = pRead; //my_wvp->is->GetLastPicture()->bitmap;
////    CVPixelBufferRef pixel_buffer;
////    CVReturn err;
////    GLenum target;
////    GLint name;
////    GLfloat topLeft[2], topRight[2], bottomRight[2], bottomLeft[2];
////    
////    uint8_t *planes[4];
////    size_t plane_w[4];
////    size_t plane_h[4];
////    size_t bytes_per_row[4];
////
////    if (!data)
////        return;
////
////    planes[0] = &data[0];
////    planes[1] = &data[h*w/2];
////    planes[2] = &data[h*w];
////    planes[3] = &data[3*h*w/2];
////
////    plane_w[0] = w;
////    plane_w[1] = w;
////    plane_w[2] = w;
////    plane_w[3] = w;
////
////    plane_h[0] = h;
////    plane_h[1] = h;
////    plane_h[2] = h;
////    plane_h[3] = h;
////
////    bytes_per_row[0] = 2*w;
////    bytes_per_row[1] = 2*w;
////    bytes_per_row[2] = 2*w;
////    bytes_per_row[3] = 2*w;
////
////    glPushAttrib(GL_ALL_ATTRIB_BITS);
////    glPushClientAttrib(GL_ALL_ATTRIB_BITS);
////    
////    CVOpenGLTextureCacheFlush(texture_cache,0);
////    
////    err = CVPixelBufferCreateWithPlanarBytes(NULL,w,h,kCVPixelFormatType_422YpCbCr8,data,h*w*2,4,(void**)planes,plane_w,plane_h, bytes_per_row,NULL,NULL,NULL,&pixel_buffer);
////    assert(kCVReturnSuccess == err);
////    
////	if(err != kCVReturnSuccess) {
////        [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
////		NSLog(@"Failed to create Pixel Buffer(%d)", err);
////		return;
////	}
////    
////    if (0 != texture) {
////        CVOpenGLTextureRelease(texture);
////    }
////    
////    err = CVOpenGLTextureCacheCreateTextureFromImage(NULL,texture_cache,pixel_buffer,NULL,&texture);
////    assert(kCVReturnSuccess == err);
////    
////    
////    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////    
////    glMatrixMode(GL_TEXTURE);
////    glLoadIdentity();
////    
////    glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();
////    
////    
////    target = CVOpenGLTextureGetTarget(texture);
////    name   = CVOpenGLTextureGetName(texture);
////    
////    CVOpenGLTextureGetCleanTexCoords(texture, bottomLeft, bottomRight, topRight, topLeft);
////    
////    // get the texture coordinates for the part of the image that should be displayed
////    glPushMatrix();
////    assert(glGetError() == GL_NO_ERROR);
////    
////    // bind the texture and draw the quad
////    glEnable(target);
////    assert(glGetError() == GL_NO_ERROR);
////    
////    glBindTexture(target, name);
////    assert(glGetError() == GL_NO_ERROR);
////    
////    glBegin(GL_QUADS);
////    glTexCoord2fv(bottomLeft);  glVertex2i(-1, -1);
////    glTexCoord2fv(topLeft);     glVertex2i(-1,  1);
////    glTexCoord2fv(topRight);    glVertex2i( 1,  1);
////    glTexCoord2fv(bottomRight); glVertex2i( 1, -1);
////    glEnd();
////    
////    assert(glGetError() == GL_NO_ERROR);
////    
////    
////    glDisable(target);
////    assert(glGetError() == GL_NO_ERROR);
////    
////    glPopMatrix();
////    assert(glGetError() == GL_NO_ERROR);
////    
////    glPopClientAttrib();
////    glPopAttrib();
////    [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
////    
////    CVPixelBufferRelease(pixel_buffer);
////    //CVOpenGLTextureRelease(texture);
////    
////    NSLog(@"Render use time:%f", 1000.0 * CFAbsoluteTimeGetCurrent() - nTime);
////    
////    return;
//#endif
    
//#define _READ_RGB_GL
    
#ifdef _READ_RGB_GL
    
    static FILE* m_hRGB = fopen("/Users/huangjiafa/Desktop/640x480'25f.rgb32", "rb");
    
    int nTmpW = 640;
    int nTmpH = 480;
    
    if (NULL == m_hRGB) {
        return [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
    }
    
    int iStride = 4;
    
    static unsigned char * pRead = new unsigned char[nTmpW*nTmpH*iStride];
    size_t iRead = fread(pRead, 1, nTmpW*nTmpH*iStride, m_hRGB);
    
    if (iRead != nTmpW*nTmpH*iStride) {
        fseek(m_hRGB, 0, SEEK_SET);
    }
    
    glTexImage2D( GL_TEXTURE_2D, 0, 3, nTmpW, nTmpH,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pRead);
#else
    glTexImage2D( GL_TEXTURE_2D, 0, 3, m_nTextureWidth, m_nTextureHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pFrameData[iIndexToRender].pBuffer);
#endif
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
	glBegin(GL_QUADS);
    glColor4f(1,1,1,1);
    
    for (int i = 0; i < 4; ++i) {
        glTexCoord2f(m_fTextureVertices[2*i], m_fTextureVertices[2*i+1]);
        glVertex2f(m_fSquareVertices[2*i], m_fSquareVertices[2*i+1]); // 左上
    }

	glEnd();
    
    //VOLOGI("GL Render use time:%f, current:%f, buffer:%d", 1000.0 * CFAbsoluteTimeGetCurrent() - nTime,  1000.0 * CFAbsoluteTimeGetCurrent(), (int)(m_pFrameData[m_iUsingBuffer]));

    [super drawInCGLContext:ctx pixelFormat:pf forLayerTime:t displayTime:ts];
    
    m_iLastRenderBuffer = iIndexToRender;
    m_pFrameData[iIndexToRender].eRender = VO_RENDER_BE_RENDERED;
    
#ifdef _VOLOG_INFO
    int nTimeE = voOS_GetSysTime();
    if (nTimeE - nTimeLastStart > 15) {
        VOLOGI("drawInCGLContext use time:%d", nTimeE - nTimeLastStart);
    }
#endif
    
    m_nLastRenderTime = voOS_GetSysTime();
    
    return;
}

-(void) updateVertices
{
    if ((0 == frame_w) || (0 == frame_h)
        || (0 == m_nTextureWidth) || (0 == m_nTextureHeight)) {
        return;
    }
    
    frame_draw_w = frame_w;
    frame_draw_h = frame_h;
	
    if (VO_ZM_FITWINDOW == m_nZoomMode)
	{
	}
    else if (VO_ZM_ORIGINAL == m_nZoomMode)
    {
        frame_draw_w = m_nTextureWidth;
        frame_draw_h = m_nTextureHeight;
    }
    else if (VO_ZM_ZOOMIN == m_nZoomMode)
    {
        // implement
    }
	else
	{
		VO_U32	nW = 0;
		VO_U32	nH = 0;
        
        if (m_nRatio == VO_RATIO_00) {
            nW = m_nTextureWidth;
            nH = m_nTextureHeight;
        }
		else if (m_nRatio == VO_RATIO_11)
		{
			nW = 1;
			nH = 1;
		}
		else if (m_nRatio == VO_RATIO_43)
		{
			nW = 4;
			nH = 3;
		}
		else if (m_nRatio == VO_RATIO_169)
		{
			nW = 16;
			nH = 9;
		}
		else if (m_nRatio == VO_RATIO_21)
		{
			nW = 2;
			nH = 1;
		}
		else
		{
			nW = m_nRatio >> 16;
			nH = m_nRatio & 0xffff;
		}
        
        if (m_nZoomMode == VO_ZM_PANSCAN)
        {
            if (frame_draw_w * nH > frame_draw_h * nW)
                frame_draw_h = frame_draw_w * nH / nW;
            else
                frame_draw_w = frame_draw_h * nW / nH;
        }
        else
        {
            if (frame_draw_w * nH > frame_draw_h * nW)
                frame_draw_w = frame_draw_h * nW / nH;
            else
                frame_draw_h = frame_draw_w * nH / nW;
        }
	}
    
    GLfloat *pTexture = m_fTextureVertices;
    *pTexture++ = 0.0;
    *pTexture++ = 0.0;
    
    *pTexture++ = 1.0;
    *pTexture++ = 0.0;
    
    *pTexture++ = 1.0;
    *pTexture++ = 1.0;
    
    *pTexture++ = 0.0;
    *pTexture++ = 1.0;
    
    frame_draw_x = (frame_w - frame_draw_w) / 2;
    frame_draw_y = (frame_h - frame_draw_h) / 2;
    
    GLfloat *pSquare = m_fSquareVertices;
    *pSquare++ = frame_draw_x; // left
    *pSquare++ = frame_draw_y; // top
    
    *pSquare++ = frame_draw_w + frame_draw_x; // right
    *pSquare++ = frame_draw_y; //top
    
    *pSquare++ = frame_draw_w + frame_draw_x; // right
    *pSquare++ = frame_draw_h + frame_draw_y; // bottom
    
    *pSquare++ = frame_draw_x; // left
    *pSquare++ = frame_draw_h + frame_draw_y; // bottom
    
    if ((NULL != m_fEventCallBack) && (NULL != m_pEventUserData)) {
        VO_RECT cRect;
        cRect.left = frame_draw_x;
        cRect.right = frame_draw_x + frame_draw_w;
        cRect.top = frame_draw_y;
        cRect.bottom = frame_draw_y + frame_draw_h;
        
        m_fEventCallBack(m_pEventUserData, VO_VR_EVENT_VIDEO_DISPLAY_RECT_CHANGED, &cRect, 0);
    }
}

-(Rect) getDrawRect
{
    Rect rect;
    rect.left = frame_draw_x;
    rect.right = frame_draw_x + frame_draw_w;
    rect.top = frame_draw_y;
    rect.bottom = frame_draw_y + frame_draw_h;
    
    return rect;
}

-(int) setDispType:(VO_IV_ZOOM_MODE)zoomMode ration:(VO_IV_ASPECT_RATIO)ration
{
    if ((m_nZoomMode != zoomMode) || (m_nRatio != ration)) {
        
        m_nZoomMode = zoomMode;
        m_nRatio = ration;
        
        [self updateVertices];
    }
    
    return VO_GL_RET_OK;
}

-(void) setFrameDiffTime:(int)time
{
    m_nFrameDiffTime = time;
}

-(void)releaseCGLContext:(CGLContextObj)glContext
{
	[super releaseCGLContext:glContext];
}

-(void)releaseCGLPixelFormat:(CGLPixelFormatObj)pixelFormat
{
	[super releaseCGLPixelFormat:pixelFormat];
}

+ (int)getTextureSize:(int)size
{
	int outSize = 64;
	while (outSize < size)
		outSize <<= 1;
	return outSize;
}

-(id)init
{
    @autoreleasepool {
    
    if (nil == (self = [super init]))
    {
        return nil;
    }
    
    frame_w = 0;
    frame_h = 0;
    
    frame_draw_x = 0;
    frame_draw_y = 0;
    frame_draw_h = 0;
    frame_draw_w = 0;
        
    g_textureID = 0;
    
    m_nTextureWidth		= 0;
    m_nTextureHeight	= 0;
        
    memset(&(m_pFrameData[0]), 0, sizeof(VO_RENDER_BUFFER));
    memset(&(m_pFrameData[1]), 0, sizeof(VO_RENDER_BUFFER));
        
    m_iLastOkBuffer = GL_INVAILD;
    m_iLastRenderBuffer = GL_INVAILD;
    
    m_bNeedRedraw = false;
    
    m_nZoomMode = VO_ZM_LETTERBOX;
    m_nRatio = VO_RATIO_00;
        
    m_nFrameDiffTime = 0;
    
    memset(&m_fTextureVertices, 0, sizeof(m_fTextureVertices));
    memset(&m_fSquareVertices, 0, sizeof(m_fSquareVertices));
    
#ifdef _READ_YUV_FILE
    texture = 0;
    texture_cache = 0;
    
    m_nTexturePlanarY = 0;
    m_nTexturePlanarU = 0;
    m_nTexturePlanarV = 0;
    m_nTextureUniformY = 0;
    m_nTextureUniformU = 0;
    m_nTextureUniformV = 0;
    
    m_nProgramHandle = 0;
    
    m_nPositionSlot = 0;
    m_nTexCoordSlot = 0;
#endif
    
    m_fEventCallBack = NULL;
    m_pEventUserData = NULL;
    }
    
	return self;
}

-(void)dealloc
{
    if(g_textureID)
    {
        glDeleteTextures( 1, &g_textureID );
    }
    
	[self deleteBuffer];
	[super dealloc];
}

-(void) deleteBuffer
{
    voCAutoLock lock (&m_cBufferMutex);
    
    m_iLastOkBuffer = GL_INVAILD;
    m_iLastRenderBuffer = GL_INVAILD;
    
    if (m_pFrameData[0].pBuffer)
	{
		delete m_pFrameData[0].pBuffer;
		m_pFrameData[0].pBuffer = NULL;
	}
    
    if (m_pFrameData[1].pBuffer)
	{
		delete m_pFrameData[1].pBuffer;
		m_pFrameData[1].pBuffer = NULL;
	}
    
    memset(&(m_pFrameData[0]), 0, sizeof(VO_RENDER_BUFFER));
    memset(&(m_pFrameData[1]), 0, sizeof(VO_RENDER_BUFFER));
}

- (int) setView:(CALayer *)layer
{
    {
        voCAutoLock cAuto(_renderLock);
        
        if (layer == self.layerSet) {
            return VO_GL_RET_OK;
        }
        
        self.layerSet = layer;
    }
    
    if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop]) {
        [self initVideoView];
    }
    else {
        VOLOGI("set view in sub thread:%p", layer);
        [self performSelectorOnMainThread:@selector(initVideoView) withObject:nil waitUntilDone:NO];
    }
    
    return VO_GL_RET_OK;
}

-(int) setEventCallBack:(VOVIDEO_EVENT_CALLBACKPROC)callback userData:(void *)userData
{
    m_fEventCallBack = callback;
    m_pEventUserData = userData;
    
    return VO_GL_RET_OK;
}

- (int) initVideoView;
{
    voCAutoLock cAuto(_renderLock);
    
    [self removeFromSuperlayer];
    
    if (nil == self.layerSet) {
        return VO_GL_RET_OK;
    }
    
    self.bounds = self.layerSet.bounds;
    self.frame = self.layerSet.frame;
    
    self.contentsGravity = kCAGravityCenter;
    self.asynchronous = YES;
    self.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    self.needsDisplayOnBoundsChange = YES;
    
    [self.layerSet insertSublayer:self atIndex:0];
    
    return VO_GL_RET_OK;
}

-(unsigned char*) lockFrameData
{
    voCAutoLock lock (&m_cBufferMutex);
    
    int iIndexToLock = 0;
    if (0 == m_iLastOkBuffer) {
        iIndexToLock = 1;
    }
    
    if (NULL == m_pFrameData[iIndexToLock].pBuffer) {
        VOLOGI("m_pFrameData[%d] is null", iIndexToLock);
        return NULL;
    }
    
    // Render is slower than buffer writter, return NULL if next buffer still using
    if ((VO_RENDER_INIT != m_pFrameData[iIndexToLock].eRender)
         && (VO_RENDER_BE_RENDERED != m_pFrameData[iIndexToLock].eRender)) {
        return NULL;
    }
    
    m_iLastStatus = m_pFrameData[iIndexToLock].eRender;
    m_pFrameData[iIndexToLock].eRender = VO_RENDER_CONVERTING;
    return m_pFrameData[iIndexToLock].pBuffer;
}

-(void) unlockFrameData:(BOOL)bWriteOk unlockBuffer:(unsigned char* )unlockBuffer
{
    if (NULL == unlockBuffer) {
        return;
    }
    
    voCAutoLock lock (&m_cBufferMutex);
    
    switch (m_iLastOkBuffer) {
        case 0:
            if (unlockBuffer == m_pFrameData[1].pBuffer) {
                if (YES == bWriteOk) {
                    m_iLastOkBuffer = 1;
                    m_pFrameData[1].eRender = VO_RENDER_READY;
                }
                else {
                    m_pFrameData[1].eRender = m_iLastStatus;
                }
#ifdef _FRAME_LOG
                VOLOGI("_FRAME_LOG unlock :%d status:%d", 1, bWriteOk);
#endif
            }
            else {
                VOLOGE("Wrong pointer");
            }
            break;
        case GL_INVAILD:
        case 1:
        default:
            if (unlockBuffer == m_pFrameData[0].pBuffer) {
                if (YES == bWriteOk) {
                    m_iLastOkBuffer = 0;
                    m_pFrameData[0].eRender = VO_RENDER_READY;
                }
                else {
                    m_pFrameData[0].eRender = m_iLastStatus;
                }
#ifdef _FRAME_LOG
                VOLOGI("_FRAME_LOG unlock :%d status:%d", 0, bWriteOk);
#endif
            }
            else {
                VOLOGE("Wrong pointer");
            }
            break;
    }
}

-(void) setVideoSize:(int)nInputWidth InputHeight:(int)nInputHeight
{    
    if ((nInputWidth != m_nTextureWidth) || (nInputHeight != m_nTextureHeight)) {
        
        m_nTextureWidth		= nInputWidth;
        m_nTextureHeight	= nInputHeight;
        
        [self reInitVideoBuffer];
        [self updateVertices];
    }
}

-(void) reInitVideoBuffer
{
    voCAutoLock lock (&m_cBufferMutex);
    
    while ((VO_RENDER_CONVERTING == m_pFrameData[0].eRender)
           || (VO_RENDER_RENDERING == m_pFrameData[0].eRender)) {
        usleep(1000);
    }
    
    [self deleteBuffer];
    
    if ((0 == m_nTextureWidth) || (0 == m_nTextureHeight)) {
        return;
    }
    
    int iStride = 4;
    
    m_pFrameData[0].pBuffer = new unsigned char[m_nTextureWidth*m_nTextureHeight*iStride];
    m_pFrameData[1].pBuffer = new unsigned char[m_nTextureWidth*m_nTextureHeight*iStride];
    
    memset(m_pFrameData[0].pBuffer, 0, m_nTextureWidth*m_nTextureHeight*iStride);
    memset(m_pFrameData[1].pBuffer, 0, m_nTextureWidth*m_nTextureHeight*iStride);
}

-(int) getTextureWidth
{
	return m_nTextureWidth;
}

-(int) getTextureHeight
{
	return m_nTextureHeight;
}

-(int) getSupportType;
{
    return VO_GL_SUPPORT_RGB;
}

-(int) setRotation:(VO_GL_ROTATION)type
{
    voCAutoLock cAuto(m_pLock);
    
    return VO_GL_RET_OK;
}

-(int) renderFrameData
{
    //[self performSelectorOnMainThread:@selector(drawInMainThread) withObject:nil waitUntilDone:NO];
    
    return VO_GL_RET_OK;
}

-(void) drawInMainThread
{
    [self setNeedsDisplay];
    return;
}

-(int) renderYUV:(VO_VIDEO_BUFFER *)pVideoBuffer
{
    voCAutoLock cAuto(m_pLock);
    
    return VO_GL_RET_FAIL;
}

-(int) redraw
{
    //[self performSelectorOnMainThread:@selector(drawInMainThread) withObject:nil waitUntilDone:NO];
    
    m_bNeedRedraw = true;
    
    return VO_GL_RET_OK;
}

@end






