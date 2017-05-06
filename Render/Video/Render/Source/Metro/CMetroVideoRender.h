#ifndef __CMetroVideoRender_H__
#define __CMetroVideoRender_H__

#include "wrl.h"
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <wincodec.h>
#include "CBaseVideoRender.h"
#include "CD2DVideoRender.h"

class CMetroVideoRender :  public CBaseVideoRender
{
public:
	// Used to control the image drawing
	CMetroVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CMetroVideoRender (void);

	virtual VO_U32 	SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_RGB32_PACKED);
	virtual VO_U32 	Start (void);
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);
	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	Windows::UI::Core::CoreWindow^				m_Windows;
	CD2DVideoRender ^							mpD2DRender;

	VO_VIDEO_BUFFER								m_outBuffer;
	BYTE *										m_pBitmapBuffer;
	BOOL										mbUpdate;
};

#endif // __CMetroVideoRender_H__
