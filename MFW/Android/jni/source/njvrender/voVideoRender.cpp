/*
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "voVideoRender"
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

#include "voVideoRender.h"

#include "vojnilog.h"

// ----------------------------------------------------------------------------
namespace android {

voVideoRender::voVideoRender(void)
	: m_pSurface (NULL)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nDispWidth (320)
	, m_nDispHeight (480)
{
    m_pCCRR = new voCCRR();
}

voVideoRender::~voVideoRender(void)
{
    if (m_pCCRR)
        delete m_pCCRR;
	m_pSurface.clear ();
}

int voVideoRender::SetVideoSurface (const sp<Surface>& surface)
{
	m_pSurface = surface;

    //LOGW("Surface.isValid() = %d", surface->isValid());
    //LOGW("Surface.isValid() = %d", m_pSurface->isValid());
    Surface::SurfaceInfo si;
    status_t s =  m_pSurface->lock(&si, true);
#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
    LOGW("Surface.lock() = %d, %d x %d, stride: %d format: %d bits: %08x", s, si.w, si.h, si.s, si.format, si.bits);
#elif defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
    LOGW("Surface.lock() = %d, %d x %d, bpr: %d format: %d bits: %08x", s, si.w, si.h, si.bpr, si.format, si.bits);
#endif
    s = m_pSurface->unlockAndPost();

	return 0;
}

int voVideoRender::SetVideoSize (int nWidth, int nHeight)
{
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	m_nVideoWidth = (m_nVideoWidth + 3) & ~0X03;
	m_nVideoHeight = (m_nVideoHeight + 1) & ~0X01;

	return 0;
}

int voVideoRender::SetDispSize (int nWidth, int nHeight)
{
	m_nDispWidth = nWidth;
	m_nDispHeight = nHeight;

	return 0;
}

bool voVideoRender::CheckColorType (VO_ANDROID_COLORTYPE nColorType)
{
	if (nColorType == VO_ANDROID_COLOR_RGB565 || nColorType == VO_ANDROID_COLOR_YUV420)
		return true;
	else
		return false;
}

void * voVideoRender::GetVideoMemOperator (void)
{
	return NULL;
}

bool voVideoRender::Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer)
{
	int						nAngle = 0;
	int						nBytesPixel = 2;

    status_t s =  m_pSurface->lock(&m_si, true);

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

//    LOGW("voVideoRender::Render(%dx%d @%d)\n", m_si.w, m_si.h, nAngle);
//    LOGW("SetInputSize(%d, %d)\n", m_nVideoWidth, m_nVideoHeight);

    if (m_pCCRR->SetInputSize (m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight) >= 0)
    {
//		LOGW("SetOutputSize(%d x %d, (%d, %d) - (%d, %d)) stride %d  Buffer: %d \n", w, h, l, t, l + w, t + h, outBuf.Stride[0], outBuf.Buffer[0]);

        if (m_pCCRR->SetOutputSize (m_nDispWidth, m_nDispHeight, 0, 0, m_nDispWidth, m_nDispHeight) >= 0)
        {
#ifndef __VONJ_FROYO__
	  m_pCCRR->ProcessRGB565 (PIXEL_FORMAT_YCbCr_420_P, &m_inBuf, &m_outBuf, nAngle);
#else
	  m_pCCRR->ProcessRGB565 (VO_COLOR_YUV_PLANAR420, &m_inBuf, &m_outBuf, nAngle);
#endif
        }
    }

/*
	int nPix = 0;
	int * prgb = (int *)outBuf.Buffer[0];

	for (int i = 0; i < m_si.h; i++)
	{
		for (int j = 0; j < m_si.w; j++)
		{
			nPix = *prgb;

			nPix = 0XFF;

			*prgb++ = nPix;
		}
	}
*/
    s = m_pSurface->unlockAndPost();

	return true;
}

} // end namespace android

