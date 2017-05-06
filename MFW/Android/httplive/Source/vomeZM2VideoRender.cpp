
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <dlfcn.h>
#include "omap24xxvout.h"

#include "volog.h"
#include "vomeZM2VideoRender.h"

#define VOINFO(format, ...) { \
  LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }

namespace android {

static void YUV420_YUV422_c(unsigned char* Dst, unsigned char* Y, unsigned char* U, unsigned char* V, int YStride, int UVStride, int w, int h);

vomeZM2VideoRender::vomeZM2VideoRender(void)
			: vomeRGB565VideoRender()
			, m_fd(-1)
			, m_x(0)
			, m_y(0)
			, m_nScreenWidth(800)
			, m_nScreenHeight(480)
			, m_nbufcount(0)
			, m_nbufsize(0)
			, m_pfconv420to422(NULL)
{}

vomeZM2VideoRender::~vomeZM2VideoRender() {
	v4lclose();
}

static char* v4lOptions[] = {
		"/dev/video1",
		"/dev/v4l/video1",
		"/dev/video0",
		"/dev/video2",
};

int vomeZM2VideoRender::v4lopen() {

	VOLOG("\n\n%s\n", "------------------------gonna render data to v4l2------------------------------"); 
	/* Set the output to lcd for video1. output can also be set to tv 
	 * by changing string in the second line to tv */
	/*if (system("echo lcd > /sys/class/display_control/omap_disp_control/video1")) {
		printf("Cannot set output to lcd...");
		exit(0);
	}*/

	/* open display channel */
	
	int ndev = sizeof(v4lOptions)/sizeof(char*);
	for (int i=0; i<ndev; ++i) {
		m_fd = ::open((const char*)v4lOptions[i], O_RDWR);
		if (m_fd > 0) {
			VOLOG("sucessfully open %s\n", v4lOptions[i]);
			break;
		} else if (m_fd < 0) {
			VOLOG("failed to open %s...\n", v4lOptions[i]); 
		}
	}

	if (m_fd < 0) {
		VOINFO("%s", "failed to open V4L devices...");
		return -1;
	}

	int ret = 0;
	struct v4l2_capability caps;
	
	ret = ioctl(m_fd, VIDIOC_QUERYCAP, &caps);
	if (ret < 0) {
		VOLOG("%s\n", "query capability failed.");
		return -1;
	}

	if (!(caps.capabilities & V4L2_CAP_STREAMING)) {
		VOLOG("%s\n", "No streaming capability in display driver!");
		return -1;
	}

	if (!(caps.capabilities & V4L2_CAP_VIDEO_OVERLAY)) {
		VOLOG("%s\n", "No v4l2_overlay_capability in display driver!");
		//return;
	}
	if (!(caps.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
		VOLOG("%s\n", "No output capability in display driver!");
		//return;
	}

	VOLOG("%s\n", "v4l2 opened successfully...");
	return 0;
}

int vomeZM2VideoRender::v4lclose() {

	StreamOff();

	::close(m_fd);
	m_fd = 0;

	return 0;
}

bool vomeZM2VideoRender::Render(VO_ANDROID_VIDEO_BUFFERTYPE* pVideoBuffer) {

	if (m_fd < 0) {
		UpdateSize();
		
		void* handle = dlopen ("voConv420To422.so", RTLD_NOW);
		if (handle != NULL) {
			m_pfconv420to422 = (VOCONV420TO422API)dlsym(handle, "voConv420To422");
			LOGW("@@@@@@  vomeZM2VideoRender voConv420To422: %x\n", m_pfconv420to422);
		}
	}

	int ret = ioctl(m_fd, VIDIOC_DQBUF, &m_v4l2buf);
	if (ret < 0) {
		VOINFO("%s", "VIDIOC_DQBUF error...");
		return false;// VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	}

	unsigned char* displaybuffer = (unsigned char*)m_bufaddr[m_v4l2buf.index];

#if V4L2_PIXFMT_TYPE == 1	

	VO_VIDEO_BUFFER vb;
	vb.Buffer[0] = displaybuffer;
	vb.Stride[0] = m_nDrawWidth * 2;

	if (!ConvertData (pVideoBuffer, &vb, nStart, bWait))
		return false;// VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

#elif V4L2_PIXFMT_TYPE == 0	

	VO_ANDROID_VIDEO_BUFFERTYPE* pd = pVideoBuffer;

#if 0
	YUV420_YUV422_c(displaybuffer, pd->virBuffer[0], pd->virBuffer[1],pd->virBuffer[2], pd->nStride[0], pd->nStride[1], m_nVideoWidth, m_nVideoHeight); 
#else
	m_pfconv420to422(displaybuffer, pd->virBuffer[0], pd->virBuffer[1], pd->virBuffer[2], pd->nStride[0], pd->nStride[1], m_nVideoWidth, m_nVideoHeight); 
#endif

#endif

	/* Now queue it back to display it */
	m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
#if V4L2_MMAP_TYPE == 0 
	m_v4l2buf.memory = V4L2_MEMORY_MMAP;
#elif V4L2_MMAP_TYPE == 1 
	m_v4l2buf.memory = V4L2_MEMORY_USERPTR;
#endif
	m_v4l2buf.m.userptr = m_bufaddr[m_v4l2buf.index];
	m_v4l2buf.length = m_nbufsize;
	ret = ioctl(m_fd, VIDIOC_QBUF, &m_v4l2buf);
	if (ret < 0) {
		VOINFO("%s", "VIDIOC_QBUF Error...");
		return false;// VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	}

	return true;
}

bool vomeZM2VideoRender::CheckColorType(VO_ANDROID_COLORTYPE nColorType) {
	if (nColorType != VO_ANDROID_COLOR_YUV420)
		return false;

	return true;
}

int vomeZM2VideoRender::UpdateSize(void) {

	m_x = 0;
	m_y = 0; 

	int ret = 0;

	if (m_fd < 0) {
		ret =	v4lopen();
		if (ret < 0)
			return 0;
	}

	StreamOff(); 

	if (!setformat())
		return 0;

	if (!updateview())
		return 0;

	if (!RequestBuffer()) {
		return 0;
	}

	if (!StreamOn()) 
		return 0;

	if (!QueueBuffer()) {
		VOLOG("%s\n", "queue buffer failed...");
		return 0;
	}

	return 0;
}

bool vomeZM2VideoRender::setformat() {
	
	m_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	m_fmt.fmt.pix.width = m_nVideoWidth;
	m_fmt.fmt.pix.height = m_nVideoHeight;
	VOLOG("video: width %d, height %d \n", m_nVideoWidth, m_nVideoHeight);
#if V4L2_PIXFMT_TYPE == 1
	m_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
#elif V4L2_PIXFMT_TYPE == 0 
	m_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
#endif
	m_fmt.fmt.pix.sizeimage = m_fmt.fmt.pix.width * m_fmt.fmt.pix.height * 2;
	m_fmt.fmt.pix.bytesperline = m_fmt.fmt.pix.width * 2;

	int ret = ioctl(m_fd, VIDIOC_TRY_FMT, &m_fmt);
	if (ret < 0) {
#if V4L2_PIXFMT_TYPE == 1
		VOLOG("set format - %s failed\n", "V4L2_PIX_FMT_RGB565");
#elif V4L2_PIXFMT_TYPE == 0 
		VOLOG("set format - %s failed\n", "V4L2_PIX_FMT_UYUV");
#endif
		::close(m_fd);
		return false;
	}

#if 0
	int val = 90;
	ret = ioctl(m_fd, VIDIOC_S_OMAP2_ROTATION, &val);
	if (ret < 0) {
		VOLOG("%s\n", "Rotation cannot be done");
		::close(m_fd);
		return -1;
	}
#endif

	ret = ioctl(m_fd, VIDIOC_S_FMT, &m_fmt);
	if (ret < 0) {
#if V4L2_PIXFMT_TYPE == 1
		VOLOG("set format - %s failed\n", "V4L2_PIX_FMT_RGB565");
#elif V4L2_PIXFMT_TYPE == 0 
		VOLOG("set format - %s failed\n", "V4L2_PIX_FMT_UYUV");
#endif
		::close(m_fd);
		return false;
	}

	return true;
}

bool vomeZM2VideoRender::updateview() {

	int ret = 0;
	struct v4l2_crop crop, dc;
	//if (!m_bFullScreen) { 
	if (1) { 
		crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		crop.c.left = 0;
		crop.c.top = 0;
#if 1
		crop.c.width = m_nVideoWidth;
		crop.c.height = m_nVideoHeight;
#else
		crop.c.width = (m_nVideoWidth <= m_nScreenWidth ? m_nVideoWidth : m_nScreenWidth);
		crop.c.height = (m_nVideoHeight <= m_nScreenHeight ? m_nVideoHeight : m_nScreenHeight);
#endif
		ret = ioctl(m_fd, VIDIOC_S_CROP, &crop);
		if (ret < 0) {
			VOLOG("%s\n", "VIDIOC_S_CROP error.");
			::close(m_fd);
			return false;
		}
	}

	dc.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl (m_fd, VIDIOC_G_CROP, &dc);
	if (ret < 0) {
		VOLOG("%s\n", "VIDIOC_G_CROP error.");
		::close(m_fd);
		return false;
	}

	//VOLOG("dest crop left: %d, top: %d, width: %d, height: %d\n", dc.c.left, dc.c.top, dc.c.width, dc.c.height)
	//VOLOG("video width: %d, height: %d\n", m_nVideoWidth, m_nVideoHeight);
	//VOLOG("old left: %d, top: %d, width: %d, height: %d\n", m_fmt.fmt.win.w.left, m_fmt.fmt.win.w.top, m_fmt.fmt.win.w.width, m_fmt.fmt.win.w.height)
	VOINFO("dest crop left: %d, top: %d, width: %d, height: %d\n", dc.c.left, dc.c.top, dc.c.width, dc.c.height)

	m_fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	//if (m_bFullScreen) { 
	if (1) { 
		int w,h,l,t;
		if (m_nScreenWidth * m_nVideoHeight > m_nVideoWidth * m_nScreenHeight) {
			h = m_nScreenHeight;
			w = m_nVideoWidth * h / m_nVideoHeight;
		} else {
			w = m_nScreenWidth;
			h = m_nVideoHeight * w / m_nVideoWidth;
		}
		m_fmt.fmt.win.w.left = (m_nScreenWidth - w) / 2;
		m_fmt.fmt.win.w.top = (m_nScreenHeight -h) / 2;

#if 1
		m_fmt.fmt.win.w.width = w; //m_nScreenHeight;
		m_fmt.fmt.win.w.height = h; //m_nScreenWidth;
#else
		m_fmt.fmt.win.w.width = (m_nVideoWidth <= m_nScreenWidth ? m_nScreenHeight : m_nVideoHeight/2);
		m_fmt.fmt.win.w.height = (m_nVideoHeight <= m_nScreenHeight ? m_nScreenWidth: m_nVideoWidth/2);
#endif
	} else {
		m_fmt.fmt.win.w.left = m_x;
		m_fmt.fmt.win.w.top = m_y;
		//m_fmt.fmt.win.w.width = m_nShowWidth;
		//m_fmt.fmt.win.w.height = m_nShowHeight;
	}

	VOINFO("pos: left %d, top %d width %d height %d\n", m_x, m_y, m_fmt.fmt.win.w.width, m_fmt.fmt.win.w.height);

	ret = ioctl(m_fd, VIDIOC_S_FMT, &m_fmt);
	if (ret < 0) {
		VOLOG("%s\n", "VIDIOC_S_FMT error.");
		::close(m_fd);
		return false;
	}

	return true;
}


bool vomeZM2VideoRender::StreamOn() {

	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int ret = ioctl(m_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		VOLOG("%s\n", "VIDIOC_STREAMON error!");
		return false;
	}

	return true;
}

bool vomeZM2VideoRender::StreamOff() {

	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ioctl(m_fd, VIDIOC_STREAMOFF, &a);

	for (int i=0; i<m_nbufcount; i++)
		munmap((void*)m_bufaddr[i], m_nbufsize);

	return  true;
}

bool vomeZM2VideoRender::RequestBuffer() {

	struct v4l2_requestbuffers reqbuf;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.count = V4L2_MAX_BUFFER;
	//reqbuf.reserved[0] = 0;
#if V4L2_MMAP_TYPE == 0
	reqbuf.memory = V4L2_MEMORY_MMAP;
#elif  V4L2_MMAP_TYPE == 1
	reqbuf.memory = V4L2_MEMORY_USERPTR;
#endif

	VOLOG("VIDIOC_REQBUFS: 0x%08x\n", VIDIOC_REQBUFS);
	int ret = ioctl(m_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		VOLOG("%s\n", "Could not allocate the buffers");
		return false;
	}
	VOLOG("requested: %d, allocated: %d\n", V4L2_MAX_BUFFER, reqbuf.count);
	m_nbufcount = reqbuf.count;
	
#if V4L2_MMAP_TYPE == 0
	
	for (int i=0; i<m_nbufcount; i++) { 
		m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		m_v4l2buf.memory = V4L2_MEMORY_MMAP;
		m_v4l2buf.index = i;
		if (ioctl(m_fd, VIDIOC_QUERYBUF, &m_v4l2buf) < 0) {
			VOLOG("%s\n", "VIDIOC_QUERYBUF error.");
			return false;
		}

		m_nbufsize = m_v4l2buf.length;
		VOLOG("buf len: %d\n", m_nbufsize);
		m_bufaddr[i] = (unsigned long)mmap(NULL, m_nbufsize, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, (off_t)m_v4l2buf.m.offset);	
		if ((int)m_bufaddr[i] == -1) {
			VOLOG("%s\n", "mmap error.");
			return false;
		}
		memset((void*)m_bufaddr[i], 0x80, m_nbufsize);
	}
#endif

	return true;
}

bool vomeZM2VideoRender::QueueBuffer() {

	/* Now queue it back to display it */
	int ret = 0;
	memset(&m_v4l2buf, 0, sizeof(v4l2_buffer));
	for (int i=0; i<m_nbufcount; ++i) {
		m_v4l2buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
#if V4L2_MMAP_TYPE == 0 
		m_v4l2buf.memory = V4L2_MEMORY_MMAP;
#elif V4L2_MMAP_TYPE == 1 
		m_v4l2buf.memory = V4L2_MEMORY_USERPTR;
#endif
		m_v4l2buf.index = i;
		m_v4l2buf.m.userptr = m_bufaddr[i];
		m_v4l2buf.length = m_nbufsize;
		ret = ioctl(m_fd, VIDIOC_QBUF, &m_v4l2buf);
		if (ret < 0) {
			VOLOG("%s\n", "VIDIOC_QBUF Error...");
			return false;
		}
	}

	return true;
}

static void YUV420_YUV422_c(unsigned char*  Dst, unsigned char*  Y, unsigned char*  U, unsigned char*  V, int YStride, int UVStride, int w, int h) {
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

	  for (int i=0; i<h; i+=2) 
	  {
			for (int j = 0; j < doublewidth; j+=4) 
			{
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

} //end of android namespace
