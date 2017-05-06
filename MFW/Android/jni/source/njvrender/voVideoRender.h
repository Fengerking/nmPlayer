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

#ifndef ANDROID_voVideoRender_H
#define ANDROID_voVideoRender_H

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

#include "voVideoRenderType.h"

#include "voVideo.h"
#include "viMem.h"

#include "voCCRR.h"

#define ANDROID_LOOP_TAG "ANDROID_LOOP"

namespace android {

class voVideoRender
{
public:
	voVideoRender(void);
	virtual ~voVideoRender(void);

	virtual int			SetVideoSurface (const sp<Surface>& surface);
	virtual int			SetVideoSize (int nWidth, int nHeight);
	virtual int			SetDispSize (int nWidth, int nHeight);
	virtual bool		CheckColorType (VO_ANDROID_COLORTYPE nColorType);

    virtual bool		Render(VO_ANDROID_VIDEO_BUFFERTYPE* pVideoBuffer);

    unsigned char *		GetOutputBuffer (void) {return NULL;}
    void *				GetVideoMemOperator (void);

protected:

protected:
	sp<Surface>				m_pSurface;
    Surface::SurfaceInfo	m_si;

  	int						m_nVideoWidth;
 	int						m_nVideoHeight;

	int						m_nDispWidth;
 	int						m_nDispHeight;

private:
    voCCRR  *				m_pCCRR;
    VO_VIDEO_BUFFER			m_inBuf;
    VO_VIDEO_BUFFER			m_outBuf;

};

}; // namespace android

#endif // ANDROID_voVideoRender_H

