/************************************************************************
 *   *                                   *
 *     *   VisualOn, Inc. Confidential and Proprietary, 2003       *
 *       *                                   *
 *         ************************************************************************/
/*******************************************************************************
 *   File:   voTIVideoRender.cpp
 *
 *   Contains: voTIVideoRender class file
 *
 *   Written by: David
 *
 *   Change History (most recent first):
 *   2009-10-15  created by david
 *
 ********************************************************************************/


#define LOG_TAG "voTIVideoRender"

#include <hardware/overlay.h>
#include <dlfcn.h>
#include "voTIVideoRender.h"
#include "OMX_IVCommon.h"
#include "voIVCommon.h"
#include "voOMXMemory.h"
#include "voinfo.h"

voTIVideoRender::voTIVideoRender(void* pview)
			: m_pview(pview)
			, m_poverlay(NULL)
			, m_pFrameHeap (NULL)
			, m_pccrr(NULL)
			, m_nVideoWidth(0)
			, m_nVideoHeight(0)
			, m_nbufcount(0)
			, m_nbufsize(0)
			, m_nqueued(0)
			, m_nbuf2enq(-1)
			, m_fmtcolor(OMX_COLOR_FormatCbYCrY)
			, m_nrenderedframes(0)
			, m_nbufskipped(-1)
			, m_bcreated(false)
			, m_bminivideo(false)
			, m_hconv(NULL)
			, m_pfconv420to422(NULL)
{
	memset(m_ppbuf, 0, sizeof(void*) * MAX_V4L2_BUFFER);
}

voTIVideoRender::~voTIVideoRender() {
	Close();
}

void voTIVideoRender::SetView(void* pview) {
	m_pview = (void*)pview;
}

void voTIVideoRender::setVideoInfo(const int w, const int h, const int clr) {

	m_nVideoWidth = w;
	m_nVideoHeight = h;
	m_nbufsize = w * m_nVideoHeight;
	if (m_nVideoHeight > 800)
		m_nVideoHeight = 800;

	if (m_nVideoWidth <= 128 && m_nVideoHeight <= 96) 
		m_bminivideo = false;

	VOINFO("video: w x h = %d x %d, color: %d, bminivideo: %d", w, h, clr, m_bminivideo);
	m_fmtcolor = clr;
}

bool voTIVideoRender::CreateOverlay(const int width, const int height) {

	Close();

	int w = width; 
	int h = height;
	if (h > 800) 
		h = 800;


	if (m_bminivideo) {
		createMemHeap();
		m_bcreated = true;
		m_nrenderedframes = 0;
		return true;
	}

	if (m_poverlay.get() == NULL) {

		sp<OverlayRef> ref = NULL;
		if (m_pview) {
#if defined __VONJ_ECLAIR__
			VOINFO("overlay: w x h = %d x %d", w, h);
#if OVERLAY720P 
			ref = ((ISurface*)m_pview)->createOverlay(1280, 720, OVERLAY_FORMAT_CbYCrY_422_I);
#else
			ref = ((ISurface*)m_pview)->createOverlay(w, h, OVERLAY_FORMAT_CbYCrY_422_I);
#endif
#elif defined __VONJ_FROYO__ || defined __VONJ_GINGERBREAD__
			ref = ((ISurface*)m_pview)->createOverlay(w, h, OVERLAY_FORMAT_CbYCrY_422_I, 0);
#endif
		} else {
			m_bcreated = false;
			return false;
		}

		if (ref.get() == NULL) {
			VOINFO("ref.get() == NULL: Vendor Speicifc(34xx)MIO: Creating overlay failed");
			return false;
		} else {
			VOINFO("Vendor Speicifc(34xx)MIO: Creating overlay OK!");
		}


		m_poverlay = new Overlay(ref);
		//m_poverlay->resizeInput(w, h);
		//m_poverlay->setParameter(CACHEABLE_BUFFERS, 0);
	} else {
		m_poverlay->resizeInput(w, h);
	}

	m_poverlay->setCrop(0, 0, w, h);
	m_nbufcount = m_poverlay->getBufferCount();
	if (m_nbufcount > MAX_V4L2_BUFFER)
		m_nbufcount = MAX_V4L2_BUFFER;
	VOINFO("buf count: %d", m_nbufcount);

	mapping_data_t *pmd = NULL;
	for (int i = 0; i < m_nbufcount; i++) {
		pmd = (mapping_data_t*)m_poverlay->getBufferAddress((void*)i);

		m_ppbuf[i] = pmd->ptr;
		strcpy((char*)m_ppbuf[i], "hello");
		if (strcmp((char*)m_ppbuf[i], "hello")) {
			VOINFO("problem with buffer");
			return false;
		} 
	}

	if (m_pfconv420to422 == NULL) {
		m_hconv = dlopen ("libvoConv420To422.so", RTLD_NOW);
		if (m_hconv != NULL) {
			m_pfconv420to422 = (VOCONV420TO422API)dlsym(m_hconv, "YUV420_UYVY422");
			if (m_pfconv420to422 == NULL) return false;

			VOINFO("vomeTIOverlay voConv420To422: %p", m_pfconv420to422);
		}
	}

	m_nbuf2enq = -1;
	m_nqueued = 0;
	m_bcreated = true;
	m_nbufskipped = -1;
	m_nrenderedframes = 0;

	m_lstqueued.setCapability(m_nbufcount);
	m_lstqueued.clearAll();
	return true;
}

bool voTIVideoRender::RequestBuffer(void** pout, const int width, const int height) {

	if (!m_bcreated) {

		if (!CreateOverlay(width, height))
			return false;
	}

	for (int i = 0; i < m_nbufcount; i++) 
		pout[i] = m_ppbuf[i];

	m_nbuf2enq = -1;
	m_nqueued = 0;
	m_nrenderedframes = 0;

	m_lstqueued.clearAll();
	return true;
}

void	voTIVideoRender::FreeBuffer() {

	voOMXMemSet(m_ppbuf, 0, sizeof(void*) * m_nbufcount);
	m_nbufcount = 0;
	m_nqueued = 0;
	m_nbuf2enq = -1;
	m_bcreated = false;
	m_nbufskipped = -1;
	m_nrenderedframes = 0;

	if (m_bminivideo) 
		releaseMemHeap();

	m_lstqueued.clearAll();
}


void voTIVideoRender::Close() {

	if (m_poverlay.get() != NULL) {
		m_poverlay->destroy();
		m_poverlay.clear();
		m_poverlay = NULL;
	}
	
	if (m_hconv) {
		dlclose(m_hconv);
		m_hconv = NULL;
		m_pfconv420to422 = NULL;
	}

	FreeBuffer();

	if (m_pccrr) {
		delete m_pccrr;
		m_pccrr = NULL;
	}
}

bool voTIVideoRender::Render(unsigned char* pBuffer) {

	voCOMXAutoLock lock (&m_mutlock);
	OMX_U8* pbuf = pBuffer;
	int ret = 0, current = 0;
	m_nbufskipped = -1;
	if (m_fmtcolor == OMX_COLOR_FormatCbYCrY) {
		int i = 0;
		for (; i < m_nbufcount; i++) {
			if (m_ppbuf[i] == pbuf) 
				break;
		}

		current = i;

		if (m_lstqueued.getCount()) {
			int nbuf = -1;
			int npos = m_lstqueued.getHeadPosition();
			while (npos > -1) {
				m_lstqueued.get(npos, nbuf);
				if (nbuf == current)  {
					m_nbufskipped = nbuf;
					break;
				}
			}
		}

	} else if (m_fmtcolor == OMX_COLOR_FormatYUV420Planar) {
		m_nbuf2enq = (++m_nbuf2enq) % m_nbufcount;
		current = m_nbuf2enq;
		OMX_U8* pd = (OMX_U8*)m_ppbuf[current];
		VO_VIDEO_BUFFER *ps = (VO_VIDEO_BUFFER*)pbuf; 

		ps->Buffer[0] = pbuf;
		ps->Buffer[1] = pbuf + m_nbufsize; 
		ps->Buffer[2] = pbuf + m_nbufsize * 5 / 4;
		ps->Stride[0] = m_nVideoWidth;
		ps->Stride[2] = ps->Stride[1] = m_nVideoWidth / 2;
		//VOINFO("video w: %d, h: %d, bufsize: %d, buffer[0]: %p, buffer[1]: %p, buffer[2]: %p, stride[0]: %d, stride[1]: %d, stride[2]: %d", m_nVideoWidth,
		//m_nVideoHeight, m_nbufsize, ps->Buffer[0],ps->Buffer[1],ps->Buffer[2],ps->Stride[0],ps->Stride[1], ps->Stride[2]); 
		if (!m_bminivideo) {
#if OVERLAY720P 
			m_pfconv420to422(pd,ps->Buffer[0],ps->Buffer[1],ps->Buffer[2],ps->Stride[0],ps->Stride[1], m_nVideoWidth, m_nVideoHeight, 1280);
#else
			m_pfconv420to422(pd,ps->Buffer[0],ps->Buffer[1],ps->Buffer[2],ps->Stride[0],ps->Stride[1], m_nVideoWidth, m_nVideoHeight, m_nVideoWidth);
#endif
		} else {
			unsigned char * pDst = (unsigned char*)m_ppbuf[0];
			VO_VIDEO_BUFFER	outBuf;
			int w = 1280;

			outBuf.Buffer[0] = pDst;
			outBuf.Stride[0] = w * 2;
#ifndef __VONJ_FROYO__
			m_pccrr->ProcessRGB565 (PIXEL_FORMAT_YCbCr_420_P, ps, &outBuf, 0);
#else
			m_pccrr->ProcessRGB565 (VO_COLOR_YUV_PLANAR420, ps, &outBuf, 0);
#endif

			((ISurface*)m_pview)->postBuffer(0);
			return true;
		}
	}

	if (m_nbufskipped < 0) {
		VOINFO("input buffer: %d", current);
		ret = m_poverlay->queueBuffer((void*)current);
		if (ret >= NO_ERROR) {
			m_nqueued++;
			m_nrenderedframes++;
			m_lstqueued.enqueue(current);
		} else {
			VOINFO("input error");
		}
	}

	overlay_buffer_t ob; int next = -1;
	ret = m_poverlay->dequeueBuffer(&ob);
	if (ret == NO_ERROR) {
		next = (int)ob;
		m_nqueued--;
		m_lstqueued.dequeue();

#if defined __VODBG__
		int nbuf = -1;
		int npos = m_lstqueued.getHeadPosition();
		while (npos > -1) {
			m_lstqueued.get(npos, nbuf);
			VOINFO("queued: %d", nbuf);
		}
		VOINFO("$$$$$$$$$$$$$$ dequeue: %d $$$$$$$$$$$$$$$$", next);
#endif
	} else {
		next = -1;
		if (ret == -EPERM) {
			VOINFO("at least 2 buffers need to be queued. cur queued: %d", m_nqueued);
		}
	}

	if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar)
		m_nbuf2enq = next;

	return true;
}

const int voTIVideoRender:: GetBufferSize() const {

	int size = m_nbufsize * 2;
	return size;
}

const int voTIVideoRender::InquireIdleBuffer() {

	voCOMXAutoLock lock (&m_mutlock);

	int ib = -2;
	if (m_fmtcolor == OMX_COLOR_FormatCbYCrY) 
		ib = m_nbuf2enq;

	VOINFO("-------------------------return buffer: %d", ib);
	return ib;
}

bool voTIVideoRender::createMemHeap()
{
	// release resources if previously initialized
	releaseMemHeap();

  VOINFO("voTIVideoRender::createMemHeap %d x %d", m_nVideoWidth, m_nVideoHeight);

	//int nFormat = PIXEL_FORMAT_RGB_565;
	int nFormat = PIXEL_FORMAT_YCbCr_420_P;
	int nWidth, nHeight;
	nWidth = m_nVideoWidth;
	nHeight = m_nVideoHeight;

	m_pFrameHeap = new MemoryHeapBase(nWidth * nHeight * 2);
	ISurface::BufferHeap Buffers (nWidth, nHeight, nWidth, nHeight, nFormat, m_pFrameHeap);
	status_t s = ((ISurface*)m_pview)->registerBuffers(Buffers);
	VOINFO("voTIVideoRender::createMemHeap registerBuffers = %d - %s", s, strerror(s));
	m_ppbuf[0] = m_pFrameHeap->base();

	memset (m_ppbuf[0], 0, nWidth * nHeight * 2);

	if (m_pccrr == NULL)
		m_pccrr = new voCCRR();

	if (m_pccrr) {
		m_pccrr->SetInputSize(m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight);
		int w = 1280, h = 720;
		int l = 0, t = 0;
		m_pccrr->SetOutputSize(w, h, l, t, l + w, t + h);
	}

	m_bcreated = true;

	VOINFO("voTIVideoRender::createMemHeap OK!");
	return true;
}

void voTIVideoRender::releaseMemHeap (void)
{
	if (m_bcreated) {
		((ISurface*)m_pview)->unregisterBuffers();

		if (m_pFrameHeap != NULL)
			m_pFrameHeap.clear();

		m_bcreated = false;
	}
}

void voTIVideoRender::Flush() { 
	m_nbuf2enq = -1; 
	m_nbufskipped = -1;
	m_nrenderedframes = 0;
}

const int voTIVideoRender::getQueuedBuffer(int* pbuf) {

	voCOMXAutoLock lock (&m_mutlock);

	if (pbuf == NULL)
		return m_nqueued;
	
	int j = 0, nval = -1;
	int p = m_lstqueued.getHeadPosition();
	
	while (p > -1) {
		m_lstqueued.get(p, nval);
		*(pbuf + j++) = nval;
	}

	return j;
}

