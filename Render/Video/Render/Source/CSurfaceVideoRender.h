	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSurfaceVideoRender.h

	Contains:	CSurfaceVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CSurfaceVideoRender_H__
#define __CSurfaceVideoRender_H__

#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>

// pmem interprocess shared memory support
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>
#include <utils/MemoryHeapPmem.h>

#include "CBaseVideoRender.h"

using namespace android;

class CSurfaceVideoRender : public CBaseVideoRender
{
public:
	// Used to control the image drawing
	CSurfaceVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CSurfaceVideoRender (void);

	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect);
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	Redraw (void);

protected:
	bool 			createMemHeap (void);
    void 			releaseMemHeap (void);

protected:
	ISurface *					m_pSurface;
    sp<MemoryHeapBase>          m_pFrameHeap;
 	ISurface::BufferHeap 	    m_BufferHeap;
 	VO_U32						m_nFrameSize;
 	VO_U32						m_nFrameCount;
 	VO_U32						m_nFrameIndex;

	VO_BOOL						m_bInit;
    VO_VIDEO_BUFFER	            m_OutBuffer;
	VO_U32						m_nOffset;
	VO_PBYTE					m_pOutBuffer;

	char						m_szLog[256];
};

#endif // __CSurfaceVideoRender_H__
