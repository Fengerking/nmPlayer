#import "voGLRenderES2.h"

#import "voLog.h"
#import "voOSFunc.h"

static const GLchar *G_VO_VERTEX_SHADER_Y_U_V = " \
attribute vec4 position; \
attribute mediump vec4 textureCoordinate; \
varying mediump vec2 coordinate; \
void main() \
{ \
gl_Position = position; \
coordinate = textureCoordinate.xy; \
}";


static const GLchar *G_VO_FRAGMENT_SHADER_Y_U_V = " \
precision highp float; \
uniform sampler2D SamplerY; \
uniform sampler2D SamplerU; \
uniform sampler2D SamplerV; \
\
varying highp vec2 coordinate; \
\
void main() \
{ \
highp vec3 yuv,yuv1; \
highp vec3 rgb; \
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

//static const GLchar *G_VO_FRAGMENT_SHADER_Y_U_V = " \
//    precision mediump float; \
//    uniform sampler2D SamplerY; \
//    uniform sampler2D SamplerU; \
//    uniform sampler2D SamplerV; \
//    varying highp vec2 coordinate; \
//    void main() \
//    { \
//    mediump vec3 yuv; \
//    lowp vec3 rgb; \
//    yuv.x = texture2D(SamplerY, coordinate).r; \
//    yuv.y = texture2D(SamplerU, coordinate).r - 0.5; \
//    yuv.z = texture2D(SamplerV, coordinate).r - 0.5; \
//    rgb = mat3(      1,       1,       1, \
//    0, -.21482, 2.12798, \
//    1.28033, -.38059,       0) * yuv; \
//    gl_FragColor = vec4(rgb, 1); \
//    }";

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voGLRenderES2::voGLRenderES2(EAGLContext* pContext)
:voGLRenderBase(pContext)
,m_pFrameData(NULL)
,m_pLastGetFrame(NULL)
,m_nOutLeft(0)
,m_nOutTop(0)
,m_nOutWidth(0)
,m_nOutHeight(0)
,m_nPositionSlot(0)
,m_nTexCoordSlot(0)
,m_nRotation(VO_GL_ROTATION_0)
,m_nColorRenderBuffer(0)
,m_nFrameBuffer(0)
,m_nProgramHandle(0)
,m_nTexturePlanarY(0)
,m_nTexturePlanarU(0)
,m_nTexturePlanarV(0)
,m_nTextureUniformY(0)
,m_nTextureUniformU(0)
,m_nTextureUniformV(0)
,m_bInitSuccess(false)
#ifdef _VOLOG_INFO
,m_fTimeAllUsed(0)
,m_nTimeCount(0)
#endif
{
    m_fTextureVertices[0] = 0;
    m_fTextureVertices[1] = 1;
    
    m_fTextureVertices[2] = 1;
    m_fTextureVertices[3] = 1;
    
    m_fTextureVertices[4] = 0;
    m_fTextureVertices[5] = 0;
    
    m_fTextureVertices[6] = 1;
    m_fTextureVertices[7] = 0;
    
    memset(&m_fSquareVertices, 0, sizeof(m_fSquareVertices));
    
    [EAGLContext setCurrentContext:m_pContext];
}

int voGLRenderES2::init()
{
    voCAutoLock cAuto(m_pLock);
    
    if (m_bInitSuccess) {
        return VO_GL_RET_OK;
    }
    
    int nRet = SetupTexture();
    if (VO_GL_RET_OK != nRet) {
        return nRet;
    }
    
    nRet = CompileAllShaders();
    if (VO_GL_RET_OK != nRet) {
        return nRet;
    }
    
    VOLOGI("init success:%p", this);
    
    m_bInitSuccess = true;
    
    return nRet;
}

bool voGLRenderES2::IsGLRenderReady() {
    
    if (!m_bInitSuccess) {
        return false;
    }
    
    if (0 == m_nColorRenderBuffer)
	{
		return false;
	}
    
    if (0 == m_nFrameBuffer)
	{
        return false;
    }
    
    return true;
}

voGLRenderES2::~voGLRenderES2()
{
    voCAutoLock cAuto(m_pLock);
    
    VOLOGI("Delete voGLRenderES2:%p", this);
    
    DeleteRenderBuffer();
    DeleteFrameBuffer();
    DeleteTexture();
    
    if (m_nProgramHandle) {
        glDeleteProgram(m_nProgramHandle);
        m_nProgramHandle = 0;
    }
    
    if (NULL != m_pFrameData) {
        delete []m_pFrameData;
        m_pFrameData = NULL;
    }
    
    if (NULL != m_pLastGetFrame) {
        free(m_pLastGetFrame);
        m_pLastGetFrame = NULL;
    }
}

#pragma mark Public function.
int voGLRenderES2::SetRotation(VO_GL_ROTATION eType)
{
    voCAutoLock cAuto(m_pLock);
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    if (m_nRotation != eType) {
        m_nRotation = eType;
        
        return SetupGLDisplayRect();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetTexture(int nWidth, int nHeight)
{
    voCAutoLock cAuto(m_pLock);
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    if ((nWidth != m_nTextureWidth) || (nHeight != m_nTextureHeight)) {
        m_nTextureWidth = nWidth;
        m_nTextureHeight = nHeight;
        TextureSizeChange();
        
        return SetupGLDisplayRect();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::TextureSizeChange()
{
    if (NULL != m_pFrameData) {
        delete []m_pFrameData;
        m_pFrameData = NULL;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetOutputRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    voCAutoLock cAuto(m_pLock);
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    if ((nLeft != m_nOutLeft)
        || (nTop != m_nOutTop)
        || (nWidth != m_nOutWidth)
        || (nHeight != m_nOutHeight)) {
        
        m_nOutLeft = nLeft;
        m_nOutTop = nTop;
        m_nOutWidth = nWidth;
        m_nOutHeight = nHeight;
        
        return SetupGLDisplayRect();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::ClearGL()
{
    voCAutoLock cAuto(m_pLock);
    
#ifdef _VOLOG_INFO
    if (m_nTimeCount) {
        VOLOGI("average render time:%f\n", m_fTimeAllUsed / m_nTimeCount);
    }

    m_fTimeAllUsed = 0;
    m_nTimeCount = 0;
#endif
    
    if (NULL != m_pFrameData) {
        delete []m_pFrameData;
        m_pFrameData = NULL;
    }
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    /* Clear the renderbuffer */
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	
	/* Refresh the screen */
	[m_pContext presentRenderbuffer: GL_RENDERBUFFER];
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::Redraw()
{
    return RedrawInner(false, NULL);
}

int voGLRenderES2::GetLastFrame(VO_IMAGE_DATA *pData)
{
    voCAutoLock cAuto(m_pLock);
    
    Swap(&(m_fSquareVertices[0]), &(m_fSquareVertices[4]));
    Swap(&(m_fSquareVertices[1]), &(m_fSquareVertices[5]));
    Swap(&(m_fSquareVertices[2]), &(m_fSquareVertices[6]));
    Swap(&(m_fSquareVertices[3]), &(m_fSquareVertices[7]));

    int nRet = RedrawInner(true, pData);
    
    Swap(&(m_fSquareVertices[0]), &(m_fSquareVertices[4]));
    Swap(&(m_fSquareVertices[1]), &(m_fSquareVertices[5]));
    Swap(&(m_fSquareVertices[2]), &(m_fSquareVertices[6]));
    Swap(&(m_fSquareVertices[3]), &(m_fSquareVertices[7]));
    
    return nRet;
}

int voGLRenderES2::RenderYUV(VO_VIDEO_BUFFER *pVideoBuffer)
{
    voCAutoLock cAuto(m_pLock);
    
    if (!IsGLRenderReady()) {
        VOLOGW("RenderYUV not ready");
        return VO_GL_RET_RETRY;
    }
    
    if (![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }

#ifdef _VOLOG_INFO
    CFAbsoluteTime fTime = CFAbsoluteTimeGetCurrent() * 1000;
#endif
    
    int nRet = UploadTexture(pVideoBuffer);
    if (VO_GL_RET_OK != nRet) {
        return nRet;
    }
    
    RenderToScreen();
    RenderCommit();
    
#ifdef _VOLOG_INFO
    CFAbsoluteTime fTimeL = CFAbsoluteTimeGetCurrent() * 1000;
    ++m_nTimeCount;
    m_fTimeAllUsed += (fTimeL - fTime);
    
    if (15 < fTimeL - fTime) {
        VOLOGI("average render time:%f, current frame render time:%f\n", m_fTimeAllUsed / m_nTimeCount, fTimeL - fTime);
    }
#endif
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::GetSupportType()
{
    return VO_GL_SUPPORT_Y_U_V;
}

#pragma mark Protect function: GL setting.
int voGLRenderES2::RefreshView()
{
    voCAutoLock cAuto(m_pLock);
    
    if (!m_pVideoView) {
        return VO_GL_RET_FAIL;
    }
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    CAEAGLLayer* _eaglLayer = (CAEAGLLayer *)m_pVideoView.layer;
    _eaglLayer.opaque = YES;
    
    if (0 == m_nOutWidth) {
        m_nOutWidth = m_pVideoView.frame.size.width;
    }
    
    if (0 == m_nOutHeight) {
        m_nOutHeight = m_pVideoView.frame.size.height;
    }
    
    SetupRenderBuffer();
    SetupFrameBuffer();
    
    SetupGlViewport();
    
    SetupGLDisplayRect();
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetupGlViewport()
{
    /* Extract renderbuffer's width and height. This should match layer's size, I assume */
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_nBackingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_nBackingHeight);
	
	/* Set viewport size to match the renderbuffer size */
	glViewport(0, 0, m_nBackingWidth, m_nBackingHeight);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetupGLDisplayRect()
{
    if (0 == m_nOutWidth || 0 == m_nOutHeight
        || 0 == m_nTextureWidth || 0 == m_nTextureHeight) {
        return VO_GL_RET_OK;
    }

    GLfloat nLengthX = (GLfloat)(2 * m_nOutWidth) / m_nBackingWidth;
    GLfloat nLengthY = (GLfloat)(2 * m_nOutHeight) / m_nBackingHeight;
    GLfloat nXLeft = -1 + (GLfloat)(2 * m_nOutLeft) / m_nBackingWidth; // from left
    GLfloat nYTop = 1 - ((GLfloat)(2 * m_nOutTop) / m_nBackingHeight); // from top

    switch (m_nRotation) {
            
        case VO_GL_ROTATION_180:
            //1 | 0
            //-   -
            //3 | 2
            m_fSquareVertices[0] = nXLeft + nLengthX;
            m_fSquareVertices[1] = nYTop;
            
            m_fSquareVertices[2] = nXLeft;
            m_fSquareVertices[3] = nYTop;
            
            m_fSquareVertices[4] = nXLeft + nLengthX;
            m_fSquareVertices[5] = nYTop - nLengthY;
            
            m_fSquareVertices[6] = nXLeft;
            m_fSquareVertices[7] = nYTop - nLengthY;
            break;
            
        case VO_GL_ROTATION_180FLIP:
            //0 | 1
            //-   -
            //2 | 3
            m_fSquareVertices[0] = nXLeft;
            m_fSquareVertices[1] = nYTop;
            
            m_fSquareVertices[2] = nXLeft + nLengthX;
            m_fSquareVertices[3] = nYTop;
            
            m_fSquareVertices[4] = nXLeft;
            m_fSquareVertices[5] = nYTop - nLengthY;
            
            m_fSquareVertices[6] = nXLeft + nLengthX;
            m_fSquareVertices[7] = nYTop - nLengthY;
            break;
            
        case VO_GL_ROTATION_0FLIP:
            //3 | 2
            //-   -
            //1 | 0
            m_fSquareVertices[0] = nXLeft + nLengthX;
            m_fSquareVertices[1] = nYTop - nLengthY;
            
            m_fSquareVertices[2] = nXLeft;
            m_fSquareVertices[3] = nYTop - nLengthY;
            
            m_fSquareVertices[4] = nXLeft + nLengthX;
            m_fSquareVertices[5] = nYTop;
            
            m_fSquareVertices[6] = nXLeft;
            m_fSquareVertices[7] = nYTop;
            break;
            
        case VO_GL_ROTATION_0:
        default:
            //2 | 3
            //-   -
            //0 | 1
            m_fSquareVertices[0] = nXLeft;            // left X
            m_fSquareVertices[1] = nYTop - nLengthY;  // bottom Y

            m_fSquareVertices[2] = nXLeft + nLengthX; // right X
            m_fSquareVertices[3] = nYTop - nLengthY;  // bottom Y

            m_fSquareVertices[4] = nXLeft;            // left X
            m_fSquareVertices[5] = nYTop;             // top Y
            
            m_fSquareVertices[6] = nXLeft + nLengthX; // right X
            m_fSquareVertices[7] = nYTop;             // top Y
            break;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetupRenderBuffer()
{
    DeleteRenderBuffer();
    glGenRenderbuffers(1, &m_nColorRenderBuffer);
    
    if (0 == m_nColorRenderBuffer)
	{
        VOLOGE("SetupRenderBuffer fail");
		return VO_GL_RET_FAIL;
	}
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_nColorRenderBuffer);
    [m_pContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)m_pVideoView.layer];
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetupFrameBuffer()
{
    DeleteFrameBuffer();
    glGenFramebuffers(1, &m_nFrameBuffer);
    
    if (0 == m_nFrameBuffer) {
        VOLOGE("SetupFrameBuffer fail");
        return VO_GL_RET_FAIL;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, m_nColorRenderBuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        VOLOGE("Failure with framebuffer generation");
		return VO_GL_RET_FAIL;
	}
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::SetupTexture()
{
    DeleteTexture();
    glGenTextures(1, &m_nTexturePlanarY);
    glGenTextures(1, &m_nTexturePlanarU);
    glGenTextures(1, &m_nTexturePlanarV);
    
    if ((0 == m_nTexturePlanarY) || (0 == m_nTexturePlanarU) || (0 == m_nTexturePlanarV)) {
        VOLOGE("SetupTexture fail, %d, %d, %d", m_nTexturePlanarY, m_nTexturePlanarU, m_nTexturePlanarV);
        return VO_GL_RET_FAIL;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::DeleteRenderBuffer()
{
    if (m_nColorRenderBuffer)
	{
		glDeleteRenderbuffers(1, &m_nColorRenderBuffer);
		m_nColorRenderBuffer = 0;
	}
    return VO_GL_RET_OK;
}

int voGLRenderES2::DeleteFrameBuffer()
{
	if (m_nFrameBuffer)
	{
		glDeleteFramebuffers(1, &m_nFrameBuffer);
		m_nFrameBuffer = 0;
	}
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::DeleteTexture()
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

GLuint voGLRenderES2::CompileShader(const GLchar *pBuffer, GLenum shaderType)
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

int voGLRenderES2::CompileAllShaders()
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
    
    GLuint vertexShader = CompileShader(G_VO_VERTEX_SHADER_Y_U_V, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(G_VO_FRAGMENT_SHADER_Y_U_V, GL_FRAGMENT_SHADER);
    
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
    
    // Wrong init
    if ((m_nTextureUniformY == m_nTextureUniformU) || (m_nTextureUniformU == m_nTextureUniformV) || (m_nTextureUniformY == m_nTextureUniformV)) {
        
        VOLOGE("Error Y:%d, U:%d, V:%d", m_nTextureUniformY, m_nTextureUniformU, m_nTextureUniformV);
        return VO_GL_RET_FAIL;
    }
    
    VOLOGI("m_nPositionSlot:%d, m_nTexCoordSlot:%d, Y:%d, U:%d, V:%d", m_nPositionSlot, m_nTexCoordSlot, m_nTextureUniformY, m_nTextureUniformU, m_nTextureUniformV);
    
    return VO_GL_RET_OK;
}

#pragma mark Protect function: GL Draw.
int voGLRenderES2::GLTexImage2D(GLuint nTexture, Byte *pDate, int nWidth, int nHeight)
{
    glBindTexture(GL_TEXTURE_2D, nTexture);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nWidth, nHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pDate);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::UploadTexture(VO_VIDEO_BUFFER *pVideoBuffer)
{
    if (NULL == m_pFrameData) {
        m_pFrameData = new unsigned char[m_nTextureWidth * m_nTextureHeight * 3 / 2];
    }
    
    if (NULL == m_pFrameData) {
        return VO_GL_RET_FAIL;
    }
    
    int i = 0;
    int nWidthUV = m_nTextureWidth / 2;
    int nHeightUV = m_nTextureHeight / 2;
    
    for (i = 0; i < m_nTextureHeight; i++)
        memcpy (m_pFrameData + m_nTextureWidth * i, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i, m_nTextureWidth);
    
    unsigned char* pBuffer = m_pFrameData + (m_nTextureWidth * m_nTextureHeight);
    for (i = 0; i < nHeightUV; i++)
        memcpy (pBuffer + (nWidthUV * i), pVideoBuffer->Buffer[1] + pVideoBuffer->Stride[1] * i, nWidthUV);
    
    pBuffer = pBuffer + (nWidthUV * nHeightUV);
    for (i = 0; i < nHeightUV; i++)
        memcpy (pBuffer + (nWidthUV * i), pVideoBuffer->Buffer[2] + pVideoBuffer->Stride[2] * i, nWidthUV);
    
    GLTexImage2D(m_nTexturePlanarY, m_pFrameData, m_nTextureWidth, m_nTextureHeight);
    GLTexImage2D(m_nTexturePlanarU, pBuffer - (nWidthUV * nHeightUV), nWidthUV, nHeightUV);
    GLTexImage2D(m_nTexturePlanarV, pBuffer, nWidthUV, nHeightUV);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::RenderToScreen()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarY);
    glUniform1i(m_nTextureUniformY, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarU);
    glUniform1i(m_nTextureUniformU, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_nTexturePlanarV);
    glUniform1i(m_nTextureUniformV, 2);
    
    // Update attribute values.
	glVertexAttribPointer(m_nPositionSlot, 2, GL_FLOAT, 0, 0, m_fSquareVertices);
	glVertexAttribPointer(m_nTexCoordSlot, 2, GL_FLOAT, 0, 0, m_fTextureVertices);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::RenderCommit()
{
    [m_pContext presentRenderbuffer:GL_RENDERBUFFER];
    return VO_GL_RET_OK;
}

#pragma mark Protect function: Other.
int voGLRenderES2::RedrawInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData)
{
    voCAutoLock cAuto(m_pLock);
    
    if (!IsGLRenderReady()) {
        VOLOGW("RenderYUV not ready");
        return VO_GL_RET_RETRY;
    }
    
    if (NULL == m_pFrameData) {
        return VO_GL_RET_ERR_STATUS;
    }
    
    if (![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
//    int nWidthUV = m_nTextureWidth / 2;
//    int nHeightUV = m_nTextureHeight / 2;
//    GLTexImage2D(m_nTexturePlanarY, m_pFrameData, m_nTextureWidth, m_nTextureHeight);
//    GLTexImage2D(m_nTexturePlanarU, m_pFrameData + (m_nTextureWidth * m_nTextureHeight), nWidthUV, nHeightUV);
//    GLTexImage2D(m_nTexturePlanarV, m_pFrameData + (m_nTextureWidth * m_nTextureHeight) * 5 / 4, nWidthUV, nHeightUV);
    
    RenderToScreen();
    
    GetLastFrameInner(bIsTryGetFrame, pData);
    
    if (!bIsTryGetFrame) {
        RenderCommit();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES2::GetLastFrameInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData)
{
    if (!bIsTryGetFrame) {
        return VO_GL_RET_OK;
    }
    
    NSInteger myDataLength = m_nBackingWidth * m_nBackingHeight * 4;
    
    if (NULL != m_pLastGetFrame) {
        free(m_pLastGetFrame);
        m_pLastGetFrame = NULL;
    }
    
    if (NULL == pData) {
        return VO_GL_RET_OK;
    }
    
    // allocate array and read pixels into it.
    m_pLastGetFrame = (GLubyte *) malloc(myDataLength);
    glReadPixels(0, 0, m_nBackingWidth, m_nBackingHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pLastGetFrame);
    
    pData->type = VO_IV_IMAGE_RGBA32;
    pData->height = m_nBackingHeight;
    pData->width = m_nBackingWidth;
    pData->size = myDataLength;
    pData->data = m_pLastGetFrame;
    
    return VO_GL_RET_OK;
}

