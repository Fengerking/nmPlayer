
/************************************************************************
 *  *                                    *
 *   *    VisualOn, Inc. Confidential and Proprietary, 2003       *
 *    *                                    *
 *     ************************************************************************/
/*******************************************************************************
 * File:   voTIVideoRender.h
 *
 * Contains: voTIVideoRender header file
 *
 * Written by: David
 *
 * Change History (most recent first):
 * 2009-10-15  David Create file
 *
 *  *******************************************************************************/

#if !defined __VO_TI_VIDEO_RENDER_H__
#define __VO_TI_VIDEO_RENDER_H__

#include <ui/Overlay.h>
#if defined __VONJ_ECLAIR__
#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>
#elif defined __VONJ_FROYO__ || defined __VONJ_GINGERBREAD__
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/SurfaceComposerClient.h>
#endif

#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>
#include <utils/MemoryHeapPmem.h>
#elif defined __VONJ_ECLAIR__  || defined __VONJ_FROYO__
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#endif

#include "voCOMXThreadMutex.h"
#include "voCCRR.h"
#include "CIntQueue.h"

using namespace android;


#define MAX_V4L2_BUFFER		6
#define CACHEABLE_BUFFERS 0x1

#ifdef TARGET_OMAP4
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    2
#else
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    4
#endif

#if defined __cplusplus
extern "C" {
#endif
	typedef void (*VOCONV420TO422API)(unsigned char* Dst, unsigned char* Y, unsigned char* U, unsigned char* V, int YStride, int UVStride, int w, int h, int outstride);
#if defined __cplusplus
}     
#endif

#define OVERLAY720P 0

typedef struct {
	int fd;
	size_t length;
	uint32_t offset;
	void *ptr;
} mapping_data_t;

typedef struct {
	int flag;
	void* buf;
} VOTIOB;

class voTIVideoRender {

friend class voCOMXTIOverlayPort;

public:
	voTIVideoRender(void* pview=NULL);
	virtual ~voTIVideoRender();

	void SetView(void*);
	bool Render(unsigned char*);
	bool CreateOverlay(const int w, const int h);
	bool RequestBuffer(void** pout, const int w, const int h);
	void FreeBuffer(); 
	void Close();
	void setVideoInfo(const int w, const int h, const int clr);
	const int GetBufferSize() const;
	const int InquireIdleBuffer();
	const int getQueuedBuffer(int* pbuf = NULL);
	const int getSkippedBuffer() { return m_nbufskipped; }

	void  Flush();
protected:

  bool createMemHeap();
	void releaseMemHeap(void);

private:
	void* m_pview;
	sp<Overlay>	m_poverlay;
  sp<MemoryHeapBase>  m_pFrameHeap;
  voCCRR*	        m_pccrr;

	int m_nVideoWidth;
	int m_nVideoHeight;
	int m_nbufcount;
	int m_nbufsize;
	int m_nqueued;
	int m_nbuf2enq;
	int m_fmtcolor;
	int m_nrenderedframes;
	int m_nbufskipped;
	bool m_bcreated;
	bool m_bminivideo;
	void* m_hconv;
	CIntQueue m_lstqueued;
	VOCONV420TO422API m_pfconv420to422;
	void* m_ppbuf[MAX_V4L2_BUFFER];
	voCOMXThreadMutex       m_mutlock;
};

#endif // __VO_TI_VIDEO_RENDER_H__

