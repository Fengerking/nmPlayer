/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CSurfaceVideoRender.h

	Contains:	CSurfaceVideoRender header file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2011-06-08		JBF			Create file

*******************************************************************************/

#ifndef __CSurfaceVideoRender_h__
#define __CSurfaceVideoRender_h__

#include <sys/types.h>
#include <utils/Errors.h>

#if !defined __VONJ_FROYO__ && !defined __VONJ_GINGERBREAD__
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
#elif defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__ || defined __VONJ_GINGERBREAD__
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#endif
 
#include <jni.h>

#include "vompType.h"
#include "voOnStreamType.h"
#include <media/AudioTrack.h>

namespace android {

// ----------------------------------------------------------------------------
class CSurfaceVideoRender
{
public:
	CSurfaceVideoRender(void * pSurface, int nFlag);
	virtual	~CSurfaceVideoRender(void);

	virtual int Lock (VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize);
	virtual int UnLock (void);
	virtual int GetParam (int nID, void * pValue);
	virtual int SetParam (int nID, void * pValue);

protected:
	sp<Surface>				m_pSurface;
	int						m_nFlag;

	int						m_nBytesPixel;
    Surface::SurfaceInfo	m_siInfo;

	int						m_nLockNum;
	int						m_nRendNum;
};

}; // namespace android

#endif //#define __CSurfaceVideoRender_h__

