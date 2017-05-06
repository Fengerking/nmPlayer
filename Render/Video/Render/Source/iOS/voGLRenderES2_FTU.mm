#import "voGLRenderES2_FTU.h"

#import "voLog.h"
#import "voOSFunc.h"
#import "voPackUV.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static const GLchar *G_VO_VERTEX_SHADER_Y_UV = " \
attribute vec4 position; \
attribute vec2 texCoord; \
varying vec2 texCoordVarying; \
void main() \
{ \
gl_Position = position; \
texCoordVarying = texCoord; \
}";


static const GLchar *G_VO_FRAGMENT_SHADER_Y_UV = " \
uniform sampler2D SamplerY; \
uniform sampler2D SamplerUV; \
varying highp vec2 texCoordVarying; \
void main() \
{ \
mediump vec3 yuv; \
lowp vec3 rgb; \
yuv.x = texture2D(SamplerY, texCoordVarying).r; \
yuv.yz = texture2D(SamplerUV, texCoordVarying).rg - vec2(0.5, 0.5); \
rgb = mat3(      1,       1,      1, \
0, -.18732, 1.8556, \
1.57481, -.46813,      0) * yuv; \
gl_FragColor = vec4(rgb, 1); \
}";

voGLRenderES2_FTU::voGLRenderES2_FTU(EAGLContext* pContext)
:voGLRenderES2(pContext)
,_lumaTexture(NULL)
,_chromaTexture(NULL)
,videoTextureCache(NULL)
,m_nTextureUniformY(0)
,m_nTextureUniformUV(0)
,pixelBuffer(nil)
{
}

voGLRenderES2_FTU::~voGLRenderES2_FTU()
{
    if (_lumaTexture)
    {
        CFRelease(_lumaTexture);
        _lumaTexture = NULL;
    }
    
    if (_chromaTexture)
    {
        CFRelease(_chromaTexture);
        _chromaTexture = NULL;
    }
    
    if (videoTextureCache) {
        CFRelease(videoTextureCache);
        videoTextureCache = 0;
    }
    
    if (NULL != pixelBuffer) {
        CVPixelBufferRelease(pixelBuffer);
        pixelBuffer = NULL;
    }
}

#pragma mark Protect function: GL setting.
int voGLRenderES2_FTU::SetupFrameBuffer()
{
    int nRet = voGLRenderES2::SetupFrameBuffer();

    if (VO_GL_RET_OK != nRet) {
        return nRet;
    }
    
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, m_pContext, NULL, &videoTextureCache);
    if (err) {
        VOLOGE("Error at CVOpenGLESTextureCacheCreate %d", err);
        return VO_GL_RET_FAIL;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::DeleteFrameBuffer()
{
    // Periodic texture cache flush every frame
    if (videoTextureCache) {
        CFRelease(videoTextureCache);
        videoTextureCache = 0;
    }
    
    return voGLRenderES2::DeleteFrameBuffer();
}

bool voGLRenderES2_FTU::IsGLRenderReady() {
    
    if (!voGLRenderES2::IsGLRenderReady()) {
        return false;
    }
    
    if (0 == videoTextureCache) {
        return false;
    }
    
    return true;
}

int voGLRenderES2_FTU::TextureSizeChange()
{
    if (NULL != pixelBuffer) {
        CVPixelBufferRelease(pixelBuffer);
        pixelBuffer = NULL;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::SetupTexture()
{
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::DeleteTexture()
{
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::CompileAllShaders()
{
    if (m_nProgramHandle) {
        glDeleteProgram(m_nProgramHandle);
        m_nProgramHandle = 0;
    }
    
    // Setup program
    m_nProgramHandle = glCreateProgram();
    if (0 == m_nProgramHandle) {
        VOLOGE("CompileAllShaders fail m_nProgramHandle:%d", m_nProgramHandle);
        return VO_GL_RET_FAIL;
    }
    
    GLuint vertexShader = CompileShader(G_VO_VERTEX_SHADER_Y_UV, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(G_VO_FRAGMENT_SHADER_Y_UV, GL_FRAGMENT_SHADER);
    
    if (0 == vertexShader || 0 == fragmentShader) {
        VOLOGE("CompileAllShaders fail vertexShader:%d, fragmentShader:%d", vertexShader, fragmentShader);
        return VO_GL_RET_FAIL;
    }
    
    glAttachShader(m_nProgramHandle, vertexShader);
    glAttachShader(m_nProgramHandle, fragmentShader);
    glLinkProgram(m_nProgramHandle);
    
    // Link program
    GLint linkSuccess;
    glGetProgramiv(m_nProgramHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        
        if (vertexShader) {
            glDetachShader(m_nProgramHandle, vertexShader);
            glDeleteShader(vertexShader);
        }
        if (fragmentShader) {
            glDetachShader(m_nProgramHandle, fragmentShader);
            glDeleteShader(fragmentShader);
        }
        
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
    
    m_nTexCoordSlot = glGetAttribLocation(m_nProgramHandle, "texCoord");
    glEnableVertexAttribArray(m_nTexCoordSlot);
    
    m_nTextureUniformY = glGetUniformLocation(m_nProgramHandle, "SamplerY");
    m_nTextureUniformUV = glGetUniformLocation(m_nProgramHandle, "SamplerUV");
    
    // Release vertex and fragment shaders.
    if (vertexShader) {
        glDetachShader(m_nProgramHandle, vertexShader);
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDetachShader(m_nProgramHandle, fragmentShader);
        glDeleteShader(fragmentShader);
    }
    
    // Wrong init
    if (m_nTextureUniformY == m_nTextureUniformUV) {
        
        VOLOGE("Error Y:%d, UV:%d", m_nTextureUniformY, m_nTextureUniformUV);
        return VO_GL_RET_FAIL;
    }
    
    VOLOGI("m_nPositionSlot:%d, m_nTexCoordSlot:%d, Y:%d, UV:%d", m_nPositionSlot, m_nTexCoordSlot, m_nTextureUniformY, m_nTextureUniformUV);
    
    return VO_GL_RET_OK;
}

#pragma mark Protect function: GL Draw
int voGLRenderES2_FTU::UploadTexture(VO_VIDEO_BUFFER *pVideoBuffer)
{
    if (NULL == pixelBuffer) {
        
        NSMutableDictionary* attributes;
        attributes = [NSMutableDictionary dictionary];
        [attributes setObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange] forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];
        [attributes setObject:[NSNumber numberWithInt:m_nTextureWidth] forKey: (NSString*)kCVPixelBufferWidthKey];
        [attributes setObject:[NSNumber numberWithInt:m_nTextureHeight] forKey: (NSString*)kCVPixelBufferHeightKey];
        NSDictionary *IOSurfaceProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                             [NSNumber numberWithBool:YES], @"IOSurfaceOpenGLESFBOCompatibility",[NSNumber numberWithBool:YES], @"IOSurfaceOpenGLESTextureCompatibility",nil];
        
        [attributes setObject:IOSurfaceProperties forKey:(NSString*)kCVPixelBufferIOSurfacePropertiesKey];
        
        CVPixelBufferPoolRef bufferPool;
        CVPixelBufferPoolCreate(kCFAllocatorDefault, NULL, (CFDictionaryRef) attributes, &bufferPool);
        
        CVPixelBufferPoolCreatePixelBuffer (NULL,bufferPool,&pixelBuffer);
        
        CVPixelBufferPoolRelease(bufferPool);
    }
    
    if (NULL == pixelBuffer) {
        return VO_GL_RET_FAIL;
    }
    
    CVPixelBufferLockBaseAddress(pixelBuffer,0);
    
    unsigned char *pixel0 = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    unsigned char *pixel1 = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    
    int nBytesPerRow1 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    
    if (pVideoBuffer->Stride[0] != m_nTextureWidth) {
        for (int i = 0; i < m_nTextureHeight; i++)
            memcpy (pixel0 + m_nTextureWidth * i, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i, m_nTextureWidth);
    }
    else {
        memcpy(pixel0, pVideoBuffer->Buffer[0], m_nTextureWidth * m_nTextureHeight);
    }
    
    voPackUV(pixel1, pVideoBuffer->Buffer[1], pVideoBuffer->Buffer[2], pVideoBuffer->Stride[1], pVideoBuffer->Stride[2], m_nTextureHeight / 2, m_nTextureWidth / 2, nBytesPerRow1);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer,0);
    
    if (_lumaTexture)
    {
        CFRelease(_lumaTexture);
        _lumaTexture = NULL;
    }
    
    if (_chromaTexture)
    {
        CFRelease(_chromaTexture);
        _chromaTexture = NULL;
    }
    
    // Periodic texture cache flush every frame
    CVOpenGLESTextureCacheFlush(videoTextureCache, 0);
    
    // Create a CVOpenGLESTexture from the CVImageBuffer
	size_t frameWidth = CVPixelBufferGetWidth(pixelBuffer);
	size_t frameHeight = CVPixelBufferGetHeight(pixelBuffer);
    
    CVReturn err = 0;
    glActiveTexture(GL_TEXTURE0);
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       videoTextureCache,
                                                       pixelBuffer,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       GL_RED_EXT,
                                                       frameWidth,
                                                       frameHeight,
                                                       GL_RED_EXT,
                                                       GL_UNSIGNED_BYTE,
                                                       0,
                                                       &_lumaTexture);
    if (err)
    {
        VOLOGE("Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
    }
    
    glBindTexture(CVOpenGLESTextureGetTarget(_lumaTexture), CVOpenGLESTextureGetName(_lumaTexture));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // UV-plane
    glActiveTexture(GL_TEXTURE1);
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       videoTextureCache,
                                                       pixelBuffer,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       GL_RG_EXT,
                                                       frameWidth/2,
                                                       frameHeight/2,
                                                       GL_RG_EXT,
                                                       GL_UNSIGNED_BYTE,
                                                       1,
                                                       &_chromaTexture);
    if (err)
    {
        VOLOGE("Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
    }
    
    glBindTexture(CVOpenGLESTextureGetTarget(_chromaTexture), CVOpenGLESTextureGetName(_chromaTexture));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::RenderToScreen()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Update attribute values.
	glVertexAttribPointer(m_nPositionSlot, 2, GL_FLOAT, 0, 0, m_fSquareVertices);
	glVertexAttribPointer(m_nTexCoordSlot, 2, GL_FLOAT, 0, 0, m_fTextureVertices);
    
    glUniform1i(m_nTextureUniformY, 0);
    glUniform1i(m_nTextureUniformUV, 1);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2_FTU::RedrawInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData)
{
    voCAutoLock cAuto(m_pLock);
    
    if (!IsGLRenderReady()) {
        VOLOGW("RenderYUV not ready");
        return VO_GL_RET_RETRY;
    }
    
    if (NULL == pixelBuffer) {
        return VO_GL_RET_ERR_STATUS;
    }
    
    if (![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    RenderToScreen();
    
    GetLastFrameInner(bIsTryGetFrame, pData);
    
    if (!bIsTryGetFrame) {
        RenderCommit();
    }
    
    return VO_GL_RET_OK;
}
