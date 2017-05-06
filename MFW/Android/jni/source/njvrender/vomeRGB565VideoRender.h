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

#ifndef ANDROID_vomeRGB565VideoRender_H
#define ANDROID_vomeRGB565VideoRender_H

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
#elif defined __VONJ_ECLAIR__  || defined __VONJ_FROYO__
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#endif

#include "voVideoRenderType.h"

#include "voVideo.h"
#include "viMem.h"
#include "voCCRR.h"

#define ANDROID_LOOP_TAG "ANDROID_LOOP"

namespace android {

class vomeRGB565VideoRender
{
public:
	vomeRGB565VideoRender(void);
	virtual ~vomeRGB565VideoRender(void);

	virtual int			SetVideoSurface (const sp<ISurface>& surface);
	virtual int			SetVideoSize (int nWidth, int nHeight);
	virtual int			SetDispSize (int nWidth, int nHeight);
	virtual bool		CheckColorType (VO_ANDROID_COLORTYPE nColorType);

    virtual bool		Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer);

    unsigned char *		GetOutputBuffer (void) {return m_pOutBuffer;}
    void *				GetVideoMemOperator (void);

protected:
    virtual bool 		createMemHeap (bool bQCOMDec = false);
    virtual void 		releaseMemHeap (void);

protected:
	sp<ISurface>		m_pSurface;
    sp<MemoryHeapBase>  m_pFrameHeap;

	bool				m_bInit;
 	unsigned char *		m_pOutBuffer;

  	int					m_nVideoWidth;
 	int					m_nVideoHeight;
 	int					m_nBufferWidth;
 	int					m_nBufferHeight;

  	int					m_nDispWidth;
 	int					m_nDispHeight;

private:
    voCCRR  *	        m_pCCRR;
};

}; // namespace android

#endif // ANDROID_vomeRGB565VideoRender_H

