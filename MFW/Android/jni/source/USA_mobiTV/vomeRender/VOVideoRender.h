	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VOVideoRender.h

	Contains:	VOVideoRender header file

	Written by:	Yu Wei (Tom)

	Change History (most recent first):
	2010-7-7		Tom			Create file

*******************************************************************************/
#ifndef ANDROID_VOVideoRender_H
#define ANDROID_VOVideoRender_H

#include <utils/threads.h>

#include <media/MediaPlayerInterface.h>
#if !defined __VONJ_FROYO__
#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>
#else
#include <surfaceflinger/ISurface.h>
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

#include "vojniconst.h"
#include "voVideo.h"
#include "viMem.h"
#include "voCCRR.h"

#include "VOBaseVideoRender.h"

namespace android {

class VOVideoRender : public VOBaseVideoRender
{
public:
	VOVideoRender(void);
	virtual ~VOVideoRender(void);

	virtual int			SetVideoSurface (const sp<Surface>& surface);
	virtual int			SetVideoSize (int nWidth, int nHeight);
	virtual bool		CheckColorType (VO_ANDROID_COLORTYPE nColorType);

	virtual bool		Render(VO_ANDROID_VIDEO_BUFFERTYPE* pVideoBuffer);

protected:
    Surface::SurfaceInfo	m_si;

	int						m_nDispWidth;
 	int						m_nDispHeight;

private:
    voCCRR  *				m_pCCRR;
    VO_VIDEO_BUFFER			m_inBuf;
    VO_VIDEO_BUFFER			m_outBuf;
};

}; // namespace android

#endif // ANDROID_VOVideoRender_H

