	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXZM2VideoSinkPort.cpp

	Contains:	voCOMXZM2VideoSinkPort class file

	Written by:	David

	Change History (most recent first):
	2009-10-15	created by david

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <hardware/overlay.h>
#include "vomeZM2Overlay.h"

#define LOG_TAG "vomeZM2Overlay"

#define VOINFO(format, ...) { \
  LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }

#define CACHEABLE_BUFFERS 0x1

static void YUV420_YUV422_c(unsigned char* Dst,unsigned char* Y,unsigned char* U,unsigned char* V,int YStride,int UVStride,int w,int h);

vomeZM2Overlay::vomeZM2Overlay(void* pview)
			: m_poverlay(NULL)
			, m_nbufcount(0)
			, m_nbufindex(-1)
			, m_hconv(NULL)
{}

vomeZM2Overlay::~vomeZM2Overlay(void) {
	Release();
}

int vomeZM2Overlay::SetVideoSize (int nWidth, int nHeight)
{
   // LOGW("@@@@@@  vomeRGB565VideoRender SetVideoSize   W %d H %d \n", nWidth, nHeight);

	m_nVideoWidth = (nWidth + 3) & ~0x03;
	m_nVideoHeight = nHeight & 0XFFFFFFFE;

	return 0;
}

bool vomeZM2Overlay::Create() {

	Release();

	int w = m_nVideoWidth;
	int h = m_nVideoHeight;

	if (m_poverlay == NULL) {

		sp<OverlayRef> ref = NULL;
		if (m_pSurface != NULL) 
			ref = m_pSurface->createOverlay(w, h, OVERLAY_FORMAT_YCbYCr_422_I);

		if (ref != NULL) {
			VOINFO("%s", "overlay created.");
		} else {
			VOINFO("%s", "Creating overlay failed.");
			return false;
		}

		m_poverlay = new Overlay(ref);
		if (m_poverlay == NULL) return false;
	}

	m_poverlay->setCrop(0, 0, w, h);
	m_poverlay->setParameter(CACHEABLE_BUFFERS, 1);
	m_nbufcount = m_poverlay->getBufferCount();
	VOINFO("buffer count: %d", m_nbufcount);
	m_hconv = dlopen ("libvoConv420To422.so", RTLD_NOW);
	if (m_hconv != NULL) {
		m_pfconv420to422 = (VOCONV420TO422API)dlsym(m_hconv, "YUV420_YUYV422");
		if (m_pfconv420to422 == NULL) return false;

		VOINFO("vomeZM2Overlay voConv420To422: %p", m_pfconv420to422);
	}

	return true;
}

void	vomeZM2Overlay::Release() {

	if (m_poverlay != NULL) {
		m_poverlay->destroy();
		m_poverlay = NULL;
	}

	if (m_hconv) {
		dlclose(m_hconv);
		m_hconv = NULL;
	}

	m_nbufcount = 0;
}


bool vomeZM2Overlay::CheckColorType (VO_ANDROID_COLORTYPE nColorType) {
	if (nColorType != VO_ANDROID_COLOR_YUV420)
		return false;

	return true;
}


bool vomeZM2Overlay::Render(VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer) {

 	if (pVideoBuffer->nColorType != VO_ANDROID_COLOR_YUV420)
		return false;

	if (m_poverlay == NULL) {
		if (!Create()) {
			VOINFO("%s", "create overlay failed!!!!");
			return -1;
		}
	}

	m_nbufindex = (++m_nbufindex) % m_nbufcount;
	int current = m_nbufindex;
	mapping_data_t* pm = (mapping_data_t*)m_poverlay->getBufferAddress((void*)current);

	unsigned char* pd = (unsigned char*)pm->ptr;
	VO_ANDROID_VIDEO_BUFFERTYPE* ps = pVideoBuffer;

	//VOINFO("stride0: %d, stride1: %d, video w: %d, h: %d, w:%d", ps->nStride[0],ps->nStride[1], m_nVideoWidth, m_nVideoHeight,m_nVideoWidth);
	m_pfconv420to422(pd,ps->virBuffer[0],ps->virBuffer[1],ps->virBuffer[2],ps->nStride[0],ps->nStride[1], m_nVideoWidth, m_nVideoHeight, m_nVideoWidth); 
	//YUV420_YUV422_c(pd,ps->virBuffer[0],ps->virBuffer[1], ps->virBuffer[2], ps->nStride[0],ps->nStride[1], m_nVideoWidth, m_nVideoHeight); 

	m_poverlay->queueBuffer((void*)current);

	overlay_buffer_t overlay_buffer;
	if (m_poverlay->dequeueBuffer(&overlay_buffer) != 0) {
		VOINFO("%s", "Video (34xx)MIO dequeue buffer failed.");
		return false;
	}

	return true;
}

static void YUV420_YUV422_c(unsigned char* Dst,unsigned char* Y,unsigned char* U,unsigned char* V,int YStride, int UVStride, int w, int h) {
	unsigned char *Des, *Des1, *Ybuf, *Ybuf1, *Ubuf, *Vbuf;
	int doublewidth = w * 2;
	int stride1 = (YStride * 2 - w);
	int stride2 = (UVStride - w/ 2);
	Ybuf = Y;
	Ybuf1 = Y + YStride;
	Ubuf = U;
	Vbuf = V;
	Des = Dst;
	Des1 = Dst + doublewidth;

	for (int i=0; i<h; i+=2) {
		for (int j = 0; j < doublewidth; j+=4) {
			*Des++ = *Ybuf++;
			*Des++ = *Ubuf;
			*Des++ = *Ybuf++;
			*Des++ = *Vbuf;

			*Des1++ = *Ybuf1++;
			*Des1++ = *Ubuf++;
			*Des1++ = *Ybuf1++;
			*Des1++ = *Vbuf++;
		}

		Des += doublewidth;
		Des1 += doublewidth;
		Ybuf += stride1;
		Ybuf1 += stride1;
		Ubuf += stride2;
		Vbuf += stride2;
	}
}

