
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
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/SurfaceComposerClient.h>

#include "CIntQueue.h"

using namespace android;

#define VOCMP_TIOMAP4_HWVIDEORENDER_NAME "OMX.VOTIOMAP4.Video.Render"

#define COLOR_FORMAT 0  // 0: nv12,  1: yuv422
#define MAX_V4L2_BUFFER	  32
#define ARMPAGESIZE				4096
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    2
#define DOUBLE_BEST_BUFFER_NUM   (NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE * 2)
#define MAX_POPUP_COUNT		(DOUBLE_BEST_BUFFER_NUM - 1)


#if defined __cplusplus
extern "C" {
#endif

#include "voCOMXThreadMutex.h"

typedef void (*VOPACKUV)(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest);

#if defined __cplusplus
}     
#endif


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
typedef VOTIOB* PVOTIOB;

class voTIVideoRender {

friend class voCOMXTIOverlayPort;

public:
	voTIVideoRender(void* pview=NULL);
	virtual ~voTIVideoRender();

	void SetView(void*);
	bool Render(unsigned char*, const int size);
	bool CreateOverlay(const int w, const int h);
	void FreeBuffer(); 
	void Close();
	void setVideoInfo(const int w, const int h, const int clr);
	void setDimensionInfo(const int w, const int h, const int degree);
	void setForceBufNum(const int num);

	const int RequestBuffer(void** pout, const bool recreated = false);
	const int GetBufferSize() const { return m_nbufsize; }
	const int InquireIdleBuffer();
	const int getQueuedBuffer();
	const int getRenderedFrames() { return m_nrenderedframes; }
	const int getPoppedBuffer();
	const int getSkippedBuffer() { return m_nbufskipped; }
	void  setUpsideDown(const bool bdown = true) { m_bupsidedown = bdown; }
	void  Flush();

private:
	void* m_pview;
	sp<Overlay>	m_poverlay;
	int m_nx;
	int m_ny;
	int m_nVideoWidth;
	int m_nVideoHeight;
	int m_nDecodedWidth;
	int m_nDecodedHeight;
	int m_nforcebufnum;
	int m_nbufcount;
	int m_nbufsize;
	int m_nqueued;
	int m_nbuf2enq;
	int m_fmtcolor;
	int m_nrenderedframes;
	int m_npoppedcount;
	int m_nrotatedegree;
	int m_nbufskipped;
	bool m_bcreated;
	bool m_bupsidedown;
	void* m_hconv;
	CIntQueue m_lstqueued;
	VOPACKUV m_pfpackuv;
	void* m_ppbuf[MAX_V4L2_BUFFER];
	voCOMXThreadMutex       m_mutlock;
};

#endif // __VO_TI_VIDEO_RENDER_H__

