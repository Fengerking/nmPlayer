
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <errno.h>
#include "voV4L2Render.h"
//#include "omap24xxvout.h"
//
#define VOINFO(format, args...) do { \
		const char* szfilename = strrchr(__FILE__, '/') + 1; \
	  printf("%s::%s()->%d: " format "\n", szfilename, __FUNCTION__, __LINE__, ## args); } while(0);

voV4L2Render::voV4L2Render(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_nfdv4l2(-1)
	, m_x(0)
	, m_y(0)
	, m_nbufcount(0)
	, m_nbufsize(0)
	, m_nqueued(0)
	, m_nbufenq(0)
	, m_hconv(NULL)
	, m_pfconv420to422(NULL)
{
	//m_nbufsize = m_nScreenWidth * m_nScreenHeight * 2;
}

voV4L2Render::~voV4L2Render() {
	v4l2close();
}

static const char* v4lOptions[] = {
		"/dev/video1",
		"/dev/v4l/video1",
		"/dev/video0",
		"/dev/video2",
	};

int voV4L2Render::v4l2open() {

	/* Set the output to lcd for video1. output can also be set to tv 
	 * by changing string in the second line to tv */
	/*if (system("echo lcd > /sys/class/display_control/omap_disp_control/video1")) {
		printf("Cannot set output to lcd...");
		exit(0);
	}*/

	/* open display channel */
	int ndev = sizeof(v4lOptions)/sizeof(char*);
	for (int i=0; i<ndev; ++i) {
		m_nfdv4l2 = ::open((const char*)v4lOptions[i], O_RDWR);
		if (m_nfdv4l2 > 0) 
			break;
		else if (m_nfdv4l2 < 0)
			printf("failed to open %s due to: %s...\n", v4lOptions[i], strerror(errno)); 
	}

	if (m_nfdv4l2 < 0) {
		VOINFO("failed to open V4L devices...");
		return -1;
	}

	int ret = 0;
#if 0
	int val = 90;
	ret = ioctl(m_nfdv4l2, VIDIOC_S_OMAP2_ROTATION, &val);
	if (ret < 0) {
		VOINFO("Rotation cannot be done");
		::close(m_nfdv4l2);
		return -1;
	}
#endif

	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	fmt.fmt.pix.width = m_nYUVWidth;
	fmt.fmt.pix.height = m_nYUVHeight;
#if VOV4L2_VIDFMT == 1
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
#elif VOV4L2_VIDFMT == 0 
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
#endif
	ret = ioctl(m_nfdv4l2, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		VOINFO("Set Format failed");
		::close(m_nfdv4l2);
		return -1;
	}

	if (m_pfconv420to422 == NULL) {
		m_hconv = dlopen ("libvoConv420To422.so", RTLD_NOW);
		if (m_hconv != NULL) {
			m_pfconv420to422 = (VOCONV420TO422API)dlsym(m_hconv, "YUV420_UYVY422");
			if (m_pfconv420to422 == NULL) return false;

			VOINFO("vomeTIOverlay voConv420To422: %p", m_pfconv420to422);
		}
	}

	return 0;
}

int voV4L2Render::v4l2close() {

	StreamOff();

	for (int i=0; i<m_nbufcount; i++)
		munmap(m_ppbuf[i].buf, m_nbufsize);

	::close(m_nfdv4l2);
	m_nfdv4l2 = 0;

	return 0;
}

bool voV4L2Render::FullScreen(const bool full) {
	voCAutoLock lock(&m_csDraw);

	if (full)
		m_bFullScreen = VO_TRUE;
	else
		m_bFullScreen = VO_FALSE;

	UpdateSize();
	
	StreamOff(); 

	if (!RequestBuffer()) {
		VOINFO("Request buffer failed");
		::close(m_nfdv4l2);
		return false;
	}

	int ret = 0;
	//if (!m_bFullScreen) 
	{ 
		struct v4l2_crop crop;
		crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		crop.c.left = 0;
		crop.c.top = 0;
		crop.c.width = m_nYUVWidth;
		crop.c.height = m_nYUVHeight;
		ret = ioctl(m_nfdv4l2, VIDIOC_S_CROP, &crop);
		if (ret < 0) {
			VOINFO("Set Crop failed");
			::close(m_nfdv4l2);
			return false;
		}
	}

	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	if (m_bFullScreen) { 
		fmt.fmt.win.w.left = 0;
		fmt.fmt.win.w.top = 0;
		fmt.fmt.win.w.width = 720;
		fmt.fmt.win.w.height = 480;
	} else {
		fmt.fmt.win.w.left = m_x;
		fmt.fmt.win.w.top = m_y;
		fmt.fmt.win.w.width = m_nDrawWidth;
		fmt.fmt.win.w.height = m_nDrawHeight;
	}

	ret = ioctl(m_nfdv4l2, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		VOINFO("Set Format failed!");
		::close(m_nfdv4l2);
		return false;
	}

	if (!QueueBuffer()) {
		VOINFO("queue buffer failed...");
		return false;
	}

	if (!StreamOn()) 
		return false;

	return true;
}


VO_U32 voV4L2Render::Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait) {
	voCAutoLock lock (&m_csDraw);

	if (m_nfdv4l2 == 0) {
		if (v4l2open() == -1)
			return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	}

	int ret = 0;
	unsigned char* pd = (unsigned char*)m_ppbuf[m_nbufenq].buf;

	/* Process it
		 In this example, the "processing" is putting a horizontally 
		 moving color bars with changing starting line of display.
		 */

#if VOV4L2_VIDFMT == 1	
	if (!ConvertSample(pSample, pd, 640))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
#elif VOV4L2_VIDFMT == 0	
	unsigned char* pbuf = (unsigned char*) pVideoBuffer;	
	VO_VIDEO_BUFFER *ps = pVideoBuffer; 
	ps->Buffer[0] = pbuf;
	ps->Buffer[1] = pbuf + m_nVideoWidth * m_nVideoHeight;
	ps->Buffer[2] = pbuf + m_nVideoWidth * m_nVideoHeight * 5 / 4;
	ps->Stride[0] = m_nVideoWidth;
	ps->Stride[2] = ps->Stride[1] = m_nVideoWidth / 2;
#if 0
	YUV420_YUV422_c(displaybuffer,pd->data_buf[0],pd->data_buf[1],pd->data_buf[2], pd->stride[0], pd->stride[1], m_nYUVWidth, m_nYUVHeight); 
#else
	m_pfconv420to422(pd,ps->Buffer[0],ps->Buffer[1],ps->Buffer[2],ps->Stride[0],ps->Stride[1], m_nVideoWidth, m_nVideoHeight, m_nVideoWidth);
#endif
#endif

	/* Now queue it back to display it */
	m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
#if VOV4L2_MEMTYPE == 0 
	m_v4l2buf.memory = V4L2_MEMORY_MMAP;
#elif VOV4L2_MEMTYPE == 1 
	m_v4l2buf.memory = V4L2_MEMORY_USERPTR;
#endif
	int current = m_nbufenq;
	m_v4l2buf.m.userptr = (unsigned long)pd;
	m_v4l2buf.length = m_nbufsize;
	ret = ioctl(m_nfdv4l2, VIDIOC_QBUF, &m_v4l2buf);
	if (ret < 0) {
		VOINFO("VIDIOC_QBUF Error...");
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	}

	m_ppbuf[current].flag = 1;
	m_nqueued++;

	if (m_nqueued > NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE) {
		ioctl(m_nfdv4l2, VIDIOC_DQBUF, &m_v4l2buf);
		if (ret < 0) {
			VOINFO("VIDIOC_DQBUF error...");
			return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
		}
		m_ppbuf[m_v4l2buf.index].flag = 0;
		m_nqueued--;
	}

	do {
		m_nbufenq = (++m_nbufenq) % m_nbufcount;
	} while(m_ppbuf[m_nbufenq].flag);

	return VO_ERR_NONE;
}

VO_U32 voV4L2Render::UpdateSize(void) {

	CBaseVideoRender::UpdateSize();

	/*voCAutoLock lock(&m_csDraw);

	int ww = m_rcWnd.right - m_rcWnd.left;
	int wh = m_rcWnd.bottom - m_rcWnd.top;

	if (m_nDrawWidth >= m_nYUVWidth) {
		m_nDrawWidth = m_nYUVWidth;
		m_nDrawHeight = m_nYUVHeight;
	} else {
		m_nDrawWidth = m_nYUVWidth / 2;
		m_nDrawHeight = m_nYUVHeight / 2;
	}
	m_x = m_rcWnd.left + (ww - m_nDrawWidth) / 2;
	m_y = m_rcWnd.top + (wh - m_nDrawHeight) / 2;
*/
	return 0;
}

bool voV4L2Render::StreamOn() {

	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int ret = ioctl(m_nfdv4l2, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		VOINFO("VIDIOC_STREAMON error!");
		return false;
	}

	return true;
}

bool voV4L2Render::StreamOff() {

	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ioctl(m_nfdv4l2, VIDIOC_STREAMOFF, &a);

	for (int i=0; i<m_nbufcount; i++)
		munmap(m_ppbuf[i].buf, m_nbufsize);

	return  true;
}

bool voV4L2Render::RequestBuffer() {

	struct v4l2_requestbuffers reqbuf;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.count = MAX_V4L2BUF_NUM;
#if VOV4L2_MEMTYPE == 0
	reqbuf.memory = V4L2_MEMORY_MMAP;
#elif  VOV4L2_MEMTYPE == 1
	reqbuf.memory = V4L2_MEMORY_USERPTR;
#endif

	int ret = ioctl(m_nfdv4l2, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		VOINFO("Could not allocate the buffers");
		return false;
	}
	m_nbufcount = reqbuf.count;
	if (m_nbufcount > MAX_V4L2BUF_NUM)
		m_nbufcount = MAX_V4L2BUF_NUM;
	
#if VOV4L2_MEMTYPE == 0
	for (int i=0; i<m_nbufcount; i++) { 
		m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		m_v4l2buf.memory = V4L2_MEMORY_MMAP;
		m_v4l2buf.index = i;
		if (ioctl(m_nfdv4l2, VIDIOC_QUERYBUF, &m_v4l2buf) < 0) {
			VOINFO("VIDIOC_QUERYBUF error");
			return false;
		}

		m_nbufsize = m_v4l2buf.length;
		m_ppbuf[i].buf = mmap(NULL, m_nbufsize, PROT_READ|PROT_WRITE, MAP_SHARED, m_nfdv4l2, (off_t)m_v4l2buf.m.offset);	
		if ((int)m_ppbuf[i].buf == -1) {
			VOINFO("mmap buf[%d] error.", i);
			return false;
		}

		memset((void*)m_ppbuf[i].buf, 0x80, m_nbufsize);
	}
#endif

	return true;
}

bool voV4L2Render::QueueBuffer() {

	/* Now queue it back to display it */
	int ret = 0;
	memset(&m_v4l2buf, 0, sizeof(v4l2_buffer));
	for (int i=0; i<m_nbufcount; ++i) {
		m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
#if VOV4L2_MEMTYPE == 0 
		m_v4l2buf.memory = V4L2_MEMORY_MMAP;
#elif VOV4L2_MEMTYPE == 1 
		m_v4l2buf.memory = V4L2_MEMORY_USERPTR;
#endif
		m_v4l2buf.index = i;
		m_v4l2buf.m.userptr = (unsigned long)m_ppbuf[i].buf;
		m_v4l2buf.length = m_nbufsize;
		ret = ioctl(m_nfdv4l2, VIDIOC_QBUF, &m_v4l2buf);
		if (ret < 0) {
			VOINFO("VIDIOC_QBUF Error...");
			return false;
		}
		m_nqueued++;
		m_ppbuf[i].flag = 1;
	}

	return true;
}

static void YUV420_YUV422_c(unsigned char *  Dst, unsigned char *  Y, unsigned char *  U, unsigned char *  V, int YStride, int UVStride, int w, int h) {
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
