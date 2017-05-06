/********************
 * david
 * 2009-05-10
 * VisualOn
 ****************************************************************/

#ifndef __CFBVideoRender_H__
#define __CFBVideoRender_H__

#include <linux/fb.h>
#include "CBaseVideoRender.h"

#define iMASK_COLORS 3
#define SIZE_MASKS (iMASK_COLORS * sizeof(DWORD))

class CFBVideoRender : public CBaseVideoRender {
public:
	// Used to control the image drawing
	CFBVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	~CFBVideoRender (void);

	VO_U32 Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	VO_U32	UpdateSize (void);
	int fbopen() __attribute__((always_inline));
	int fbclose() __attribute__((always_inline));
	void render32(); //__attribute__((always_inline));

protected:
	int m_fb;
	int m_nimgbuflen;
	int m_nbpp;
	int m_nstride;
	//int m_nsrcstride;
	long m_loffset;	
	unsigned char* m_pimgbuf;
	void* m_pfbbuf;
	struct fb_fix_screeninfo m_fi;
	struct fb_var_screeninfo m_vi;

};

#endif // __CFBVideoRender_H__

