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
#define LOG_TAG "vomeRGB565VideoRender"
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

#include "vomeRGB565VideoRender.h"

// ----------------------------------------------------------------------------
namespace android {

vomeRGB565VideoRender::vomeRGB565VideoRender(void)
	: m_pSurface (NULL)
	, m_pFrameHeap (NULL)
	, m_bInit (false)
	, m_pOutBuffer (NULL)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nBufferWidth (0)
	, m_nBufferHeight (0)
	, m_nDispWidth (800)
	, m_nDispHeight (480)
    , m_pCCRR(NULL)
{
    LOGW("@@@@@@  vomeRGB565VideoRender vomeRGB565VideoRender V0201B\n");
}

vomeRGB565VideoRender::~vomeRGB565VideoRender(void)
{
	releaseMemHeap ();

    if (m_pCCRR)
    {
        delete m_pCCRR;
        m_pCCRR = NULL;
    }

    LOGW("@@@@@@  vomeRGB565VideoRender destructor \n");
}

int vomeRGB565VideoRender::SetVideoSurface (const sp<ISurface>& surface)
{
    LOGW("@@@@@@  vomeRGB565VideoRender SetVideoSurface   %x \n", (unsigned int)surface.get ());

	m_pSurface = surface;

	return 0;
}

int vomeRGB565VideoRender::SetVideoSize (int nWidth, int nHeight)
{
    LOGW("@@@@@@  vomeRGB565VideoRender SetVideoSize   W %d H %d \n", nWidth, nHeight);

	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	m_nVideoWidth = (m_nVideoWidth + 3) & ~0X03;
	m_nVideoHeight = (m_nVideoHeight + 1) & ~0X01;

	if (m_nBufferWidth == 0)
		m_nBufferWidth = m_nVideoWidth;
	if (m_nBufferHeight == 0)
		m_nBufferHeight = m_nVideoHeight;

	return 0;
}

int vomeRGB565VideoRender::SetDispSize (int nWidth, int nHeight)
{
    LOGW("@@@@@@  vomeRGB565VideoRender SetDispSize   W %d H %d \n", nWidth, nHeight);

	m_nDispWidth = nWidth;
	m_nDispHeight = nHeight;

	return 0;
}

bool vomeRGB565VideoRender::CheckColorType (VO_ANDROID_COLORTYPE nColorType)
{
	if (nColorType == VO_ANDROID_COLOR_RGB565 || nColorType == VO_ANDROID_COLOR_YUV420)
		return true;
	else
		return false;
}

void * vomeRGB565VideoRender::GetVideoMemOperator (void)
{
	return NULL;
//	return &g_vmVROP;
}

bool vomeRGB565VideoRender::Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer)
{
    //LOGW("@@@@@@  vomeRGB565VideoRender Render. \n");
	if (m_pOutBuffer == NULL)
	{
		if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_RGB565)
			createMemHeap (false);
		else
			createMemHeap (true);
	}

	if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_RGB565)
	{
		if (m_pOutBuffer != pVideoBuffer->virBuffer[0])
		{
		    LOGW("@@@@@@  vomeRGB565VideoRender Render RGB565. \n");
			for (int i = 0; i < m_nVideoHeight; i++)
				memcpy (m_pOutBuffer + m_nVideoWidth * 2 * i, pVideoBuffer->virBuffer[0] + pVideoBuffer->nStride[0] * i, m_nVideoWidth * 2);
		}
		m_pSurface->postBuffer (0);
	}
	else if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_YUV420)
	{
        LOGW("@@@@@@  vomeRGB565VideoRender Render YUV420. \n");
		unsigned char * pDst = m_pOutBuffer;
#if 0
		int i = 0;
		for (i = 0; i < m_nVideoHeight; i++)
			memcpy (pDst + m_nVideoWidth * i, pVideoBuffer->virBuffer[0] + pVideoBuffer->nStride[0] * i, m_nVideoWidth);

		pDst = m_pOutBuffer + m_nVideoWidth * m_nVideoHeight;
		for (i = 0; i < m_nVideoHeight / 2; i++)
			memcpy (pDst + m_nVideoWidth / 2 * i, pVideoBuffer->virBuffer[1] + pVideoBuffer->nStride[1] * i, m_nVideoWidth / 2);

		pDst = m_pOutBuffer + m_nVideoWidth * m_nVideoHeight * 5 / 4;
		for (i = 0; i < m_nVideoHeight / 2; i++)
			memcpy (pDst + m_nVideoWidth / 2 * i, pVideoBuffer->virBuffer[2] + pVideoBuffer->nStride[2] * i, m_nVideoWidth / 2);
#else //Convert to RGB565
        if (m_pCCRR == NULL)
            m_pCCRR = new voCCRR();
        VO_VIDEO_BUFFER inBuf;
        VO_VIDEO_BUFFER	outBuf;

        inBuf.Buffer[0] = pVideoBuffer->virBuffer[0];
        inBuf.Buffer[1] = pVideoBuffer->virBuffer[1];
        inBuf.Buffer[2] = pVideoBuffer->virBuffer[2];
        
        inBuf.Stride[0] = pVideoBuffer->nStride[0];
        inBuf.Stride[1] = pVideoBuffer->nStride[1];
        inBuf.Stride[2] = pVideoBuffer->nStride[2];

        int w, h, l, t;
#if 1 // full screen
        w = m_nDispWidth;
        h = m_nDispHeight;
#else // keep aspect
        //if (m_nDispWidth / m_nDispHeight > m_nVideoWidth / m_nVideoHeight)
        if (m_nDispWidth * m_nVideoHeight > m_nVideoWidth * m_nDispHeight)
        {
            h = m_nDispHeight;
            w = m_nVideoWidth * h / m_nVideoHeight;
        }
        else
        {
            w = m_nDispWidth;
            h = m_nVideoHeight * w / m_nVideoWidth;
        }
#endif
        w &= ~0x3;
        h &= ~0x1;

        l = (m_nDispWidth - w) / 2;
        t = (m_nDispHeight - h) / 2;
        pDst += (m_nDispWidth * t + l) * 2;
        outBuf.Buffer[0] = pDst;
        outBuf.Stride[0] = m_nDispWidth * 2;

        LOGW("SetInputSize(%d, %d)", m_nVideoWidth, m_nVideoHeight);
        if (m_pCCRR->SetInputSize (m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight) >= 0)
        {
            LOGW("SetOutputSize(%d, %d, %d, %d, %d, %d)", w, h, l, t, l + w, t + h);
            if (m_pCCRR->SetOutputSize (w, h, l, t, l + w, t + h) >= 0)
            {
                //LOGW("ProcessRGB565");
                int nAngle = 0;
#ifndef __VONJ_FROYO__
                m_pCCRR->ProcessRGB565 (PIXEL_FORMAT_YCbCr_420_P, &inBuf, &outBuf, nAngle);
#else
                m_pCCRR->ProcessRGB565 (VO_COLOR_YUV_PLANAR420, &inBuf, &outBuf, nAngle);
#endif
                //LOGW("ProcessRGB565 Done");
            }
        }

#endif

		m_pSurface->postBuffer(0);
	}

	return true;
}

bool vomeRGB565VideoRender::createMemHeap (bool bYUV)
{
	// release resources if previously initialized
	releaseMemHeap();

    LOGW("vomeRGB565VideoRender::createMemHeap %d x %d bYUV: %d", m_nVideoWidth, m_nVideoHeight, bYUV);

	int nFormat = PIXEL_FORMAT_RGB_565;
    int nWidth, nHeight;
	if (bYUV)
    {
        nWidth = m_nDispWidth;
        nHeight = m_nDispHeight;
    }
    else
    {
        nWidth = m_nVideoWidth;
        nHeight = m_nVideoHeight;
    }
	m_pFrameHeap = new MemoryHeapBase(nWidth * nHeight * 2);
	ISurface::BufferHeap Buffers (nWidth, nHeight, nWidth, nHeight, nFormat, m_pFrameHeap);
	status_t s = m_pSurface->registerBuffers(Buffers);
    LOGW("vomeRGB565VideoRender::createMemHeap registerBuffers = %d - %s", s, strerror(s));
	m_pOutBuffer = static_cast<unsigned char*>(m_pFrameHeap->base());

	memset (m_pOutBuffer, 0, nWidth * nHeight * 2);

	m_bInit = true;

	return true;
}

void vomeRGB565VideoRender::releaseMemHeap (void)
{
	if (m_bInit)
	{
		m_pSurface->unregisterBuffers();

		if (m_pFrameHeap != NULL)
			m_pFrameHeap.clear();

		m_bInit = false;
	}
}

} // end namespace android

