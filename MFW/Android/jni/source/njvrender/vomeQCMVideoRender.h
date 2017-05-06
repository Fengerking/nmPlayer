/*
**
** Copyright 2008, The Android Open Source Project
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

#ifndef ANDROID_vomeQCMVideoRender_H
#define ANDROID_vomeQCMVideoRender_H

#include <utils/threads.h>

#include <media/MediaPlayerInterface.h>
#ifndef __VONJ_FROYO__
#include <ui/Surface.h>
#include <ui/SurfaceComposerClient.h>
#else
#include <surfaceflinger/Surface.h>
#include <surfaceflinger/SurfaceComposerClient.h>
#endif

// pmem interprocess shared memory support
#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>
#include <utils/MemoryHeapPmem.h>
#elif defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#endif

#include "vomeRGB565VideoRender.h"

#define ANDROID_LOOP_TAG "ANDROID_LOOP"

namespace android {

class vomeQCMVideoRender : public vomeRGB565VideoRender
{
public:
	vomeQCMVideoRender(void);
	virtual ~vomeQCMVideoRender(void);

	virtual bool		CheckColorType (VO_ANDROID_COLORTYPE nColorType);

    virtual bool		Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer);

protected:
    virtual bool 		createMemHeap (bool bQCOMDec = false);
    virtual void 		releaseMemHeap (void);

protected:
    void 				convertFrame (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer, unsigned char * pDst);
    bool				GetMemInfo (PLATFORM_PRIVATE_LIST * pList, PLATFORM_PRIVATE_PMEM_INFO ** ppMemInfo);


protected:
    sp<MemoryHeapPmem>          m_pFramepmemHeap;
	PLATFORM_PRIVATE_LIST *		m_pMemList;

 	int							m_nBufferWidth;
	int							m_nBufferHeight;

	void *						m_hModule;
	VOPACKUVAPI					m_fPackUV;
	unsigned char *				m_pUVBuffer;
};

}; // namespace android

#endif // ANDROID_vomeQCMVideoRender_H

