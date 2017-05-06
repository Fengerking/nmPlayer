
/*
 * Video Render for Zoom2
 * David 2009-08-11
 */


#if !defined __VOME_ZM2_VIDEO_RENDER_H__
#define __VOME_ZM2_VIDEO_RENDER_H__

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include "vomeRGB565VideoRender.h"

#define V4L2_MAX_BUFFER			4
#define V4L2_MMAP_TYPE			0		// 0: V4L2_MEMORY_MMAP, 1:V4L2_MEMORY_USERPTR
#define V4L2_PIXFMT_TYPE		0		// 0: YUV, 1: RGB565

namespace android {

#if defined __cplusplus
extern "C" {
#endif

typedef void (*VOCONV420TO422API)(unsigned char* Dst, unsigned char* Y, unsigned char* U, unsigned char* V, int YStride, int UVStride, int w, int h);

#if defined __cplusplus
}
#endif


class vomeZM2VideoRender : public vomeRGB565VideoRender {

public:
	vomeZM2VideoRender(void);
	virtual ~vomeZM2VideoRender();

	int v4lopen();
	int v4lclose();

	int UpdateSize(void);
  bool		Render(VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer);
	bool		CheckColorType(VO_ANDROID_COLORTYPE nColorType);
  //void		Render(OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer);

protected:
	inline bool setformat();
	inline bool updateview();
	inline bool StreamOn();
	inline bool StreamOff();
	inline bool	RequestBuffer();
	inline bool	QueueBuffer();

private:
	int m_fd;
	int m_x;
	int m_y;
	int m_nScreenWidth;
	int m_nScreenHeight;
	int m_nbufcount;
	int m_nbufsize;
	int m_ndatasize;
	VOCONV420TO422API m_pfconv420to422;
	unsigned long m_bufaddr[V4L2_MAX_BUFFER];
	

	struct v4l2_format m_fmt;
	struct v4l2_buffer m_v4l2buf;
};
};
#endif //__VOME_ZM2_VIDEO_RENDER_H__

