	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VOVideoRender.cpp

	Contains:	VOVideoRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#include "utils/Log.h"

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <utils/threads.h>

#include "VOVideoRender.h"
#include "vojniconst.h"

#define LOG_TAG "VOVideoRender"
#include "voLog.h"
// ----------------------------------------------------------------------------
namespace android {

VOVideoRender::VOVideoRender(void)
	: VOBaseVideoRender ()
	, m_nDispWidth (320)
	, m_nDispHeight (480)
	, m_pCCRR(NULL)
{
    LOGI("NDK VideoRender - Version/Build: 2010-07-31 15:37");
}

VOVideoRender::~VOVideoRender(void)
{
    if (m_pCCRR)
        delete m_pCCRR;
}

int VOVideoRender::SetVideoSurface (const sp<Surface>& surface)
{
    //VOLOGI("Jason/Trace");
    Surface::SurfaceInfo si;

#if 0//ndef _VISUALON
	// add ndef for the flicker issue. 
	Surface* oldSurface = m_pSurface.get();
    if (oldSurface)
    {
        VOLOGW("Switch from non-surface: %p", oldSurface);
        if (oldSurface->isValid())
        {
            if (oldSurface->lock(&si, true))
            {
                VOLOGW("Lock old surface failed");
                oldSurface = NULL;
            }
			
        }
        else
            oldSurface = NULL;
    }

	m_pSurface = surface;

    if (oldSurface)
        oldSurface->unlockAndPost();
#else
	Surface* oldSurface = m_pSurface.get();
	oldSurface = NULL;
	m_pSurface = surface;

#endif

    VOLOGI("SetVideoSurface to %p", m_pSurface.get());
    if (m_pSurface == NULL)
    {
        LOGW("SetVideoSurface to NULL");
        return 0;
    }

    if (!m_pSurface->isValid())
    {
        LOGW("SetVideoSurface to invalid");
        return 0;
    }

    status_t s =  m_pSurface->lock(&si, true);
    if (s)
    {
        LOGW("surface lock failed: 0x%x", s);
        return s;
    }
	else
	{
		int nBytesPixel = 0;
		if (si.format == GGL_PIXEL_FORMAT_RGBA_8888)
		{
			m_outBuf.ColorType = VO_COLOR_ARGB32_PACKED;
			nBytesPixel = 4;
		}
		else if (si.format == GGL_PIXEL_FORMAT_RGBX_8888)
		{
			m_outBuf.ColorType = VO_COLOR_RGB32_PACKED;
			nBytesPixel = 4;
		}
		else if (si.format ==  GGL_PIXEL_FORMAT_RGB_888)
		{
			m_outBuf.ColorType = VO_COLOR_RGB888_PACKED;
			nBytesPixel = 3;
		}
		else
		{
			m_outBuf.ColorType = VO_COLOR_RGB565_PACKED;
			nBytesPixel = 2;
		}

		unsigned char *pBuf = (unsigned char*) si.bits;
		memset(pBuf , 0 , si.w * si.h * nBytesPixel);
	}

#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
    LOGW("Surface.lock() = %d, %d x %d, stride: %d format: %d bits: %08x", s, si.w, si.h, si.s, si.format, si.bits);
#elif defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
    LOGW("Surface.lock() = %d, %d x %d, bpr: %d format: %d bits: %08x", s, si.w, si.h, si.bpr, si.format, si.bits);
#endif
    s = m_pSurface->unlockAndPost();

	return 0;
}

int VOVideoRender::SetVideoSize (int nWidth, int nHeight)
{
    //VOLOGI("Jason/Trace");
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	m_nVideoWidth = (m_nVideoWidth + 3) & ~0X03;
	m_nVideoHeight = (m_nVideoHeight + 1) & ~0X01;

	return 0;
}


bool VOVideoRender::CheckColorType (VO_ANDROID_COLORTYPE nColorType)
{
    //VOLOGI("Jason/Trace");
	if (nColorType == VO_ANDROID_COLOR_RGB565 || nColorType == VO_ANDROID_COLOR_YUV420)
		return true;

	return false;
}

#if 0 //for debug only, Jason
void GetRandomSize(int& nOutW, int& nOutH)
{
    static int count = 240;
    static int width = 533;
    static int height = 320;
    if (--count == 0)
    {
        count = 120;
        width = rand() % 400;
        height = rand() % 300;
        LOGW("GetRandomSize - Output %d x %d", width, height);
    }
    nOutW = width;
    nOutH = height;
}
#endif //debug, Jason

bool VOVideoRender::Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer)
{
	if (m_pCCRR == NULL)
	{
		m_pCCRR = new voCCRR();
		
		char szPackageName[2048];
		FILE *cmdline = fopen("/proc/self/cmdline", "rb");
		char *arg = 0;
		size_t size = 0;
		if (cmdline != NULL)
		{		
			fgets(szPackageName, 2048, cmdline);
			sprintf(m_szWorkingPath, "/data/data/%s/", szPackageName);
			fclose(cmdline);
		}
		else
			strcpy(m_szWorkingPath, "/data/data/com.visualon.vome");

		m_pCCRR->setWorkingPath(m_szWorkingPath);
	}

    //VOLOGI("Jason/Trace");
	int						nAngle = 0;
	int						nBytesPixel = 2;

    if (m_pSurface == NULL)
    {
        LOGW("Render on NULL surface");
        return false;
    }
    
    if (!m_pSurface->isValid())
    {
        LOGE("Render on invalid surface");
        return false;
    }
    
    status_t s =  m_pSurface->lock(&m_si, true);
    if (s)
    {
        LOGW("surface lock failed: 0x%x", s);
        return s;
    }

	if (((m_nVideoWidth >= m_nVideoHeight && m_si.w >= m_si.h) ||
		(m_nVideoWidth <= m_nVideoHeight && m_si.w <= m_si.h)))
	{
		nAngle = 0;
        m_nVideoWidth &= ~0x3;
        m_nVideoHeight &= ~0x1;
	}
	else
	{
		nAngle = 90;
        m_nVideoWidth &= ~0x3;
        m_nVideoHeight &= ~0x3;
	}
	m_nDispWidth = m_si.w & ~0X3;
	m_nDispHeight = m_si.h & ~0X1;

    //GetRandomSize(m_nDispWidth, m_nDispHeight);

    m_inBuf.Buffer[0] = pVideoBuffer->virBuffer[0];
    m_inBuf.Buffer[1] = pVideoBuffer->virBuffer[1];
    m_inBuf.Buffer[2] = pVideoBuffer->virBuffer[2];
    
    m_inBuf.Stride[0] = pVideoBuffer->nStride[0];
    m_inBuf.Stride[1] = pVideoBuffer->nStride[1];
    m_inBuf.Stride[2] = pVideoBuffer->nStride[2];

	if (m_si.format == GGL_PIXEL_FORMAT_RGBA_8888)
	{
		m_outBuf.ColorType = VO_COLOR_ARGB32_PACKED;
		nBytesPixel = 4;
	}
	else if (m_si.format == GGL_PIXEL_FORMAT_RGBX_8888)
	{
		m_outBuf.ColorType = VO_COLOR_RGB32_PACKED;
		nBytesPixel = 4;
	}
	else if (m_si.format ==  GGL_PIXEL_FORMAT_RGB_888)
	{
		m_outBuf.ColorType = VO_COLOR_RGB888_PACKED;
		nBytesPixel = 3;
	}
	else
	{
		m_outBuf.ColorType = VO_COLOR_RGB565_PACKED;
		nBytesPixel = 2;
	}

	m_outBuf.Buffer[0] = (unsigned char*) m_si.bits;

#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
	m_outBuf.Stride[0] = m_si.s * nBytesPixel;
#elif defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
	m_outBuf.Stride[0] = m_si.bpr; 
#endif

	//VOLOGE("VOVideoRender::Render(%dx%d @%d)\n", m_si.w, m_si.h, nAngle);

	if (m_pCCRR->SetInputSize (m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight) >= 0)
	{
		if (m_pCCRR->SetOutputSize (m_nDispWidth, m_nDispHeight, 0, 0, m_nDispWidth, m_nDispHeight) >= 0)
		{
			m_pCCRR->ProcessRGB565 (VO_COLOR_YUV_PLANAR420, &m_inBuf, &m_outBuf, nAngle);
		}
	}

    s = m_pSurface->unlockAndPost();

	return true;
}

} // end namespace android

