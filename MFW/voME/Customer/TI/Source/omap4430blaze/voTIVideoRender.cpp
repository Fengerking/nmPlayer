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
#include "voLog.h"
#include "voinfo.h"

#define OPTIMAL_QBUF_CNT  0x4
#define OVERLAY_NUM_BUFFERS 7


static int Calculate_TotalRefFrames(int nWidth, int nHeight);

voTIVideoRender::voTIVideoRender(void* pview)
			: m_pview(pview)
			, m_poverlay(NULL)
			, m_nx(-1)
			, m_ny(-1)
			, m_nVideoWidth(0)
			, m_nVideoHeight(0)
			, m_nDecodedWidth(0)
			, m_nDecodedHeight(0)
			, m_nforcebufnum(0)
			, m_nbufcount(0)
			, m_nbufsize(0)
			, m_nqueued(0)
			, m_nbuf2enq(-1)
			, m_fmtcolor(OMX_COLOR_FormatYUV420PackedSemiPlanar)
			, m_nrenderedframes(0)
			, m_npoppedcount(0)
			, m_nrotatedegree(0)
			, m_nbufskipped(-1)
			, m_bcreated(false)
			, m_bupsidedown(false)
			, m_hconv(NULL)
			, m_pfpackuv(NULL)
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

	m_nVideoWidth = (w + 0x07) & ~0x07;
	m_nVideoHeight = h & 0XFFFFFFFE;
	m_fmtcolor = clr;
	VOINFO("real video : w x h = %d x %d, color format: %d", m_nVideoWidth, m_nVideoHeight, clr);
}

void voTIVideoRender::setDimensionInfo(const int w, const int h, const int degree) {

	m_nDecodedWidth = w;
	m_nDecodedHeight = h;
	m_nrotatedegree = degree;
	if (m_nrotatedegree < 0)
		m_nrotatedegree = 0;
	m_nrotatedegree %= 360;
	//m_fmtcolor = OMX_COLOR_FormatYUV420PackedSemiPlanar;
	VOINFO("decoded video: w x h = %d x %d, rotate: %d", m_nDecodedWidth, m_nDecodedHeight, degree);
}

bool voTIVideoRender::CreateOverlay(const int w, const int h) {

	Close();

	m_nbufsize = w * h * 2;
#if 0
	FILE* pf = fopen("/data/local/config", "r+");
	if (pf) {
		char cv;
		cv = fgetc(pf);
		VOINFO("<<<<<<<<<<<<<<<<<<<<<< config: %c >>>>>>>>>>>>>>>>>>>>>>>>", cv);
		if (cv == '0')
			g_nrun = 0;
		else 
			g_nrun = 1;
		VOINFO("---------------------------------------- g_nrun: %d -----------------------------------------", g_nrun);
		fseek(pf, 0, SEEK_SET);
		if (g_nrun == 0) {
			fputc('1', pf);
		} else  {
			fputc('0', pf);
		}

		fflush(pf);
		fclose(pf);
	}
#endif

	VOINFO("calculated buffer size: %d", m_nbufsize);
	if (m_poverlay.get() == NULL) {

		int orientation = 0;
		switch (m_nrotatedegree) {
			case 0: orientation = ISurface::BufferHeap::ROT_0; break;
			case 90: orientation = ISurface::BufferHeap::ROT_90; break;
			case 180: orientation = ISurface::BufferHeap::ROT_180; break;
			case 270: orientation = ISurface::BufferHeap::ROT_270; break;
			default: orientation = ISurface::BufferHeap::ROT_0; break;
		}


		sp<OverlayRef> ref = NULL;
		if (m_pview != NULL) {
			VOLOGI("create overlay: w x h = %d x %d, color = OVERLAY_FORMAT_YCbCr_420_SP, rotate = %d", w, h, m_nrotatedegree);
			if (m_fmtcolor == OMX_COLOR_FormatYUV420PackedSemiPlanar || m_fmtcolor == OMX_COLOR_FormatYUV420Planar)
				ref = ((ISurface*)m_pview)->createOverlay(w, h, OVERLAY_FORMAT_YCbCr_420_SP, orientation, 0);
			else if (m_fmtcolor == 0x7F000001 /*OMX_TI_COLOR_FormatYUV420PackedSemiPlanar_Sequential_TopBottom*/)
				ref = ((ISurface*)m_pview)->createOverlay(w, h, OVERLAY_FORMAT_YCbCr_420_SP_SEQ_TB, orientation, 0);
		}

		if (ref.get() != NULL) {
			VOLOGI("overlay created.");
		} else {
			VOLOGI("Creating overlay failed.");
			return false;
		}

		m_poverlay = new Overlay(ref);
		if (m_poverlay.get() == NULL) return false;

		/* Calculate the number of overlay buffers required, based on the video resolution
		 * and resize the overlay for the new number of buffers
		 */
		int overlaybuffcnt = Calculate_TotalRefFrames(w, h);
		if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar) {
			if (m_nforcebufnum)
				overlaybuffcnt = m_nforcebufnum;
		}

		int initialcnt = m_poverlay->getBufferCount();
		VOINFO("refframes: %d, initialframes: %d", overlaybuffcnt, initialcnt);
		if (overlaybuffcnt != initialcnt) {
			m_poverlay->setParameter(OVERLAY_NUM_BUFFERS, overlaybuffcnt);

			VOINFO("resize: w x h = %d x %d", w, h);
			m_poverlay->resizeInput(w, h);
		}
	} else {
		m_poverlay->setParameter(OVERLAY_NUM_BUFFERS, m_nbufcount);

		VOINFO("resize: w x h = %d x %d", w, h);
		m_poverlay->resizeInput(w, h);
	}

	m_poverlay->setParameter(OPTIMAL_QBUF_CNT, NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE * 2);

	m_nbufcount = m_poverlay->getBufferCount();
	if (m_nbufcount > MAX_V4L2_BUFFER)
		m_nbufcount = MAX_V4L2_BUFFER;

	VOINFO("final buffer count: %d", m_nbufcount);

	mapping_data_t *pmd = NULL;
	for (int i = 0; i < m_nbufcount; i++) {
		pmd = (mapping_data_t*)m_poverlay->getBufferAddress((void*)i);
		m_ppbuf[i] = pmd->ptr;
		if (m_nbufsize < (int)pmd->length)
			m_nbufsize = pmd->length;

		strcpy((char*)m_ppbuf[i], "hello");
		if (strcmp((char*)m_ppbuf[i], "hello")) {
			VOLOGI("problem with buffer");
			return false;
		} else {
			VOINFO("buffer[%d]: allocated addr = %#lx", i, (unsigned long)m_ppbuf[i]);
		}
	}

	VOINFO("actual overlay buffer size: %d", m_nbufsize);
	m_hconv = dlopen("libvoPackUV.so", RTLD_NOW);
	if (m_hconv != NULL) {
		
		if (!m_bupsidedown) {
			VOINFO("normal packuv.....");
			m_pfpackuv = (VOPACKUV)dlsym(m_hconv, "voPackUV");
		} else {
			VOINFO("unnormal packuv.....");
			m_pfpackuv = (VOPACKUV)dlsym(m_hconv, "voPackUV_FLIP_Y");
		}

		if (m_pfpackuv == NULL) return false;

		VOINFO("vomeTIOverlay voPackUV: %p", m_pfpackuv);
	}

	m_bcreated = true;
	m_nbufskipped = -1;
	m_nbuf2enq = -1;
	m_nqueued = 0;
	m_nrenderedframes = 0;
	m_lstqueued.setCapability(m_nbufcount);
	m_lstqueued.clearAll();
	return true;
}

const int voTIVideoRender::RequestBuffer(void** pout, const bool recreated) {

	if (recreated)
		m_bcreated = false;

	if (!m_bcreated) {
		int w = m_nVideoWidth, h = m_nVideoHeight;
		if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar) {
			w = m_nDecodedWidth;
			h = m_nDecodedHeight;
		} 

		if (!CreateOverlay(w, h))
			return false;
	}

	for (int i = 0; i < m_nbufcount; i++) 
		pout[i] = m_ppbuf[i];

	m_nbuf2enq = -1;
	m_nqueued = 0;
	m_nrenderedframes = 0;

	m_lstqueued.clearAll();
	return m_nbufcount;
}

void	voTIVideoRender::FreeBuffer() {

	voOMXMemSet(m_ppbuf, 0, sizeof(void*) * m_nbufcount);
	m_nx = m_ny = -1;
	m_nbufcount = 0;
	m_nqueued = 0;
	m_nbuf2enq = -1;
	m_nbufskipped = -1;
	//m_bcreated = false;
	m_lstqueued.clearAll();
	m_nrenderedframes = 0;
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
		m_pfpackuv = NULL;
	}
		
	FreeBuffer();
}

bool voTIVideoRender::Render(unsigned char* pBuffer, const int size) {

	voCOMXAutoLock lock (&m_mutlock);
	OMX_U8* pbuf = pBuffer;
	int ret = 0, current = 0, nx = 0, ny = 0;

	m_nbufskipped = -1;
	if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar) {

		/*for HW OMX_COLOR_FormatYUV420PackedSemiPlanar, OMX_TI_COLOR_FormatYUV420PackedSemiPlanar_Sequential_TopBottom*/
		int i = 0;
		for (; i < m_nbufcount; i++) {
			int offset = (char*)pBuffer - (char*)m_ppbuf[i];
			if (offset < size) {
				ny = (offset/ARMPAGESIZE);
				nx = (offset%ARMPAGESIZE);
				if (m_nx != nx || ny != m_ny) {
					VOLOGI("x = %d y = %d, video: w x h = %d x %d, decoded: w x h = %d x %d", nx, ny, m_nVideoWidth, m_nVideoHeight, m_nDecodedWidth, m_nDecodedHeight);
					m_nx = nx;
					m_ny = ny;
					m_poverlay->setCrop(nx, ny, m_nVideoWidth, m_nVideoHeight);
				}
				break;
			}
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

	} else /*if (m_fmtcolor == OMX_COLOR_FormatYUV420Planar) */{
		int nstride = 4096;
		m_nbuf2enq = (++m_nbuf2enq) % m_nbufcount;
		current = m_nbuf2enq;
		OMX_U8* pd = (OMX_U8*)m_ppbuf[current];
		VO_VIDEO_BUFFER *ps = (VO_VIDEO_BUFFER*)pbuf; 
		
		if (!m_bupsidedown) {
			for (int i = 0; i < m_nVideoHeight; i++)
				memcpy(pd + nstride * i, ps->Buffer[0] + ps->Stride[0] * i, m_nVideoWidth);
		} else {
			for (int i = m_nVideoHeight - 1; i > -1; i--)
				memcpy(pd + nstride * (m_nVideoHeight - i - 1), ps->Buffer[0] + ps->Stride[0] * i, m_nVideoWidth);
		}

		unsigned char * pUV = pd + nstride * m_nVideoHeight;
		//VOINFO("STRIDE[0]: %d, STRIDE[1]: %d, STRIDE[2]: %d", ps->Stride[0], ps->Stride[1], ps->Stride[2]);
		if (m_pfpackuv != NULL)
			m_pfpackuv(pUV, (unsigned char*)ps->Buffer[1], (unsigned char*)ps->Buffer[2], ps->Stride[1], ps->Stride[2], m_nVideoHeight / 2, m_nVideoWidth/ 2, nstride);
	}
	
	//VOINFO("overlay: w x h = %d x %d, wstride x hstride = %d x %d", m_poverlay->getWidth(), m_poverlay->getHeight(), m_poverlay->getWidthStride(), m_poverlay->getHeightStride());
	if (m_nbufskipped < 0) {
	VOINFO("------------ input buffer[%d]: %p ------------, queued: %d", current, m_ppbuf[current], m_nqueued);
		ret = m_poverlay->queueBuffer((void*)current);
		if (ret > -1) {
			m_nqueued++;
			//VOINFO("current: %d, queued: %d, real queued: %d, sizelist: %d", current, m_nqueued, ret, m_lstqueued.GetCount());
			m_nrenderedframes++;
			if (ret != m_nqueued) {
				int npopped = m_lstqueued.getCount();
				m_nqueued -= npopped;
				VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<< switched! >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
				m_npoppedcount += npopped;
			}
			m_lstqueued.enqueue(current);
		} 
	}
	
	overlay_buffer_t ob; int next = -1;
	ret = m_poverlay->dequeueBuffer(&ob);
	if (ret == NO_ERROR) {
		next = (int)ob;
		m_nqueued--;
#if defined __VODBG__
		int nbuf = -1;
		int npos = m_lstqueued.getHeadPosition();
		while (npos > -1) {
			m_lstqueued.get(npos, nbuf);
			VOINFO("queued: %d", nbuf);
		}
		VOINFO("$$$$$$$$$$$$$$ dequeue: %d $$$$$$$$$$$$$$$$", next);
#endif
		m_lstqueued.dequeue();
	} else { 
		next = -1;
		VOINFO("popped count: %d", m_npoppedcount);
		if (m_npoppedcount > 0) {
			next = -3;
			m_npoppedcount--;
			if (m_fmtcolor == OMX_COLOR_FormatYUV420Planar)
				m_lstqueued.dequeue();
		}

		if (ret == -EPERM)
			VOLOGI("At least %d buffers before dequeue. cur queued: %d, poped: %d", NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE * 2, m_nqueued, (int)ob);
	}

	if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar)
		m_nbuf2enq = next;

	return true;
}

const int voTIVideoRender::InquireIdleBuffer() {

	voCOMXAutoLock lock (&m_mutlock);

	int ib = -2;
	if (m_fmtcolor != OMX_COLOR_FormatYUV420Planar)
		ib = m_nbuf2enq;

	//VOINFO("return buffer: %d", ib);
	return ib;
}

void voTIVideoRender::Flush() { 
	m_nbuf2enq = -1; 
	m_nbufskipped = -1;
}

void voTIVideoRender::setForceBufNum(const int num) {
	m_nforcebufnum = num;
	if (m_nforcebufnum > MAX_V4L2_BUFFER)
		m_nforcebufnum = MAX_V4L2_BUFFER;
}

const int voTIVideoRender::getQueuedBuffer() {

#if defined __VODBG__
	int nval = -1;
	int p = m_lstqueued.getHeadPosition();
	while (p > -1) {
		m_lstqueued.get(p, nval);
		VOINFO("+++++++++++++++++++++++++++ queued: %d ++++++++++++++++++++++++++++", nval);
	}
#endif

	return m_nqueued;
}

const int voTIVideoRender::getPoppedBuffer() { 

	int nb = -1;
	m_lstqueued.dequeue(&nb);
	
	return nb; 
}

static int Calculate_TotalRefFrames(int nWidth, int nHeight) {

    uint32_t ref_frames = 0;
    uint32_t MaxDpbMbs;
    uint32_t PicWidthInMbs;
    uint32_t FrameHeightInMbs;

    MaxDpbMbs = 32768; //Maximum value for upto level 4.1
    PicWidthInMbs = nWidth / 16;
    FrameHeightInMbs = nHeight / 16;

    ref_frames =  (uint32_t)(MaxDpbMbs / (PicWidthInMbs * FrameHeightInMbs));

    VOINFO("nWidth [%d] PicWidthInMbs [%d] nHeight [%d] FrameHeightInMbs [%d] ref_frames [%d]", nWidth, PicWidthInMbs, nHeight, FrameHeightInMbs, ref_frames);

    ref_frames = (ref_frames > 16) ? 16 : ref_frames;
    VOINFO("Final ref_frames [%d]", ref_frames);
    return (ref_frames + 3 + 2*NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE);
}

