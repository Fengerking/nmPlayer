#if !defined __VO_V4L2_RENDER_H__
#define __VO_V4L2_RENDER_H__

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include "CBaseVideoRender.h"

#define MAX_V4L2BUF_NUM				6
#define VOV4L2_VIDFMT					0  // 0: YUV, 1: RGB565
#define VOV4L2_MEMTYPE				0  // 0: V4L2_MEMORY_MMAP, 1:V4L2_MEMORY_USERPTR

#if defined TARGET_OMAP4
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    2
#else
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    5
#endif

#if defined __cplusplus
extern "C" {
#endif
	typedef void (*VOCONV420TO422API)(unsigned char* Dst, unsigned char* Y, unsigned char* U, unsigned char* V, int YStride, int UVStride, int w, int h, int outstride);
#if defined __cplusplus
}     
#endif

typedef struct __VOV4L2BUFFER__ {
	int flag;
	void* buf;
} VOV4L2BUFFER;

class voV4L2Render : public CBaseVideoRender{
public:
	voV4L2Render(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~voV4L2Render();

	VO_U32 Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

	int v4l2open();
	int v4l2close();

	bool FullScreen(const bool);

protected:
	VO_U32	UpdateSize(void);
	inline bool StreamOn();
	inline bool StreamOff();
	inline bool	RequestBuffer();
	inline bool	QueueBuffer();

private:
	int													m_nfdv4l2;
	int													m_x;
	int													m_y;
	int													m_nbufcount;
	int													m_nbufsize;
	int													m_nqueued;
	int													m_nbufenq;
	void*												m_hconv;
	VOCONV420TO422API						m_pfconv420to422;
	VOV4L2BUFFER								m_ppbuf[MAX_V4L2BUF_NUM];
	struct v4l2_buffer					m_v4l2buf;
};

#endif //__VO_V4L2_RENDER_H__

