/*******************************************************************************
	Written by: david	

	Change History (most recent first):
	2009-05-10		david Create file

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "cmnMemory.h"
#include "voIVCommon.h"
#include "CFBVideoRender.h"

#define VOINFO(format, args...) do { \
		const char* szfilename = strrchr(__FILE__, '/') + 1; \
	  printf("%s::%s()->%d: " format "\n", szfilename, __FUNCTION__, __LINE__, ## args); } while(0);

static const char* g_szfboptions[] = {
	"/dev/fb0",
	"/dev/fb1",
	"/dev/graphics/fb0",
	"/dev/graphics/fb1",
};

CFBVideoRender::CFBVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_fb(-1) 
	, m_nimgbuflen(0)
	, m_nbpp(2)
	, m_nstride(0)
	, m_loffset(0)
	, m_pimgbuf(NULL)
	, m_pfbbuf(NULL)
{
	if(hView)
		pMemOP->Copy(VO_INDEX_SNK_VIDEO, &m_rcDisplay, (VO_RECT*)hView, sizeof(VO_RECT));
	/*else
	{
		m_rcDisplay.left = m_rcDisplay.top = 0;
		m_rcDisplay.right = 480;
		m_rcDisplay.bottom= 272;
	}*/
	fbopen();
}

CFBVideoRender::~CFBVideoRender() {
	fbclose();
}

VO_U32 CFBVideoRender::Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait) {

	voCAutoLock lock (&m_csDraw);

	unsigned char* pbuf = NULL;
#if defined __VOTT_ARM__
	pbuf = (unsigned char*)m_pfbbuf + m_loffset;
#elif defined __VOTT_PC__ 
	pbuf = m_pimgbuf;
#endif

	VO_VIDEO_BUFFER out;
	out.Buffer[0] = pbuf;
	out.Stride[0] = m_nstride;

	VOINFO("stride: %d", m_nstride);
	if (!ConvertData (pVideoBuffer, &out, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

#if defined __VOTT_PC__
		render32();
#endif

	return VO_ERR_NONE;
}


VO_U32 CFBVideoRender::UpdateSize (void) {

	CBaseVideoRender::UpdateSize();
	voCAutoLock lock (&m_csDraw);
	VO_U32 w, h;
	if (!m_bRotate) {
		w = m_nDrawWidth;
		h = m_nDrawHeight;
	} else {
		w = m_nDrawHeight;
		h = m_nDrawWidth;
	}

#if defined __VOTT_PC__
	m_nimgbuflen = w * h * 4;
	if (m_pimgbuf != NULL)
		delete []m_pimgbuf;
	m_pimgbuf = new unsigned char[m_nimgbuflen];
#endif

	if (m_nDrawTop == 0)
		m_loffset =  m_nDrawLeft * m_nbpp;
	else
		m_loffset = (m_vi.xres * (m_nDrawTop - 1) + m_nDrawLeft) * m_nbpp;

	VOINFO("offset: %d", m_loffset);
	VOINFO("video: w: %d, h: %d, YUV w: %d, h: %d, show w: %d, h: %d", 
					m_nVideoWidth,
					m_nVideoHeight,
					m_nYUVWidth,
					m_nYUVHeight,
					m_nShowWidth,
					m_nShowHeight);
	VOINFO("target w: %d, h: %d, rotate: %d", w, h, m_bRotate);
	return 0;
}

int CFBVideoRender::fbopen() {

	int ndev = sizeof(g_szfboptions)/sizeof(char*);
	for (int i=0; i<ndev; ++i) {
		m_fb = open((const char*)g_szfboptions[i], O_RDWR);
		if (m_fb > 0)
			break;
		else if (m_fb < 0)
			printf("failed to open %s...\n", g_szfboptions[i]); 
	}

	if (-1 == ioctl(m_fb, FBIOGET_VSCREENINFO, &m_vi)){
		//VOINFO("ioctl FBIOGET_VSCREENINFO error");
		return -1;
	}

	VOINFO("x: %d, y: %d, d: %d",m_vi.xres,m_vi.yres,m_vi.bits_per_pixel);

	if (-1 == ioctl(m_fb, FBIOGET_FSCREENINFO, &m_fi)) {
		//VOINFO("ioctl FBIOGET_FSCREENINFO error");
		return false;
	}

#if defined __VOTT_PC__

	m_nbpp = 4;

#elif defined __VOTT_ARM__

#if defined __VOPRJ_INST__
	m_nbpp = 4;
#else
	m_nbpp = m_vi.bits_per_pixel / 8;
#endif

#endif

	m_nstride = m_vi.xres * m_nbpp;
	VOINFO("xres: %d, yres: %d, phy stride: %d", m_vi.xres, m_vi.yres, m_nstride);
	long lbuflen = m_nstride * m_vi.yres;
	m_pfbbuf = mmap(0, lbuflen, PROT_READ|PROT_WRITE, MAP_SHARED, m_fb, 0);
	if (-1L == (long) m_pfbbuf) {
		VOINFO("mmap error! mem:%d offset:%d\n", m_pfbbuf, lbuflen);
		return -1;
	}

	VOINFO("open successfully!");
	return 0;
}

int CFBVideoRender::fbclose() {
	voCAutoLock lock (&m_csDraw);
	
#if defined __VOTT_PC__
	if (NULL != m_pimgbuf) {
		delete []m_pimgbuf;
		m_pimgbuf = NULL;
	}
#endif

	long lbuflen = m_nstride * m_vi.yres;
	if (munmap(m_pfbbuf, lbuflen) == -1)
		printf("munmap error\n");

	close(m_fb);
	m_fb = -1;

	return 0;
}

void CFBVideoRender::render32() {

	unsigned char* pimg = m_pimgbuf;
	unsigned char* pbuf = (unsigned char*)m_pfbbuf + m_loffset;

  // rendering to framebuffer
	unsigned short w = 0; 
	unsigned long dwVal = 0; 
	unsigned char* p = (unsigned char*)&dwVal;
	for (int i=0; i<m_nShowHeight; ++i) {
		for (int j=0, k=0; j<m_nstride;) {
			w = *((unsigned short*)(pimg+j));
			p[0]=((unsigned char)(w&0x001f))<<3;		//B
			p[1]=((unsigned char)((w>>5)&0x3f))<<2;	//G
			p[2]=((unsigned char)((w>>11)&0x001f))<<3;	//R
			//p[3]=0;
			*((unsigned long*)(pbuf+k)) = dwVal;
			j += 2;
			k += 4;
		}
		pimg += m_nstride;
		pbuf += m_nstride;
	}
}

