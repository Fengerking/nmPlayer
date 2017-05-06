
// CSurfaceVRender.h
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSurfaceVRender.h

    aim:    for providing the  render based on Surface(android platform)

	Written by:	Xia GuangTai

	Change History (most recent first):
	2009-5-18		gtxia			Create file

*******************************************************************************/


#ifndef __CSurfaceVRender_H__
#define __CSurfaceVRender_H__

// SurfaceFlinger
#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>

// pmem interprocess shared memory support
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>
#include <utils/MemoryHeapPmem.h>

#include "CBaseVideoRender.h"

using namespace android;

class CSurfaceVRender : public CBaseVideoRender
{
public:
	// Used to control the image drawing
	CSurfaceVRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	~CSurfaceVRender (void);

	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	virtual VO_U32	UpdateSize (void);

    bool   CreateSurface (void);
    bool   ReleaseSurface (void);

    void 	convertFrame (VO_VIDEO_BUFFER * pVideoBuffer, unsigned char * pDst);

protected:
    sp<SurfaceComposerClient>   mClient;
    sp<Surface>                 mSurface;
    sp<ISurface>                mISurface;

    sp<MemoryHeapBase>          mFrameHeap;

    VO_U32                      mOutStride;
	VO_U32						mOffset;
    VO_VIDEO_BUFFER	            mOutBuffer;


 	ISurface::BufferHeap 	    m_BufferHeap;
};

#endif // __CSurfaceVRender_H__

