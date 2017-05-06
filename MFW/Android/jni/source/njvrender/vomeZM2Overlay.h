/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003				*
 *																		*
 ************************************************************************/
/*******************************************************************************
File:		vomeZM2Overlay.h

Contains:	vomeZM2Overlay header file

Written by:	David

Change History (most recent first):
2009-10-15	David Create file

 *******************************************************************************/

#ifndef __vomeZM2Overlay_H__
#define __vomeZM2Overlay_H__

#include <ui/Overlay.h>
#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>
#include "vomeRGB565VideoRender.h"


using namespace android;

#if defined __cplusplus
extern "C" {
#endif

typedef void (*VOCONV420TO422API)(unsigned char* Dst, unsigned char* Y, unsigned char* U, unsigned char* V, int YStride, int UVStride, int w, int h, int outstride);

#if defined __cplusplus
}
#endif

typedef struct
{            
	int fd;
	size_t length;
	uint32_t offset;
	void *ptr;
} mapping_data_t;

class vomeZM2Overlay : public vomeRGB565VideoRender {

public:
	vomeZM2Overlay(void* pview=NULL);
	virtual ~vomeZM2Overlay();

	int		SetVideoSize (int nWidth, int nHeight);
	bool	CheckColorType (VO_ANDROID_COLORTYPE nColorType);
	bool	Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer);

protected:
	bool	Create();
	void	Release(); 

private:
	sp<Overlay>	m_poverlay;
	int m_nbufcount;
	int m_nbufindex;
	void* m_hconv;
	VOCONV420TO422API m_pfconv420to422;
};

#endif // #ifndef __vomeZM2Overlay_H__

