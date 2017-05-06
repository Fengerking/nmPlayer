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
#define LOG_TAG "vomeQCMVideoRender"
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

#include "vomeQCMVideoRender.h"

#include "vomeAPPath.h"

static const char* pmem_adsp = "/dev/pmem_adsp";
static const char* pmem = "/dev/pmem";

// ----------------------------------------------------------------------------

namespace android {

vomeQCMVideoRender::vomeQCMVideoRender(void)
	: vomeRGB565VideoRender ()
	, m_pFramepmemHeap (NULL)
	, m_pMemList (NULL)
	, m_nBufferWidth (0)
	, m_nBufferHeight (0)
	, m_pUVBuffer (NULL)
{
    LOGW("@@@@@@  vomeQCMVideoRender vomeQCMVideoRender V0201A\n");

	m_fPackUV = NULL;
	m_hModule = NULL;
}

vomeQCMVideoRender::~vomeQCMVideoRender(void)
{
    releaseMemHeap ();

    if (m_hModule != NULL)
    	dlclose (m_hModule);

    LOGW("@@@@@@  vomeQCMVideoRender destructor \n");
}

bool vomeQCMVideoRender::createMemHeap (bool bQCOMDec)
{
    LOGW("@@@@@@  vomeQCMVideoRender createMemHeap  HW %d m_nVideoWidth = %d m_nVideoHeight = %d\n", bQCOMDec, m_nVideoWidth, m_nVideoHeight);

	// release resources if previously initialized
	releaseMemHeap();

	m_nBufferWidth = (m_nVideoWidth + 15) & ~0X0F;
	m_nBufferHeight = (m_nVideoHeight + 15) & ~0X0F;

	// create frame buffer heap
	sp<MemoryHeapBase> master = NULL;
	if (bQCOMDec)
	{
		PLATFORM_PRIVATE_PMEM_INFO * pMemInfo = NULL;
		GetMemInfo (m_pMemList, &pMemInfo);
		if (pMemInfo != NULL)
			master = (MemoryHeapBase *)pMemInfo->pmem_fd;
		else
			return false;
	}
	else
	{
		// YUV420 frames are 1.5 bytes/pixel
		master = new MemoryHeapBase(pmem_adsp, (m_nBufferWidth * m_nBufferHeight * 3) / 2);
	    if (master->heapID() < 0)
	        return false;
	}

	master->setDevice(pmem);
	m_pFramepmemHeap = new MemoryHeapPmem(master, 0);
	m_pFramepmemHeap->slap();
	master.clear();

	// register frame buffers with SurfaceFlinger
#ifndef __VONJ_FROYO__
	ISurface::BufferHeap buffers(m_nVideoWidth, m_nVideoHeight, m_nBufferWidth, m_nBufferHeight,
				     PIXEL_FORMAT_YCbCr_420_SP, m_pFramepmemHeap);
#else
	ISurface::BufferHeap buffers(m_nVideoWidth, m_nVideoHeight, m_nBufferWidth, m_nBufferHeight,
				     VO_COLOR_YUV_PLANAR420, m_pFramepmemHeap);
#endif
	m_pSurface->registerBuffers(buffers);

	m_pOutBuffer = static_cast<unsigned char*>(m_pFramepmemHeap->base());
	if (!bQCOMDec)
	{
		memset (m_pOutBuffer, 0, m_nBufferWidth * m_nBufferHeight);
		memset (m_pOutBuffer + m_nBufferWidth * m_nBufferHeight, 127, m_nBufferWidth * m_nBufferHeight / 2);

		m_pUVBuffer = new unsigned char[m_nBufferWidth * m_nBufferHeight / 2];
		memset (m_pUVBuffer, 127, m_nBufferWidth * m_nBufferHeight / 2);
	}

	if (m_hModule == NULL)
	{
	  //m_hModule = dlopen (VO_APPLICATION_LIB_LOCATION "libvoPackUV.so", RTLD_NOW);
	  char s[256];
	  strcpy(s, vo_application_lib_location); strcat(s, "libvoPackUV.so");
	    
	  m_hModule = dlopen (s, RTLD_NOW);
	  if (m_hModule == NULL)
	    m_hModule = dlopen ("voPackUV.so", RTLD_NOW);
	  if (m_hModule != NULL)
	    {
	      m_fPackUV = (VOPACKUVAPI) dlsym(m_hModule, "voPackUV");
	      LOGW("@@@@@@  vomeQCMVideoRender PackUV %x\n", (int)m_fPackUV);
	    }
	}

	m_bInit = true;

	return true;
}

void vomeQCMVideoRender::releaseMemHeap (void)
{
	if (m_bInit)
	{
		m_pSurface->unregisterBuffers();

		m_pFramepmemHeap.clear();

		if (m_pUVBuffer != NULL)
			delete []m_pUVBuffer;
		m_pUVBuffer = NULL;

		m_pSurface.clear ();

		m_bInit = false;
	}
}

bool vomeQCMVideoRender::CheckColorType (VO_ANDROID_COLORTYPE nColorType)
{
	if (nColorType == VO_ANDROID_COLOR_YUV420 || nColorType == VO_ANDROID_COLOR_NV12)
		return true;
	else
		return false;
}

bool vomeQCMVideoRender::Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer)
{
	if (m_pSurface == NULL)
		return true;

	m_pMemList = (PLATFORM_PRIVATE_LIST *)pVideoBuffer->virBuffer[2];

	if (m_pOutBuffer == NULL)
	{
		if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_NV12)
			createMemHeap (true);
		else
			createMemHeap (false);
	}

	if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_NV12)
	{
        LOGW("VO_ANDROID_COLOR_NV12");
		PLATFORM_PRIVATE_PMEM_INFO * pMemInfo = NULL;
		GetMemInfo (m_pMemList, &pMemInfo);

		if (pMemInfo != NULL)
		{
			m_pSurface->postBuffer (pMemInfo->offset);
		}
	}
	else
	{
        LOGW("convertFrame");
		convertFrame (pVideoBuffer, m_pOutBuffer);

		m_pSurface->postBuffer (0);
	}

	return true;
}

void vomeQCMVideoRender::convertFrame (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer, unsigned char * pDst)
{
	int i = 0;
	for (i = 0; i < m_nVideoHeight; i++)
		memcpy (pDst + m_nBufferWidth * i, pVideoBuffer->virBuffer[0] + pVideoBuffer->nStride[0] * i, m_nVideoWidth);

	unsigned char * pUV = pDst + m_nBufferWidth * m_nBufferHeight;
	if (pVideoBuffer->nColorType == VO_ANDROID_COLOR_YUV420)
	{
		if (m_fPackUV != NULL)
		{
			unsigned char * pUVMem = m_pUVBuffer;
			m_fPackUV (pUVMem, pVideoBuffer->virBuffer[2], pVideoBuffer->virBuffer[1], pVideoBuffer->nStride[2], pVideoBuffer->nStride[1],
						   m_nVideoHeight / 2, m_nVideoWidth/ 2, m_nBufferWidth);

			memcpy (pUV, m_pUVBuffer, m_nBufferWidth * m_nBufferHeight / 2);
		}
	}
	else
	{
		for (i = 0; i < m_nVideoHeight / 2; i++)
			memcpy (pUV + m_nBufferWidth * i, pVideoBuffer->virBuffer[1] + pVideoBuffer->nStride[1] * i, m_nVideoWidth);
	}
}

bool vomeQCMVideoRender::GetMemInfo (PLATFORM_PRIVATE_LIST * pList, PLATFORM_PRIVATE_PMEM_INFO ** ppMemInfo)
{
	if (pList == NULL || ppMemInfo == NULL)
		return false;

	*ppMemInfo = NULL;
	for (int i = 0; i < pList->nEntries; i++)
	{
		PLATFORM_PRIVATE_ENTRY * pEntry = &pList->entryList[i];

		if (pEntry->type == PLATFORM_PRIVATE_PMEM)
		{
			* ppMemInfo = (PLATFORM_PRIVATE_PMEM_INFO *)pEntry->entry;
			break;
		}
	}

	return *ppMemInfo != NULL;
}

} // end namespace android

